#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "resource.hpp"

// core textures
#define PATH_TXT_PIPBUCK_OVERLAY "res/hud/pipbuck.png"

// core audio
#define PATH_AUD_PIPBUCK_OPENCLOSE "res/audio/pipbuck_openclose.wav"

enum FontType
{
	FONT_NORMAL,
	FONT_MEDIUM,
	FONT_FIXED,
	_FONT_CNT
};

/**
 * The role of the ResourceManager is to basically avoid a situation where a resource
 * (e.g. texture, audio file, etc.) is loaded multiple times.
 *
 * Care must be taken to ensure that the required resources are loaded.
 *
 * One approach would be to load all available resources immediately, however this would result
 * in needlessly high usage of memory and lots of resources left unused.
 *
 * A better approach works like this: each room defines resources it needs to use, e.g. it needs
 * certain textures for texturing blocks. When a location loads its list of rooms, it compiles all
 * resources required by its rooms into a set. When a location is being loaded, it can request all
 * needed resources from resource manager, which then loads resources that are currently missing.
 * When loading next location, a different one than current one (excluding basecamps), we can
 * unload resources loaded by previous location. We can also skip unloading entirely, however
 * this should be implemented as a dev setting.
 *
 * Resource manager does not store text (json) files. These are loaded when the game/campaign is
 * being loaded and are not unloaded (unless unloading a whole campaign).
 *
 * Resource files are identified by their path in filesystem. The same string is used for loading
 * and getting resources.
 *
 * There's also a group of resources that need to be loaded all the time, e.g. some textures.
 * These resources need to be marked as "core" to prevent unloading them at any point.
 * Paths of core resources are hardcoded. They must be loaded immediately after creating the
 * resource manager.
 *
 * Fonts are special cases of core resources, i.e. they are stored differently (in arrays).
 * Cursors are handled separately via the CursorManager class.
 *
 * TODO? res mgr could potentially be made into a "static class", same as with Log, to avoid
 * passing it along everywhere.
 */
class ResourceManager
{
	private:
		sf::Font fonts[_FONT_CNT];
		std::unordered_map<std::string, Resource<sf::Texture>> textures;
		std::unordered_map<std::string, Resource<sf::SoundBuffer>> audios;
		// TODO std::unordered_map<char, sf::Texture*> charToBlock; // map for getting texture resource to draw blocks
		// TODO std::unordered_map<char, sf::Texture*> charToBackground; // map for getting texture resource to draw backgrounds

	public:
		bool loadCore();
		// TODO? presence of isCoreRes in get method might be a bit confusing, maybe thing about having separate load/get methods
		sf::Texture* getTexture(std::string path, bool isCoreRes=true);
		sf::SoundBuffer* getSoundBuffer(std::string path, bool isCoreRes=true);
		sf::Font* getFont(FontType fontType);
		void clearAllNonCore();
};
