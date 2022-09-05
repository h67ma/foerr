#include "pipbuck_page_save.hpp"

PipBuckPageSave::PipBuckPageSave(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("save");
}

std::string PipBuckPageSave::getLabel()
{
	return "Save"; // TODO translate
}

void PipBuckPageSave::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
