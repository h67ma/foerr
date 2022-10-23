#include "pipbuck_page_skills.hpp"
#include <string>

PipBuckPageSkills::PipBuckPageSkills(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("skills");
}

std::string PipBuckPageSkills::getLabel()
{
	return "Skills"; // TODO translate
}

void PipBuckPageSkills::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
