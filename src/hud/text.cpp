#include "text.hpp"

/**
 * Modifies input string by changing some spaces to newlines to prevent text overflow.
 * Based on https://gist.github.com/andrew-d-jackson/7858095
 */
void Text::setString(std::string newText, uint maxWidth)
{
	uint fontSize = this->text.getCharacterSize();
	const sf::Font *font = this->text.getFont();
	bool bold = this->text.getStyle() == sf::Text::Bold;
	uint currentOffset = 0;
	bool firstWord = true;
	uint wordStart = 0;

	for (uint i = 0; i < newText.length(); i++)
	{
		char currentChar = newText[i];
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
		if (!firstWord && currentOffset > maxWidth)
		{
			i = wordStart;
			newText[i] = '\n';
			firstWord = true;
			currentOffset = 0;
		}
	}

	this->text.setString(newText);
}

void Text::setString(const std::string &newText)
{
	this->text.setString(newText);
}

void Text::setFont(const sf::Font& font)
{
	this->text.setFont(font);
}

void Text::setFillColor(const sf::Color& color)
{
	this->text.setFillColor(color);
}

void Text::setCharacterSize(unsigned int size)
{
	this->text.setCharacterSize(size);
}

void Text::handleSettingsChange()
{
	this->handleGuiScaleChange();
}

void Text::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->text, states);
}