#pragma once

#define STACKTRACE_MAX_CNT 10
#define ANIM_FRAME_DURATION_MS 33 // around 30fps

// game area size
#define GAME_AREA_WIDTH 1920.f
#define GAME_AREA_HEIGHT 1000.f
#define GAME_AREA_MID_X 960.f
#define GAME_AREA_MID_Y 500.f

// paths
#define PATH_SETTINGS "config.json"
#define PATH_LOGFILE "foerr.log"
#define PATH_CURSOR_ARROW "res/hud/cursor/cursor.png"
#define PATH_CURSOR_CROSS_WHITE "res/hud/cursor/crosshair_white.png"
#define PATH_CURSOR_CROSS_YELLOW "res/hud/cursor/crosshair_yellow.png"
#define PATH_CURSOR_CROSS_RED "res/hud/cursor/crosshair_red.png"
#define PATH_PIPBUCK_OVERLAY "res/hud/pipbuck.png"
#define DIR_LOCATIONS "locations"
#define FILENAME_INDEX "_index.json"
#define EXT_JSON ".json"

// json keys
#define FOERR_JSON_KEY_BLOCKS "blocks"
#define FOERR_JSON_KEY_BACKGROUNDS "backgrounds"
#define FOERR_JSON_KEY_BACKGROUND_FULL "background_full"
#define FOERR_JSON_KEY_TYPE_GRIND "grind"
#define FOERR_JSON_KEY_TYPE_BASECAMP "basecamp"
#define FOERR_JSON_KEY_WIDTH "width"
#define FOERR_JSON_KEY_HEIGHT "height"
#define FOERR_JSON_KEY_ROOM_MAP "room_map"
#define FOERR_JSON_KEY_TITLE "title"
#define FOERR_JSON_KEY_DESCRIPTION "description"
#define FOERR_JSON_KEY_START_LOC "startLocation"

// font sizes
#define FONT_SIZE_SMALL 14U
#define FONT_SIZE_SMALL_WITH_GAP 16U
#define FONT_SIZE_NORMAL 18U
#define FONT_SIZE_NORMAL_WITH_GAP 20U
#define FONT_SIZE_LARGE 24U
#define FONT_SIZE_LARGE_WITH_GAP 26U

#define ROOM_EMPTY "/EMPTY" // '/' should be an illegal filename character in both linux and windows

// seriously though
#ifndef uint
#define uint unsigned int
#endif

enum GameState
{
	STATE_LOADINGSCREEN,
	STATE_MAINMENU,
	STATE_PLAYING,
	STATE_PIPBUCK,
};
