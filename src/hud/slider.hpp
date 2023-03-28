#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>

#include "configurable_hud_component.hpp"
#include "hud_transformable.hpp"
#include "slider_handle.hpp"

#define SLIDER_DEFAULT_MIN 0
#define SLIDER_DEFAULT_DEFAULT 50
#define SLIDER_DEFAULT_MAX 100

/**
 * An UI component that allows inputting an integer value in a designated range by dragging or clicking mouse on a
 * horizontal bar.
 */
class Slider : public HudTransformable, public sf::Drawable, public ConfigurableHudComponent
{
	private:
		const int minVal;
		const int maxVal;
		const int possibleValCnt;
		int currentVal;
		sf::Text currValueText;
		sf::RectangleShape sliderOutline;
		SliderHandle handle;
		bool dragging = false;

		void updateDisplay();
		void setSliderPos(int mouseX);

	public:
		explicit Slider(const sf::Font &font, int minVal = SLIDER_DEFAULT_MIN,
						int maxVal = SLIDER_DEFAULT_MAX, int defaultVal = SLIDER_DEFAULT_DEFAULT);
		int getValue() const;
		void setValue(int value);
		bool handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
