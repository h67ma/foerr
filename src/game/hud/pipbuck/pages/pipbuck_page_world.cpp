#include "pipbuck_page_world.hpp"

PipBuckPageWorld::PipBuckPageWorld(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("world");
}

std::string PipBuckPageWorld::getLabel()
{
	return "World"; // TODO translate
}

void PipBuckPageWorld::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
