#include "dev_console.hpp"

#include <vector>

#include "../settings/settings_manager.hpp"
#include "../util/i18n.hpp"
#include "../util/util.hpp"
#include "log.hpp"

#define CONSOLE_MARGIN 40
#define CONSOLE_WIDTH 1000

// storing this in SettingsManager would be an overkill
#define MAX_HISTORY_SIZE 25

DevConsole::DevConsole(sf::Vector2u windowSize, const sf::Font &font, Campaign &campaign) :
	inputField(FONT_H2, CONSOLE_WIDTH, font),
	campaign(campaign)
{
	this->setPosition(windowSize);
}

void DevConsole::open()
{
	if (!SettingsManager::debugConsoleEnabled)
		return;

	this->historyElem = this->history.end();

	// the key used to bring up the console won't be entered into input field
	this->ignoreNextTextEntered = true;

	this->isOpen = true;
}

bool DevConsole::getIsOpen() const
{
	return this->isOpen;
}

void DevConsole::setPosition(sf::Vector2u windowSize)
{
	switch(SettingsManager::debugConsoleAnchor)
	{
		case SIDE_TOP:
			this->inputField.setPosition(CONSOLE_MARGIN, CONSOLE_MARGIN);
			break;
		default: // SIDE_BOTTOM
			this->inputField.setPosition(CONSOLE_MARGIN,
										 windowSize.y - CONSOLE_MARGIN - this->inputField.getHeight());
			break;
	}
}

/**
 * @brief Handles a key press. A text entered event should be also handled to process regular characters.
 *
 * Only handles the following characters:
 *   - Esc - closes the console
 *   - Enter - executes the current input and closes the console
 *   - Up/Down - selects previous/next input from history
 *   - Left/Right/Backspace - handled by the input field
 *
 * Other characters are ignored.
 *
 * @param character character to interpret
 */
void DevConsole::handleKeyPressed(sf::Keyboard::Key key)
{
	if (key == sf::Keyboard::Escape)
	{
		this->isOpen = false;
	}
	else if (key == sf::Keyboard::Enter)
	{
		this->execute(this->inputField.getCurrentInput());
		this->inputField.clearInput();
		this->isOpen = false;
	}
	else if (key == sf::Keyboard::Up)
	{
		if (this->historyElem <= this->history.begin())
			return;

		this->historyElem--;
		this->inputField.setInput(*this->historyElem);
	}
	else if (key == sf::Keyboard::Down)
	{
		if (this->historyElem >= this->history.end() - 1)
			return;

		this->historyElem++;
		this->inputField.setInput(*this->historyElem);
	}

	this->inputField.handleKeyPress(key);
}

/**
 * Handles text entered event. A key press event should be also handled to process special keys (enter, arrows, etc).
 *
 * Only basic ASCII characters are handled, the rest are ignored.
 */
void DevConsole::handleTextEntered(uint keycode)
{
	if (this->ignoreNextTextEntered)
	{
		this->ignoreNextTextEntered = false;
		return;
	}

	if (keycode < ' ' || keycode > '~')
		return;

	this->inputField.putCharacter(keycode);

	this->historyElem = this->history.end();
}

/**
 * Re-executes last entered command.
 */
void DevConsole::executeLast()
{
	this->execute(this->lastCommand);
}

/**
 * Parses the current command line and executes an action based on it.
 *
 * @param cmdline command line to execute
 */
void DevConsole::execute(const std::string &cmdline)
{
	if (!SettingsManager::debugConsoleEnabled)
		return;

	std::vector<std::string> tokens;
	splitString(tokens, cmdline, ' ');
	size_t tokenCnt = tokens.size();

	this->lastCommand = cmdline;

	if (tokenCnt == 0)
		return; // empty input

	const char* commandName = tokens[0].c_str();

	if (tokens[0] == "where" || tokens[0] == "whereami")
	{
		this->campaign.logWhereAmI();
	}
	else if (tokens[0] == "goto")
	{
		// accepts 2 or 3 arguments (third defaults to 0)

		if (tokenCnt < 3)
		{
			Log::e(STR_MISSING_OPERANDS, commandName);
			return;
		}

		int x, y, z = 0;

		if ((!strToInt(tokens[1], x) || !strToInt(tokens[2], y)) || (tokenCnt > 3 && !strToInt(tokens[3], z)))
		{
			Log::e(STR_INVALID_OPERANDS, commandName);
			return;
		}

		if (this->campaign.gotoRoom({ x, y, z }))
			this->campaign.logWhereAmI();
		else
			Log::e(STR_INVALID_COORDS, commandName);
	}
	else if (tokens[0] == "box" || tokens[0] == "boxen")
	{
		SettingsManager::debugBoundingBoxes = !SettingsManager::debugBoundingBoxes;
		this->campaign.redraw();
	}
	else if (tokens[0] == "nav")
	{
		SettingsManager::debugNavigation = !SettingsManager::debugNavigation;
	}
	else
	{
		Log::e(STR_UNKNOWN_COMMAND, commandName);
	}

	// add the commandline to history, even if resulted in an error
	this->history.push_back(cmdline);

	// remove old history items if there are too many
	if (this->history.size() > MAX_HISTORY_SIZE)
		this->history.pop_front();
}

void DevConsole::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->inputField, states);
}
