#pragma once

#include <string>

#include <SFML/System/Vector2.hpp>

// representation of back object json node
struct back_obj_data
{
	std::string id;
	sf::Vector2u coordinates;
	int variantIdx;
};
