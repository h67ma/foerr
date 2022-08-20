#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "hud.hpp"
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

enum ButtonSize
{
	BTN_NORMAL,
	BTN_NARROW,
	BTN_BIG,
};

// TODO? change btn color on hover. could be a nice addition, but kinda tedious to implement
// another nice thing would be to change cursor color/shape on hover, maybe it would be easier to implement
class Button : public sf::Drawable, public sf::Transformable
{
	private:
		sf::RectangleShape rect;
		ButtonSize size;
		GuiScale scale;
		bool selected;
		sf::Text text;
		sf::VertexArray gradient = sf::VertexArray(sf::Quads, 8);
		std::function<void(void)> callback = nullptr;
		void setThickness();

	public:
		Button(GuiScale scale, ButtonSize size, std::string text, sf::Font &font, std::function<void(void)> callback = nullptr);
		void setScale(GuiScale scale);
		void setSelected(bool selected);
		void setCallback(std::function<void(void)> callback);
		bool maybeHandleClick(int x, int y);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
