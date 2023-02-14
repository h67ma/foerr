#include "pipbuck.hpp"

#include <cmath>

#include <memory>

#include "pages/pipbuck_page_inventory_other.hpp"
#include "pages/pipbuck_page_status_main.hpp"
#include "pages/pipbuck_page_equipment.hpp"
#include "pages/pipbuck_page_settings.hpp"
#include "pages/pipbuck_page_controls.hpp"
#include "pages/pipbuck_page_effects.hpp"
#include "pages/pipbuck_page_weapons.hpp"
#include "pages/pipbuck_page_enemies.hpp"
#include "pages/pipbuck_page_health.hpp"
#include "pages/pipbuck_page_skills.hpp"
#include "pages/pipbuck_page_quests.hpp"
#include "pages/pipbuck_page_perks.hpp"
#include "pages/pipbuck_page_world.hpp"
#include "pages/pipbuck_page_armor.hpp"
#include "pages/pipbuck_page_notes.hpp"
#include "pages/pipbuck_page_ammo.hpp"
#include "pages/pipbuck_page_map.hpp"
#include "pages/pipbuck_page_load.hpp"
#include "pages/pipbuck_page_save.hpp"
#include "pages/pipbuck_page_log.hpp"
#include "../../util/i18n.hpp"
#include "../../util/util.hpp"
#include "../log.hpp"

#define COLOR_SCREEN_GRAY COLOR_GRAY(0x39)
#define SCREEN_X 365
#define SCREEN_Y 178
#define SCREEN_WIDTH 1094
#define SCREEN_HEIGHT 685

PipBuck::PipBuck(ResourceManager &resMgr, Campaign &campaign, GameState &gameState) :
	resMgr(resMgr),
	gameState(gameState),
	campaign(campaign),
	categories {
		{ PIPB_CAT_STATUS, { resMgr, PIPB_PAGE_STATUS_MAIN, {
			{ PIPB_PAGE_STATUS_MAIN, std::make_shared<PipBuckPageMainStatus>(resMgr) },
			{ PIPB_PAGE_SKILLS, std::make_shared<PipBuckPageSkills>(resMgr) },
			{ PIPB_PAGE_PERKS, std::make_shared<PipBuckPagePerks>(resMgr) },
			{ PIPB_PAGE_EFFECTS, std::make_shared<PipBuckPageEffects>(resMgr) },
			{ PIPB_PAGE_HEALTH, std::make_shared<PipBuckPageHealth>(resMgr) }
		} } },
		{ PIPB_CAT_INVENTORY, { resMgr, PIPB_PAGE_WEAPONS, {
			{ PIPB_PAGE_WEAPONS, std::make_shared<PipBuckPageWeapons>(resMgr) },
			{ PIPB_PAGE_ARMOR, std::make_shared<PipBuckPageArmor>(resMgr) },
			{ PIPB_PAGE_EQUIPMENT, std::make_shared<PipBuckPageEquipment>(resMgr) },
			{ PIPB_PAGE_INVENTORY_OTHER, std::make_shared<PipBuckPageInventoryOther>(resMgr) },
			{ PIPB_PAGE_AMMO, std::make_shared<PipBuckPageAmmo>(resMgr) }
		} } },
		{ PIPB_CAT_INFO, { resMgr, PIPB_PAGE_MAP, {
			{ PIPB_PAGE_MAP, std::make_shared<PipBuckPageMap>(resMgr) },
			{ PIPB_PAGE_WORLD, std::make_shared<PipBuckPageWorld>(resMgr, campaign) },
			{ PIPB_PAGE_QUESTS, std::make_shared<PipBuckPageQuests>(resMgr) },
			{ PIPB_PAGE_NOTES, std::make_shared<PipBuckPageNotes>(resMgr) },
			{ PIPB_PAGE_ENEMIES, std::make_shared<PipBuckPageEnemies>(resMgr) }
		} } },
		{ PIPB_CAT_MAIN, { resMgr, PIPB_PAGE_LOAD, {
			{ PIPB_PAGE_LOAD, std::make_shared<PipBuckPageLoad>(resMgr) },
			{ PIPB_PAGE_SAVE, std::make_shared<PipBuckPageSave>(resMgr) },
			{ PIPB_PAGE_SETTINGS, std::make_shared<PipBuckPageSettings>(resMgr) },
			{ PIPB_PAGE_CONTROLS, std::make_shared<PipBuckPageControls>(resMgr) },
			{ PIPB_PAGE_LOG, std::make_shared<PipBuckPageLog>(resMgr) }
		} } }
	},
	categoryButtons {
		{ PIPB_CAT_STATUS, { BTN_BIG, resMgr, { 650, 900 }, STR_PIPBUCK_STATUS } },
		{ PIPB_CAT_INVENTORY, { BTN_BIG, resMgr, { 855, 915 }, STR_PIPBUCK_INV } },
		{ PIPB_CAT_INFO, { BTN_BIG, resMgr, { 1055, 900 }, STR_PIPBUCK_INFO } },
		{ PIPB_CAT_MAIN, { BTN_BIG, resMgr, { 55, 700 }, STR_PIPBUCK_MAINMENU } }
	},
	closeBtn(BTN_BIG, resMgr, { 55, 800 }, STR_PIPBUCK_CLOSE, nullptr, CLICK_CONSUMED_CLOSE),
	soundOpenClose(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_OPENCLOSE)),
	soundCategoryBtn(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGECHANGE)),
	pipBuckSprite(resMgr.getTexture(PATH_TXT_PIPBUCK_OVERLAY)),
	pipBuckScreenRadialGrad(resMgr.getTexture(PATH_TXT_PIPBUCK_BG_RADIAL))
{
	float initialVolume = static_cast<float>(SettingsManager::fxVolume);
	this->soundOpenClose.setVolume(initialVolume);
	this->soundCategoryBtn.setVolume(initialVolume);

	this->radIndicator.setPointCount(3);
	this->radIndicator.setOrigin(5.F, 0.F);
	this->radIndicator.setPosition(130.F, 190.F);
	this->radIndicator.setPoint(0, { 0.F, 0.F });
	this->radIndicator.setPoint(1, { 10.F, 0.F });
	this->radIndicator.setPoint(2, { 5.F, 60.F });
	this->radIndicator.setFillColor(sf::Color::Black);

	for (auto &btn : this->categoryButtons)
	{
		this->hoverMgr += &btn.second;
	}
	this->hoverMgr += &this->closeBtn;

	this->screenBackgroundGray.setFillColor(COLOR_SCREEN_GRAY);
	this->screenBackgroundGray.setPosition(SCREEN_X, SCREEN_Y);
	this->screenBackgroundGray.setSize({ SCREEN_WIDTH, SCREEN_HEIGHT });

	// transparent radial gradient, tinted with hud color
	this->pipBuckScreenRadialGrad.setColor(SettingsManager::hudColor);
	this->pipBuckScreenRadialGrad.setPosition(SCREEN_X, SCREEN_Y);

	// "scan lines"
	sf::Image img;
	img.create(1, 2, sf::Color::Transparent);
	img.setPixel(0, 0, sf::Color::Black);

	this->screenBackgroundStripesTxt.loadFromImage(img);
	this->screenBackgroundStripesTxt.setRepeated(true);

	this->screenBackgroundStripes.setTexture(this->screenBackgroundStripesTxt);
	this->screenBackgroundStripes.setTextureRect({0, 0, SCREEN_WIDTH, SCREEN_HEIGHT});
	this->screenBackgroundStripes.setPosition(SCREEN_X, SCREEN_Y);
}

