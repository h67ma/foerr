// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Window/Keyboard.hpp>

#include "configurable_gui_component.hpp"
#include "text_label.hpp"

/**
 * A single line text input field with a cursor.
 * Supports backspace and delete and moving the cursor to select where next character will be placed.
 * Only supports fixed-width fonts.
 */
class TextInput : public sf::Drawable, public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		TextLabel text;
		std::string currentInput;
		sf::RectangleShape box;
		sf::RectangleShape cursor;
		const uint fontSize;
		const uint maxCharacters;
		const uint width;
		sf::Vector2f cursorStartPosition;
		uint characterWidth;
		uint fontAdvance;
		uint cursorIdx = 0; // 0 means the cursor is before the first character
		void updateCursorPosition();
		void moveCursorLeft();
		void moveCursorRight();
		void setSize();

	public:
		TextInput(uint fontSize, uint width, const sf::Font& font, uint maxCharacters);
		std::string getCurrentInput() const;
		uint getHeight() const;
		bool handleTextEntered(uint keycode);
		void setInput(const std::string& input);
		void clearInput();
		void handleKeyPress(sf::Keyboard::Key key);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
