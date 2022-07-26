#include "resource_manager.hpp"
#include "util/i18n.hpp"
#include "consts.h"

ResourceManager::ResourceManager(Log* log)
{
	// any image we won't or can't load will be shown as white rectangle, could think of adding some fallback img
	this->loadImg(log, SPRITESHEET_FIRE, PATH_SPRITESHEET_FIRE);
	this->loadImg(log, SPRITESHEET_MCHAVI, PATH_SPRITESHEET_MCHAVI);
}

void ResourceManager::loadImg(Log* log, ImageName idx, std::string path)
{
	if (!this->images[idx].loadFromFile(path))
		log->log(LOG_ERROR, STR_IMG_LOAD_FAIL, path);
}

sf::Image* ResourceManager::getImage(ImageName imgName)
{
	if (imgName >= _IMGS_CNT)
		return nullptr; // sorry

	return &this->images[imgName];
}
