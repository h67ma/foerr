#pragma once

#include <memory>
#include <unordered_map>
#include <string>
#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

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
 * The role of the ResourceManager is to basically avoid a situation where a resource
 * (e.g. texture, audio file, etc.) is loaded multiple times.
 *
 * Care must be taken to ensure that the required resources are loaded.
 *
 * One approach would be to load all available resources immediately, however this would result
 * in needlessly high usage of memory and lots of resources left unused in most cases.
 *
 * A better approach works like this: each room defines resources it needs to use, e.g. it needs
 * certain textures for texturing cells. When a location loads its list of rooms, it compiles all
 * resources required by its rooms into a set. When a location is being loaded, it can request all
 * needed resources from resource manager, which then loads resources that are currently missing.
 * When loading next location, a different one than current one (excluding basecamps), we can
 * unload resources loaded by previous location. We can also skip unloading entirely, however
 * this should be implemented as a dev setting, if at all (TODO?).
 *
 * Shared pointers are used to keep track of different resources. This way the Resource Manager
 * does not own the resources, but rather provides unified access to them and prevents duplication.
 * Because actual usage of resources is tracked by shared pointers, we can easily remove unused
 * resources, e.g. when jumping between locations, or when unloading a campaign.
 *
 * Care must be taken to ensure that resources that are being used are not deallocated.
 * It is recommended to use the correct wrapper class (SoundResource/SpriteResource), or store
 * a shared pointer to the needed resource as a private member, in addition to passing it to
 * some SFML object (because SFML objects typically use references, so shared ptr usage count
 * would be lost).
 *
 * Resource manager does not store text (json) resources.
 *
 * Resource files are identified by their path in filesystem. The same string is used for loading
 * and getting resources.
 *
 * There's a group of resources that need to be loaded all the time, e.g. some textures.
 * Paths of these resources are hardcoded. They must be loaded before starting the game.
 * Game should not continue if any of those "core" resources is missing.
 *
 * Fonts are special cases of core resources, i.e. they are stored differently (in arrays)
 * and are never unloaded, therefore their usage is not tracked with smart pointers.
 *
 * Cursors are handled separately via the CursorManager class.
 *
 * TODO? res mgr could potentially be made into a "static class", same as with Log, to avoid
 * passing it along everywhere.
 */
class ResourceManager
{
	private:
		sf::Font fonts[_FONT_CNT]; // NOLINT(runtime/arrays)
		std::unordered_map<std::string, std::shared_ptr<sf::Texture>> textures;

		// it would be more convenient to store sf::Sound, however then we'd be unable
		// to play the same sound multiple times at the same time, which will definitely happen
		std::unordered_map<std::string, std::shared_ptr<sf::SoundBuffer>> audios;

	public:
		bool loadFonts();
		bool loadCore();
		std::shared_ptr<sf::Texture> getTexture(std::string path);
		std::shared_ptr<sf::SoundBuffer> getSoundBuffer(std::string path);
		sf::Font* getFont(FontType fontType);
		void cleanUnused();
};
