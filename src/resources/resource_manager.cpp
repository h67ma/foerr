#include "resource_manager.hpp"
#include <filesystem>
#include <string>
#include "../util/i18n.hpp"
#include "../hud/log.hpp"
#include "../consts.hpp"

// order matters!
const char* _fonts[] = {
	"res/fonts/OpenSans-Regular.ttf",	// FONT_NORMAL
	"res/fonts/OpenSans-Medium.ttf",	// FONT_MEDIUM
	"res/fonts/Inconsolata-Regular.ttf"	// FONT_FIXED
};

const char* _coreTextures[] = {
	PATH_TXT_PIPBUCK_OVERLAY
};

const char* _coreAudio[] = {
	PATH_AUD_PIPBUCK_OPENCLOSE,
	PATH_AUD_PIPBUCK_PAGECHANGE,
	PATH_AUD_PIPBUCK_PAGE_CLICK,
};

/**
 * Loads fonts.
 * If the loading fails, then the program must be exited.
 *
 * @returns `true` if the loading succeeded, `false` otherwise
 */
bool ResourceManager::loadFonts()
{
	for (size_t i = 0; i < _FONT_CNT; i++)
	{
		if (!this->fonts[i].loadFromFile(_fonts[i]))
		{
			Log::e(STR_FONT_LOAD_FAIL, _fonts[i]);
			return false;
		}
	}

	return true;
}

/**
 * Preloads essential resources, except fonts.
 * If the loading fails, then the program must be exited.
 *
 * @returns `true` if the loading succeeded, `false` otherwise
 */
bool ResourceManager::loadCore()
{
	Log::d(STR_LOADING_CORE_RES);

	for (const char* txt : _coreTextures)
	{
		if (!this->getTexture(txt))
			return false;
	}

	for (const char* buf : _coreAudio)
	{
		if (!this->getSoundBuffer(buf))
			return false;
	}

	std::shared_ptr<sf::Texture> missingTxt = this->getTexture(PATH_TEXT_MISSING, false);
	if (missingTxt == nullptr)
		return false; // missing texture could not be loaded

	missingTxt->setRepeated(true);
	this->notFoundTexture.set(missingTxt);

	Log::d(STR_LOADING_CORE_RES_DONE);

	return true;
}

/**
 * Loads a texture from specified path into resource manager object.
 * Pointer to the loaded texture is returned. If the texture is already
 * loaded, duplicate loading does not occur.
 *
 * @param path image resource path
 * @param returnSomething if true, and requested texture is not found, a dummy texture will be returned instead of nullptr
 * @returns shared pointer to the loaded texture resource (can be `nullptr` if loading fails and !returnSomething)
 */
std::shared_ptr<sf::Texture> ResourceManager::getTexture(std::string path, bool returnSomething)
{
	auto search = this->textures.find(path);
	if (search != this->textures.end())
		return search->second; // resource already loaded

	std::shared_ptr<sf::Texture> txt = std::make_shared<sf::Texture>();
	if (!std::filesystem::exists(path) || !txt->loadFromFile(path))
	{
		if (returnSomething)
		{
			// do not output a warning as it would produce way too much spam. the problem will be clearly visible anyway
			return this->notFoundTexture.get();
		}
		else
		{
			Log::e(STR_LOAD_FAIL, path.c_str());
			return nullptr;
		}
	}

	txt->setSmooth(true);

	Log::v(STR_LOADED_FILE, path.c_str());

	this->textures[path] = txt;
	return txt;
}

/**
 * Loads audio from specified path into resource manager object.
 * Pointer to the loaded sound buffer is returned. If the buffer is already
 * loaded, duplicate loading does not occur.
 *
 * @param path audio resource path
 * @returns shared pointer to the loaded sound buffer resource (can be `nullptr` if loading fails)
 */
std::shared_ptr<sf::SoundBuffer> ResourceManager::getSoundBuffer(std::string path)
{
	auto search = this->audios.find(path);
	if (search != this->audios.end())
		return search->second; // resource already loaded

	std::shared_ptr<sf::SoundBuffer> buf = std::make_shared<sf::SoundBuffer>();
	if (!buf->loadFromFile(path))
	{
		Log::e(STR_LOAD_FAIL, path.c_str());
		return nullptr;
	}

	Log::v(STR_LOADED_FILE, path.c_str());

	this->audios[path] = buf;
	return buf;
}

/**
 * @param fontType font type enum
 * @returns pointer to font resource if it's loaded and fontType is valid, `nullptr` otherwise.
 */
sf::Font* ResourceManager::getFont(FontType fontType)
{
	if (fontType >= _FONT_CNT)
		return nullptr;

	return &this->fonts[fontType];
}

/**
 * Unloads all unused resources.
 * Used to clear resources used by campaign after unloading a location, or whole campaign.
 */
void ResourceManager::cleanUnused()
{
	size_t oldSize = this->textures.size();

	for (auto it = this->textures.begin(); it != this->textures.end(); )
	{
		if (it->second.use_count() <= 1) // the only shared ptr exists in res mgr itself
			it = this->textures.erase(it);
		else
			it++;
	}

	size_t cleaned = oldSize - this->textures.size();

	oldSize = this->audios.size();

	for (auto it = this->audios.begin(); it != this->audios.end(); )
	{
		if (it->second.use_count() <= 1) // the only shared ptr exists in res mgr itself
			it = this->audios.erase(it);
		else
			it++;
	}

	cleaned += (oldSize - this->audios.size());

	Log::d(STR_CLEANED_UNUSED_RES, cleaned);

	// TODO same for other non core res
}
