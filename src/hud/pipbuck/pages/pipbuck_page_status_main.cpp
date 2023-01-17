#include "pipbuck_page_status_main.hpp"

#include <string>

PipBuckPageMainStatus::PipBuckPageMainStatus(ResourceManager &resMgr) :
	PipBuckPage("Main") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("main");
}

void PipBuckPageMainStatus::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
