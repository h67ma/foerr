// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include "gui_transformable.hpp"
#include "text_label.hpp"

constexpr uint TEXT_NO_WRAP = 0;

/**
 * A wrapper class for TextLabel with added features:
 *   - Automated position update after GUI scale change
 *   - Ability to wrap text
 *
 * This is kind of ugly workaround for simply extending TextLabel, which unfortunately is problematic, as
 * GuiTransformable also inherits sf::Transformable, so when e.g. ::setPosition() is called, only one of the base class
 * ::setPosition() gets called (and not the good one either). Current approach which encapsulates TextLabel is not
 * ideal, but gets the job done, without straight copying parts of GuiTransformable here. TODO?
 */
class WrappableText : public GuiTransformable, public sf::Drawable
{
	private:
		TextLabel text;
		std::string textValue;
		uint maxWidth = TEXT_NO_WRAP;
		void setStringWrap();

	public:
		WrappableText(const sf::Font& font, uint fontSize, const sf::Vector2f& position);
		void setString(const std::string& newText, uint maxWidth);
		void setString(const std::string& newText);
		void setFillColor(const sf::Color& color);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
