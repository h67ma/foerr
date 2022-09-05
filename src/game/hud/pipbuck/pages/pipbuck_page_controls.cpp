#include "pipbuck_page_controls.hpp"

PipBuckPageControls::PipBuckPageControls(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("controls");
}

std::string PipBuckPageControls::getLabel()
{
	return "Controls"; // TODO translate
}

void PipBuckPageControls::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
