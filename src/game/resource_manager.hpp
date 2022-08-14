#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>

/**
 * The role of the ResourceManager is to basically avoid a situation where a resource
 * (e.g. image, audio file, etc.) is loaded multiple times.
 *
 * Care must be taken to ensure that the required resources are loaded.
 *
 * One approach would be to load all available resources immediately, however this would result
 * in needlessly high usage of memory and lots of resources left unused.
 *
 * A better approach works like this: each room defines resources it needs to use, e.g. it needs
 * certain images for texturing blocks. When a location loads its list of rooms, it compiles all
 * resources required by its rooms into a set. When a location is being loaded, it can request all
 * needed resources from resource manager, which then loads resources that are currently missing.
 * When loading next location, a different one than current one (excluding basecamps), we can
 * unload resources loaded by previous location. We can also skip unloading entirely, however
 * this should be implemented as a dev setting.
 *
 * Resource manager does not store text (json) files. These are loaded when the game/campaign is
 * being loaded and are not unloaded (unless unloading a whole campaign).
 *
 * There's also a group of resources that needs to be loaded all the time, e.g. fonts, some
 * translations, cursors, etc. Those resources need to be marked as "core" to prevent unloading
 * them at any point.
 *
 * Resource files are identified by their path in filesystem. The same string is used for loading
 * and getting resources.
 */
class ResourceManager
{
	private:
		std::unordered_map<std::string, sf::Image*> images;
		// TODO std::unordered_map<std::string, sf::SoundBuffer*> audios;
		// TODO std::unordered_map<char, sf::Image*> charToBlock; // map for getting image resource to draw blocks
		// TODO std::unordered_map<char, sf::Image*> charToBackground; // map for getting image resource to draw backgrounds

	public:
		sf::Image* loadImage(std::string path);
		// TODO bool loadAudio(std::string path);
		sf::Image* getImage(std::string path);
		// TODO sf::SoundBuffer* getAudio(std::string path);
		~ResourceManager();
};
