#pragma once

#include <SFML/Graphics/Drawable.hpp>

#include "../resources/resource_manager.hpp"
#include "animation.hpp"

class Player : public sf::Drawable, public sf::Transformable
{
	private:
		Animation animation;

	public:
		explicit Player(ResourceManager &resMgr);
		void nextFrame();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
