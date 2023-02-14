#include "pipbuck_page_save.hpp"

#include <string>

PipBuckPageSave::PipBuckPageSave(ResourceManager &resMgr) :
	PipBuckPage("Save") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.F, 500.F);
	this->dummy.setString("save");
}

void PipBuckPageSave::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
