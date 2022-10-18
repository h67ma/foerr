#pragma once

#include "button.hpp"
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>
#include "../../resources/sprite_resource.hpp"
#include "../hud.hpp"

class LocButton : public Button
{
	private:
		bool isBaseCamp;
		bool isBig; // some locations are just bigger than other, that's how it is
		bool selected; // mouse selection
		bool active = false; // location where we currently are
		bool hover = false;
		// TODO showAnimatedHint = false; // four animated arrows showing the next mane quest location
		GuiScale scale;
		sf::Color colorBasecampHover;
		sf::Color colorHover;
		sf::Color colorBasecamp;
		sf::RectangleShape rect;
		SpriteResource icon;
		sf::RectangleShape activeIndicator[2];
		void setThickness();
		void updateState();
		uint getSideLen();

	public:
		LocButton(GuiScale scale, bool isBig, bool isBaseCamp, sf::Color color, sf::Vector2u position, std::shared_ptr<sf::Texture> iconTexture, std::function<void(void)> callback=nullptr);
		void setSelected(bool selected);
		bool containsPoint(sf::Vector2i coords) override;
		void setGuiScale(GuiScale scale) override;
		void setColor(sf::Color color);
		void setHover(bool hover) override;
		void setActive(bool active);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
