// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>
#include <unordered_map>
#include <vector>

#include <SFML/Window/Keyboard.hpp>

enum KeyAction
{
	ACTION_PIPBUCK_TOGGLE_OPEN,

	// most of the pipbuck goto actions are pretty useless, but if someone really wants to map them, they can
	ACTION_PIPB_GOTO_LOAD,
	ACTION_PIPB_GOTO_SAVE,
	ACTION_PIPB_GOTO_SETTINGS,
	ACTION_PIPB_GOTO_CONTROLS,
	ACTION_PIPB_GOTO_LOG,
	ACTION_PIPB_GOTO_STATUS_MAIN,
	ACTION_PIPB_GOTO_SKILLS,
	ACTION_PIPB_GOTO_PERKS,
	ACTION_PIPB_GOTO_EFFECTS,
	ACTION_PIPB_GOTO_HEALTH,
	ACTION_PIPB_GOTO_WEAPONS,
	ACTION_PIPB_GOTO_ARMOR,
	ACTION_PIPB_GOTO_EQUIPMENT,
	ACTION_PIPB_GOTO_INVENTORY_OTHER,
	ACTION_PIPB_GOTO_AMMO,
	ACTION_PIPB_GOTO_MAP,
	ACTION_PIPB_GOTO_WORLD,
	ACTION_PIPB_GOTO_QUESTS,
	ACTION_PIPB_GOTO_NOTES,
	ACTION_PIPB_GOTO_ENEMIES,

	ACTION_TOGGLE_FULLSCREEN,

	// TODO? for movement actions we could set enum values to be subsequent bits. then in main loop key press/released
	// event we could quickly check if key press action is movement or not, e.g.:
	// if (action & (ACTION_PLAYER_MOVE_LEFT & ACTION_PLAYER_MOVE_RIGHT & ...))
	ACTION_PLAYER_MOVE_LEFT,
	ACTION_PLAYER_MOVE_RIGHT,
	ACTION_PLAYER_MOVE_UP,
	ACTION_PLAYER_MOVE_DOWN,
	ACTION_PLAYER_SPRINT,
	ACTION_PLAYER_JUMP,

	ACTION_DEBUG_TOGGLE_CONSOLE,
	ACTION_DEBUG_REPEAT_LAST_CONSOLE_CMD,
	ACTION_DEBUG_NAV_LEFT,
	ACTION_DEBUG_NAV_RIGHT,
	ACTION_DEBUG_NAV_UP,
	ACTION_DEBUG_NAV_DOWN,
	ACTION_DEBUG_NAV_FRONT,
	ACTION_DEBUG_NAV_BACK,

	ACTION_NO_ACTION,
};

struct action_def
{
		std::string serializeKey;
		std::string displayName;

		// could be a set, but it wouldn't really change anything, just add a bit of complexity
		std::vector<sf::Keyboard::Key> defaultKeys;
};

/**
 * Used for keeping a mapping of key to action according to player preferences.
 * Supports saving keybindings to a file. The file can be parsed later in order
 * to restore user preferences.
 *
 * Keymap file structure:
 * {
 *	"api_version": 1,
 *	"keys": {
 *		"A": "move_left",
 *		"Esc": "pipbuck_toggle_open",
 "		"Tab": "pipbuck_toggle_open"	// multiple keys can map to the same action
 *		...
 *	}
 * }
 */
class Keymap
{
	private:
		static const std::unordered_map<KeyAction, action_def> actionsMap;
		static std::unordered_map<sf::Keyboard::Key, KeyAction> keyToActionMap;
		static std::unordered_map<std::string, sf::Keyboard::Key> keyStringToKeyMap;
		static std::unordered_map<std::string, KeyAction> actionStringToActionMap;
		static std::unordered_map<sf::Keyboard::Key, std::string> keyNamesMap;
		static sf::Keyboard::Key keyStringToKey(const std::string& keyStr);

		// flags used to determine if a key is being pressed. this is pretty lame, but for a different approach
		// (directly checking keyboard state via sf::Keyboard::isKeyPressed()), supporting configurable keys would be
		// quite challenging. for now this will do.
		static bool rightHeld;
		static bool leftHeld;
		static bool upHeld;
		static bool downHeld;
		static bool sprintHeld;
		static bool jumpHeld;

	public:
		static bool setup();
		static const std::unordered_map<sf::Keyboard::Key, KeyAction>& getKeyToActionMap();
		static KeyAction keyToAction(sf::Keyboard::Key key);
		static std::string keyToString(sf::Keyboard::Key key);
		static std::string actionToString(KeyAction action);
		static KeyAction actionStringToAction(const std::string& actionStr);
		static std::string actionToDisplayString(KeyAction action);
		static bool handleKeyUpDown(enum KeyAction action, bool down);
		static bool isLeftHeld();
		static bool isRightHeld();
		static bool isUpHeld();
		static bool isDownHeld();
		static bool isSprintHeld();
		static bool isJumpHeld();
		static void resetToDefault();
		static void load();
		static void save();
};
