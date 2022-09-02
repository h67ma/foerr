#include "button.hpp"
#include "../consts.hpp"

Button::Button(GuiScale scale, ButtonSize size, sf::Color color, sf::Font &font, uint x, uint y, std::string text, std::function<void(void)> callback)
{
	this->callback = callback;
	this->size = size;
	
	this->setPosition(static_cast<float>(x), static_cast<float>(y));

	this->setColor(color);

	// text
	this->text.setFont(font);
	this->text.setString(text);

	// disabled by default
	this->setSelected(false);

	this->setGuiScale(scale);
}

void Button::setGuiScale(GuiScale scale)
{
	uint w, h, textTopOffset;
	this->scale = scale;

	if (scale == GUI_SMALL)
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
	else if (scale == GUI_LARGE)
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
		case GUI_SMALL:
			thicc = this->selected ? BTN_BORDER_THICKNESS_SMALL_SELECTED : BTN_BORDER_THICKNESS_SMALL;
			break;
		case GUI_LARGE:
			thicc = this->selected ? BTN_BORDER_THICKNESS_LARGE_SELECTED : BTN_BORDER_THICKNESS_LARGE;
			break;
		case GUI_NORMAL:
		default:
			thicc = this->selected ? BTN_BORDER_THICKNESS_NORMAL_SELECTED : BTN_BORDER_THICKNESS_NORMAL;
	}

	this->rect.setOutlineThickness(thicc);
}

/**
 * Sets the correct button state (rect background color)
 * based on whether the button is selected and/or being hovered.
 */
void Button::updateState()
{
	if (this->hover)
		this->rect.setFillColor(this->colorHover);
	else
		this->rect.setFillColor(this->selected ? this->colorSelected : this->colorUnselected);

	this->setThickness(); // thickness changes between selected/deselected state
}

void Button::setSelected(bool selected)
{
	this->selected = selected;
	this->updateState();
}

void Button::setHover(bool hover)
{
	this->hover = hover;
	this->updateState();
}

void Button::setColor(sf::Color color)
{
	// rectangle border
	this->rect.setOutlineColor(color);

	// text
	this->text.setFillColor(color);

	// hover/selected/deselected colors are the same color toned down
	this->colorHover = color * sf::Color(BTN_COLOR_HOVER_FACTOR, BTN_COLOR_HOVER_FACTOR, BTN_COLOR_HOVER_FACTOR);
	this->colorSelected = color * sf::Color(BTN_COLOR_SEL_FACTOR, BTN_COLOR_SEL_FACTOR, BTN_COLOR_SEL_FACTOR);
	this->colorUnselected = color * sf::Color(BTN_COLOR_UNSEL_FACTOR, BTN_COLOR_UNSEL_FACTOR, BTN_COLOR_UNSEL_FACTOR);
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
 * @returns `true` if point belongs to button area, `false` otherwise
*/
bool Button::containsPoint(int x, int y)
{
	// basically the entity which bounds we check on click is only moved when it's being drawn,
	// so ::getGlobalBounds() returns a rectangle starting at (0, 0), so might as well
	// call ::getLocalBounds() instead.
	// TODO there might be a better way to handle this
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	return this->rect.getLocalBounds().contains(static_cast<float>(x), static_cast<float>(y));
}

/**
 * Checks if click was placed inside button area.
 * If it was, and the callback function exists, the callback will be called.
 *
 * @param x click x coordinate
 * @param y click y coordinate
 * @returns `true` if click was consumed, `false` otherwise
*/
bool Button::maybeHandleLeftClick(int x, int y)
{
	if (!this->containsPoint(x, y))
		return false;

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
