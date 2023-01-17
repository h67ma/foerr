#include "pipbuck_page_perks.hpp"
#include <string>

PipBuckPagePerks::PipBuckPagePerks(ResourceManager &resMgr) :
	PipBuckPage("Perks") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("perks");
}

void PipBuckPagePerks::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
