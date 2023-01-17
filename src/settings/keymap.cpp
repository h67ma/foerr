#include "keymap.hpp"

#include "settings_manager.hpp"
#include "../util/i18n.hpp"
#include "../util/json.hpp"
#include "../hud/log.hpp"
#include "../consts.hpp"

#define CONFUSION "???"

// defines available actions, along with action strings, display strings, and default keys
// ::serializeKey must be unique (will be checked in ::setup())
const std::unordered_map<KeyAction, action_def> Keymap::actionsMap {
	{ ACTION_PIPBUCK_TOGGLE_OPEN, { "pipbuck_toggle_open", "Open/close PipBuck", { sf::Keyboard::Tab, sf::Keyboard::Escape } } },
	{ ACTION_PIPB_GOTO_LOAD, { "pipbuck_open_load", "Open Load page", { } } },
	{ ACTION_PIPB_GOTO_SAVE, { "pipbuck_open_save", "Open Save page", { } } },
	{ ACTION_PIPB_GOTO_SETTINGS, { "pipbuck_open_sett", "Open Settings page", { } } },
	{ ACTION_PIPB_GOTO_CONTROLS, { "pipbuck_open_contr", "Open Controls page", { } } },
	{ ACTION_PIPB_GOTO_LOG, { "pipbuck_open_log", "Open Log page", { sf::Keyboard::L } } },
	{ ACTION_PIPB_GOTO_STATUS_MAIN, { "pipbuck_open_statmain", "Open Main status page", { } } },
	{ ACTION_PIPB_GOTO_SKILLS, { "pipbuck_open_skills", "Open Skills page", { sf::Keyboard::K } } },
	{ ACTION_PIPB_GOTO_PERKS, { "pipbuck_open_perks", "Open Perks page", { } } },
	{ ACTION_PIPB_GOTO_EFFECTS, { "pipbuck_open_eff", "Open Effects page", { sf::Keyboard::O } } },
	{ ACTION_PIPB_GOTO_HEALTH, { "pipbuck_open_health", "Open Health page", { sf::Keyboard::H } } },
	{ ACTION_PIPB_GOTO_WEAPONS, { "pipbuck_open_wpn", "Open Weapons page", { sf::Keyboard::I } } },
	{ ACTION_PIPB_GOTO_ARMOR, { "pipbuck_open_armor", "Open Armor page", { } } },
	{ ACTION_PIPB_GOTO_EQUIPMENT, { "pipbuck_open_eq", "Open Equipment page", { } } },
	{ ACTION_PIPB_GOTO_INVENTORY_OTHER, { "pipbuck_open_invother", "Open Other inventory page", { } } },
	{ ACTION_PIPB_GOTO_AMMO, { "pipbuck_open_ammo", "Open Ammo page", { } } },
	{ ACTION_PIPB_GOTO_MAP, { "pipbuck_open_map", "Open Map page", { sf::Keyboard::M } } },
	{ ACTION_PIPB_GOTO_WORLD, { "pipbuck_open_world", "Open World page", { sf::Keyboard::N } } },
	{ ACTION_PIPB_GOTO_QUESTS, { "pipbuck_open_quests", "Open Quests page", { sf::Keyboard::J } } },
	{ ACTION_PIPB_GOTO_NOTES, { "pipbuck_open_notes", "Open Notes page", { } } },
	{ ACTION_PIPB_GOTO_ENEMIES, { "pipbuck_open_enemies", "Open Enemies page", { } } },
	{ ACTION_TOGGLE_FULLSCREEN, { "toggle_fullscreen", "Toggle fullscreen mode", { sf::Keyboard::F11 } } },
	// TODO { ACTION_CONSOLE_OPEN, { "console_open", "Open console", { sf::Keyboard::Slash } } },
	// TODO { ACTION_CONSOLE_REPEAT_LAST, { "console_repeat_last", "Repeat last console command", { sf::Keyboard::Slash + shift or ctrl } } },
	{ ACTION_DEBUG_NAV_LEFT, { "debug_nav_left", "Debug navigation left", { sf::Keyboard::Left } } },
	{ ACTION_DEBUG_NAV_RIGHT, { "debug_nav_right", "Debug navigation right", { sf::Keyboard::Right } } },
	{ ACTION_DEBUG_NAV_UP, { "debug_nav_up", "Debug navigation up", { sf::Keyboard::Up } } },
	{ ACTION_DEBUG_NAV_DOWN, { "debug_nav_down", "Debug navigation down", { sf::Keyboard::Down } } },
	{ ACTION_DEBUG_NAV_FRONT, { "debug_nav_front", "Debug navigation front", { sf::Keyboard::PageUp } } },
	{ ACTION_DEBUG_NAV_BACK, { "debug_nav_back", "Debug navigation back", { sf::Keyboard::PageDown } } },
	{ ACTION_NO_ACTION, { "no_action", "No action", { } } },
};

