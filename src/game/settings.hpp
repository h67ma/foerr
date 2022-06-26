#pragma once

#include "gui.hpp"
#include "setting.hpp"

#define DEF_SETTING(sKey, sDefault) Setting sKey = Setting(#sKey, sDefault)
#define GET_SETT_UINT(key) this->settings->key.val.numeric
#define GET_SETT_BOOL(key) this->settings->key.val.logic
#define GET_SETT_SCR_CORNER(key) this->settings->key.val.enumScreenCorner

/**
 * The Settings class only represents a "live" representation of settings.
 * All changes to settings are temporary, unless they are written to the settings file.
 *
 * Note: another approach to storing settings was considered: use an std::unordered_map.
 * This was problematic however, because we store different data types.
 * In the end it boiled down to the problem of what to return if we don't find a key in the map.
 * We can't really return a default value, as we don't know what type was requested.
 * This solution is pretty elegant, but also pretty risky - caller can put any string as a key.
 * Of course this assumes the caller makes the mistake of not using a constant key, which could be
 * enforced. Another thing that could be enforced is return nullptr when key is not found and make
 * the caller check, maybe even provide a macro for that.
 * Approach with simple fields is instead more messy, relies on macros, loading/saving each setting
 * to file is tedious, and loading from file is less efficient (because we have to search for a
 * setting every time a line is read, instead of just accessing the correct array item), but it's much
 * safer to use and more efficient to read values, making it viable to read straight from settings
 * object in various places in code, even in the rendering loop, instead of saving the setting value in
 * a private field and updating it every time settings are changed during runtime.
 */
class Settings
{
	// TODO maybe add some sort of private array/list and store pointers to internal fields inside xd
	// so we can iterate through them in save/loadConfig()
	public:
		void saveConfig();
		void loadConfig();

		// window
		DEF_SETTING(fullscreenEnabled, false);
		DEF_SETTING(fpsLimitEnabled, true);
		DEF_SETTING(fpsLimit, 60U);
		DEF_SETTING(fakeVsyncEnabled, false);
		DEF_SETTING(windowWidth, 1280U);
		DEF_SETTING(windowHeight, 720U);
		// TODO? windowed size + position override

		// logging
		DEF_SETTING(displayDebugInLog, true);
		DEF_SETTING(writeDebugLog, true);

		// font sizes
		DEF_SETTING(normalFontSize, 18U);
		DEF_SETTING(normalFontSizeWithGap, 20U);
		DEF_SETTING(smallFontSize, 14U);

		// gui
		DEF_SETTING(preferCustomCursor, true);
		DEF_SETTING(showFpsCounter, true);
		DEF_SETTING(logAnchor, CORNER_TOP_RIGHT);
		//DEF_SETTING(uint, logMsgTimeout, 3); // is this really necessary?
};
