#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "room.hpp"
#include "../consts.hpp"
#include "../resource_manager.hpp"

// TODO? maybe inherit UniqueLocation and GeneratedLocation
class Location
{
	private:
		bool isGrind;
		bool isBasecamp;
		sf::Image *backgroundFull;
		std::vector<std::unique_ptr<Room>> rooms; // TODO class Vector2d storing also w, h, and with methods to get/set by (x, y)
		uint width; // number of rooms horizontally (including empty)
		uint height; // number of rooms vertically (including empty)

	public:
		bool load(std::string locDir, ResourceManager& resMgr);
};