void PipBuck::handleScreenResize(sf::Vector2u windowSize)
{
	// for now copy the behaviour of Remains pipbuck, i.e. display it unscaled in bottom left corner
	// TODO scale should affect sprite size and individual buttons/labels/etc scale and placement
	this->setPosition(0, static_cast<float>(windowSize.y - this->pipBuckSprite.getLocalBounds().height));
}

/**
 * Opens PipBuck by changing game state and plays the open/close sound.
 *
 * Because the mouse can move during the time PipBuck is closed, it will most probably end up in a position far from
 * the one previously handled in ::handleMouseMove(). Because of this, hover state could not reflect current mouse
 * position. To fix this, emulate a single mouse movement event.
 */
void PipBuck::open(sf::Vector2i mousePos, bool sound)
{
	this->handleMouseMove(mousePos);

	this->gameState = STATE_PIPBUCK;
	if (sound)
		this->soundOpenClose.play();
	Log::d(STR_GAME_PAUSED);
}

/**
 * Closes PipBuck by changing game state and plays the open/close sound.
 */
void PipBuck::close()
{
	this->gameState = STATE_PLAYING;
	this->soundOpenClose.play();
	Log::d(STR_GAME_RESUMED);
}

bool PipBuck::changeCategory(PipBuckCategoryType categoryType)
{
	auto found = this->categories.find(categoryType);
	if (found == this->categories.end())
		return false;

	// ::categoryButtons map has the same keys as ::categories map
	// ...again, famous last words. let's hope not
	this->categoryButtons.at(this->selectedCategory).setSelected(false);
	this->categoryButtons.at(categoryType).setSelected(true);

	this->selectedCategory = categoryType;
	return true;
}

