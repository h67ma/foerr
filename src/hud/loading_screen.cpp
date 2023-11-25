// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "loading_screen.hpp"

#include "../settings/settings_manager.hpp"
#include "../util/i18n.hpp"

LoadingScreen::LoadingScreen(ResourceManager& resMgr, sf::Vector2u windowSize) :
	loadingText(STR_LOADING, *resMgr.getFont(FONT_FIXED), FONT_H1, SettingsManager::hudColor)
{
	this->loadingText.setPosition(static_cast<float>((windowSize.x - this->loadingText.getLocalBounds().width) / 2),
								  static_cast<float>((windowSize.y - this->loadingText.getLocalBounds().height) / 2));
}

void LoadingScreen::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(this->loadingText, states);
}
