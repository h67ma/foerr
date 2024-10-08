// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "pipbuck.hpp"

#include <cmath>

#include <memory>
#include <vector>

#include "../../util/i18n.hpp"
#include "../../util/util.hpp"
#include "../buttons/pipbuck_category_button.hpp"
#include "../log.hpp"
#include "../pages/gui_page_ammo.hpp"
#include "../pages/gui_page_armor.hpp"
#include "../pages/gui_page_controls.hpp"
#include "../pages/gui_page_effects.hpp"
#include "../pages/gui_page_enemies.hpp"
#include "../pages/gui_page_equipment.hpp"
#include "../pages/gui_page_health.hpp"
#include "../pages/gui_page_inventory_other.hpp"
#include "../pages/gui_page_load.hpp"
#include "../pages/gui_page_log.hpp"
#include "../pages/gui_page_map.hpp"
#include "../pages/gui_page_notes.hpp"
#include "../pages/gui_page_perks.hpp"
#include "../pages/gui_page_quests.hpp"
#include "../pages/gui_page_save.hpp"
#include "../pages/gui_page_settings.hpp"
#include "../pages/gui_page_skills.hpp"
#include "../pages/gui_page_status_main.hpp"
#include "../pages/gui_page_weapons.hpp"
#include "../pages/gui_page_world.hpp"

const sf::Color COLOR_SCREEN_GRAY = COLOR_GRAY(0x39);
const sf::Vector2f POS_SCREEN { 365, 178 };
const sf::Vector2f SIZE_SCREEN { 1094, 685 };

const sf::Vector2f POS_RAD_INDICATOR { 130, 190 };
const sf::Vector2f ORIGIN_RAD_INDICATOR { 5, 0 };
const std::vector<sf::Vector2f> POS_RAD_INDICATOR_POINTS {
	{ 0, 0 },
	{ 10, 0 },
	{ 5, 60 },
};

