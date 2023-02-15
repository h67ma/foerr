#include "wrappable_text.hpp"

/**
 * Modifies input string by changing some spaces to newlines to prevent text overflow.
 * Based on https://gist.github.com/andrew-d-jackson/7858095
 */
void WrappableText::setString(std::string text, uint maxWidth)
{
	uint fontSize = this->getCharacterSize();
	const sf::Font *font = this->getFont();
	bool bold = this->getStyle() == Bold;
	uint currentOffset = 0;
	bool firstWord = true;
	uint wordStart = 0;

	for (uint i = 0; i < text.length(); i++)
	{
		char currentChar = text[i];
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
			text[i] = '\n';
			firstWord = true;
			currentOffset = 0;
		}
	}

	sf::Text::setString(text);
}
