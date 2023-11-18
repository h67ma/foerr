// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "main_menu.hpp"

#include "../settings/settings_manager.hpp"
#include "loading_screen.hpp"
#include "../util/i18n.hpp"
#include "git_version.h"
#include "log.hpp"

MainMenu::MainMenu(ResourceManager &resMgr, CursorManager &cursorMgr, sf::RenderWindow &window, Campaign &campaign,
				   GameState &gameState, PipBuck &pipBuck) :
	buttons({
		{ BTN_NORMAL, resMgr, { 100, 100 }, STR_CONTINUE, [&resMgr, &cursorMgr, &campaign, &gameState, &window,
														   &pipBuck](){
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
		}},
		{ BTN_NORMAL, resMgr, { 100, 150 }, STR_QUIT_GAME, [&window](){
			Log::d(STR_SHUTTING_DOWN);
			window.close();
		}}
	}),
	btnSound(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGE_CLICK)),
	versionText(GIT_VERSION, *resMgr.getFont(FONT_FIXED), FONT_H3, SettingsManager::hudColor),
	licenseText(GPL_SPLAT, *resMgr.getFont(FONT_NORMAL), FONT_H3, SettingsManager::hudColor)
{
	this->btnSound.setVolume(static_cast<float>(SettingsManager::fxVolume));

	for (auto &btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->handleScreenResize(window.getSize());
}

void MainMenu::handleLeftClick(sf::Vector2i clickPos)
{
	// don't need to subtract anything as menu position is (0, 0)
	if (this->clickMgr.handleLeftClick(clickPos) == CLICK_CONSUMED)
		this->btnSound.play();
}

void MainMenu::handleMouseMove(sf::Vector2i mousePos)
{
	// don't need to subtract anything as menu position is (0, 0)
	this->hoverMgr.handleMouseMove(mousePos);
}

void MainMenu::handleScreenResize(sf::Vector2u newSize)
{
	this->versionText.setPosition(
		newSize.x - 5 - this->versionText.getLocalBounds().width,
		newSize.y - 10 - this->versionText.getLocalBounds().height
	);

	this->licenseText.setPosition(
		5,
		newSize.y - 10 - this->licenseText.getLocalBounds().height
	);
}

void MainMenu::handleSettingsChange()
{
	this->versionText.handleSettingsChange();
	this->licenseText.handleSettingsChange();

	for (auto &btn : this->buttons)
	{
		btn.handleSettingsChange();
	}
}

void MainMenu::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->versionText, states);
	target.draw(this->licenseText, states);
}
