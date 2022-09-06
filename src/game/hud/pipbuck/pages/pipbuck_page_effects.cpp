#include "pipbuck_page_effects.hpp"

PipBuckPageEffects::PipBuckPageEffects(ResourceManager &resMgr)
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("effects");
}

std::string PipBuckPageEffects::getLabel()
{
	return "Effects"; // TODO translate
}

void PipBuckPageEffects::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}