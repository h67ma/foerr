// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "main_menu.hpp"

#include "../../settings/settings_manager.hpp"
#include "../../util/i18n.hpp"
#include "../loading_screen.hpp"
#include "../log.hpp"
#include "../pipbuck/pages/pipbuck_page_controls.hpp"
#include "../pipbuck/pages/pipbuck_page_load.hpp"
#include "../pipbuck/pages/pipbuck_page_settings.hpp"
#include "git_version.h"

static const sf::Vector2f PAGE_OFFSET = { 0.F, -200.F };

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
				  { 100, 100 },
				  STR_CONTINUE,
				  [&resMgr, &cursorMgr, &campaign, &gameState, &window, &pipBuck]()
				  { menuItemContinue(resMgr, cursorMgr, window, campaign, gameState, pipBuck); } } },
			  { MAINM_PAGE_LOAD, { BTN_NORMAL, resMgr, { 100, 150 } } },
			  { MAINM_PAGE_SETTINGS, { BTN_NORMAL, resMgr, { 100, 200 } } },
			  { MAINM_PAGE_CONTROLS, { BTN_NORMAL, resMgr, { 100, 250 } } },
			  { MAINM_PAGE_QUIT,
				{ BTN_NORMAL, resMgr, { 100, 300 }, STR_QUIT_GAME, [&window]() { menuItemQuit(window); } } } }),
	pages({ { MAINM_PAGE_LOAD, std::make_shared<PipBuckPageLoad>(resMgr, true) },
			{ MAINM_PAGE_SETTINGS, std::make_shared<PipBuckPageSettings>(resMgr) },
			{ MAINM_PAGE_CONTROLS, std::make_shared<PipBuckPageControls>(resMgr) } }),
	btnSound(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGE_CLICK)),
	versionText(GIT_VERSION, *resMgr.getFont(FONT_FIXED), FONT_H3, SettingsManager::hudColor),
	licenseText(GPL_SPLAT, *resMgr.getFont(FONT_NORMAL), FONT_H3, SettingsManager::hudColor)
{
	this->btnSound.setVolume(static_cast<float>(SettingsManager::fxVolume));

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
		// found new button, select it
		foundNew->second.setSelected(true);
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
}

void MainMenu::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto& btn : this->buttons)
	{
		target.draw(btn.second, states);
	}

	if (this->selectedPage != nullptr)
		target.draw(*this->selectedPage, states);

	target.draw(this->versionText, states);
	target.draw(this->licenseText, states);
}
