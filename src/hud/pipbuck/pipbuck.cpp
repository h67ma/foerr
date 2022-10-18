#include <math.h>
#include "pipbuck.hpp"
#include "../../util/i18n.hpp"
#include "../log.hpp"
#include "../../util/util.hpp"
#include "pages/pipbuck_page_status_main.hpp"
#include "pages/pipbuck_page_skills.hpp"
#include "pages/pipbuck_page_perks.hpp"
#include "pages/pipbuck_page_effects.hpp"
#include "pages/pipbuck_page_health.hpp"
#include "pages/pipbuck_page_weapons.hpp"
#include "pages/pipbuck_page_armor.hpp"
#include "pages/pipbuck_page_equipment.hpp"
#include "pages/pipbuck_page_inventory_other.hpp"
#include "pages/pipbuck_page_ammo.hpp"
#include "pages/pipbuck_page_map.hpp"
#include "pages/pipbuck_page_quests.hpp"
#include "pages/pipbuck_page_world.hpp"
#include "pages/pipbuck_page_notes.hpp"
#include "pages/pipbuck_page_enemies.hpp"
#include "pages/pipbuck_page_load.hpp"
#include "pages/pipbuck_page_save.hpp"
#include "pages/pipbuck_page_settings.hpp"
#include "pages/pipbuck_page_controls.hpp"
#include "pages/pipbuck_page_log.hpp"

PipBuck::PipBuck(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign, GameState &gameState, SettingsManager &settings) :
	resMgr(resMgr),
	gameState(gameState),
	campaign(campaign),
	categories {
		{ PIPB_CAT_STATUS, { scale, hudColor, fxVolume, resMgr, PIPB_PAGE_STATUS_MAIN, {
			{ PIPB_PAGE_STATUS_MAIN, std::make_shared<PipBuckPageMainStatus>(resMgr) },
			{ PIPB_PAGE_SKILLS, std::make_shared<PipBuckPageSkills>(resMgr) },
			{ PIPB_PAGE_PERKS, std::make_shared<PipBuckPagePerks>(resMgr) },
			{ PIPB_PAGE_EFFECTS, std::make_shared<PipBuckPageEffects>(resMgr) },
			{ PIPB_PAGE_HEALTH, std::make_shared<PipBuckPageHealth>(resMgr) }
		} } },
		{ PIPB_CAT_INVENTORY, { scale, hudColor, fxVolume, resMgr, PIPB_PAGE_WEAPONS, {
			{ PIPB_PAGE_WEAPONS, std::make_shared<PipBuckPageWeapons>(resMgr) },
			{ PIPB_PAGE_ARMOR, std::make_shared<PipBuckPageArmor>(resMgr) },
			{ PIPB_PAGE_EQUIPMENT, std::make_shared<PipBuckPageEquipment>(resMgr) },
			{ PIPB_PAGE_INVENTORY_OTHER, std::make_shared<PipBuckPageInventoryOther>(resMgr) },
			{ PIPB_PAGE_AMMO, std::make_shared<PipBuckPageAmmo>(resMgr) }
		} } },
		{ PIPB_CAT_INFO, { scale, hudColor, fxVolume, resMgr, PIPB_PAGE_MAP, {
			{ PIPB_PAGE_MAP, std::make_shared<PipBuckPageMap>(resMgr) },
			{ PIPB_PAGE_WORLD, std::make_shared<PipBuckPageWorld>(scale, hudColor, resMgr, campaign) },
			{ PIPB_PAGE_QUESTS, std::make_shared<PipBuckPageQuests>(resMgr) },
			{ PIPB_PAGE_NOTES, std::make_shared<PipBuckPageNotes>(resMgr) },
			{ PIPB_PAGE_ENEMIES, std::make_shared<PipBuckPageEnemies>(resMgr) }
		} } },
		{ PIPB_CAT_MAIN, { scale, hudColor, fxVolume, resMgr, PIPB_PAGE_LOAD, {
			{ PIPB_PAGE_LOAD, std::make_shared<PipBuckPageLoad>(scale, hudColor, resMgr, campaign, gameState) },
			{ PIPB_PAGE_SAVE, std::make_shared<PipBuckPageSave>(resMgr) },
			{ PIPB_PAGE_SETTINGS, std::make_shared<PipBuckPageSettings>(scale, hudColor, resMgr, settings) },
			{ PIPB_PAGE_CONTROLS, std::make_shared<PipBuckPageControls>(resMgr, scale, hudColor) },
			{ PIPB_PAGE_LOG, std::make_shared<PipBuckPageLog>(resMgr) }
		} } }
	},
	categoryButtons {
		{ PIPB_CAT_STATUS, { scale, BTN_BIG, hudColor, resMgr, { 650, 900 }, STR_PIPBUCK_STATUS } },
		{ PIPB_CAT_INVENTORY, { scale, BTN_BIG, hudColor, resMgr, { 855, 915 }, STR_PIPBUCK_INV } },
		{ PIPB_CAT_INFO, { scale, BTN_BIG, hudColor, resMgr, { 1055, 900 }, STR_PIPBUCK_INFO } },
		{ PIPB_CAT_MAIN, { scale, BTN_BIG, hudColor, resMgr, { 55, 700 }, STR_PIPBUCK_MAINMENU } }
	},
	closeBtn(scale, BTN_BIG, hudColor, resMgr, { 55, 800 }, STR_PIPBUCK_CLOSE, nullptr, CLICK_CONSUMED_CLOSE),
	soundOpenClose(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_OPENCLOSE)),
	soundCategoryBtn(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGECHANGE)),
	pipBuckSprite(resMgr.getTexture(PATH_TXT_PIPBUCK_OVERLAY))
{

	this->soundOpenClose.get().setVolume(static_cast<float>(fxVolume));
	this->soundCategoryBtn.get().setVolume(static_cast<float>(fxVolume));

	this->radIndicator.setPointCount(3);
	this->radIndicator.setOrigin(5.f, 0.f);
	this->radIndicator.setPosition(130.f, 190.f);
	this->radIndicator.setPoint(0, { 0.f, 0.f });
	this->radIndicator.setPoint(1, { 10.f, 0.f });
	this->radIndicator.setPoint(2, { 5.f, 60.f });
	this->radIndicator.setFillColor(sf::Color::Black);

	for (auto &btn : this->categoryButtons)
	{
		this->hoverMgr += &btn.second;
	}
	this->hoverMgr += &this->closeBtn;
}

