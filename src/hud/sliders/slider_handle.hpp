#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include "../configurable_gui_component.hpp"

#define SLIDER_HANDLE_HEIGHT 15
#define SLIDER_HANDLE_WIDTH 21

#define SLIDER_HANDLE_THINGY_VERT_CNT 8

/**
 * Represents the draggable handle used to input value into Slider. Could be replaced by a sprite.
 */
class SliderHandle : public sf::Drawable, public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		sf::RectangleShape outlineRect;

		// what else to call something that looks like this |||| ? it's a thingy.
		sf::VertexArray thingy = sf::VertexArray(sf::Lines, SLIDER_HANDLE_THINGY_VERT_CNT);

	public:
		SliderHandle();
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
