#pragma once

#include <SFML/Graphics/Transformable.hpp>

/**
 * A wrapper class for Transformable meant to be inherited by hud elements
 * which want to know their position with int precision.
 */
class HudTransformable : public sf::Transformable
{
	public:
		sf::Vector2i getIntPosition() const;
};
