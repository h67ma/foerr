#include "sprite_resource.hpp"

SpriteResource::SpriteResource(std::shared_ptr<sf::Texture> txt) : txt(txt)
{
	if (txt != nullptr)
		sf::Sprite::setTexture(*txt);
}

void SpriteResource::setTexture(std::shared_ptr<sf::Texture> txt)
{
	if (txt != nullptr)
		sf::Sprite::setTexture(*txt);

	this->txt = txt;
}

bool SpriteResource::isTextureSet() const
{
	return this->txt != nullptr;
}

void SpriteResource::clearPtr()
{
	this->txt = nullptr;
}
