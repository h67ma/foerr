#include <SFML/Graphics.hpp>
#include <json/json.h>
#include "../hud/log.hpp"

#define ROOM_WIDTH_WITH_BORDER 7 // TODO 46
#define ROOM_HEIGHT_WITH_BORDER 5 // TODO 23

// TODO rooms CAN and WILL appear multiple times per location, especially in grind
// locations, so Room objects would either have to be cloned (easy), or we'd need to
// create some kind of RoomInstance class or whatever

/**
 * Room class is a representation of a part of a location that fits on a single screen.
 */
class Room
{
	private:
		char blocks[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		char backgrounds[ROOM_HEIGHT_WITH_BORDER][ROOM_WIDTH_WITH_BORDER];
		bool loadArray(Json::Value &root, const char* key, const char* filename, char (*arr)[ROOM_WIDTH_WITH_BORDER], Log &log);

	public:
		bool load(std::string roomFilePath, Log &log);
};
