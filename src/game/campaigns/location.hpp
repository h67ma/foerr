#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "room_grid.hpp"
#include "../consts.hpp"
#include "../resource_manager.hpp"

// TODO? maybe inherit UniqueLocation and GeneratedLocation
class Location : public sf::Drawable
{
	private:
		bool isGrind;
		bool isBasecamp;
		sf::Sprite backgroundFullSprite;
		RoomGrid rooms;

	public:
		bool load(std::string locDir, ResourceManager& resMgr);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
