#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include "hud_transformable.hpp"
#include "slider.hpp"

class ColorSelector : public sf::Drawable, public HudTransformable
{
	private:
		sf::RectangleShape selectedColorPreview, labelR, labelG, labelB;
		Slider sliderR, sliderG, sliderB;
		sf::Color selectedColor;

		void updateColor();

	public:
		explicit ColorSelector(const sf::Font &font);
		sf::Color getSelectedColor() const;
		bool handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
