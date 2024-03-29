// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022 h67ma <szycikm@gmail.com>

#include "sound_resource.hpp"

SoundResource::SoundResource(std::shared_ptr<sf::SoundBuffer> buffer) : buffer(buffer)
{
	if (buffer != nullptr)
		sf::Sound::setBuffer(*buffer);
}

void SoundResource::setBuffer(std::shared_ptr<sf::SoundBuffer> buffer)
{
	if (buffer != nullptr)
		sf::Sound::setBuffer(*buffer);

	this->buffer = buffer;
}

void SoundResource::clear()
{
	this->resetBuffer();
	this->buffer = nullptr;
}