PipBuck::PipBuck(ResourceManager& resMgr, CursorManager& cursorMgr, Campaign& campaign, GameState& gameState) :
	resMgr(resMgr),
	cursorMgr(cursorMgr),
	gameState(gameState),
	campaign(campaign),
	categories {
		{ PIPB_CAT_STATUS,
		  std::make_shared<PipBuckCategory>(resMgr, PIPB_PAGE_STATUS_MAIN,
											std::map<PipBuckPageType, std::shared_ptr<GuiPage>> {
												{ PIPB_PAGE_STATUS_MAIN, std::make_shared<GuiPageMainStatus>(resMgr) },
												{ PIPB_PAGE_SKILLS, std::make_shared<GuiPageSkills>(resMgr) },
												{ PIPB_PAGE_PERKS, std::make_shared<GuiPagePerks>(resMgr) },
												{ PIPB_PAGE_EFFECTS, std::make_shared<GuiPageEffects>(resMgr) },
												{ PIPB_PAGE_HEALTH, std::make_shared<GuiPageHealth>(resMgr) } }) },
		{ PIPB_CAT_INVENTORY, std::make_shared<PipBuckCategory>(
								  resMgr, PIPB_PAGE_WEAPONS,
								  std::map<PipBuckPageType, std::shared_ptr<GuiPage>> {
									  { PIPB_PAGE_WEAPONS, std::make_shared<GuiPageWeapons>(resMgr) },
									  { PIPB_PAGE_ARMOR, std::make_shared<GuiPageArmor>(resMgr) },
									  { PIPB_PAGE_EQUIPMENT, std::make_shared<GuiPageEquipment>(resMgr) },
									  { PIPB_PAGE_INVENTORY_OTHER, std::make_shared<GuiPageInventoryOther>(resMgr) },
									  { PIPB_PAGE_AMMO, std::make_shared<GuiPageAmmo>(resMgr) } }) },
		{ PIPB_CAT_INFO,
		  std::make_shared<PipBuckCategory>(resMgr, PIPB_PAGE_MAP,
											std::map<PipBuckPageType, std::shared_ptr<GuiPage>> {
												{ PIPB_PAGE_MAP, std::make_shared<GuiPageMap>(resMgr) },
												{ PIPB_PAGE_WORLD, std::make_shared<GuiPageWorld>(resMgr, campaign) },
												{ PIPB_PAGE_QUESTS, std::make_shared<GuiPageQuests>(resMgr) },
												{ PIPB_PAGE_NOTES, std::make_shared<GuiPageNotes>(resMgr) },
												{ PIPB_PAGE_ENEMIES, std::make_shared<GuiPageEnemies>(resMgr) } }) },
		{ PIPB_CAT_GAME,
		  std::make_shared<PipBuckCategory>(resMgr, PIPB_PAGE_LOAD,
											std::map<PipBuckPageType, std::shared_ptr<GuiPage>> {
												{ PIPB_PAGE_LOAD, std::make_shared<GuiPageLoad>(resMgr, false) },
												{ PIPB_PAGE_SAVE, std::make_shared<GuiPageSave>(resMgr) },
												{ PIPB_PAGE_SETTINGS, std::make_shared<GuiPageSettings>(resMgr) },
												{ PIPB_PAGE_CONTROLS, std::make_shared<GuiPageControls>(resMgr) },
												{ PIPB_PAGE_LOG, std::make_shared<GuiPageLog>(resMgr) } }) }
	},
	closeBtn(BTN_BIG, resMgr, { 55, 800 }, STR_PIPBUCK_CLOSE, nullptr, CLICK_CONSUMED_CLOSE),
	soundOpenClose(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_OPENCLOSE)),
	soundCategoryBtn(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGECHANGE)),
	pipBuckSprite(resMgr.getTexture(PATH_TXT_PIPBUCK_OVERLAY)),
	pipBuckScreenRadialGrad(resMgr.getTexture(PATH_TXT_PIPBUCK_BG_RADIAL))
{
	this->categoryButtons.emplace(PIPB_CAT_STATUS,
								  std::make_unique<PipBuckCategoryButton>(resMgr, sf::Vector2u(625, 893),
																		  PIPBUCK_CAT_BTN_LEFT, STR_PIPBUCK_STATUS,
																		  PATH_TXT_PIPBUCK_CAT_BTN_LEFT));
	this->categoryButtons.emplace(PIPB_CAT_INVENTORY,
								  std::make_unique<PipBuckCategoryButton>(resMgr, sf::Vector2u(807, 906),
																		  PIPBUCK_CAT_BTN_MID, STR_PIPBUCK_INV,
																		  PATH_TXT_PIPBUCK_CAT_BTN_MID));
	this->categoryButtons.emplace(PIPB_CAT_INFO,
								  std::make_unique<PipBuckCategoryButton>(resMgr, sf::Vector2u(1030, 893),
																		  PIPBUCK_CAT_BTN_RIGHT, STR_PIPBUCK_INFO,
																		  PATH_TXT_PIPBUCK_CAT_BTN_RIGHT));

	// TODO this btn currently does not react properly to hud color change, but it will be replaced with a nicer, custom
	// btn later
	this->categoryButtons.emplace(PIPB_CAT_GAME, std::make_unique<SimpleButton>(BTN_BIG, resMgr, sf::Vector2u(55, 700),
																				STR_PIPBUCK_GAME));

	this->setupRadIndicator();
	this->setupScreenBackground();
	this->setGuiScale();
	this->setAudioVolume();

	for (auto& btn : this->categoryButtons)
	{
		this->hoverMgr += btn.second.get(); // FIXME this is pretty bad...
	}
	this->hoverMgr += &this->closeBtn;
}

void PipBuck::setAudioVolume()
{
	this->soundOpenClose.setVolume(SettingsManager::fxVolume);
	this->soundCategoryBtn.setVolume(SettingsManager::fxVolume);
}

/**
 * Sets up the black arrow displayed on rad indicator. The arrow is simply a triangle.
 */
void PipBuck::setupRadIndicator()
{
	this->radIndicator.setPointCount(POS_RAD_INDICATOR_POINTS.size());
	this->radIndicator.setFillColor(sf::Color::Black);
}

void PipBuck::setRadIndicatorScale()
{
	this->radIndicator.setOrigin(ORIGIN_RAD_INDICATOR * SettingsManager::guiScale);
	this->radIndicator.setPosition(POS_RAD_INDICATOR * SettingsManager::guiScale);

	for (int i = 0; i < POS_RAD_INDICATOR_POINTS.size(); i++)
	{
		this->radIndicator.setPoint(i, POS_RAD_INDICATOR_POINTS[i] * SettingsManager::guiScale);
	}
}

