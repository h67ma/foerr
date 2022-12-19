#include "texture_resource.hpp"

TextureResource::TextureResource(std::shared_ptr<sf::Texture> txt) : txt(txt)
{
	// "Fate has been cruel and order unkind, how can I have sent you away?"
}

void TextureResource::set(std::shared_ptr<sf::Texture> txt)
{
	this->txt = txt;
}

std::shared_ptr<sf::Texture> TextureResource::get() const
{
	return this->txt;
}

void TextureResource::clearPtr()
{
	this->txt = nullptr;
}
