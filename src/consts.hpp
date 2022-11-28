#pragma once

#include <SFML/System/Vector3.hpp>

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
#define PATH_LOCATIONS_META "locations.json"
#define PATH_LOGFILE "foerr.log"
#define PATH_CURSOR_ARROW "res/hud/cursor/cursor.png"
#define PATH_CURSOR_CROSS_WHITE "res/hud/cursor/crosshair_white.png"
#define PATH_CURSOR_CROSS_YELLOW "res/hud/cursor/crosshair_yellow.png"
#define PATH_CURSOR_CROSS_RED "res/hud/cursor/crosshair_red.png"
#define PATH_DIR_ROOMS "rooms"
#define FILENAME_INDEX "_index.json"

// seriously though
#ifndef uint
typedef unsigned int uint;
#endif

typedef sf::Vector3<unsigned int> Vector3u;

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
	DIR_FRONT,
	DIR_BACK, // the backrooms o_O
};
