#include "sprite_resource.hpp"

SpriteResource::SpriteResource(std::shared_ptr<sf::Texture> txt) : txt(txt)
{
	if (txt != nullptr)
		this->sprite.setTexture(*txt);
}

void SpriteResource::setTexture(std::shared_ptr<sf::Texture> txt)
{
	if (txt != nullptr)
		this->sprite.setTexture(*txt);

	this->txt = txt;
}

sf::Sprite& SpriteResource::get()
{
	return this->sprite;
}

void SpriteResource::clearPtr()
{
	this->txt = nullptr;
}
