#include "loading_screen.hpp"
#include "../util/i18n.hpp"
#include "../util/util.hpp"

LoadingScreen::LoadingScreen(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, sf::Vector2u windowSize)
{
	this->loadingText.setFont(*resMgr.getFont(FONT_FIXED));
	this->loadingText.setFillColor(hudColor);
	this->loadingText.setString(STR_LOADING);

	this->loadingText.setCharacterSize(getFontSize(scale, FONT_H1));

	this->loadingText.setPosition(
		static_cast<float>((windowSize.x - this->loadingText.getLocalBounds().width) / 2),
		static_cast<float>((windowSize.y - this->loadingText.getLocalBounds().height) / 2)
	);
}

void LoadingScreen::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->loadingText, states);
}
