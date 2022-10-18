#include "hud_transformable.hpp"

sf::Vector2i HudTransformable::getIntPosition() const
{
	return static_cast<sf::Vector2i>(this->getPosition());
}
