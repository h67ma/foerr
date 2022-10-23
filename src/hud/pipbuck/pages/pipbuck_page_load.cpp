#include "pipbuck_page_load.hpp"
#include <string>
#include "../../../util/i18n.hpp"

PipBuckPageLoad::PipBuckPageLoad(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign, GameState &gameState) :
	exitBtn(scale, BTN_NORMAL, hudColor, resMgr, { 400, 815 }, STR_EXIT_TO_MAIN_MENU, [&campaign, &gameState, &resMgr]() {
		// TODO display confirm box
		// TODO also save game before unloading campaign
	}, CLICK_CONSUMED_UNLOAD)
{
	this->hoverMgr += &this->exitBtn;
	this->clickMgr += &this->exitBtn;
}

std::string PipBuckPageLoad::getLabel()
{
	return "Load"; // TODO translate
}

void PipBuckPageLoad::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->exitBtn, states);
}
