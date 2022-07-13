#pragma once

#include <SFML/Graphics.hpp>
#include "hud/log.hpp"

enum ImageName
{
	IMG_FIRE,
	_IMGS_CNT
};

class ResourceManager
{
	private:
		sf::Image images[_IMGS_CNT];

	public:
		ResourceManager(Log* log);
		sf::Image* getImage(ImageName imgName);
};
