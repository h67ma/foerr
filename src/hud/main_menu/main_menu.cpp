// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "main_menu.hpp"

#include "../../settings/settings_manager.hpp"
#include "../../util/i18n.hpp"
#include "../hud.hpp"
#include "../loading_screen.hpp"
#include "../log.hpp"
#include "../pages/gui_page_controls.hpp"
#include "../pages/gui_page_load.hpp"
#include "../pages/gui_page_settings.hpp"
#include "git_version.h"

static const sf::Vector2f PAGE_OFFSET = { -70.F, -175.F };
static const sf::Vector2f PAGE_POS(300, 50);
static const sf::Vector2f PAGE_SIZE(1100, 650);
constexpr std::size_t PAGE_VERT_COUNT = 9;
constexpr float ITEM_FRAME_START_X = 87.F;
constexpr float ITEM_FRAME_OFFSET_TOP = -13.F;
constexpr float ITEM_FRAME_OFFSET_BOTTOM = 39.F;

static void menuItemContinue(ResourceManager& resMgr, CursorManager& cursorMgr, sf::RenderWindow& window,
							 Campaign& campaign, GameState& gameState, PipBuck& pipBuck)
{
	// TODO some kind of campaign select

	// this is a pretty terrible way of showing a loading screen, but it will do for now
	// TODO load on thread, display loading screen in main loop with a progress bar
	LoadingScreen loadingScreen(resMgr, window.getSize());
	window.clear();
	window.draw(loadingScreen);
	window.display();

	if (!campaign.load("res/campaigns/remains"))
	{
		Log::e(STR_CAMPAIGN_LOAD_FAILED);
		return;
	}

	if (!pipBuck.setupCampaignInfos())
	{
		Log::e(STR_PIPBUCK_SETUP_FAILED);
		return;
	}

	gameState = STATE_PLAYING;

	// TODO query campaign to check what the player is actually pointing at and set proper cursor color
	cursorMgr.setCursor(CROSSHAIR_WHITE);
}

static void menuItemQuit(sf::RenderWindow& window)
{
	Log::d(STR_SHUTTING_DOWN);
	window.close();
}

MainMenu::MainMenu(ResourceManager& resMgr, CursorManager& cursorMgr, sf::RenderWindow& window, Campaign& campaign,
				   GameState& gameState, PipBuck& pipBuck) :
	buttons({ { MAINM_PAGE_CONTINUE,
				{ BTN_NORMAL,
				  resMgr,
				  { 100, 50 },
				  STR_CONTINUE,
				  [&resMgr, &cursorMgr, &campaign, &gameState, &window, &pipBuck]()
				  { menuItemContinue(resMgr, cursorMgr, window, campaign, gameState, pipBuck); } } },
			  { MAINM_PAGE_LOAD, { BTN_NORMAL, resMgr, { 100, 100 } } },
			  { MAINM_PAGE_SETTINGS, { BTN_NORMAL, resMgr, { 100, 150 } } },
			  { MAINM_PAGE_CONTROLS, { BTN_NORMAL, resMgr, { 100, 200 } } },
			  { MAINM_PAGE_QUIT,
				{ BTN_NORMAL, resMgr, { 100, 250 }, STR_QUIT_GAME, [&window]() { menuItemQuit(window); } } } }),
	pages({ { MAINM_PAGE_LOAD, std::make_shared<GuiPageLoad>(resMgr, true) },
			{ MAINM_PAGE_SETTINGS, std::make_shared<GuiPageSettings>(resMgr) },
			{ MAINM_PAGE_CONTROLS, std::make_shared<GuiPageControls>(resMgr) } }),
	btnSound(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGE_CLICK)),
	versionText(GIT_VERSION, *resMgr.getFont(FONT_FIXED), FONT_H3, SettingsManager::hudColor),
	licenseText(GPL_SPLAT, *resMgr.getFont(FONT_NORMAL), FONT_H3, SettingsManager::hudColor),
	pageFrame(sf::LineStrip, PAGE_VERT_COUNT)
{
	this->btnSound.setVolume(static_cast<float>(SettingsManager::fxVolume));
	this->setupPageFrameStatic();

	for (auto& btn : this->buttons)
	{
		this->hoverMgr += &btn.second;
	}

	// set button labels (only those that have an associated page)
	for (const auto& page : this->pages)
	{
		this->buttons.at(page.first).setText(page.second->pageTitle);
		page.second->setPosition(PAGE_OFFSET);
	}

	this->handleScreenResize(window.getSize());
}

