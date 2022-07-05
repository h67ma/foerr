#include "button.hpp"
#include "../consts.h"

Button::Button(uint x, uint y, ButtonSize size, std::string text, sf::Font *font, bool selected, std::function<void(void)> callback, bool setSelectedOnClick)
{
	uint w, h;

	this->callback = callback;
	this->setSelectedOnClick = setSelectedOnClick;

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

	// border

	this->rect = sf::RectangleShape(sf::Vector2f(w, h));

	// will be covered by gradient anyway, let's say this is a sort of "fallback" color
	this->rect.setFillColor(sf::Color(0, 11, 6));

	this->rect.setOutlineColor(sf::Color(0, 255, 153));
	this->rect.setOutlineThickness(BTN_BORDER_THICKNESS);
	this->rect.move(x, y);

	// text

	this->text.setFont(*font);
	this->text.setFillColor(sf::Color(0, 255, 153));
	this->text.setString(text);
	this->text.setCharacterSize(BTN_FONT_SIZE);

	// FONT_BUTTON_TOP_OFFSET: for some reason sf::Text is drawn with a little top padding, need to offset that
	this->text.move(x + (w - this->text.getLocalBounds().width) / 2, y - BTN_FONT_TOP_OFFSET + (h - this->text.getLocalBounds().height) / 2);

	// gradient fills
	this->prepareGradient(true, x, y, w, h); // selected
	this->prepareGradient(false, x, y, w, h); // deselected

	this->setSelected(selected);
}

void Button::prepareGradient(bool selected, uint x, uint y, uint w, uint h)
{
	sf::Color black(0, 11, 6);
	sf::Color green;

	uint midX = x + w/2;
	uint rightX = x + w;
	uint bottomY = y + h;

	sf::Vertex *gradient;

	if (selected)
	{
		gradient = this->gradientSelected;
		green = sf::Color(0, 103, 60);
	}
	else
	{
		gradient = this->gradientDeselected;
		green = sf::Color(0, 67, 39);
	}

	// 0 -- 1
	// |	|
	// 3 -- 2

	// left half
	gradient[0] = sf::Vertex(sf::Vector2f(x, y), green);
	gradient[1] = sf::Vertex(sf::Vector2f(midX, y), black);
	gradient[2] = sf::Vertex(sf::Vector2f(midX, bottomY), black);
	gradient[3] = sf::Vertex(sf::Vector2f(x, bottomY), green);

	// right half
	gradient[4] = sf::Vertex(sf::Vector2f(midX, y), black);
	gradient[5] = sf::Vertex(sf::Vector2f(rightX, y), green);
	gradient[6] = sf::Vertex(sf::Vector2f(rightX, bottomY), green);
	gradient[7] = sf::Vertex(sf::Vector2f(midX, bottomY), black);
}

void Button::setSelected(bool selected)
{
	this->isSelected = selected;

	this->rect.setOutlineThickness(selected ? BTN_BORDER_THICKNESS_SELECTED : BTN_BORDER_THICKNESS);
}

void Button::setCallback(std::function<void(void)> callback)
{
	this->callback = callback;
}

void Button::onClick()
{
	if (this->setSelectedOnClick)
		this->setSelected(true);

	if (this->callback != nullptr)
		this->callback();
}

bool Button::containsPoint(float x, float y)
{
	return this->rect.getGlobalBounds().contains(x, y);
}

void Button::draw(sf::RenderWindow *window)
{
	window->draw(this->rect);
	window->draw(this->isSelected ? this->gradientSelected : this->gradientDeselected, 8, sf::Quads);
	window->draw(this->text);
}