void PipBuck::handleScreenResize(uint screenW, uint screenH)
{
	// for now copy the behaviour of Remains pipbuck, i.e. display it unscaled in bottom left corner
	// TODO scale should affect sprite size and individual buttons/labels/etc scale and placement
	this->setPosition(0, static_cast<float>(screenH - this->pipBuckSprite.get().getLocalBounds().height));
}

/**
 * Opens PipBuck by changing game state and plays the open/close sound.
 */
void PipBuck::open(bool sound)
{
	this->gameState = STATE_PIPBUCK;
	if (sound)
		this->soundOpenClose.get().play();
	Log::d(STR_GAME_PAUSED);
}

/**
 * Closes PipBuck by changing game state and plays the open/close sound.
 */
void PipBuck::close()
{
	this->gameState = STATE_PLAYING;
	this->soundOpenClose.get().play();
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
	clickPos -= static_cast<sf::Vector2i>(this->getPosition());

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
				this->soundCategoryBtn.get().play();
			}
			return;
		}
	}

	if (this->closeBtn.handleLeftClick(clickPos) == CLICK_CONSUMED_CLOSE)
		this->close();
}

void PipBuck::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= static_cast<sf::Vector2i>(this->getPosition());

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
 *
 * @param pageType the page type
 * @param quiet whether to not play a page changing sound
 * @return true if switch was successful
 * @return false if page -> category mapping is incorrect
 */
bool PipBuck::switchToPage(PipBuckPageType pageType, bool quiet)
{
	if (!quiet)
		this->soundCategoryBtn.get().play();

	PipBuckCategoryType newCat = PipBuckCategory::pageTypeToCategoryType(pageType);
	if (newCat == PIPB_CAT_NO_CAT)
		return false;

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
	if (rads > 1.f)
		this->radIndicatorLevel = 1.f;
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

void PipBuck::updateDraw()
{
	this->radIndicator.setRotation(
		this->radIndicatorLevel * -180 +
		static_cast<float>(this->getSmoothNoise(this->timer.getElapsedTime().asSeconds())/2)
	);
}

void PipBuck::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->pipBuckSprite.sprite, states);
	target.draw(this->radIndicator, states);

	target.draw(this->categories.at(this->selectedCategory), states);

	for (const auto &btn : this->categoryButtons)
	{
		target.draw(btn.second, states);
	}

	target.draw(this->closeBtn, states);
}
