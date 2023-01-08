#pragma once

#include <SFML/Graphics/BlendMode.hpp>

#define STACKTRACE_MAX_CNT 10
#define ANIM_FRAME_DURATION_MS 33 // around 30fps

// game area size
#define GAME_AREA_WIDTH 1920
#define GAME_AREA_HEIGHT 1000
#define GAME_AREA_MID_X 960.f
#define GAME_AREA_MID_Y 500.f

// paths
#define PATH_DIR_GAMEDATA ".foerr"
#define PATH_DIR_SAVES "savegames"
#define PATH_SETTINGS "config.json"
#define PATH_KEYMAP "keymap.json"
#define PATH_LOCATIONS_META "locations.json"
#define PATH_LOGFILE "foerr.log"
#define PATH_MATERIALS "res/materials.json"
#define PATH_OBJS "res/objs.json"
#define PATH_CURSOR_ARROW "res/hud/cursor/cursor.png"
#define PATH_CURSOR_CROSS_WHITE "res/hud/cursor/crosshair_white.png"
#define PATH_CURSOR_CROSS_YELLOW "res/hud/cursor/crosshair_yellow.png"
#define PATH_CURSOR_CROSS_RED "res/hud/cursor/crosshair_red.png"
#define PATH_TEXT_MISSING "res/texture/missing.png"
#define PATH_BACKGROUNDS_FULL "res/texture/full_backgrounds"
#define PATH_TEXT_CELLS "res/texture/cells"
#define PATH_TEXT_OBJS "res/texture/objects"
#define PATH_DIR_ROOMS "rooms"
#define FILENAME_INDEX "_index.json"

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
	DIR_FRONT,
	DIR_BACK, // the backrooms o_O
};

// credits to oomek on https://en.sfml-dev.org/forums/index.php?topic=24250.msg164091#msg164091
const sf::BlendMode BlendAlphaTransparent(sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha);

// makes a hole in dst
const sf::BlendMode BlendSubtractOrSomething(sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcAlpha);

// mixes src and dst where alpha != 0, makes src transparent where dst alpha == 0
// TODO? this looks pretty ok, but in the future could be replaced with a shader, or maybe some better blend mode
const sf::BlendMode BlendOverlayOrSomething(sf::BlendMode::SrcColor, sf::BlendMode::One,
											sf::BlendMode::ReverseSubtract, sf::BlendMode::OneMinusDstColor,
											sf::BlendMode::One, sf::BlendMode::ReverseSubtract);
