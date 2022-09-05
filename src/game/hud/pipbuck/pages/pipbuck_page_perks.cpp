#include "pipbuck_page_perks.hpp"

PipBuckPagePerks::PipBuckPagePerks(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("perks");
}

std::string PipBuckPagePerks::getLabel()
{
	return "Perks"; // TODO translate
}

void PipBuckPagePerks::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
