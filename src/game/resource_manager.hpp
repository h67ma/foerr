#pragma once

#include <SFML/Graphics.hpp>
#include "hud/log.hpp"

enum ImageName
{
	SPRITESHEET_FIRE,
	SPRITESHEET_MCHAVI,
	_IMGS_CNT
};

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
 * There's also a group of resources that needs to be loaded all the time, e.g. fonts, some
 * translations, cursors, etc. Those resources need to be marked as "core" to prevent unloading
 * them at any point.
 */
class ResourceManager
{
	private:
		sf::Image images[_IMGS_CNT];
		void loadImg(Log* log, ImageName idx, std::string path);

	public:
		ResourceManager(Log* log);
		sf::Image* getImage(ImageName imgName);
};
