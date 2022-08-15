#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "room_grid.hpp"
#include "../consts.hpp"
#include "../resource_manager.hpp"

// TODO? maybe inherit UniqueLocation and GeneratedLocation
class Location
{
	private:
		bool isGrind;
		bool isBasecamp;
		sf::Image *backgroundFull;
		RoomGrid rooms;

	public:
		bool load(std::string locDir, ResourceManager& resMgr);
};
