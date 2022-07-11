#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "../consts.h"

#define BTN_FONT_SIZE 18U
#define BTN_FONT_TOP_OFFSET 12U
#define BTN_BORDER_THICKNESS 1U
#define BTN_BORDER_THICKNESS_SELECTED 3U

enum ButtonSize
{
	BTN_NORMAL,
	BTN_NARROW,
	BTN_BIG,
};

// TODO? change btn color on hover. could be a nice addition, but kinda tedious to implement
class Button
{
	private:
		sf::RectangleShape rect;
		sf::Text text;
		sf::Vertex gradient[8];
		std::function<void(void)> callback = nullptr;

	public:
		Button(uint x, uint y, ButtonSize size, std::string text, sf::Font *font, std::function<void(void)> callback = nullptr);
		void setSelected(bool selected);
		void setCallback(std::function<void(void)> callback);
		bool maybeHandleClick(float x, float y);
		void draw(sf::RenderWindow *window);
};
