#pragma once

#include <string>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Color.hpp>

enum MaterialType
{
	MAT_SOLID = 1,
	MAT_BG = 2,
	MAT_LADDER = 3,
	MAT_PLATFORM = 4,
	MAT_STAIRS = 5,
	MAT_LIQUID = 6,
};

struct material
{
	enum MaterialType type;
	std::string texturePath;
	std::string textureDelimPath;
	std::string maskTexturePath;
	bool isRight; // stairs, ladders
	int offsetLeft; // can be negative
	sf::Vector2i delimOffset;
	sf::Color color;
};
