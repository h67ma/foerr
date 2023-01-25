#pragma once

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/Graphics/Font.hpp>

#include "../campaigns/campaign.hpp"
#include "text_input.hpp"

/**
 * A developer console, displayed as an overlay with a text field.
 * Any output is displayed in the global Log.
 */
class DevConsole : public sf::Drawable
{
	private:
		TextInput inputField;
		bool isOpen = false;
		bool ignoreNextTextEntered = false;
		std::string lastCommand = "";

		// needed to actually make things happen in the game
		Campaign &campaign;

		void execute(const std::string &cmdline);

	public:
		DevConsole(sf::Vector2u windowSize, const sf::Font &font, Campaign &campaign);
		void open();
		bool getIsOpen() const;
		void setPosition(sf::Vector2u windowSize);
		void handleKeyPressed(sf::Keyboard::Key key);
		void handleTextEntered(uint keycode);
		void executeLast();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
