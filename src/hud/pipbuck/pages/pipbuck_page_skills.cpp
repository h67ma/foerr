#include "pipbuck_page_skills.hpp"

#include <string>

PipBuckPageSkills::PipBuckPageSkills(ResourceManager &resMgr) :
	PipBuckPage("Skills") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.F, 500.F);
	this->dummy.setString("skills");
}

void PipBuckPageSkills::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
