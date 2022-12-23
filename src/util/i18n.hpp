#pragma once

// bruh, that's some next level i18n

#define STR_WINDOW_TITLE "FoE: Remains of Remains"
#define STR_CURSOR_LOAD_IMG_ERR "Error loading custom cursor image resource (%s), fallback cursor will be used."
#define STR_CURSOR_LOAD_PIXELS_ERR "Error loading image data (%s), fallback cursor will be used."
#define STR_CURSOR_LOAD_FAIL "Error loading cursors."
#define STR_LOAD_FAIL "Cannot load resource file (%s)"
#define STR_FONT_LOAD_FAIL "Error loading font %s."
#define STR_LOAD_CORE_FAIL "Error loading core resources."
#define STR_KEYMAP_SETUP_FAIL "Error setting up keymap."
#define STR_LOAD_FONTS_FAIL "Error loading fonts."
#define STR_SETTINGS_OPEN_ERROR "Cannot read settings file - using default settings"
#define STR_KEYMAP_OPEN_ERROR "Cannot read keymap file - using default keymap"
#define STR_FILE_OPEN_ERROR "Error opening file (%s)."
#define STR_ERROR_PARSING_JSON_FILE "Error parsing file (%s): %s"
#define STR_JSON_API_VERSION_MISMATCH "API version mismatch (%s): got %d, should be %d"
#define STR_SETTINGS_KEY_MISSING "Key is missing from settings file: \"%s\" - using default value."
#define STR_INVALID_TYPE_EX "Error loading file (%s) - invalid value for key \"%s\": \"%s\""
#define STR_INVALID_VALUE "Invalid value for %s: \"%u\" - using default value."
#define STR_INVALID_VALUE_HINT "Invalid value for %s: \"%u\" - using default value. Value should be %s."
#define STR_INVALID_VALUE_COLOR "Invalid color value for %s: \"%s\" - using default value. Value should be formatted as #RRGGBB or #RGB."
#define STR_SUS_LARGE_VALUE "Suspiciously large value for key \"%s\""
#define STR_RESETTING_TO_DEFAULT "Resetting %s to default value"
#define STR_INVALID_TYPE "Error loading file (%s) - invalid value for key \"%s\""
#define STR_INVALID_ARR_SIZE "Error loading file (%s) - invalid array size for key \"%s\""
#define STR_MISSING_KEY "Problem loading file (%s) - key is missing: \"%s\"."
#define STR_ROOM_LINE_TOO_LONG "Error loading room (%s, \"%s\") - line longer than %u, skipping remaining cells."
#define STR_ROOM_ROW_TOO_SHORT "Error loading room (%s, \"%s\") - row %d too short."
#define STR_UNKNOWN_SYMBOL_AT_POS "Error loading room (%s, \"%s\") - unknown symbol at (%d, %d)."
#define STR_ROOM_MISSING "Error loading location (%s) - room is missing: \"%s\"."
#define STR_ROOM_MISSING_COORDS "Error loading location (%s) - room is missing: (%d, %d, %d)."
#define STR_DUPLICATE_START_ROOM "Error loading location (%s) - multiple start rooms defined (%d, %d, %d)."
#define STR_MISSING_START_ROOM "Error loading location (%s) - start room is missing."
#define STR_DUPLICATE_ROOM_IN_SAME_COORDS "Error loading location (%s) - multiple rooms defined for (%d, %d, %d)."
#define STR_LOC_MISSING_DATA "Error loading location (%s) - room map data is missing: \"%s\"."
#define STR_FILE_NOT_FOUND "Error: file not found (%s)."
#define STR_LOC_INVALID_TYPES "Error: location cannot be grind and basecamp at the same time (%s)."
#define STR_LOC_CHANGED "Changed location to %s."
#define STR_LOC_NOT_FOUND "Error changing location - location not found (%s)."
#define STR_CAMPAIGN_LOAD_ERR "Error loading campaign (%s)."
#define STR_CAMPAIGN_LOADING "Loading campaign (%s)..."
#define STR_CAMPAIGN_LOADED "Finished loading campaign (%s)."
#define STR_CAMPAIGN_UNLOADING "Unloading campaign..."
#define STR_CAMPAIGN_UNLOADED "Campaign unloaded."
#define STR_CAMPAIGN_LOAD_FAILED "Failed to load campaign."
#define STR_LOADING_LOCATION_META "Loading location metadata (%s)..."
#define STR_LOADED_LOCATION_META "Finished loading location metadata (%s)."
#define STR_LOADING_LOCATION_META_ERROR "Loading location metadata failed (%s)."
#define STR_LOADING_LOCATION_CONTENT "Loading location content (%s)..."
#define STR_LOADED_LOCATION_CONTENT "Finished loading location content (%s)."
#define STR_LOADING_LOCATION_CONTENT_ERROR "Loading location content failed (%s)."
#define STR_LOADED_FILE "Loaded file (%s)."
#define STR_LOADED_SETTING_D "Loaded setting: %s = %d."
#define STR_LOADED_SETTING_U "Loaded setting: %s = %u."
#define STR_LOADED_SETTING_S "Loaded setting: %s = %s."
#define STR_LOADED_SETTING_SQ "Loaded setting: %s = \"%s\"."
#define STR_GAME_PAUSED "Game paused"
#define STR_GAME_RESUMED "Game resumed"
#define STR_WINDOW_WINDOWED "Switching to windowed mode"
#define STR_WINDOW_FULLSCREEN "Switching to fullscreen mode"
#define STR_IDX_OUTTA_BOUNDS "Index out of bounds!"
#define STR_SETT_NOT_PRESENT "Setting (%d) not present."
#define STR_CLEANED_UNUSED_RES "Cleaned %u unused resources."
#define STR_PIPBUCK_CLOSE "Close [TAB]"
#define STR_PIPBUCK_STATUS "Status"
#define STR_PIPBUCK_INV "Inventory"
#define STR_PIPBUCK_INFO "Information"
#define STR_PIPBUCK_MAINMENU "Main Menu"
#define STR_CONTINUE "Continue"
#define STR_QUIT_GAME "Quit game"
#define STR_SHUTTING_DOWN "Shutting down"
#define STR_LOADING_CORE_RES "Loading core resources..."
#define STR_LOADING_CORE_RES_DONE "Finished loading core resources."
#define STR_LOADING "Loading..."
#define STR_EXIT_TO_MAIN_MENU "Exit to main menu"
#define STR_SAVE "Save"
#define STR_RESET_DEFAULT "Reset to defaults"
#define STR_KEYMAP_RESETTED "Restored default keymap"
#define STR_PIPBUCK_SETUP_FAILED "Failed to setup PipBuck."
#define STR_PIPBUCK_UNLOADING_CAMPAIGN "Unloding campaign infos from PipBuck..."
#define STR_RECOMMENDED_LVL "\n\nRecommended level: %u"
#define STR_KEY_NOT_FOUND "Key not found (%d)"
#define STR_KEY_STR_NOT_FOUND "Unknown key string (%s)"
#define STR_ACTION_NOT_FOUND "Action not found (%d)"
#define STR_ACTION_STR_NOT_FOUND "Unknown action string (%s)"
#define STR_KEYMAP_SAVED "Saved keymap"
#define STR_KEYMAP_INIT_FAIL_ACTION_STR_DUPL "Error setting up keymap - duplicate action string \"%s\""
#define STR_KEYMAP_INIT_FAIL_KEY_STR_DUPL "Error setting up keymap - duplicate key string \"%s\""
#define STR_CREATED_GAME_DIRS "Created game directories."
#define STR_CREATE_DIRS_FAIL "Failed to create directories for saving game files."
#define STR_AUTLOADING_CAMPAIGN "Autoloading campaign"
#define STR_LOADING_MATERIALS "Loading materials..."
#define STR_LOADING_MATERIALS_DONE "Finished loading materials"
#define STR_MAT_LOAD_KEY_NOT_1CHAR "Cannot load material \"%s\": symbol should be a single character."
#define STR_MAT_SYMBOL_TYPE_ALREADY_PRESENT "Symbol type already present in cell ('%c')"
#define STR_MAT_MISSING_OR_WRONG_TYPE "Material '%c' is missing or has invalid type"
