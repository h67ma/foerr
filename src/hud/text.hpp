#pragma once

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>

#include "hud_transformable.hpp"

/**
 * A wrapper class for sf::Text with added features:
 *   - Support for adjusting to GUI scale
 *   - Ability to wrap text
 *
 * This is kind of ugly workaround for simply extending sf::Text, which unfortunately is problematic, as
 * HudTransformable also inherits sf::Transformable, so when e.g. ::setPosition() is called, only one of the base class
 * ::setPosition() gets called (and not the good one either). Current approach which encapsulates sf::Text is not ideal,
 * but gets the job done, without straight copying parts of HudTransformable here.
 */
class Text : public HudTransformable, public sf::Drawable
{
	private:
		sf::Text text;

	public:
		void setString(std::string newText, uint maxWidth);
		void setString(const std::string &newText);
		void setFont(const sf::Font& font);
		void setFillColor(const sf::Color& color);
		void setCharacterSize(unsigned int size);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
