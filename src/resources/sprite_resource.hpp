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
class SpriteResource
{
	private:
		std::shared_ptr<sf::Texture> txt;

	public:
		// can't use ::get() for SFML's ::draw() which requires a const value :/
		// member access should be used *only* in ::draw()
		// TODO there might be a better way to solve this, but I'm too sleepy to find it now
		sf::Sprite sprite;

		SpriteResource() { /* kappa */ };
		SpriteResource(std::shared_ptr<sf::Texture> txt);
		void setTexture(std::shared_ptr<sf::Texture> txt);
		sf::Sprite& get();
		void clearPtr();
};
