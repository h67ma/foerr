#pragma once

#define STACKTRACE_MAX_CNT 10
#define ANIM_FRAME_DURATION_MS 33 // around 30fps

// paths
#define PATH_SETTINGS "config.json"
#define PATH_LOGFILE "foerr.log"
#define PATH_CURSOR_ARROW "res/hud/cursor.png"
#define PATH_CURSOR_CROSS_WHITE "res/hud/crosshair_white.png"
#define PATH_CURSOR_CROSS_YELLOW "res/hud/crosshair_yellow.png"
#define PATH_CURSOR_CROSS_RED "res/hud/crosshair_red.png"
#define PATH_FONT_NORMAL "res/fonts/OpenSans-Regular.ttf"
#define PATH_FONT_MEDIUM "res/fonts/OpenSans-Medium.ttf"
#define PATH_FONT_FIXED "res/fonts/Inconsolata-Regular.ttf"
#define PATH_SPRITESHEET_FIRE "res/entities/fire.png"
#define PATH_SPRITESHEET_MCHAVI "res/entities/mchavi.png"
#define DIR_LOCATIONS "locations"
#define FILENAME_INDEX "_index.json"

// json keys
#define FOERR_JSON_KEY_BLOCKS "blocks"
#define FOERR_JSON_KEY_BACKGROUNDS "backgrounds"
#define FOERR_JSON_KEY_BACKGROUND_FULL "background_full"
#define FOERR_JSON_KEY_TYPE_GRIND "grind"
#define FOERR_JSON_KEY_TYPE_BASECAMP "basecamp"
#define FOERR_JSON_KEY_WIDTH "width"
#define FOERR_JSON_KEY_HEIGHT "height"
#define FOERR_JSON_KEY_ROOM_MAP "room_map"

// font sizes
#define FONT_SIZE_NORMAL 18
#define FONT_SIZE_NORMAL_WITH_GAP 20

#define ROOM_EMPTY "/EMPTY" // '/' should be an illegal filename character in both linux and windows

// seriously though
#ifndef uint
#define uint unsigned int
#endif
