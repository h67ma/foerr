// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <memory>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>

#include "../../resources/sprite_resource.hpp"
#include "button.hpp"
#include "../hud.hpp"

class LocButton : public Button
{
	private:
		bool isBaseCamp;
		bool isBig; // some locations are just bigger than other, that's how it is
		bool selected; // mouse selection
		bool hover = false;
		// TODO showAnimatedHint = false; // four animated arrows showing the next mane quest location
		sf::Color colorBasecampHover;
		sf::Color colorHover;
		sf::Color colorBasecamp;
		sf::RectangleShape rect;
		SpriteResource icon;
		void setThickness();
		void updateState();
		void setColor();
		void setGuiScale();

	public:
		LocButton(bool isBig, bool isBaseCamp, sf::Vector2u position, std::shared_ptr<sf::Texture> iconTexture);
		void setSelected(bool selected) override;
		bool containsPoint(sf::Vector2i coords) override;
		void setHover(bool hover) override;
		bool getIsBig() const;
		static uint getSideLen(bool big);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
