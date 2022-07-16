#pragma once

#include <SFML/Graphics.hpp>
#include "../consts.h"

class Animation : public sf::Drawable, public sf::Transformable
{
	private:
		sf::Texture texture;
		uint textureWidth;
		sf::Sprite sprite;
		sf::IntRect textureRect;
		sf::Clock clock;
		uint width;
		uint height;

	public:
		Animation(sf::Image textureImg, uint width, uint height);
		bool loadTexture();
		void maybeNextFrame();
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