/**
 * Changes currently highlighted button, only if it matches currently selected page.
 * Previously highlighted button is de-highlighted.
 */
void MainMenu::changeActiveButton(MainMenuPageType newPageType)
{
	auto foundOld = this->buttons.find(this->selectedPageType);
	if (foundOld != this->buttons.end())
	{
		// found previously selected button, deselect it
		foundOld->second.setSelected(false);
	}

	auto foundNew = this->buttons.find(newPageType);
	if (foundNew != this->buttons.end())
	{
		// found new button, select it and calculate new positions for selected item frame
		foundNew->second.setSelected(true);
		this->setupPageFrameSelectedItem(foundNew->second.getPosition().y);
	}
}

/**
 * Changes selected page. If no such page exists, selected page will be unset.
 */
void MainMenu::changePage(MainMenuPageType newPageType)
{
	auto foundPage = this->pages.find(newPageType);
	if (foundPage != this->pages.end())
	{
		// target page found, switch to it
		this->changeActiveButton(newPageType);
		this->selectedPageType = newPageType;
		this->selectedPage = foundPage->second;
		return;
	}

	// target page not found, unset page and deselect button
	this->changeActiveButton(MAINM_PAGE_NONE);
	this->selectedPageType = MAINM_PAGE_NONE;
	this->selectedPage = nullptr;
}

/**
 * Calculates points for the page frame that don't change position when changing page.
 * Concept art of what we want to draw (with vertex numbers):
 * 0--1
 *    |
 *    |
 *    |
 * 3--2
 */
void MainMenu::setupPageFrameStatic()
{
	// point names refer to the bigger rectangle of the shape

	sf::Vector2f topLeft = calculateGuiAwarePoint(PAGE_POS);
	sf::Vector2f topRight = calculateGuiAwarePoint({ PAGE_POS.x + PAGE_SIZE.x, PAGE_POS.y });
	sf::Vector2f bottomRight = calculateGuiAwarePoint({ PAGE_POS.x + PAGE_SIZE.x, PAGE_POS.y + PAGE_SIZE.y });
	sf::Vector2f bottomLeft = calculateGuiAwarePoint({ PAGE_POS.x, PAGE_POS.y + PAGE_SIZE.y });

	this->pageFrame[0] = sf::Vertex(topLeft, SettingsManager::hudColor);
	this->pageFrame[1] = sf::Vertex(topRight, SettingsManager::hudColor);
	this->pageFrame[2] = sf::Vertex(bottomRight, SettingsManager::hudColor);
	this->pageFrame[3] = sf::Vertex(bottomLeft, SettingsManager::hudColor);
	this->pageFrame[8] = this->pageFrame[0];
}

/**
 * Calculates points for the page frame that depend on selected item position.
 * Concept art of what we want to draw (with vertex numbers, in brackets are already set up):
 *   (8)
 *    |
 * 6--7
 * |
 * 5--4
 *    |
 *   (3)
 *
 * @param selectedItemOffset y position of currently selected menu item (button)
 */
