#include "pipbuck_page_load.hpp"
#include "../../../util/i18n.hpp"

PipBuckPageLoad::PipBuckPageLoad(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign, GameState &gameState) :
	buttons({
		{scale, BTN_NORMAL, hudColor, resMgr, 400, 810, STR_EXIT_TO_MAIN_MENU, [&campaign, &gameState, &resMgr]() {
			// TODO display confirm box
			// TODO also save game before unloading campaign
			campaign.unload(resMgr);
			gameState = STATE_MAINMENU;
		}}
	})
{
	for (auto &btn : this->buttons)
	{
		this->hoverMgr.addHoverable(&btn);
	}
}

ClickStatus PipBuckPageLoad::handleLeftClick(int x, int y)
{
	for (auto &btn : this->buttons)
	{
		if (btn.handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
			return CLICK_CONSUMED;
	}

	return CLICK_NOT_CONSUMED;
}

std::string PipBuckPageLoad::getLabel()
{
	return "Load"; // TODO translate
}

void PipBuckPageLoad::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}
}
