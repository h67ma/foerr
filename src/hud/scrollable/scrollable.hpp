// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2024 h67ma <szycikm@gmail.com>

#pragma once

#include <memory>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
#include <SFML/Graphics/Sprite.hpp>
#include <SFML/System/Vector2.hpp>

#include "../gui_transformable.hpp"

enum ScrollPosition
{
	SCROLL_TOP,
	SCROLL_UNCHANGED,
	SCROLL_BOTTOM,
};

/**
 * Abstract class for components which contain scrollable content. Components inheriting from it should not inherit
 * sf::Drawable nor sf::Transformable. Instead, they should implement ::drawScrollableContent().
 *
 * Component which inherits Scrollable should always notify the base class when the scrollable content height changes
 * (via ::handleScrollableContentHeightChanged()). This ensures that scroll limits and scroll bar will behave correctly.
 */
class Scrollable : public sf::Drawable, public GuiTransformable
{
	private:
		float scrollOffset = 0;
		float scrollableContentHeight;
		float bottomScrollLimit;
		std::unique_ptr<sf::RenderTexture> scrollableContentTexture;
		sf::Sprite scrollableContentSprite;
		sf::RectangleShape scrollArea;
		const sf::Vector2f scrollableAreaSize; // original size, GUI-independent
		void setGuiScale();

	protected:
		void redrawScrollableContent(bool resize);
		void handleScrollableContentHeightChanged(float newHeight, enum ScrollPosition scrollTo = SCROLL_UNCHANGED,
												  bool resize = false);
		bool shouldDrawItem(float elemHeight, float elemOffset) const;
		bool isWithinScrollArea(sf::Vector2f point) const;
		float getScrollOffset() const;
		virtual void drawScrollableContent(sf::RenderTarget& target, sf::RenderStates states) = 0;

	public:
		explicit Scrollable(const sf::Vector2f& scrollableAreaSize);
		void handleScroll(float delta, sf::Vector2i mousePos);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
