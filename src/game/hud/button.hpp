#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "hud.hpp"
#include "hoverable.hpp"
#include "../consts.hpp"

#define BTN_TEXT_SMALL_TOP_OFFSET 8U
#define BTN_TEXT_NORMAL_TOP_OFFSET 12U
#define BTN_TEXT_LARGE_TOP_OFFSET 17U

#define BTN_BORDER_THICKNESS_SMALL 1.f
#define BTN_BORDER_THICKNESS_SMALL_SELECTED 2.f
#define BTN_BORDER_THICKNESS_NORMAL 1.f
#define BTN_BORDER_THICKNESS_NORMAL_SELECTED 3.f
#define BTN_BORDER_THICKNESS_LARGE 2.f
#define BTN_BORDER_THICKNESS_LARGE_SELECTED 6.f

// values from 0 to 255, where 255 is the original color and 0 is black
#define BTN_COLOR_SEL_FACTOR 104
#define BTN_COLOR_UNSEL_FACTOR 68
#define BTN_COLOR_HOVER_FACTOR 140

enum ButtonSize
{
	BTN_NORMAL,
	BTN_NARROW,
	BTN_BIG,
};

class Button : public sf::Drawable, public sf::Transformable, public Hoverable
{
	private:
		sf::RectangleShape rect;
		ButtonSize size;
		GuiScale scale;
		bool selected = false;
		bool hover = false;
		sf::Text text;
		sf::Color colorHover; // hover background color
		sf::Color colorSelected; // selected background color
		sf::Color colorUnselected; // unselected background color
		sf::VertexArray gradient = sf::VertexArray(sf::Quads, 8);
		std::function<void(void)> callback = nullptr;
		void setThickness();
		void updateState();

	public:
		Button(GuiScale scale, ButtonSize size, sf::Color color, sf::Font &font, uint x = 0, uint y = 0, std::string text="Button", std::function<void(void)> callback = nullptr);
		void setGuiScale(GuiScale scale);
		void setSelected(bool selected);
		void setHover(bool hover);
		void setColor(sf::Color color);
		void setCallback(std::function<void(void)> callback);
		bool containsPoint(int x, int y);
		bool maybeHandleLeftClick(int x, int y);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
