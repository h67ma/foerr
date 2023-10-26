#include "text_label.hpp"

#include <string>

#include "../settings/settings_manager.hpp"

TextLabel::TextLabel(const std::string &text, const sf::Font &font, uint fontSize, const sf::Color &color) :
	fontSize(fontSize)
{
	this->setString(text);
	this->setFont(font);
	this->setFillColor(color);

	this->handleSettingsChange();
}

void TextLabel::handleSettingsChange()
{
	this->setCharacterSize(static_cast<uint>(SettingsManager::guiScale * this->fontSize));
}
