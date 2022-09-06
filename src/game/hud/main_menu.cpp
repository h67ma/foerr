#include "main_menu.hpp"
#include "../util/i18n.hpp"
#include "log.hpp"
#include "loading_screen.hpp"


MainMenu::MainMenu(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, sf::RenderWindow &window, Campaign &campaign, GameState &gameState) :
	buttons({
		Button(scale, BTN_NORMAL, hudColor, resMgr, 100, 100, STR_CONTINUE, [scale, hudColor, &resMgr, &campaign, &gameState, &window](){
			// TODO some kind of campaign select

			// this is a pretty terrible way of showing a loading screen, but it will do for now
			// TODO load on thread, display loading screen in main loop with a progress bar
			LoadingScreen loadingScreen(scale, hudColor, resMgr, window.getSize().x, window.getSize().y);
			window.clear();
			window.draw(loadingScreen);
			window.display();

			campaign.load("res/campaigns/test", resMgr);

			gameState = STATE_PLAYING;
		}),
		Button(scale, BTN_NORMAL, hudColor, resMgr, 100, 150, STR_QUIT_GAME, [&window](){
			Log::d(STR_SHUTTING_DOWN);
			window.close();
		})
	})
{
	for (auto &btn : this->buttons)
	{
		this->hoverMgr.addHoverable(&btn);
	}
}

void MainMenu::handleLeftClick(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	for (auto &btn : this->buttons)
	{
		if (btn.handleLeftClick(x, y))
			return;
	}
}

void MainMenu::handleMouseMove(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	this->hoverMgr.handleMouseMove(x, y);
}

void MainMenu::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}
}
