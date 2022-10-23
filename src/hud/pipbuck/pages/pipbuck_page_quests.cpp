#include "pipbuck_page_quests.hpp"
#include <string>

PipBuckPageQuests::PipBuckPageQuests(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("quests");
}

std::string PipBuckPageQuests::getLabel()
{
	return "Quests"; // TODO translate
}

void PipBuckPageQuests::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
