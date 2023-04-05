#include "loading_screen.hpp"

#include "../settings/settings_manager.hpp"
#include "../util/i18n.hpp"

LoadingScreen::LoadingScreen(ResourceManager &resMgr, sf::Vector2u windowSize)
{
	this->loadingText.setFont(*resMgr.getFont(FONT_FIXED));
	this->loadingText.setFillColor(SettingsManager::hudColor);
	this->loadingText.setString(STR_LOADING);

	this->loadingText.setCharacterSize(static_cast<uint>(SettingsManager::guiScale * FONT_H1));

	this->loadingText.setPosition(
		static_cast<float>((windowSize.x - this->loadingText.getLocalBounds().width) / 2),
		static_cast<float>((windowSize.y - this->loadingText.getLocalBounds().height) / 2)
	);
}

void LoadingScreen::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->loadingText, states);
}
