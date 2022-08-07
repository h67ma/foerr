#include <SFML/Graphics.hpp>
#include "../hud/log.hpp"

#define ROOM_WIDTH_WITH_BORDER 7 // TODO 46
#define ROOM_HEIGHT_WITH_BORDER 5 // TODO 23

#define FOERR_JSON_KEY_ROOM_BLOCKS "blocks"
#define FOERR_JSON_KEY_ROOM_BACKGROUNDS "backgrounds"

/**
 * Room class is a representation of a part of a location that fits on a single screen.
 * It can't draw itself; it only stores the information read from room file.
 * Location that contains this room is responsible for reading the room's info and then
 * drawing it, spawning entitles, enemies, items, etc.
 */
class Room
{
	private:
		char blocks[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		char backgrounds[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		Log *log;
		bool loadArray(Json::Value &root, const char* key, const char* filename, char (*arr)[ROOM_WIDTH_WITH_BORDER], Log &log);

	public:
		bool load(std::string roomFilePath, Log &log);
};
