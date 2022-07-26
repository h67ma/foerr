#pragma once

#include <SFML/Graphics.hpp>
#include "hud/log.hpp"

enum ImageName
{
	SPRITESHEET_FIRE,
	SPRITESHEET_MCHAVI,
	_IMGS_CNT
};

class ResourceManager
{
	private:
		sf::Image images[_IMGS_CNT];
		void loadImg(Log* log, ImageName idx, std::string path);

	public:
		ResourceManager(Log* log);
		sf::Image* getImage(ImageName imgName);
};
