#include "pipbuck_page_map.hpp"
#include <string>

PipBuckPageMap::PipBuckPageMap(ResourceManager &resMgr) :
	PipBuckPage("Map") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("map");

	// TODO map should display the current layer, not only front/main layer. if player enters backstage, map should also
	// change to backstage. same for minimap.

	// TODO map should allow the player to browse other layers (fore/back), but opening pipbuck should reset display
	// to current layer
}

void PipBuckPageMap::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
