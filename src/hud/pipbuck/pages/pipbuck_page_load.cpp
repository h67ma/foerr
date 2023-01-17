#include "pipbuck_page_load.hpp"
#include <string>
#include "../../../util/i18n.hpp"

PipBuckPageLoad::PipBuckPageLoad(ResourceManager &resMgr) :
	PipBuckPage("Load"), // TODO translate
	exitBtn(BTN_NORMAL, resMgr, { 400, 815 }, STR_EXIT_TO_MAIN_MENU, []() {
		// TODO display confirm box
		// TODO also save game before unloading campaign
	}, CLICK_CONSUMED_UNLOAD)
{
	this->hoverMgr += &this->exitBtn;
	this->clickMgr += &this->exitBtn;
}

void PipBuckPageLoad::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->exitBtn, states);
}
