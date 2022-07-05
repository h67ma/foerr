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
		sf::Vertex gradientSelected[8]; // gradient lightgreen-black-lightgreengreen
		sf::Vertex gradientDeselected[8]; // gradient darkgreen-black-darkgreen
		bool isSelected = false;
		bool setSelectedOnClick;
		std::function<void(void)> callback = nullptr;
		void prepareGradient(bool selected, uint x, uint y, uint w, uint h);

	public:
		Button(uint x, uint y, ButtonSize size, std::string text, sf::Font *font, bool selected, std::function<void(void)> callback, bool setSelectedOnClick = true);
		void setSelected(bool selected);
		void setCallback(std::function<void(void)> callback);
		bool Button::containsPoint(float x, float y);
		void onClick();
		void draw(sf::RenderWindow *window);
};
