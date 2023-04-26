#pragma once

#include <SFML/Graphics/Transformable.hpp>

#include "configurable_gui_component.hpp"

/**
 * A wrapper class for sf::Transformable, aware of GUI scale.
 *
 * When setting position, an internal "original position" will be saved, and position will be adjusted for GUI scale.
 * Another method, ::handleGuiScaleChange(), sets the position using saved "original position" and current GUI scale.
 */
class GuiTransformable : public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		sf::Vector2f origPos;

	protected:
		void handleGuiScaleChange();

	public:
		void setPosition(float x, float y);
		void setPosition(const sf::Vector2f& position);
};
