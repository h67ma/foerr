#pragma once

#define STACKTRACE_MAX_CNT 10
#define ANIM_FRAME_DURATION_MS 33 // around 30fps

// game area size
#define GAME_AREA_WIDTH 1920.f
#define GAME_AREA_HEIGHT 1000.f
#define GAME_AREA_MID_X 960.f
#define GAME_AREA_MID_Y 500.f

// paths
#define PATH_DIR_GAMEDATA ".foerr"
#define PATH_DIR_SAVES "savegames"
#define PATH_SETTINGS "config.json"
#define PATH_KEYMAP "keymap.json"
#define PATH_LOGFILE "foerr.log"
#define PATH_CURSOR_ARROW "res/hud/cursor/cursor.png"
#define PATH_CURSOR_CROSS_WHITE "res/hud/cursor/crosshair_white.png"
#define PATH_CURSOR_CROSS_YELLOW "res/hud/cursor/crosshair_yellow.png"
#define PATH_CURSOR_CROSS_RED "res/hud/cursor/crosshair_red.png"
#define DIR_LOCATIONS "locations"
#define FILENAME_INDEX "_index.json"
#define EXT_JSON ".json"

// json keys
#define FOERR_JSON_KEY_BLOCKS "blocks"
#define FOERR_JSON_KEY_SHOW_ROOM_BG "bg"
#define FOERR_JSON_KEY_BACKGROUND_FULL "background_full"
#define FOERR_JSON_KEY_WORLDMAP_ICON "worldmap_icon"
#define FOERR_JSON_KEY_WORLDMAP_COORDS "worldmap_coords"
#define FOERR_JSON_KEY_COORDS "coords"
#define FOERR_JSON_KEY_IS_START "is_start"
#define FOERR_JSON_KEY_WORLDMAP_ICON_BIG "worldmap_icon_big"
#define FOERR_JSON_KEY_WORLDMAP_BACKGROUND "worldmap_background"
#define FOERR_JSON_KEY_TYPE_GRIND "grind"
#define FOERR_JSON_KEY_TYPE_BASECAMP "basecamp"
#define FOERR_JSON_KEY_RECOMMENDED_LVL "rec_lvl"
#define FOERR_JSON_KEY_ROOMS "rooms"
#define FOERR_JSON_KEY_TITLE "title"
#define FOERR_JSON_KEY_DESCRIPTION "description"
#define FOERR_JSON_KEY_START_LOC "start_location"

// seriously though
#ifndef uint
typedef unsigned int uint;
#endif

enum GameState
{
	STATE_MAINMENU,
	STATE_PLAYING,
	STATE_PIPBUCK,
};

enum Direction
{
	DIR_LEFT,
	DIR_RIGHT,
	DIR_UP,
	DIR_DOWN,
};