void MainMenu::setupPageFrameSelectedItem(float selectedItemOffset)
{
	float offTop = calculateGuiAwareScalar(ITEM_FRAME_OFFSET_TOP);
	float offBottom = calculateGuiAwareScalar(ITEM_FRAME_OFFSET_BOTTOM);

	// point names refer to the smaller rectangle of the shape

	// selectedItemOffset is already adjusted to GUI scale, don't multiply it again
	sf::Vector2f topLeft(calculateGuiAwareScalar(ITEM_FRAME_START_X), selectedItemOffset + offTop);
	sf::Vector2f topRight(calculateGuiAwareScalar(PAGE_POS.x), selectedItemOffset + offTop);
	sf::Vector2f bottomRight(calculateGuiAwareScalar(PAGE_POS.x), selectedItemOffset + offBottom);
	sf::Vector2f bottomLeft(calculateGuiAwareScalar(ITEM_FRAME_START_X), selectedItemOffset + offBottom);

	this->pageFrame[4] = sf::Vertex(bottomRight, SettingsManager::hudColor);
	this->pageFrame[5] = sf::Vertex(bottomLeft, SettingsManager::hudColor);
	this->pageFrame[6] = sf::Vertex(topLeft, SettingsManager::hudColor);
	this->pageFrame[7] = sf::Vertex(topRight, SettingsManager::hudColor);
}

ClickStatus MainMenu::handleLeftClick(sf::Vector2i clickPos)
{
	// don't need to subtract anything as menu position is (0, 0)

	if (this->selectedPage != nullptr)
	{
		ClickStatus status = this->selectedPage->handleLeftClick(clickPos);
		if (status != CLICK_NOT_CONSUMED)
		{
			this->btnSound.play();
			return status;
		}
	}

	for (auto& btn : this->buttons)
	{
		if (btn.second.handleLeftClick(clickPos) == CLICK_CONSUMED)
		{
			// if the button has a callback defined, it will fire automatically in its ::handleLeftClick().
			// if it has an associated page, it will open.
			// both can even happen.

			// if there's no associated page, ::changePage() will automatically deselect page, so e.g. when player
			// selects "Settings" page, then clicks "Continue", plays the game a bit, and comes back to main menu, it
			// will visually be in its initial state (no page selected), which is exactly what we want to happen.
			// TODO? probably there will be other cases of this, where game state change initiated from inside a page
			// ("Load" page). we'd need to somehow deselect page then.
			this->changePage(btn.first);

			this->btnSound.play();
			return CLICK_CONSUMED;
		}
	}

	return CLICK_NOT_CONSUMED;
}

void MainMenu::handleLeftClickUp()
{
	if (this->selectedPage != nullptr)
		this->selectedPage->handleLeftClickUp();
}

void MainMenu::handleMouseMove(sf::Vector2i mousePos)
{
	// don't need to subtract anything as menu position is (0, 0)

	if (this->selectedPage != nullptr && this->selectedPage->handleMouseMove(mousePos))
		return;

	this->hoverMgr.handleMouseMove(mousePos);
}

void MainMenu::handleScreenResize(sf::Vector2u newSize)
{
	this->versionText.setPosition(newSize.x - 5 - this->versionText.getLocalBounds().width,
								  newSize.y - 10 - this->versionText.getLocalBounds().height);

	this->licenseText.setPosition(5, newSize.y - 10 - this->licenseText.getLocalBounds().height);
}

void MainMenu::handleSettingsChange()
{
	this->versionText.handleSettingsChange();
	this->versionText.setFillColor(SettingsManager::hudColor);
	this->licenseText.handleSettingsChange();
	this->licenseText.setFillColor(SettingsManager::hudColor);

	for (auto& btn : this->buttons)
	{
		btn.second.handleSettingsChange();
	}

	for (const auto& page : this->pages)
	{
		page.second->handleSettingsChange();
	}

	this->setupPageFrameStatic();

	auto foundActiveBtn = this->buttons.find(this->selectedPageType);
	if (foundActiveBtn != this->buttons.end())
		this->setupPageFrameSelectedItem(foundActiveBtn->second.getPosition().y);
}

void MainMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto& btn : this->buttons)
	{
		target.draw(btn.second, states);
	}

	if (this->selectedPage != nullptr)
	{
		target.draw(*this->selectedPage, states);
		target.draw(this->pageFrame, states);
	}

	target.draw(this->versionText, states);
	target.draw(this->licenseText, states);
}
