#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "../consts.h"

#define BTN_FONT_SIZE 23U
#define BTN_FONT_TOP_OFFSET 7U
#define BTN_BORDER_THICKNESS 1U
#define BTN_BORDER_THICKNESS_SELECTED 3U

enum ButtonSize
{
	BTN_NORMAL,
	BTN_NARROW,
	BTN_BIG,
};

class Button
{
	private:
		sf::RectangleShape rect;
		sf::Text text;
		sf::Vertex gradient[8];
		bool setSelectedOnClick;
		std::function<void(void)> callback = nullptr;

	public:
		Button(uint x, uint y, ButtonSize size, std::string text, sf::Font *font, bool selected, std::function<void(void)> callback, bool setSelectedOnClick = true);
		void setSelected(bool selected);
		void setCallback(std::function<void(void)> callback);
		bool Button::containsPoint(float x, float y);
		void onClick();
		void draw(sf::RenderWindow *window);
};
