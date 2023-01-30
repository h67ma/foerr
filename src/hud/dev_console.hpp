#pragma once

#include <string>
#include <deque>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
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
		std::deque<std::string> history;
		std::deque<std::string>::iterator historyElem;

		// needed to actually make things happen in the game
		Campaign &campaign;

		void execute(const std::string &cmdline, sf::Vector2f mouseCoords);

	public:
		DevConsole(sf::Vector2u windowSize, const sf::Font &font, Campaign &campaign);
		void open();
		bool getIsOpen() const;
		void setPosition(sf::Vector2u windowSize);
		void handleKeyPressed(sf::Keyboard::Key key, sf::Vector2f mouseCoords);
		void handleTextEntered(uint keycode);
		void executeLast(sf::Vector2f mouseCoords = { 0.f, 0.f });
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
