#include "resource_manager.hpp"
#include "util/i18n.hpp"
#include "../hud/log.hpp"
#include "consts.hpp"

/**
 * Loads a texture from specified path into resource manager object.
 * This texture resource will be accessible via the `getTexture` method.
 *
 * @param path image resource path
 * @returns pointer to the loaded texture resource, or `nullptr` if loading failed
 */
sf::Texture* ResourceManager::loadTexture(std::string path)
{
	auto search = this->textures.find(path);
	if (search != this->textures.end())
		return search->second.get(); // resource already loaded

	sf::Texture *txt = new sf::Texture();
	if (!txt->loadFromFile(path))
	{
		Log::e(STR_IMG_LOAD_FAIL, path.c_str());
		delete txt;
		return nullptr;
	}

	txt->setSmooth(true);

	Log::v(STR_LOADED_FILE, path.c_str());
	this->textures[path] = std::unique_ptr<sf::Texture>(txt);
	return txt;
}

/**
 * @param path image resource path
 * @returns pointer to texture resource, or `nullptr` if specified resource is not currently loaded
 */
sf::Texture* ResourceManager::getTexture(std::string path)
{
	if (this->textures.find(path) == this->textures.end())
		return nullptr;

	return this->textures[path].get();
}
