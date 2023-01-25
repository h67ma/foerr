#pragma once

#include <string>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Text.hpp>

#include "hud.hpp"

/**
 * A single line text input field with a cursor.
 * Supports backspace and delete and moving the cursor to select where next character will be placed.
 * Only supports fixed-width fonts.
 */
class TextInput : public sf::Drawable, public sf::Transformable
{
	private:
		sf::Text text;
		std::string currentInput;
		sf::RectangleShape box;
		sf::RectangleShape cursor;
		enum FontSize fontSize;
		uint width;
		uint characterWidth;
		uint cursorIdx = 0; // 0 means the cursor is before the first character
		void updateCursorPosition();
		void moveCursorLeft();
		void moveCursorRight();
		void setSize();

	public:
		TextInput(enum FontSize fontSize, uint width, const sf::Font &font);
		std::string getCurrentInput() const;
		uint getHeight() const;
		void putCharacter(char character);
		void clearInput();
		void handleKeyPress(sf::Keyboard::Key key);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
