#include "text_input.hpp"

#include "../settings/settings_manager.hpp"
#include "hud.hpp"

constexpr uint TEXT_INPUT_PADDING = 5;
constexpr uint TEXT_INPUT_PADDING2 = TEXT_INPUT_PADDING * 2;
constexpr float CURSOR_WIDTH = 1;
constexpr float BOX_OUTLINE_THICKNESS = 1;

/**
 * Note: font must use fixed-width characters.
 */
TextInput::TextInput(uint fontSize, uint width, const sf::Font &font, uint maxCharacters) :
	fontSize(fontSize),
	width(width),
	maxCharacters(maxCharacters)
{
	this->text.setFont(font);

	// TODO this only works for fixed-width fonts, should be more generic
	this->fontAdvance = font.getGlyph('E', fontSize, false).advance;

	this->handleSettingsChange();
}

uint TextInput::getHeight() const
{
	return static_cast<uint>(SettingsManager::guiScale * this->fontSize) + TEXT_INPUT_PADDING2;
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
 * Appends a character to the input box.
 * If the field is already full, the character is not added (false is returned).
 * Only printable ASCII characters are supported, all else are ignored (false is returned).
 *
 * @return true if event was consumed, false otherwise
 */
bool TextInput::handleTextEntered(uint keycode)
{
	if (this->currentInput.length() >= this->maxCharacters)
		return false;

	if (keycode < ' ' || keycode > '~')
		return false;

	this->currentInput.insert(this->cursorIdx, 1, keycode);
	this->moveCursorRight();
	this->text.setString(this->currentInput);
	return true;
}

/**
 * Replaces current input text with the specified one.
 * Moves the cursor to the end.
 *
 * @param input new input text
 */
void TextInput::setInput(const std::string &input)
{
	this->currentInput = input;
	this->text.setString(this->currentInput);
	this->cursorIdx = this->currentInput.length();
	this->updateCursorPosition();
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

void TextInput::handleSettingsChange()
{
	this->characterWidth = this->fontAdvance * SettingsManager::guiScale;

	this->box.setOutlineThickness(calculateGuiAwareScalar(BOX_OUTLINE_THICKNESS));
	this->box.setSize(sf::Vector2f(SettingsManager::guiScale * this->width, this->getHeight()));
	this->box.setOutlineColor(SettingsManager::hudColor);
	this->box.setFillColor(DIM_COLOR(SettingsManager::hudColor, 0x20));

	this->cursor.setSize(calculateGuiAwarePoint(sf::Vector2f(CURSOR_WIDTH, this->fontSize)));
	this->cursor.setFillColor(SettingsManager::hudColor);

	this->text.setCharacterSize(static_cast<uint>(SettingsManager::guiScale * this->fontSize));
	this->text.setPosition(calculateGuiAwarePoint({ TEXT_INPUT_PADDING,
													TEXT_INPUT_PADDING + getFontVOffset(this->fontSize) }));
	this->text.setFillColor(SettingsManager::hudColor);
	
	this->updateCursorPosition();
}

void TextInput::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->box, states);
	target.draw(this->text, states);
	target.draw(this->cursor, states);
}
