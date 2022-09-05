#include "resource_manager.hpp"
#include "util/i18n.hpp"
#include "../hud/log.hpp"
#include "consts.hpp"

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
	PATH_AUD_PIPBUCK_OPENCLOSE
};

/**
 * Loads essential resources, such as fonts.
 * If the loading fails, then the program must be exited.
 *
 * @returns `true` if the loading succeeded, `false` otherwise
 */
bool ResourceManager::loadCore()
{
	for (size_t i = 0; i < _FONT_CNT; i++)
	{
		if (!this->fonts[i].loadFromFile(_fonts[i]))
		{
			Log::e(STR_FONT_LOAD_FAIL, _fonts[i]);
			return false;
		}
	}

	Log::d(STR_LOADING_CORE_RES);

	// preload core textures
	for (const char* txt : _coreTextures)
	{
		if (!this->getTexture(txt, true))
			return false;
	}

	// preload core audio
	for (const char* buf : _coreAudio)
	{
		if (!this->getSoundBuffer(buf, true))
			return false;
	}

	Log::d(STR_LOADING_CORE_RES_DONE);

	return true;
}

/**
 * Loads a texture from specified path into resource manager object.
 * Pointer to the loaded texture is returned. If the texture is already
 * loaded, "duplicate" loading does not occur.
 *
 * @param path image resource path
 * @param isCoreRes `true` if this resource should not be unloaded when unloading campaigns, `false` otherwise. Has no meaning if the texture is already loaded.
 * @returns pointer to the loaded texture resource, or `nullptr` if loading failed
 */
sf::Texture* ResourceManager::getTexture(std::string path, bool isCoreRes)
{
	auto search = this->textures.find(path);
	if (search != this->textures.end())
		return search->second.payload.get(); // resource already loaded

	std::unique_ptr<sf::Texture> txt = std::make_unique<sf::Texture>();
	if (!txt->loadFromFile(path))
	{
		Log::e(STR_LOAD_FAIL, path.c_str());
		return nullptr;
	}

	txt->setSmooth(true);

	Log::v(STR_LOADED_FILE, path.c_str());

	// TODO refactor
	this->textures[path].payload = std::move(txt);
	this->textures[path].isCoreRes = isCoreRes;
	return this->textures[path].payload.get();
}

/**
 * Loads audio from specified path into resource manager object.
 * Pointer to the loaded sound buffer is returned. If the buffer is already
 * loaded, "duplicate" loading does not occur.
 *
 * @param path audio resource path
 * @param isCoreRes `true` if this resource should not be unloaded when unloading campaigns, `false` otherwise. Has no meaning if the sound is already loaded.
 * @return pointer to the loaded sound buffer resource, or `nullptr` if loading failed
 */
sf::SoundBuffer* ResourceManager::getSoundBuffer(std::string path, bool isCoreRes)
{
	auto search = this->audios.find(path);
	if (search != this->audios.end())
		return search->second.payload.get(); // resource already loaded

	std::unique_ptr<sf::SoundBuffer> buf = std::make_unique<sf::SoundBuffer>();
	if (!buf->loadFromFile(path))
	{
		Log::e(STR_LOAD_FAIL, path.c_str());
		return nullptr;
	}

	Log::v(STR_LOADED_FILE, path.c_str());

	// TODO refactor
	this->audios[path].payload = std::move(buf);
	this->audios[path].isCoreRes = isCoreRes;
	return this->audios[path].payload.get();
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
 * Unloads all resources marked as "not core".
 * Used to clear resources used by campaign after unloading a campaign.
 */
void ResourceManager::clearAllNonCore()
{
	// note: alternatively, shared pointers could be used to track actual usage of
	// resources, instead of manually marking them as core/not core. However this
	// doesn't really give any benefits over current approach, as realistically we'll only
	// ever want to unload campaign-specific resources, and in addition shared pointers
	// add a bit of unnecessary overhead.

	size_t oldSize = this->textures.size();

	for (auto it = this->textures.begin(); it != this->textures.end(); )
	{
		if (!it->second.isCoreRes)
			it = this->textures.erase(it);
		else
			it++;
	}

	size_t cleaned = oldSize - this->textures.size();

	oldSize = this->audios.size();

	for (auto it = this->audios.begin(); it != this->audios.end(); )
	{
		if (!it->second.isCoreRes)
			it = this->audios.erase(it);
		else
			it++;
	}

	cleaned += (oldSize - this->audios.size());

	Log::d(STR_CLEANED_UNUSED_RES, cleaned);

	// TODO same for other non core res
}
