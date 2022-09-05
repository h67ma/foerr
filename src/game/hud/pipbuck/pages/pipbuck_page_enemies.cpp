#include "pipbuck_page_enemies.hpp"

PipBuckPageEnemies::PipBuckPageEnemies(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("enemies");
}

std::string PipBuckPageEnemies::getLabel()
{
	return "Enemies"; // TODO translate
}

void PipBuckPageEnemies::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