/**
 * Sets up components used to display PipBuck screen background:
 *	- Solid gray rectangle
 *	- Transparent image of a radial gradient, tinted
 *	- "Scan lines" - a simple, repeated two-pixel texture created programmatically
 */
void PipBuck::setupScreenBackground()
{
	this->screenBackgroundGray.setFillColor(COLOR_SCREEN_GRAY);

	// transparent radial gradient, tinted with hud color
	this->setScreenTint();

	// "scan lines"
	sf::Image img;
	img.create(1, 2, sf::Color::Transparent);
	img.setPixel(0, 0, sf::Color::Black);

	this->screenBackgroundStripesTxt.loadFromImage(img);
	this->screenBackgroundStripesTxt.setRepeated(true);

	this->screenBackgroundStripes.setTexture(this->screenBackgroundStripesTxt);
}

void PipBuck::setScreenBackgroundScale()
{
	this->screenBackgroundGray.setPosition(POS_SCREEN * SettingsManager::guiScale);
	this->screenBackgroundGray.setSize(SIZE_SCREEN * SettingsManager::guiScale);

	this->pipBuckScreenRadialGrad.setPosition(POS_SCREEN * SettingsManager::guiScale);
	this->pipBuckScreenRadialGrad.setScale(SettingsManager::guiScale, SettingsManager::guiScale);

	this->screenBackgroundStripes.setTextureRect(
		sf::IntRect(0, 0, SIZE_SCREEN.x * SettingsManager::guiScale, SIZE_SCREEN.y * SettingsManager::guiScale));
	this->screenBackgroundStripes.setPosition(POS_SCREEN * SettingsManager::guiScale);
}

void PipBuck::handleScreenResize(sf::Vector2u windowSize)
{
	// for now copy the behaviour of Remains pipbuck, i.e. display it unscaled in bottom left corner
	this->setPosition(0, windowSize.y - std::round(this->pipBuckSprite.getGlobalBounds().height));
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
	this->updateCampaignInfos();

	this->handleMouseMove(mousePos);

	this->gameState = STATE_PIPBUCK;
	this->cursorMgr.setCursor(POINTER);

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

	// TODO query campaign and check what the player is actually pointing at and set proper cursor color
	this->cursorMgr.setCursor(CROSSHAIR_WHITE);

	this->soundOpenClose.play();
	Log::d(STR_GAME_RESUMED);
}

void PipBuck::changeActiveCategoryButton(PipBuckCategoryType newCategoryType)
{
	auto foundOld = this->categoryButtons.find(this->selectedCategoryType);
	if (foundOld != this->categoryButtons.end())
	{
		// found previously selected button, deselect it
		foundOld->second->setSelected(false);
	}

	auto foundNew = this->categoryButtons.find(newCategoryType);
	if (foundNew != this->categoryButtons.end())
	{
		// found new button, select it
		foundNew->second->setSelected(true);
	}
}

bool PipBuck::changeCategory(PipBuckCategoryType newCategoryType)
{
	auto foundCat = this->categories.find(newCategoryType);
	if (foundCat == this->categories.end())
		return false;

	this->changeActiveCategoryButton(newCategoryType);

	this->selectedCategoryType = newCategoryType;
	this->selectedCategory = foundCat->second;

	return true;
}

ClickStatus PipBuck::handleLeftClick(sf::Vector2i clickPos)
{
	if (this->selectedCategory == nullptr)
		return CLICK_NOT_CONSUMED;

	clickPos -= this->getPosition();

	ClickStatus status = this->selectedCategory->handleLeftClick(clickPos);
	if (status == CLICK_CONSUMED || status == CLICK_CONSUMED_SETTINGS_CHANGED)
		return status;

	if (status == CLICK_CONSUMED_CLOSE)
	{
		this->close();
		return status;
	}

	if (status == CLICK_CONSUMED_UNLOAD)
	{
		// note: we could unload campaign inside the page (inside btn callback),
		// but then we'd have to clear resmgr two times (unload campaign + clear resmgr,
		// unload pipbuck infos, clear resmgr)
		this->unloadCampaignInfos();
		this->campaign.unload();
		this->gameState = STATE_MAINMENU;
		// no need to set pointer as it's already POINTER in PipBuck
		return status;
	}

	for (auto& btn : this->categoryButtons)
	{
		if (btn.second->containsPoint(clickPos))
		{
			if (btn.first != this->selectedCategoryType)
			{
				this->changeCategory(btn.first);
				this->soundCategoryBtn.play();
			}

			return CLICK_CONSUMED;
		}
	}

	status = this->closeBtn.handleLeftClick(clickPos);
	if (status == CLICK_CONSUMED_CLOSE)
	{
		this->close();
		return CLICK_CONSUMED;
	}

	return status;
}

