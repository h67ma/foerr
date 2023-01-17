#include "pipbuck_page_notes.hpp"

#include <string>

PipBuckPageNotes::PipBuckPageNotes(ResourceManager &resMgr) :
	PipBuckPage("Notes") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("notes");
}

void PipBuckPageNotes::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
