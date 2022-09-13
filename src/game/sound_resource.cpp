#include "sound_resource.hpp"

SoundResource::SoundResource(std::shared_ptr<sf::SoundBuffer> buffer) : buffer(buffer)
{
	this->sound.setBuffer(*buffer);
}

void SoundResource::setBuffer(std::shared_ptr<sf::SoundBuffer> buffer)
{
	this->sound.setBuffer(*buffer);
	this->buffer = buffer;
}

sf::Sound& SoundResource::get()
{
	return this->sound;
}

void SoundResource::clear()
{
	this->sound.resetBuffer();
	this->buffer = nullptr;
}
