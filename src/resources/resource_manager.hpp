#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>
#include "texture_resource.hpp"

// core textures
#define PATH_TXT_PIPBUCK_OVERLAY "res/hud/pipbuck.png"

// core audio
#define PATH_AUD_PIPBUCK_OPENCLOSE "res/audio/pipbuck_openclose.wav"
#define PATH_AUD_PIPBUCK_PAGECHANGE "res/audio/pipbuck_change_page.wav"
#define PATH_AUD_PIPBUCK_PAGE_CLICK "res/audio/pipbuck_click.wav"

enum FontType
{
	FONT_NORMAL,
	FONT_MEDIUM,
	FONT_FIXED,
	_FONT_CNT
};

/**
 * The role of the ResourceManager is to avoid resource (e.g. textures, audio files, etc.) duplication in memory.
 *
 * Care must be taken to ensure that the required resources are loaded, but also that we don't keep unused resources
 * loaded.
 *
 * The current approach works like this: various elements everywhere in the game define the resources they need - e.g.
 * certain textures to draw self. These elements can request the needed resources from Resource Manager, which will
 * load them if they are not already loaded, and return a shared pointer. Elements using such resources must store a
 * shared_ptr to the desired resource, to prevent it from being deallocated. Utility classes such as SpriteResource are
 * provided to make it clear that certain resource is managed by ResourceManager.
 *
 * Deallocation of resources can happen e.g. when the player changes locations and game decides to unload the old
 * location. Resource Manager then goes through all managed resources (::cleanUnused()) and checks which ones are unused
 * (via shared_ptr usage count).
 *
 * Resource Manager does not store text (json) resources - they should be loaded by specialized classes which validate
 * their structure and provide convenient methods for getting/modifying data.
 *
 * Resource files are identified by their path in filesystem. The same string is used for loading and getting resources.
 *
 * There's a group of resources that need to be loaded all the time, e.g. some textures. Paths of these resources are
 * hardcoded. They must be loaded before starting the game. Game should not continue if any of those "core" resources
 * is missing.
 *
 * Fonts are special cases of core resources, i.e. they are stored differently (in an array), and are never unloaded,
 * therefore their usage is not tracked with smart pointers.
 *
 * Cursors are handled separately via the CursorManager class.
 *
 * TODO? res mgr could potentially be made into a "static class", same as with Log, to avoid passing it along everywhere
 */
class ResourceManager
{
	private:
		sf::Font fonts[_FONT_CNT]; // NOLINT(runtime/arrays)
		std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures;

		// returned when requested texture could not be loaded. ptr stored here in order to always keep it loaded.
		TextureResource notFoundTexture;

		// it would be more convenient to store sf::Sound, however then we'd be unable
		// to play the same sound multiple times at the same time, which will definitely happen
		std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> audios;

	public:
		bool loadFonts();
		bool loadCore();
		std::shared_ptr<sf::Texture> getTexture(const std::string &path, bool returnSomething = true);
		std::shared_ptr<sf::Texture> getNotFoundTexture() const;
		std::shared_ptr<sf::SoundBuffer> getSoundBuffer(const std::string &path);
		sf::Font* getFont(FontType fontType);
		void cleanUnused();
};
