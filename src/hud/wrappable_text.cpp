// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#include "wrappable_text.hpp"

#include "../settings/settings_manager.hpp"

WrappableText::WrappableText(const sf::Font& font, uint fontSize, const sf::Vector2f& position) : text(font, fontSize)
{
	this->setPosition(position);
}

/**
 * Modifies input string by changing some spaces to newlines to prevent text overflow.
 * Based on https://gist.github.com/andrew-d-jackson/7858095
 */
void WrappableText::setStringWrap()
{
	if (maxWidth == TEXT_NO_WRAP)
		return;

	uint fontSize = this->text.getCharacterSize();
	const sf::Font* font = this->text.getFont();
	bool bold = this->text.getStyle() == sf::Text::Bold;
	uint currentOffset = 0;
	bool firstWord = true;
	uint wordStart = 0;

	for (uint i = 0; i < this->textValue.length(); i++)
	{
		char currentChar = this->textValue[i];
		if (currentChar == '\n')
		{
			currentOffset = 0;
			firstWord = true;
			continue;
		}

		if (currentChar == ' ')
		{
			wordStart = i;
			firstWord = false;
		}

		currentOffset += font->getGlyph(currentChar, fontSize, bold).advance;

		// the word that we're inside of is too long - replace space before it with newline
		if (!firstWord && (currentOffset > (maxWidth * SettingsManager::guiScale)))
		{
			i = wordStart;
			this->textValue[i] = '\n';
			firstWord = true;
			currentOffset = 0;
		}
	}

	this->text.setString(this->textValue);
}

void WrappableText::setString(const std::string& newText, uint maxWidth)
{
	this->maxWidth = maxWidth;
	this->textValue = newText;
	this->setStringWrap();
}

void WrappableText::setString(const std::string& newText)
{
	this->textValue = newText;
	this->text.setString(newText);
}

void WrappableText::setFillColor(const sf::Color& color)
{
	this->text.setFillColor(color);
}

void WrappableText::handleSettingsChange()
{
	this->text.handleSettingsChange();
	this->handleGuiScaleChange();
	this->setStringWrap();
}

void WrappableText::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->text, states);
}
