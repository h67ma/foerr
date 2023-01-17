#include "pipbuck_page_enemies.hpp"
#include <string>

PipBuckPageEnemies::PipBuckPageEnemies(ResourceManager &resMgr) :
	PipBuckPage("Enemies") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("enemies");
}

void PipBuckPageEnemies::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
