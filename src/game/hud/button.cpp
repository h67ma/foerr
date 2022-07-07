#include "button.hpp"
#include "../consts.h"

Button::Button(uint x, uint y, ButtonSize size, std::string text, sf::Font *font, std::function<void(void)> callback)
{
	uint w, h;

	this->callback = callback;

	switch (size)
	{
		case BTN_BIG:
			w = 128;
			h = 65;
			break;
		case BTN_NARROW:
			w = 131;
			h = 27;
			break;
		case BTN_NORMAL:
		default:
			w = 187;
			h = 27;
	}

	// rectangle & border
	this->rect = sf::RectangleShape(sf::Vector2f(w, h));
	this->rect.setOutlineColor(sf::Color(0, 255, 153));
	this->rect.setPosition(x, y);

	// text

	this->text.setFont(*font);
	this->text.setFillColor(sf::Color(0, 255, 153));
	this->text.setString(text);
	this->text.setCharacterSize(BTN_FONT_SIZE);

	// center button text
	// FONT_BUTTON_TOP_OFFSET: for some reason sf::Text is drawn with a little top padding, need to offset that
	this->text.setPosition(x + (w - this->text.getLocalBounds().width) / 2, y - BTN_FONT_TOP_OFFSET + (h - this->text.getLocalBounds().height) / 2);

	// gradient fill (transparent - almost black - transparent)
	// this->rect color will "shine" through left & right side, this way we don't have to prepare multiple gradients

	sf::Color black(0, 11, 6);
	sf::Color transparent(0, 0, 0, 0);;

	uint midX = x + w/2;
	uint rightX = x + w;
	uint bottomY = y + h;

	// 0 -- 1
	// |	|
	// 3 -- 2

	// left half
	gradient[0] = sf::Vertex(sf::Vector2f(x, y), transparent);
	gradient[1] = sf::Vertex(sf::Vector2f(midX, y), black);
	gradient[2] = sf::Vertex(sf::Vector2f(midX, bottomY), black);
	gradient[3] = sf::Vertex(sf::Vector2f(x, bottomY), transparent);

	// right half
	gradient[4] = sf::Vertex(sf::Vector2f(midX, y), black);
	gradient[5] = sf::Vertex(sf::Vector2f(rightX, y), transparent);
	gradient[6] = sf::Vertex(sf::Vector2f(rightX, bottomY), transparent);
	gradient[7] = sf::Vertex(sf::Vector2f(midX, bottomY), black);

	// disabled by default
	this->setSelected(false);
}

void Button::setSelected(bool selected)
{
	this->rect.setFillColor(selected ? sf::Color(0, 104, 60) : sf::Color(0, 68, 39));
	this->rect.setOutlineThickness(selected ? BTN_BORDER_THICKNESS_SELECTED : BTN_BORDER_THICKNESS);
}

void Button::setCallback(std::function<void(void)> callback)
{
	this->callback = callback;
}

/**
 * Checks if click was placed inside button area.
 * If it was, and the callback function exists, the callback will be called.
 * @param x click x coordinate
 * @param y click y coordinate
 * @returns `true` if click was consumed, `false` otherwise
*/
bool Button::maybeHandleClick(float x, float y)
{
	if (!this->rect.getGlobalBounds().contains(x, y))
		return false; // click outside of btn bounds

	if (this->callback != nullptr)
		this->callback();

	return true;
}

void Button::draw(sf::RenderWindow *window)
{
	window->draw(this->rect);
	window->draw(this->gradient, 8, sf::Quads);
	window->draw(this->text);
}
