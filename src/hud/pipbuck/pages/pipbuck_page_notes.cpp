#include "pipbuck_page_notes.hpp"

PipBuckPageNotes::PipBuckPageNotes(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("notes");
}

std::string PipBuckPageNotes::getLabel()
{
	return "Notes"; // TODO translate
}

void PipBuckPageNotes::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
