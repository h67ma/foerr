#include "resource_manager.hpp"
#include "util/i18n.hpp"
#include "consts.h"

ResourceManager::ResourceManager(Log* log)
{
	if (!this->images[IMG_FIRE].loadFromFile(PATH_ENTITY_FIRE))
		log->log(LOG_ERROR, STR_IMG_LOAD_FAIL, PATH_ENTITY_FIRE);
}

sf::Image* ResourceManager::getImage(ImageName imgName)
{
	if (imgName >= _IMGS_CNT)
		return nullptr; // sorry

	return &this->images[imgName];
}
