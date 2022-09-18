#include <math.h>
#include "location_button.hpp"

// values from 0 to 255, where 255 is the original color and 0 is black
#define BTN_COLOR_BASECAMP_FACTOR 90
#define BTN_COLOR_BASECAMP_HOVER_FACTOR 120
#define BTN_COLOR_HOVER_FACTOR 60

#define ACTIVE_INDICATOR_NEG_LEN -14
#define ACTIVE_INDICATOR_DOUBLE_LEN 28

LocButton::LocButton(GuiScale scale, bool isBig, bool isBaseCamp, sf::Color color, uint x, uint y, std::shared_ptr<sf::Texture> iconTexture, std::function<void(void)> callback) :
	Button(scale, x, y, callback),
	icon(iconTexture)
{
	this->isBaseCamp = isBaseCamp;
	this->isBig = isBig;

	this->setGuiScale(scale);
	this->setColor(color);
	this->setSelected(false);
}

void LocButton::updateState()
{
	if (this->hover && this->isBaseCamp)
		this->rect.setFillColor(this->colorBasecampHover);
	else if (this->hover)
		this->rect.setFillColor(this->colorHover);
	else if (this->isBaseCamp)
		this->rect.setFillColor(this->colorBasecamp);
	else
		this->rect.setFillColor(sf::Color::Black);

	this->setThickness(); // thickness changes between selected/deselected state
}

uint LocButton::getSideLen()
{
	if (scale == GUI_SMALL)
	{
		if (this->isBig)
			return 39;
		else
			return 31;
	}
	else if (scale == GUI_LARGE)
	{
		if (this->isBig)
			return 67;
		else
			return 53;
	}
	else // normal/default
	{
		if (this->isBig)
			return 50;
		else
			return 40;
	}
}

void LocButton::setThickness()
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

void LocButton::setSelected(bool selected)
{
	this->selected = selected;
	this->updateState();
}

/**
 * Checks if button area contains a given point.
 *
 * @param x point x coordinate
 * @param y point y coordinate
 * @returns true if point belongs to button area
 * @returns false if point doesn't belong to button
*/
bool LocButton::containsPoint(int x, int y)
{
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	return this->rect.getLocalBounds().contains(static_cast<float>(x), static_cast<float>(y));
}

void LocButton::setGuiScale(GuiScale scale)
{
	this->scale = scale;
	float sideLen = static_cast<float>(this->getSideLen());

	this->rect.setSize(sf::Vector2f(sideLen, sideLen));
	this->setThickness();

	// center icon
	// floor the coordinates to avoid pixel misalignment
	this->icon.get().setPosition(
		floor((sideLen - this->icon.get().getLocalBounds().width) / 2),
		floor((sideLen - this->icon.get().getLocalBounds().height) / 2)
	);

	float indicatorWidth;
	if (scale == GUI_SMALL)
		indicatorWidth = 2;
	else if (scale == GUI_LARGE)
		indicatorWidth = 6;
	else // normal/default
		indicatorWidth = 4;

	float centerOffset = sideLen/2 - indicatorWidth/2;

	float indicatorLength = sideLen + ACTIVE_INDICATOR_DOUBLE_LEN;

	this->activeIndicator[0].setSize({ indicatorLength, indicatorWidth });
	this->activeIndicator[1].setSize({ indicatorWidth, indicatorLength });

	this->activeIndicator[0].setPosition({ ACTIVE_INDICATOR_NEG_LEN, centerOffset });
	this->activeIndicator[1].setPosition({ centerOffset, ACTIVE_INDICATOR_NEG_LEN });
}

void LocButton::setColor(sf::Color color)
{
	// rectangle border, active indicator
	this->rect.setOutlineColor(color);
	this->activeIndicator[0].setFillColor(color);
	this->activeIndicator[1].setFillColor(color);

	// TODO? somehow change icon tint

	// hover/selected/deselected colors are the same color toned down
	this->colorHover = color * sf::Color(BTN_COLOR_HOVER_FACTOR, BTN_COLOR_HOVER_FACTOR, BTN_COLOR_HOVER_FACTOR);
	this->colorBasecampHover = color * sf::Color(BTN_COLOR_BASECAMP_HOVER_FACTOR, BTN_COLOR_BASECAMP_HOVER_FACTOR, BTN_COLOR_BASECAMP_HOVER_FACTOR);
	this->colorBasecamp = color * sf::Color(BTN_COLOR_BASECAMP_FACTOR, BTN_COLOR_BASECAMP_FACTOR, BTN_COLOR_BASECAMP_FACTOR);
}

void LocButton::setHover(bool hover)
{
	this->hover = hover;
	this->updateState();
}

void LocButton::setActive(bool active)
{
	this->active = active;
}

void LocButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	if (this->active)
	{
		target.draw(this->activeIndicator[0], states);
		target.draw(this->activeIndicator[1], states);
	}

	target.draw(this->rect, states);
	target.draw(this->icon.sprite, states);
}
