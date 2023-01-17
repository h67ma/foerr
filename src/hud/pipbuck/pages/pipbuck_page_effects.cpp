#include "pipbuck_page_effects.hpp"

#include <string>

PipBuckPageEffects::PipBuckPageEffects(ResourceManager &resMgr) :
	PipBuckPage("Effects") // TODO translate
{
	this->dummy.setFont(*resMgr.getFont(FONT_FIXED));
	this->dummy.setPosition(500.f, 500.f);
	this->dummy.setString("effects");
}

void PipBuckPageEffects::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->dummy, states);
}