#define KEY_STR(keyCode, str) { sf::Keyboard::keyCode, str }
#define KEY_STR_DEFAULT(keyCode) KEY_STR(keyCode, #keyCode)

// all from 2.5.1 <SFML/Window/Keyboard.hpp>, excluding the super key
// key string must be unique (will be checked in ::setup())
std::unordered_map<sf::Keyboard::Key, std::string> Keymap::keyNamesMap {
	KEY_STR_DEFAULT(A),
	KEY_STR_DEFAULT(B),
	KEY_STR_DEFAULT(C),
	KEY_STR_DEFAULT(D),
	KEY_STR_DEFAULT(E),
	KEY_STR_DEFAULT(F),
	KEY_STR_DEFAULT(G),
	KEY_STR_DEFAULT(H),
	KEY_STR_DEFAULT(I),
	KEY_STR_DEFAULT(J),
	KEY_STR_DEFAULT(K),
	KEY_STR_DEFAULT(L),
	KEY_STR_DEFAULT(M),
	KEY_STR_DEFAULT(N),
	KEY_STR_DEFAULT(O),
	KEY_STR_DEFAULT(P),
	KEY_STR_DEFAULT(Q),
	KEY_STR_DEFAULT(R),
	KEY_STR_DEFAULT(S),
	KEY_STR_DEFAULT(T),
	KEY_STR_DEFAULT(U),
	KEY_STR_DEFAULT(V),
	KEY_STR_DEFAULT(W),
	KEY_STR_DEFAULT(X),
	KEY_STR_DEFAULT(Y),
	KEY_STR_DEFAULT(Z),
	KEY_STR_DEFAULT(Num0),
	KEY_STR_DEFAULT(Num1),
	KEY_STR_DEFAULT(Num2),
	KEY_STR_DEFAULT(Num3),
	KEY_STR_DEFAULT(Num4),
	KEY_STR_DEFAULT(Num5),
	KEY_STR_DEFAULT(Num6),
	KEY_STR_DEFAULT(Num7),
	KEY_STR_DEFAULT(Num8),
	KEY_STR_DEFAULT(Num9),
	KEY_STR(Escape, "Esc"),
	KEY_STR(LControl, "LCtrl"),
	KEY_STR_DEFAULT(LShift),
	KEY_STR_DEFAULT(LAlt),
	// KEY_STR_DEFAULT(LSystem),
	KEY_STR(RControl, "RCtrl"),
	KEY_STR_DEFAULT(RShift),
	KEY_STR_DEFAULT(RAlt),
	// KEY_STR_DEFAULT(RSystem),
	KEY_STR_DEFAULT(Menu),
	KEY_STR(LBracket, "["),
	KEY_STR(RBracket, "]"),
	KEY_STR(Semicolon, ";"),
	KEY_STR(Comma, ","),
	KEY_STR(Period, "."),
	KEY_STR(Quote, "'"),
	KEY_STR(Slash, "/"),
	KEY_STR(Backslash, "\\"),
	KEY_STR(Tilde, "`"),
	KEY_STR(Equal, "="),
	KEY_STR(Hyphen, "-"),
	KEY_STR(Space, " "),
	KEY_STR_DEFAULT(Enter),
	KEY_STR_DEFAULT(Backspace),
	KEY_STR_DEFAULT(Tab),
	KEY_STR_DEFAULT(PageUp),
	KEY_STR_DEFAULT(PageDown),
	KEY_STR_DEFAULT(End),
	KEY_STR_DEFAULT(Home),
	KEY_STR_DEFAULT(Insert),
	KEY_STR(Delete, "Del"),
	KEY_STR(Add, "+"),
	KEY_STR(Subtract, "Numpad-"),
	KEY_STR(Multiply, "*"),
	KEY_STR(Divide, "Numpad/"),
	KEY_STR_DEFAULT(Left),
	KEY_STR_DEFAULT(Right),
	KEY_STR_DEFAULT(Up),
	KEY_STR_DEFAULT(Down),
	KEY_STR_DEFAULT(Numpad0),
	KEY_STR_DEFAULT(Numpad1),
	KEY_STR_DEFAULT(Numpad2),
	KEY_STR_DEFAULT(Numpad3),
	KEY_STR_DEFAULT(Numpad4),
	KEY_STR_DEFAULT(Numpad5),
	KEY_STR_DEFAULT(Numpad6),
	KEY_STR_DEFAULT(Numpad7),
	KEY_STR_DEFAULT(Numpad8),
	KEY_STR_DEFAULT(Numpad9),
	KEY_STR_DEFAULT(F1),
	KEY_STR_DEFAULT(F2),
	KEY_STR_DEFAULT(F3),
	KEY_STR_DEFAULT(F4),
	KEY_STR_DEFAULT(F5),
	KEY_STR_DEFAULT(F6),
	KEY_STR_DEFAULT(F7),
	KEY_STR_DEFAULT(F8),
	KEY_STR_DEFAULT(F9),
	KEY_STR_DEFAULT(F10),
	KEY_STR_DEFAULT(F11),
	KEY_STR_DEFAULT(F12),
	KEY_STR_DEFAULT(F13),
	KEY_STR_DEFAULT(F14),
	KEY_STR_DEFAULT(F15),
	KEY_STR_DEFAULT(Pause),
};

