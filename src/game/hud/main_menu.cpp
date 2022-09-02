#include "main_menu.hpp"
#include "../util/i18n.hpp"
#include "log.hpp"


MainMenu::MainMenu(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, sf::RenderWindow &window, Campaign &campaign, GameState &gameState) :
	buttons({
		Button(scale, BTN_NORMAL, hudColor, resMgr, 100, 100, STR_CONTINUE, [&resMgr, &campaign, &gameState](){
			// TODO some kind of campaign select
			// TODO load on thread, display loading screen
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

void MainMenu::handleMouseMove(int x, int y)
{
	// need to account for this component's position
	this->hoverMgr.handleMouseMove(x - static_cast<int>(this->getPosition().x),
								   y - static_cast<int>(this->getPosition().y));
}

/**
 * Handles left mouse click event.
 *
 * @param x x click coordinate
 * @param y y click coordinate
 * @return true if game should enter loading state
 * @return false if main menu should remain open
 */
void MainMenu::handleLeftClick(int x, int y)
{
	// need to account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	for (auto &btn : this->buttons)
	{
		if (btn.maybeHandleLeftClick(x, y))
			return;
	}
}

void MainMenu::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}
}
