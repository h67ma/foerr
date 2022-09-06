#include "loading_screen.hpp"
#include "../util/i18n.hpp"

#define LOADINGSCR_FONT_SMALL 30U
#define LOADINGSCR_FONT_NORMAL 40U
#define LOADINGSCR_FONT_LARGE 52U

LoadingScreen::LoadingScreen(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, uint screenW, uint screenH)
{
	this->loadingText.setFont(*resMgr.getFont(FONT_FIXED));
	this->loadingText.setFillColor(hudColor);
	this->loadingText.setString(STR_LOADING);

	switch (scale)
	{
		case GUI_SMALL:
			this->loadingText.setCharacterSize(LOADINGSCR_FONT_SMALL);
			break;
		case GUI_LARGE:
			this->loadingText.setCharacterSize(LOADINGSCR_FONT_LARGE);
			break;
		case GUI_NORMAL:
		default:
			this->loadingText.setCharacterSize(LOADINGSCR_FONT_NORMAL);
	}

	this->loadingText.setPosition(
		static_cast<float>((screenW - this->loadingText.getLocalBounds().width) / 2),
		static_cast<float>((screenH - this->loadingText.getLocalBounds().height) / 2)
	);
}

void LoadingScreen::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->loadingText, states);
}
