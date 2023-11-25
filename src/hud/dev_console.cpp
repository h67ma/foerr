// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#include "dev_console.hpp"

#include "../settings/settings_manager.hpp"
#include "../util/i18n.hpp"
#include "../util/util.hpp"
#include "log.hpp"

#define CONSOLE_MARGIN 40
#define CONSOLE_WIDTH 1000

// storing this in SettingsManager would be an overkill
#define MAX_HISTORY_SIZE 25

#define MAX_INPUT_CHARS 80

static void cmdFly(struct dev_console_cmd_params params)
{
	params.campaign.getPlayer().debugToggleFlight();
}

static void cmdGoto(struct dev_console_cmd_params params)
{
	// accepts 2 or 3 arguments (third defaults to 0)

	if (params.tokens.size() < 3)
	{
		Log::e(STR_MISSING_OPERANDS, params.tokens[0].c_str());
		return;
	}

	int x, y, z = 0;

	if (!strToInt(params.tokens[1], x) || !strToInt(params.tokens[2], y) ||
		(params.tokens.size() > 3 && !strToInt(params.tokens[3], z)))
	{
		Log::e(STR_INVALID_OPERANDS, params.tokens[0].c_str());
		return;
	}

	if (params.campaign.gotoRoom({ x, y, z }))
		params.campaign.logWhereAmI();
	else
		Log::e(STR_INVALID_COORDS, params.tokens[0].c_str());
}

static void cmdLights(struct dev_console_cmd_params params)
{
	if (params.tokens.size() < 2)
	{
		Log::e(STR_MISSING_OPERANDS, params.tokens[0].c_str());
		return;
	}

	int lon;

	if (!strToInt(params.tokens[1], lon))
	{
		Log::e(STR_INVALID_OPERANDS, params.tokens[0].c_str());
		return;
	}

	params.campaign.setRoomLightsState(lonToLightObjectsState(lon));
	params.campaign.redraw();
}

static void cmdTeleport(struct dev_console_cmd_params params)
{
	params.campaign.teleportPlayer(params.mousePos);
}

static void cmdToggleBoundingBoxes(struct dev_console_cmd_params params)
{
	SettingsManager::debugBoundingBoxes = !SettingsManager::debugBoundingBoxes;
	params.campaign.redraw();
}

static void cmdToggleDebugNav(struct dev_console_cmd_params params)
{
	SettingsManager::debugNavigation = !SettingsManager::debugNavigation;
}

static void cmdVariant(struct dev_console_cmd_params params)
{
	params.campaign.rerollObjVariants();
	params.campaign.redraw();
}

static void cmdWhere(struct dev_console_cmd_params params)
{
	params.campaign.logWhereAmI();
}

// note: std::map used instead of std::unordered_map only so that `?`/`help` prints a sorted list
const std::map<std::string, struct dev_console_cmd> DevConsole::commands {
	{ "backvariant", { cmdVariant, STR_CMD_VARIANT } },
	{ "box", { cmdToggleBoundingBoxes, STR_CMD_BOX } },
	{ "boxen", { cmdToggleBoundingBoxes, STR_CMD_BOX } },
	{ "fly", { cmdFly, STR_CMD_FLY } },
	{ "goto", { cmdGoto, STR_CMD_GOTO, "$1 $2 [$3]" } },
	{ "lights", { cmdLights, STR_CMD_LIGHTS, "$1" } },
	{ "nav", { cmdToggleDebugNav, STR_CMD_NAV } },
	{ "port", { cmdTeleport, STR_CMD_PORT } },
	{ "tp", { cmdTeleport, STR_CMD_PORT } },
	{ "where", { cmdWhere, STR_CMD_WHERE } },
	{ "whereami", { cmdWhere, STR_CMD_WHERE } },
};

DevConsole::DevConsole(sf::Vector2u windowSize, const sf::Font& font, Campaign& campaign) :
	inputField(FONT_H2, CONSOLE_WIDTH, font, MAX_INPUT_CHARS),
	campaign(campaign)
{
	this->handleScreenResize(windowSize);
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

void DevConsole::handleScreenResize(sf::Vector2u windowSize)
{
	switch (SettingsManager::debugConsoleAnchor)
	{
		case SIDE_TOP:
			this->inputField.setPosition(CONSOLE_MARGIN, CONSOLE_MARGIN);
			break;
		default: // SIDE_BOTTOM
			this->inputField.setPosition(CONSOLE_MARGIN, windowSize.y - CONSOLE_MARGIN - this->inputField.getHeight());
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
void DevConsole::handleKeyPressed(sf::Keyboard::Key key, sf::Vector2f mouseCoords)
{
	if (key == sf::Keyboard::Escape)
	{
		this->isOpen = false;
	}
	else if (key == sf::Keyboard::Enter)
	{
		this->execute(this->inputField.getCurrentInput(), mouseCoords);
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

	if (!this->inputField.handleTextEntered(keycode))
		return;

	this->historyElem = this->history.end();
}

/**
 * Re-executes last entered command.
 */
void DevConsole::executeLast(sf::Vector2f mouseCoords)
{
	this->execute(this->lastCommand, mouseCoords);
}

/**
 * Parses the current command line and executes an action based on it.
 *
 * @param cmdline command line to execute
 */
void DevConsole::execute(const std::string& cmdline, sf::Vector2f mouseCoords)
{
	if (!SettingsManager::debugConsoleEnabled)
		return;

	std::vector<std::string> tokens;
	splitString(tokens, cmdline, ' ');

	this->lastCommand = cmdline;

	if (tokens.size() == 0)
		return; // empty input

	if (tokens[0] == "?" || tokens[0] == "help")
	{
		// help command entered - print all available commands
		for (const auto& cmd : this->commands)
		{
			if (cmd.second.helpArgs != nullptr)
				Log::i("%s %s - %s", cmd.first.c_str(), cmd.second.helpArgs, cmd.second.helpString);
			else
				Log::i("%s - %s", cmd.first.c_str(), cmd.second.helpString);
		}
	}
	else
	{
		// non-help command entered - find the command, and if it exists, execute it
		auto search = this->commands.find(tokens[0]);
		if (search != this->commands.end())
			search->second.cmd({ tokens, mouseCoords, this->campaign });
		else
			Log::e(STR_UNKNOWN_COMMAND, tokens[0].c_str());
	}

	// if previous command was identical, don't add it to history
	if (!this->history.empty() && this->history.back() == cmdline)
		return;

	// add the cmdline to history, even if command does not exist or resulted in an error
	this->history.push_back(cmdline);

	// remove old history items if there are too many
	if (this->history.size() > MAX_HISTORY_SIZE)
		this->history.pop_front();
}

void DevConsole::handleSettingsChange()
{
	this->inputField.handleSettingsChange();
}

void DevConsole::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(this->inputField, states);
}
