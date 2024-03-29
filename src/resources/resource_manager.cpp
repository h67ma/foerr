// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "resource_manager.hpp"

#include <filesystem>
#include <string>

#include "../consts.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

// order matters!
const std::array<std::string, 3> FONTS = {
	"fonts/OpenSans-Regular.ttf", // FONT_NORMAL
	"fonts/OpenSans-Medium.ttf", // FONT_MEDIUM
	"fonts/Inconsolata-Regular.ttf" // FONT_FIXED
};

// clang-format off
const std::array<std::string, 5> TEXTURES_CORE = {
	PATH_TXT_PIPBUCK_OVERLAY,
	PATH_TXT_PIPBUCK_CAT_BTN_LEFT,
	PATH_TXT_PIPBUCK_CAT_BTN_MID,
	PATH_TXT_PIPBUCK_CAT_BTN_RIGHT,
	PATH_TXT_PIPBUCK_BG_RADIAL,
};
// clang-format on

const std::array<std::string, 3> AUDIO_CORE = {
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
		if (!this->fonts[i].loadFromFile(FONTS[i]))
		{
			Log::e(STR_FONT_LOAD_FAIL, FONTS[i].c_str());
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

	for (const auto& txt : TEXTURES_CORE)
	{
		if (!this->getTexture(txt))
			return false;
	}

	for (const auto& buf : AUDIO_CORE)
	{
		if (!this->getSoundBuffer(buf))
			return false;
	}

	std::shared_ptr<sf::Texture> missingTxt = this->getTexture(PATH_TEXT_MISSING, false);
	if (missingTxt == nullptr)
		return false; // missing texture could not be loaded

	missingTxt->setRepeated(true);
	this->notFoundTexture = missingTxt;

	Log::d(STR_LOADING_CORE_RES_DONE);

	return true;
}

/**
 * Loads a texture from specified path into resource manager object.
 * Pointer to the loaded texture is returned. If the texture is already
 * loaded, duplicate loading does not occur.
 *
 * @param path image resource path
 * @param returnSomething if true, and requested texture is not found, a dummy texture will be returned instead of
 * nullptr
 * @returns shared pointer to the loaded texture resource (can be `nullptr` if loading fails and !returnSomething)
 */
std::shared_ptr<sf::Texture> ResourceManager::getTexture(const std::string& path, bool returnSomething)
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
			return this->notFoundTexture;
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

std::shared_ptr<sf::Texture> ResourceManager::getNotFoundTexture() const
{
	return this->notFoundTexture;
}

/**
 * Loads audio from specified path into resource manager object.
 * Pointer to the loaded sound buffer is returned. If the buffer is already
 * loaded, duplicate loading does not occur.
 *
 * @param path audio resource path
 * @returns shared pointer to the loaded sound buffer resource (can be `nullptr` if loading fails)
 */
std::shared_ptr<sf::SoundBuffer> ResourceManager::getSoundBuffer(const std::string& path)
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

	for (auto it = this->textures.begin(); it != this->textures.end();)
	{
		if (it->second.use_count() <= 1) // the only shared ptr exists in res mgr itself
			it = this->textures.erase(it);
		else
			it++;
	}

	size_t cleaned = oldSize - this->textures.size();

	oldSize = this->audios.size();

	for (auto it = this->audios.begin(); it != this->audios.end();)
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
