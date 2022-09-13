#pragma once

#include <memory>
#include <SFML/Audio/Sound.hpp>
#include <SFML/Audio/SoundBuffer.hpp>

/**
 * Used to track usage of resources in ResourceManager (shared ptr must be kept
 * in order not to unload SoundBuffer on ::cleanUnused() when it's actually being used).
 *
 * Note: we could get all fancy with templates, but it would be pretty annoying
 * to type Resource<sf::SoundBuffer, sf::Sound>, etc. everywhere.
 * Besides, we can conveniently set sf::Sound buffer here as a little bonus.
 */
class SoundResource
{
	private:
		std::shared_ptr<sf::SoundBuffer> buffer;
		sf::Sound sound;

	public:
		SoundResource() { /* kappa */ }
		SoundResource(std::shared_ptr<sf::SoundBuffer> buffer);
		void setBuffer(std::shared_ptr<sf::SoundBuffer> buffer);
		sf::Sound& get();
		void clear();
};
