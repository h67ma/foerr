#pragma once

#include <functional>
#include <string>
#include <vector>
#include <deque>
#include <map>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Font.hpp>

#include "configurable_gui_component.hpp"
#include "../campaigns/campaign.hpp"
#include "text_input.hpp"

struct dev_console_cmd_params
{
	const std::vector<std::string> &tokens;
	const sf::Vector2f &mousePos;
	Campaign &campaign;
};

struct dev_console_cmd
{
	const std::function<void(struct dev_console_cmd_params)> cmd;
	const char* helpString;
	const char* helpArgs = nullptr;
};

/**
 * A developer console, displayed as an overlay with a text field.
 * Any output is displayed in the global Log.
 */
class DevConsole : public sf::Drawable, public ConfigurableGuiComponent
{
	private:
		TextInput inputField;
		bool isOpen = false;
		bool ignoreNextTextEntered = false;
		std::string lastCommand;
		std::deque<std::string> history;
		std::deque<std::string>::iterator historyElem;
		static const std::map<std::string, struct dev_console_cmd> commands;

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
		void executeLast(sf::Vector2f mouseCoords = { 0.F, 0.F });
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
