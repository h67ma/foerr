#pragma once

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include "sliders/int_slider.hpp"
#include "gui_transformable.hpp"

class ColorSelector : public sf::Drawable, public GuiTransformable
{
	private:
		sf::RectangleShape selectedColorPreview, labelR, labelG, labelB;
		IntSlider sliderR, sliderG, sliderB;
		sf::Color selectedColor;

		void updateColor();

	public:
		ColorSelector(const sf::Font &font, sf::Color initialColor);
		sf::Color getSelectedColor() const;
		void setSelectedColor(sf::Color color);
		bool handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
