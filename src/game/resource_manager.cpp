#include "resource_manager.hpp"
#include "util/i18n.hpp"
#include "../hud/log.hpp"
#include "consts.hpp"

/**
 * Loads image from specified path into resource manager object.
 * This image resource will be accessible via the `getImage` method.
 *
 * @param path image resource path
 * @returns pointer to the loaded image resource, or `nullptr` if loading failed
 */
sf::Image* ResourceManager::loadImage(std::string path)
{
	auto search = this->images.find(path);
	if (search != this->images.end())
		return search->second; // resource already loaded

	sf::Image *img = new sf::Image();
	if (!img->loadFromFile(path))
	{
		Log::log(Log::LOG_DEBUG, STR_IMG_LOAD_FAIL, path.c_str());
		delete img;
		return nullptr;
	}

	Log::log(Log::LOG_DEBUG, STR_LOADED_FILE, path.c_str());
	this->images[path] = img;
	return img;
}

/**
 * @param path image resource path
 * @returns pointer to image resource, or `nullptr` if specified resource is not currently loaded
 */
sf::Image* ResourceManager::getImage(std::string path)
{
	if (this->images.find(path) == this->images.end())
		return nullptr;

	return this->images[path];
}

ResourceManager::~ResourceManager()
{
	for (const auto &img : this->images)
	{
		delete img.second;
	}
}