std::unordered_map<sf::Keyboard::Key, KeyAction> Keymap::keyToActionMap;

// keeping reverse maps is kinda lame, but we need to have some way of translating both ways.
std::unordered_map<std::string, sf::Keyboard::Key> Keymap::keyStringToKeyMap;
std::unordered_map<std::string, KeyAction> Keymap::actionStringToActionMap;

/**
 * Initializes the default keymap and internal maps.
 *
 * @return true if setup was successful
 * @return false if setup failed
 */
bool Keymap::setup()
{
	Keymap::resetToDefault();

	for (const auto &item : Keymap::actionsMap)
	{
		std::string actionStr = item.second.serializeKey;
		if (Keymap::actionStringToActionMap.find(actionStr) != Keymap::actionStringToActionMap.end())
		{
			// we have an action string duplicate
			Log::w(STR_KEYMAP_INIT_FAIL_ACTION_STR_DUPL, actionStr.c_str());
			return false;
		}
		Keymap::actionStringToActionMap.emplace(actionStr, item.first);
	}

	for (const auto item : Keymap::keyNamesMap)
	{
		if (Keymap::keyStringToKeyMap.find(item.second) != Keymap::keyStringToKeyMap.end())
		{
			// we have an key string duplicate
			Log::w(STR_KEYMAP_INIT_FAIL_KEY_STR_DUPL, item.second.c_str());
			return false;
		}
		Keymap::keyStringToKeyMap.emplace(item.second, item.first);
	}

	return true;
}

const std::unordered_map<sf::Keyboard::Key, KeyAction>& Keymap::getKeyToActionMap()
{
	return Keymap::keyToActionMap;
}

KeyAction Keymap::keyToAction(sf::Keyboard::Key key)
{
	auto search = Keymap::keyToActionMap.find(key);
	if (search == Keymap::keyToActionMap.end())
		return ACTION_NO_ACTION;

	return search->second;
}

/**
 * Gets key name from key. Can be used for displaying or for serialization.
 *
 * @param key SFML key
 * @return key string
 */
std::string Keymap::keyToString(sf::Keyboard::Key key)
{
	auto search = Keymap::keyNamesMap.find(key);
	if (search == Keymap::keyNamesMap.end())
	{
		Log::w(STR_KEY_NOT_FOUND, key);
		return CONFUSION;
	}

	return search->second;
}

