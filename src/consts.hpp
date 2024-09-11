// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/BlendMode.hpp>
#include <SFML/Graphics/Color.hpp>

const std::string CONFUSION = "???";

constexpr int ANIM_FRAME_DURATION_MS = 33; // around 30fps

// game area size
constexpr float GAME_AREA_WIDTH = 1920;
constexpr float GAME_AREA_HEIGHT = 1000;
constexpr float GAME_AREA_MID_X = 960;
constexpr float GAME_AREA_MID_Y = 500;

// paths
const std::string PATH_DIR_GAMEDATA = ".foerr";
const std::string PATH_DIR_SAVES = "savegames";
const std::string PATH_SETTINGS = "config.json";
const std::string PATH_KEYMAP = "keymap.json";
const std::string PATH_LOCATIONS_META = "locations.json";
const std::string PATH_LOGFILE = "foerr.log";
const std::string PATH_MATERIALS = "res/materials.json";
const std::string PATH_OBJS = "res/objs.json";
const std::string PATH_CURSOR_ARROW = "res/hud/cursor/cursor.png";
const std::string PATH_CURSOR_CROSS_WHITE = "res/hud/cursor/crosshair_white.png";
const std::string PATH_CURSOR_CROSS_YELLOW = "res/hud/cursor/crosshair_yellow.png";
const std::string PATH_CURSOR_CROSS_RED = "res/hud/cursor/crosshair_red.png";
const std::string PATH_TEXT_MISSING = "res/texture/missing.png";
const std::string PATH_BACKGROUNDS_FULL = "res/texture/full_backgrounds";
const std::string PATH_TEXT_CELLS = "res/texture/cells";
const std::string PATH_TEXT_OBJS_BACK = "res/texture/objects_back";
const std::string PATH_CAMPAIGNS = "res/campaigns";
const std::string PATH_DIR_ROOMS = "rooms";
const std::string FILENAME_INDEX = "_index.json";
const std::string PATH_WORLD_MAP = "hud/world_map.png";

// seriously though
#ifndef uint
typedef unsigned int uint;
#endif
#ifndef uchar
typedef unsigned char uchar;
#endif

using StringAndColor = std::pair<std::string, sf::Color>;

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

constexpr uchar COLOR_MAX_CHANNEL_VALUE = 0xFF;

// credits to oomek on https://en.sfml-dev.org/forums/index.php?topic=24250.msg164091#msg164091
const sf::BlendMode BLEND_ALPHA_TRANSPARENT(sf::BlendMode::One, sf::BlendMode::OneMinusSrcAlpha);

// makes a hole in dst.
// note: hole textures ("*_h") come in various colors, but only alpha channel seems to matter in Remains, and this
// blend mode works in a similar way.
const sf::BlendMode BLEND_SUBTRACT_OR_SOMETHING(sf::BlendMode::Zero, sf::BlendMode::OneMinusSrcAlpha);

// mixes src and dst where alpha != 0, makes src transparent where dst alpha == 0
// TODO? this looks pretty ok, but in the future could be replaced with a shader, or maybe some better blend mode
const sf::BlendMode BLEND_OVERLAY_OR_SOMETHING(sf::BlendMode::SrcColor, sf::BlendMode::One,
											   sf::BlendMode::ReverseSubtract, sf::BlendMode::OneMinusDstColor,
											   sf::BlendMode::One, sf::BlendMode::ReverseSubtract);
