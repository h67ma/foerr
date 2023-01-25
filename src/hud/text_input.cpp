#include "text_input.hpp"

#include "../settings/settings_manager.hpp"
#include "../util/util.hpp"

#define TEXT_INPUT_PADDING 5
#define TEXT_INPUT_PADDING2 TEXT_INPUT_PADDING * 2
#define CURSOR_WIDTH 1.0f

/**
 * Note: font must use fixed-width characters.
 */
TextInput::TextInput(enum FontSize fontSize, uint width, const sf::Font &font) :
	fontSize(fontSize),
	width(width)
{
	this->box.setOutlineColor(SettingsManager::hudColor);
	this->box.setFillColor(DIM_COLOR(SettingsManager::hudColor, 0x20));
	this->box.setOutlineThickness(1.0f);

	this->cursor.setFillColor(SettingsManager::hudColor);

	this->text.setFont(font);
	this->text.setFillColor(SettingsManager::hudColor);

	// TODO this only works for fixed-width fonts, should be more generic
	this->characterWidth = font.getGlyph('E', getFontSize(SettingsManager::guiScale, fontSize), false).advance;

	this->setSize();
	this->updateCursorPosition();
}

uint TextInput::getHeight() const
{
	return getFontSize(SettingsManager::guiScale, this->fontSize) + TEXT_INPUT_PADDING2;
}

void TextInput::setSize()
{
	this->box.setSize(sf::Vector2f(this->width, this->getHeight()));

	this->cursor.setSize(sf::Vector2f(CURSOR_WIDTH, getFontSize(SettingsManager::guiScale, this->fontSize)));

	this->text.setCharacterSize(getFontSize(SettingsManager::guiScale, this->fontSize));
	this->text.setPosition(TEXT_INPUT_PADDING,
						   TEXT_INPUT_PADDING + getFontVOffset(SettingsManager::guiScale, this->fontSize));
}

void TextInput::updateCursorPosition()
{
	this->cursor.setPosition(TEXT_INPUT_PADDING - 1 + (this->characterWidth * this->cursorIdx), TEXT_INPUT_PADDING);
}

void TextInput::moveCursorLeft()
{
	this->cursorIdx--;
	this->cursor.move(-static_cast<int>(this->characterWidth), 0);
}

void TextInput::moveCursorRight()
{
	this->cursorIdx++;
	this->cursor.move(this->characterWidth, 0);
}

std::string TextInput::getCurrentInput() const
{
	return this->currentInput;
}

/**
 * Appends a character to the input box. Only ASCII characters are supported.
 */
void TextInput::putCharacter(char character)
{
	this->currentInput.insert(this->cursorIdx, 1, character);
	this->moveCursorRight();
	this->text.setString(this->currentInput);
}

void TextInput::clearInput()
{
	this->currentInput.clear();
	this->cursorIdx = 0;
	this->updateCursorPosition();
	this->text.setString(this->currentInput);
}

/**
 * @brief Handles a key press.
 *
 * Supported keys:
 *   - Left/Right - move cursor left/right
 *   - Home/End - move cursor to start/end
 *   - Backspace - removes the character behind the cursor and moves cursor one character to the left
 *   - Delete - removes the character in front of the cursor
 *
 * If the key is unsupported, nothing will happen.
 *
 * @param key the key to handle
 */
void TextInput::handleKeyPress(sf::Keyboard::Key key)
{
	if (key == sf::Keyboard::Backspace)
	{
		if (this->currentInput.length() == 0 || this->cursorIdx == 0)
			return; // if we don't check this, ::erase() might crash

		this->moveCursorLeft();
		this->currentInput.erase(this->cursorIdx, 1);
		this->text.setString(this->currentInput);
	}
	if (key == sf::Keyboard::Delete)
	{
		if (this->currentInput.length() == 0 || this->cursorIdx >= this->currentInput.length())
			return; // if we don't check this, ::erase() might crash

		this->currentInput.erase(this->cursorIdx, 1);
		this->text.setString(this->currentInput);
	}
	else if (key == sf::Keyboard::Left)
	{
		if (this->cursorIdx > 0)
			this->moveCursorLeft();
	}
	else if (key == sf::Keyboard::Right)
	{
		if (this->cursorIdx < this->currentInput.length())
			this->moveCursorRight();
	}
	else if (key == sf::Keyboard::Home)
	{
		this->cursorIdx = 0;
		this->updateCursorPosition();
	}
	else if (key == sf::Keyboard::End)
	{
		this->cursorIdx = this->currentInput.length();
		this->updateCursorPosition();
	}
}

void TextInput::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->box, states);
	target.draw(this->text, states);
	target.draw(this->cursor, states);
}
