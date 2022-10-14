#include "pipbuck_page_health.hpp"

PipBuckPageHealth::PipBuckPageHealth(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("health");
}

std::string PipBuckPageHealth::getLabel()
{
	return "Health"; // TODO translate
}

void PipBuckPageHealth::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
