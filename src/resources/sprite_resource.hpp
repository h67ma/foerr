#pragma once

#include <memory>

#include <SFML/Graphics/Texture.hpp>
#include <SFML/Graphics/Sprite.hpp>

/**
 * Used to track usage of resources in ResourceManager (shared ptr must be kept
 * in order not to unload Texture on ::cleanUnused() when it's actually being used).
 *
 * Note: we could get all fancy with templates, but it would be pretty annoying
 * to type Resource<sf::Texture, sf::Sprite>, etc. everywhere.
 * Besides, we can conveniently set sprite's texture here here as a little bonus.
 */
class SpriteResource : public sf::Sprite
{
	private:
		std::shared_ptr<sf::Texture> txt;

	public:
		SpriteResource() { /* kappa */ }
		explicit SpriteResource(std::shared_ptr<sf::Texture> txt);
		void setTexture(std::shared_ptr<sf::Texture> txt);
		bool isTextureSet() const;
		void clearPtr();
};
