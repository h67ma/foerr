#pragma once

#include <SFML/Graphics/Transformable.hpp>

#include "configurable_hud_component.hpp"

/**
 * A wrapper class for sf::Transformable, aware of GUI scale.
 *
 * When setting position, an internal "original position" will be saved, and position will be adjusted for GUI scale.
 * Another method, ::handleGuiScaleChange(), sets the position using saved "original position" and current GUI scale.
 */
class HudTransformable : public sf::Transformable, public ConfigurableHudComponent
{
	private:
		sf::Vector2f origPos;
		static sf::Vector2f calculateGuiAwarePosition(sf::Vector2f position);

	public:
		void setPosition(float x, float y);
		void setPosition(const sf::Vector2f& position);
		void handleGuiScaleChange();
};