sf::Keyboard::Key Keymap::keyStringToKey(const std::string &keyStr)
{
	auto search = Keymap::keyStringToKeyMap.find(keyStr);
	if (search == Keymap::keyStringToKeyMap.end())
	{
		Log::w(STR_KEY_STR_NOT_FOUND, keyStr.c_str());
		return sf::Keyboard::Unknown;
	}

	return search->second;
}

std::string Keymap::actionToString(KeyAction action)
{
	auto search = Keymap::actionsMap.find(action);
	if (search == Keymap::actionsMap.end())
	{
		Log::w(STR_ACTION_NOT_FOUND, action);
		return CONFUSION;
	}

	return search->second.serializeKey;
}

KeyAction Keymap::actionStringToAction(const std::string &actionStr)
{
	auto search = Keymap::actionStringToActionMap.find(actionStr);
	if (search == Keymap::actionStringToActionMap.end())
	{
		Log::w(STR_ACTION_STR_NOT_FOUND, actionStr.c_str());
		return ACTION_NO_ACTION;
	}

	return search->second;
}

std::string Keymap::actionToDisplayString(KeyAction action)
{
	auto search = Keymap::actionsMap.find(action);
	if (search == Keymap::actionsMap.end())
	{
		Log::w(STR_ACTION_NOT_FOUND, action);
		return CONFUSION;
	}

	return search->second.displayName;
}

void Keymap::resetToDefault()
{
	Keymap::keyToActionMap.clear();

	for (const auto &action : Keymap::actionsMap)
	{
		for (const auto key : action.second.defaultKeys)
		{
			Keymap::keyToActionMap.emplace(key, action.first);
		}
	}
}

/**
 * Loads keymap from file. For every key-action mapping defined in the file, creates an entry in ::keyToActionMap, or
 * replaces the default one if it exists. Basicaly the settings file overwrites the default mapping, while keeping
 * default keybindings (if they are not overwritten).
 *
 * Keys with invalid actions, or "no_action" will be also added with ACTION_NO_ACTION.
 * If file cannot be read, default actions are already loaded.
 */
void Keymap::load()
{
	json root;

	if (!loadJsonFromFile(root, pathCombine(SettingsManager::getGameRootDir(), std::string(PATH_KEYMAP)), true))
	{
		Log::d(STR_KEYMAP_OPEN_ERROR);
		return;
	}

	auto keysSearch = root.find(FOERR_JSON_KEY_KEYS);
	if (keysSearch == root.end())
	{
		Log::w(STR_MISSING_KEY, PATH_KEYMAP, FOERR_JSON_KEY_KEYS);
		return;
	}

	if (!keysSearch->is_object())
	{
		Log::w(STR_INVALID_TYPE, PATH_KEYMAP, FOERR_JSON_KEY_KEYS);
		return;
	}

	for (const auto &node : keysSearch->items())
	{
		sf::Keyboard::Key key = Keymap::keyStringToKey(node.key());
		if (key == sf::Keyboard::Unknown)
			continue;

		// in case of invalid action string, ACTION_NO_ACTION will be added.
		// note: in this case only "no_action" will not output a warning.
		Keymap::keyToActionMap[key] = Keymap::actionStringToAction(node.value());
	}
}

/**
 * Saves the current keymap to a file.
 */
void Keymap::save()
{
	json root;

	root.emplace(FOERR_JSON_API_VERSION, JSON_API_VERSION);

	json keysNode;
	for (const auto &item : Keymap::keyToActionMap)
	{
		// translate bolth key and action to strings.
		// there shouldn't be key duplicates as we've already checked for it in ::setup(),
		// but check again just to be sure

		std::string keyStr = Keymap::keyToString(item.first);
		if (keyStr == CONFUSION)
			continue;

		std::string actionStr = Keymap::actionToString(item.second);
		if (actionStr == CONFUSION)
			continue;

		keysNode.emplace(keyStr, actionStr);
	}

	root.emplace(FOERR_JSON_KEY_KEYS, keysNode);

	writeJsonToFile(root, pathCombine(SettingsManager::getGameRootDir(), std::string(PATH_KEYMAP)));
	Log::i(STR_KEYMAP_SAVED);
}
