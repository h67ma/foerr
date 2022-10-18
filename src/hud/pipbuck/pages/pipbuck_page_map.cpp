#include "pipbuck_page_map.hpp"

PipBuckPageMap::PipBuckPageMap(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("map");

	// TODO if player enters the backstage, display on map should also change to backrooms. same for minimap
}

std::string PipBuckPageMap::getLabel()
{
	return "Map"; // TODO translate
}

void PipBuckPageMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
