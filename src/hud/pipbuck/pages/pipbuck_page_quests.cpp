#include "pipbuck_page_quests.hpp"
#include <string>

PipBuckPageQuests::PipBuckPageQuests(ResourceManager &resMgr) :
	PipBuckPage("Quests") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("quests");
}

void PipBuckPageQuests::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
