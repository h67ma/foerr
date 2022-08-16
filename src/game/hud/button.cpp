#include "button.hpp"
#include "../consts.hpp"

Button::Button(HudScale scale, ButtonSize size, std::string text, sf::Font &font, std::function<void(void)> callback)
{
	this->callback = callback;
	this->size = size;

	// rectangle border
	this->rect.setOutlineColor(sf::Color(0, 255, 153));

	// text
	this->text.setFont(font);
	this->text.setFillColor(sf::Color(0, 255, 153));
	this->text.setString(text);

	// disabled by default
	this->setSelected(false);

	this->setScale(scale);
}

void Button::setScale(HudScale scale)
{
	uint w, h, textTopOffset;
	this->scale = scale;

	if (scale == HUD_SMALL)
	{
		this->text.setCharacterSize(FONT_SIZE_SMALL);
		textTopOffset = BTN_TEXT_SMALL_TOP_OFFSET;

		switch (size)
		{
			case BTN_BIG:
				w = 100;
				h = 51;
				break;
			case BTN_NARROW:
				w = 102;
				h = 21;
				break;
			case BTN_NORMAL:
			default:
				w = 145;
				h = 21;
		}
	}
	else if (scale == HUD_LARGE)
	{
		this->text.setCharacterSize(FONT_SIZE_LARGE);
		textTopOffset = BTN_TEXT_LARGE_TOP_OFFSET;

		switch (size)
		{
			case BTN_BIG:
				w = 171;
				h = 87;
				break;
			case BTN_NARROW:
				w = 175;
				h = 36;
				break;
			case BTN_NORMAL:
			default:
				w = 249;
				h = 36;
		}
	}
	else // normal/default
	{
		this->text.setCharacterSize(FONT_SIZE_NORMAL);
		textTopOffset = BTN_TEXT_NORMAL_TOP_OFFSET;

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
	}

	this->rect.setSize({ static_cast<float>(w), static_cast<float>(h) });
	this->setThickness();

	// gradient fill (transparent - almost black - transparent)
	// this->rect color will "shine" through left & right side, this way we don't have to prepare multiple gradients

	sf::Color black(0, 11, 6);
	sf::Color transparent(0, 0, 0, 0);;

	uint midX = w/2;

	// 0 -- 1
	// |	|
	// 3 -- 2

	// left half
	gradient[0] = sf::Vertex({ 0.f, 0.f }, transparent);
	gradient[1] = sf::Vertex({ static_cast<float>(midX), 0.f }, black);
	gradient[2] = sf::Vertex({ static_cast<float>(midX), static_cast<float>(h) }, black);
	gradient[3] = sf::Vertex({ 0.f, static_cast<float>(h) }, transparent);

	// right half
	gradient[4] = sf::Vertex({ static_cast<float>(midX), 0.f }, black);
	gradient[5] = sf::Vertex({ static_cast<float>(w), 0.f }, transparent);
	gradient[6] = sf::Vertex({ static_cast<float>(w), static_cast<float>(h) }, transparent);
	gradient[7] = sf::Vertex({ static_cast<float>(midX), static_cast<float>(h) }, black);

	// center button text
	// to center vertically, we can't use local bounds, as the baselines on different buttons would not match.
	// use a constant top offset instead
	this->text.setPosition(
		static_cast<float>(((w - this->text.getLocalBounds().width) / 2)),
		static_cast<float>((h / 2) - textTopOffset)
	);
}

void Button::setThickness()
{
	float thicc;
	switch (this->scale)
	{
		case HUD_SMALL:
			thicc = this->selected ? BTN_BORDER_THICKNESS_SMALL_SELECTED : BTN_BORDER_THICKNESS_SMALL;
			break;
		case HUD_LARGE:
			thicc = this->selected ? BTN_BORDER_THICKNESS_LARGE_SELECTED : BTN_BORDER_THICKNESS_LARGE;
			break;
		case HUD_NORMAL:
		default:
			thicc = this->selected ? BTN_BORDER_THICKNESS_NORMAL_SELECTED : BTN_BORDER_THICKNESS_NORMAL;
	}

	this->rect.setOutlineThickness(thicc);
}

void Button::setSelected(bool selected)
{
	this->selected = selected;
	this->rect.setFillColor(selected ? sf::Color(0, 104, 60) : sf::Color(0, 68, 39));
	this->setThickness();
}

void Button::setCallback(std::function<void(void)> callback)
{
	this->callback = callback;
}

/**
 * Checks if click was placed inside button area.
 * If it was, and the callback function exists, the callback will be called.
 *
 * @param x click x coordinate
 * @param y click y coordinate
 * @returns `true` if click was consumed, `false` otherwise
*/
bool Button::maybeHandleClick(int x, int y)
{
	// basically the entity which bounds we check on click is only moved when it's being drawn,
	// so ::getGlobalBounds() returns a rectangle starting at (0, 0).
	// TODO there might be a better way to handle this
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	if (!this->rect.getGlobalBounds().contains(static_cast<float>(x), static_cast<float>(y)))
		return false; // click outside of btn bounds

	if (this->callback != nullptr)
		this->callback();

	return true;
}

void Button::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->rect, states);
	target.draw(this->gradient, states);
	target.draw(this->text, states);
}
