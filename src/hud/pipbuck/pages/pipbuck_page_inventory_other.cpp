#include "pipbuck_page_inventory_other.hpp"

#include <string>

PipBuckPageInventoryOther::PipBuckPageInventoryOther(ResourceManager &resMgr) :
	PipBuckPage("Other") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.F, 500.F);
	this->dummy.setString("inv other");
}

void PipBuckPageInventoryOther::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
