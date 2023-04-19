#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>

#include "../hud_transformable.hpp"
#include "slider_handle.hpp"

constexpr uint SLIDER_WIDTH = 215;
constexpr uint SLIDER_HANDLE_HALF = SLIDER_HANDLE_WIDTH / 2;
constexpr uint SLIDER_MOUSE_POSSIBLE_VALS = SLIDER_WIDTH - SLIDER_HANDLE_WIDTH;

/**
 * A base class for a UI component that allows inputting a value in a designated range by dragging or clicking mouse
 * on a horizontal bar. Derived classes implement either integer or floating point value sliders.
 */
class Slider : public HudTransformable, public sf::Drawable
{
	private:
		sf::RectangleShape sliderOutline;
		bool dragging = false;

	protected:
		sf::Text currValueText;
		SliderHandle handle;

		virtual void setSliderPos(int mouseX) = 0;

	public:
		explicit Slider(const sf::Font &font);
		bool handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
