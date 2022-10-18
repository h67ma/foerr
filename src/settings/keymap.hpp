#include <string>
#include <unordered_map>
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
	ACTION_NO_ACTION,
};

struct action_def
{
	std::string serializeKey;
	std::string displayName;
	std::vector<sf::Keyboard::Key> defaultKeys; // could be a set, but it wouldn't really change anything, just add a bit of complexity
};

/**
 * Used for keeping a mapping of key to action according to player preferences.
 * Supports saving keybindings to a file. The file can be parsed later in order
 * to restore user preferences.
 *
 * Keymap file structure:
 * {
 *		"A": "move_left",
 *		"Esc": "pipbuck_toggle_open",
 "		"Tab": "pipbuck_toggle_open"	// multiple keys can map to the same action
 *		...
 *	},
 */
class Keymap
{
	private:
		const static std::unordered_map<KeyAction, action_def> actionsMap;
		static std::unordered_map<sf::Keyboard::Key, KeyAction> keyToActionMap;
		static std::unordered_map<std::string, sf::Keyboard::Key> keyStringToKeyMap;
		static std::unordered_map<std::string, KeyAction> actionStringToActionMap;
		static std::unordered_map<sf::Keyboard::Key, std::string> keyNamesMap;

	public:
		static bool setup();
		const static std::unordered_map<sf::Keyboard::Key, KeyAction>& getkeyToActionMap();
		static KeyAction keyToAction(sf::Keyboard::Key key);
		static std::string keyToString(sf::Keyboard::Key key);
		static sf::Keyboard::Key keyStringToKey(std::string keyStr);
		static std::string actionToString(KeyAction action);
		static KeyAction actionStringToAction(std::string actionStr);
		static std::string actionToDisplayString(KeyAction action);
		static void resetToDefault();
		static void load();
		static void save();
};