#include "pipbuck_page_status_main.hpp"
#include <string>

PipBuckPageMainStatus::PipBuckPageMainStatus(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("main");
}

std::string PipBuckPageMainStatus::getLabel()
{
	return "Main"; // TODO translate
}

void PipBuckPageMainStatus::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