void PipBuck::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getIntPosition();

	ClickStatus catResult = this->categories.at(this->selectedCategory).handleLeftClick(clickPos);
	if (catResult == CLICK_CONSUMED)
		return;
	else if (catResult == CLICK_CONSUMED_CLOSE)
	{
		this->close();
		return;
	}
	else if (catResult == CLICK_CONSUMED_UNLOAD)
	{
		// note: we could unload campaign inside the page (inside btn callback),
		// but then we'd have to clear resmgr two times (unload campaign + clear resmgr,
		// unload pipbuck infos, clear resmgr)
		this->unloadCampaignInfos();
		this->campaign.unload();
		this->gameState = STATE_MAINMENU;
		return;
	}

	for (auto &btn : this->categoryButtons)
	{
		if (btn.second.containsPoint(clickPos))
		{
			if (btn.first != this->selectedCategory)
			{
				this->changeCategory(btn.first);
				this->soundCategoryBtn.play();
			}
			return;
		}
	}

	if (this->closeBtn.handleLeftClick(clickPos) == CLICK_CONSUMED_CLOSE)
		this->close();
}

void PipBuck::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getIntPosition();

	if (this->categories.at(this->selectedCategory).handleMouseMove(mousePos))
		return; // hover "consumed"

	this->hoverMgr.handleMouseMove(mousePos);
}

/**
 * Setups the PipBuck pages according to campaign data.
 * Locations in the campaign should be already loaded.
 *
 * @return true if setup was successful
 * @return false if there was an error during setup
 */
bool PipBuck::setupCampaignInfos()
{
	for (auto &cat : this->categories)
	{
		if (!cat.second.setupCampaignInfos())
		{
			this->unloadCampaignInfos();
			this->resMgr.cleanUnused();
			return false;
		}
	}

	return true;
}

void PipBuck::unloadCampaignInfos()
{
	Log::d(STR_PIPBUCK_UNLOADING_CAMPAIGN);

	for (auto &cat : this->categories)
	{
		cat.second.unloadCampaignInfos();
	}
}

/**
 * Changes current page.
 * If the mapping page -> category is incorrect, page will not be changed.
 * If requested page is already open, PipBuck will be closed.
 *
 * @param pageType the page type
 * @param mousePos current mouse position (required to refresh hover state when opening PipBuck)
 * @return true if switch was successful
 * @return false if page -> category mapping is incorrect
 */
bool PipBuck::switchToPage(PipBuckPageType pageType, sf::Vector2i mousePos)
{
	PipBuckCategoryType newCat = PipBuckCategory::pageTypeToCategoryType(pageType);

	if (newCat == PIPB_CAT_NO_CAT)
		return false;

	if (this->gameState == STATE_PIPBUCK)
	{
		if (newCat == this->selectedCategory && this->categories.at(newCat).getSelectedPage() == pageType)
		{
			// we're already on requested category & page and switch was requested again, close PipBuck
			this->close();
			return true;
		}

		// we only want to play category change sound if we're not opening/closing PipBuck,
		// as then another sound will play
		this->soundCategoryBtn.play();
	}
	else
		this->open(mousePos);

	return this->changeCategory(newCat) && this->categories.at(newCat).changePage(pageType);
}

bool PipBuck::setup()
{
	for (auto &cat : this->categories)
	{
		if (!cat.second.setup())
			return false;
	}

	if (!this->changeCategory(this->selectedCategory)) // default category
		return false;

	return true;
}

/**
 * Sets the value displayed by "hardware" rad indicator.
 *
 * @param rads amount of rads, in fraction (1.0 = max)
 */
void PipBuck::setRadLevel(float rads)
{
	if (rads > 1.F)
		this->radIndicatorLevel = 1.F;
	else
		this->radIndicatorLevel = rads;
}

/**
 * Don't expect this function to deploy smooth jazz.
 */
double PipBuck::getSmoothNoise(double time)
{
	return sin(time * 7) + cos(time * 12) - cos(time * 9);
}

void PipBuck::tick()
{
	this->radIndicator.setRotation(
		this->radIndicatorLevel * -180 +
		static_cast<float>(this->getSmoothNoise(this->timer.getElapsedTime().asSeconds())/2)
	);
}

void PipBuck::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->screenBackgroundGray, states);
	target.draw(this->pipBuckScreenRadialGrad, states);
	target.draw(this->screenBackgroundStripes, states);

	target.draw(this->pipBuckSprite, states);
	target.draw(this->radIndicator, states);

	target.draw(this->categories.at(this->selectedCategory), states);

	for (const auto &btn : this->categoryButtons)
	{
		target.draw(btn.second, states);
	}

	target.draw(this->closeBtn, states);
}