void PipBuck::handleLeftClickUp()
{
	if (this->selectedCategory != nullptr)
		this->selectedCategory->handleLeftClickUp();
}

void PipBuck::handleScroll(float delta, sf::Vector2i mousePos)
{
	if (this->selectedCategory != nullptr)
	{
		mousePos -= this->getPosition();

		this->selectedCategory->handleScroll(delta, mousePos);
	}
}

void PipBuck::handleMouseMove(sf::Vector2i mousePos)
{
	if (this->selectedCategory == nullptr)
		return;

	mousePos -= this->getPosition();

	if (this->selectedCategory->handleMouseMove(mousePos))
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
	for (auto& cat : this->categories)
	{
		if (!cat.second->setupCampaignInfos())
		{
			this->unloadCampaignInfos();
			this->resMgr.cleanUnused();
			return false;
		}
	}

	return true;
}

/**
 * Adds a log message to the Log PipBuck page.
 */
void PipBuck::addLogMessage(const StringAndColor& strAndColor)
{
	auto foundCat = this->categories.find(PIPB_CAT_GAME);
	if (foundCat == this->categories.end())
		return;

	auto* logPage = foundCat->second->getPage<GuiPageLog>(PIPB_PAGE_LOG);
	if (logPage == nullptr)
		return;

	logPage->addMsg(strAndColor);
}

void PipBuck::unloadCampaignInfos()
{
	Log::d(STR_PIPBUCK_UNLOADING_CAMPAIGN);

	for (auto& cat : this->categories)
	{
		cat.second->unloadCampaignInfos();
	}
}

/**
 * Updates campaign data displayed in PipBuck. Should be called when PipBuck is opened.
 */
void PipBuck::updateCampaignInfos()
{
	this->setRadLevel(0.3F); // TODO read this (and other things too) from campaign->player
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
	PipBuckCategoryType newCatType = PipBuckCategory::pageTypeToCategoryType(pageType);

	if (newCatType == PIPB_CAT_NO_CAT)
		return false;

	auto newCat = this->categories.find(newCatType);
	if (newCat == this->categories.end())
		return false;

	if (this->gameState == STATE_PIPBUCK)
	{
		if (newCatType == this->selectedCategoryType && newCat->second->getSelectedPageType() == pageType)
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

	return this->changeCategory(newCatType) && newCat->second->changePage(pageType);
}

bool PipBuck::setup()
{
	for (auto& cat : this->categories)
	{
		if (!cat.second->setup())
			return false;
	}

	if (!this->changeCategory(this->selectedCategoryType)) // default category
		return false;

	return true;
}

void PipBuck::setScreenTint()
{
	this->pipBuckScreenRadialGrad.setColor(SettingsManager::hudColor);
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
		static_cast<float>(this->getSmoothNoise(this->timer.getElapsedTime().asSeconds()) / 2));
}

void PipBuck::setGuiScale()
{
	this->pipBuckSprite.setScale(SettingsManager::guiScale, SettingsManager::guiScale);
	this->setRadIndicatorScale();
	this->setScreenBackgroundScale();
}

void PipBuck::handleSettingsChange()
{
	this->setGuiScale();
	this->setAudioVolume();
	this->setScreenTint();

	this->closeBtn.handleSettingsChange();

	for (auto& cat : this->categories)
	{
		cat.second->handleSettingsChange();
	}

	for (auto& btn : this->categoryButtons)
	{
		btn.second->handleSettingsChange();
	}
}

void PipBuck::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->screenBackgroundGray, states);
	target.draw(this->pipBuckScreenRadialGrad, states);
	target.draw(this->screenBackgroundStripes, states);

	target.draw(this->pipBuckSprite, states);
	target.draw(this->radIndicator, states);

	if (this->selectedCategory != nullptr)
		target.draw(*this->selectedCategory, states);

	for (const auto& btn : this->categoryButtons)
	{
		target.draw(*btn.second, states);
	}

	target.draw(this->closeBtn, states);
}
