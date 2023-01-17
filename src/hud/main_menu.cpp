#include "main_menu.hpp"

#include "../settings/settings_manager.hpp"
#include "loading_screen.hpp"
#include "../util/i18n.hpp"
#include "git_version.h"
#include "log.hpp"

MainMenu::MainMenu(ResourceManager &resMgr, sf::RenderWindow &window, Campaign &campaign,
				   GameState &gameState, PipBuck &pipBuck) :
	buttons({
		{ BTN_NORMAL, resMgr, { 100, 100 }, STR_CONTINUE, [&resMgr, &campaign, &gameState, &window,
														   &pipBuck](){
			// TODO some kind of campaign select

			// this is a pretty terrible way of showing a loading screen, but it will do for now
			// TODO load on thread, display loading screen in main loop with a progress bar
			LoadingScreen loadingScreen(resMgr, window.getSize());
			window.clear();
			window.draw(loadingScreen);
			window.display();

			if (!campaign.load("res/campaigns/test"))
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
		}},
		{ BTN_NORMAL, resMgr, { 100, 150 }, STR_QUIT_GAME, [&window](){
			Log::d(STR_SHUTTING_DOWN);
			window.close();
		}}
	}),
	btnSound(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGE_CLICK))
{
	this->btnSound.setVolume(static_cast<float>(SettingsManager::fxVolume));

	for (auto &btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->versionText.setFont(*resMgr.getFont(FONT_FIXED));
	this->versionText.setFillColor(SettingsManager::hudColor);
	this->versionText.setString(GIT_VERSION);
	this->versionText.setCharacterSize(getFontSize(SettingsManager::guiScale, FONT_H3));
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
}

void MainMenu::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->versionText, states);
}
