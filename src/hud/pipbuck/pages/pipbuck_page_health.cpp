#include "pipbuck_page_health.hpp"

#include <string>

PipBuckPageHealth::PipBuckPageHealth(ResourceManager &resMgr) :
	PipBuckPage("Health") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.F, 500.F);
	this->dummy.setString("health");
}

void PipBuckPageHealth::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
