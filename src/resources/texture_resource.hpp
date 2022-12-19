#pragma once

#include <memory>
#include <SFML/Graphics/Texture.hpp>

/**
 * Used to track usage of resources in ResourceManager (shared ptr must be kept
 * in order not to unload Texture on ::cleanUnused() when it's actually being used).
 *
 * Note: we could get all fancy with templates, but it would be pretty annoying
 * to type Resource<sf::Texture, sf::Texture>, etc. everywhere.
 *
 * Note: this class can be considered pretty useless, as it could be easily replaced with just
 * std::shared_ptr<sf::Texture>. However it improves readability a bit by suggesting that a texture is managed by
 * Resource Manager.
 */
class TextureResource
{
	private:
		std::shared_ptr<sf::Texture> txt;

	public:
		TextureResource() { /* kappa */ }
		explicit TextureResource(std::shared_ptr<sf::Texture> txt);
		void set(std::shared_ptr<sf::Texture> txt);
		std::shared_ptr<sf::Texture> get() const;
		void clearPtr();
};
