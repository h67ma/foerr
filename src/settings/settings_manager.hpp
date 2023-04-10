#pragma once

#include <vector>
#include <memory>
#include <string>

#include "../util/serializable_color.hpp"
#include "../hud/hud.hpp"
#include "../consts.hpp"
#include "setting.hpp"

constexpr float GUI_SCALE_MIN_VALUE = 0.25F;
constexpr float GUI_SCALE_MAX_VALUE = 4.F;

/**
 * The SettingsManager class is a convenient place to store persistent settings unrelated to any particular savegame.
 * It is a "live" representation of settings - all changes are temporary, until they are written to the settings file.
 *
 * Settings are implemented as static fields, which allows them to be read from anywhere in the code without passing
 * along the settings object. It is also as efficient as it can get - without using a collection of any kind, which
 * makes it viable to use the settings in time-sensitive context, such as in the draw() method.
 *
 * Internally, SettingsManager also keeps track of all the settings via a vector, which stores references to each
 * setting, along with information such as default values. This makes it convenient to perform operations on all
 * defined settings, particularly loading and saving them. While it would be possible to hardcode this functionality,
 * the implementation would be prone to human error and would likely end up with some settings missing from load/save,
 * or being overwritten, etc. Besides, programmers are lazy.
 *
 * A problem with using public, static fields is that they can be written to from *any* part of the program, without
 * any validation. For now, settings which define a constraint are checked against it when writing settings. If the
 * constraint is not met, a default value is written.
 * TODO? think about using getters/setters + some macro to define them.
 *
 * Previously the implementation of SettingsManager was based on an unordered map with all datatypes stored in a single
 * map. To get the value of some setting, something like `getBool(SETT_NAME)` needed to be used, which then looked up
 * the enum value (here: `SETT_NAME`) in the map, and returned the value of union field in the requested type. While it
 * was convenient to define all settings in a single place, the implementation was significantly slower, as every
 * setting needed to be retrieved from a map. Because of this, when some object needed to read a value of some setting
 * in a time-sensitive place, it needed to read the setting in some kind of setup step, and store a local copy for quick
 * access, which introduced needless complexity to code. It also lacked some finesse, which perhaps could be improved
 * with clever usage of templates.
 *
 * Current strategy:
 * At the start, we initialize various objects with settings read from the SettingsManager
 * object. When settings change, main calls the proper update methods for all objects
 * which should adjust their behaviour/display according to the new settings (new values
 * passed as args, or via a ref to SettingsManager).
 *
 * Alternate strategy:
 * Objects which rely on various settings could register a callback to SettingsManager
 * (callbacks would be stored in a vector), the callback being responsible for updating
 * the object on settings change. We could then simply call all these callbacks from
 * inside SettingsManager.
 *
 * Settings file structure:
 * {
 *	"api_version": 1,
 *	"settings": {
 *		"setting_key": "value",
 *		"key2": 42,
 *		...
 *	}
 * }
 */
class SettingsManager
{
	private:
		static std::vector<std::unique_ptr<Setting>> settings;

		// SettingsManager is a kinda dumb place to put paths, but can't think of a better place rn.
		// paths are not stored in the main settings vector. they are instead generated based on user home dir,
		// with ::generatePathsAndMkdir()
		static std::string gameRootDir;
		static std::string saveDir;

	public:
		static void setup();
		static void saveConfig();
		static void loadConfig();
		static void resetAllToDefault();
		static bool generatePathsAndMkdir();
		static std::string getGameRootDir();
		static std::string getSaveDir();

		// when adding a new setting, it needs to be initialized in ::setup() to support serdes

		///// window /////
		static bool fullscreen;
		static bool fpsLimitEnabled;
		static uint fpsLimit;
		static bool fakeVsync;
		static uint roomTransitionDurationMs;

		///// hud /////
		static bool preferCustomCursor;
		static bool showFpsCounter;
		static ScreenCorner logAnchor;
		static ScreenCorner fpsAnchor;
		static float guiScale; // all calculated positions should be rounded to integer value to avoid blurriness
		static SerializableColor hudColor;
		static bool pauseOnFocusLoss;

		///// audio /////
		static uint fxVolume;

		///// video /////
		static uint antiAliasing;
		static uint windowWidth;
		static uint windowHeight;

		///// debug - name must start with "debug" /////
		static std::string debugAutoloadCampaign;
		static bool debugWriteLogToFile;
		static bool debugPrintToStderr;
		static bool debugVerbose;
		static bool debugNavigation;
		static bool debugBoundingBoxes;
		static ScreenSide debugConsoleAnchor;
		static bool debugConsoleEnabled;
};
