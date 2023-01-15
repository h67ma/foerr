#include "location_button.hpp"
#include <math.h>
#include "../../settings/settings_manager.hpp"
#include "../../util/util.hpp"

// values from 0 to 255, where 255 is the original color and 0 is black
#define BTN_COLOR_BASECAMP_FACTOR 90
#define BTN_COLOR_BASECAMP_HOVER_FACTOR 120
#define BTN_COLOR_HOVER_FACTOR 60

LocButton::LocButton(bool isBig, bool isBaseCamp, sf::Vector2u position, std::shared_ptr<sf::Texture> iconTexture) :
	Button(position, nullptr, CLICK_CONSUMED),
	icon(iconTexture)
{
	this->isBaseCamp = isBaseCamp;
	this->isBig = isBig;

	this->setGuiScale(SettingsManager::getGuiScale(SETT_GUI_SCALE));
	this->setColor(SettingsManager::getColor(SETT_HUD_COLOR));
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

uint LocButton::getSideLen(GuiScale scale, bool big)
{
	if (scale == GUI_SMALL)
	{
		if (big)
			return 39;
		else
			return 31;
	}
	else if (scale == GUI_LARGE)
	{
		if (big)
			return 67;
		else
			return 53;
	}
	else // normal/default
	{
		if (big)
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

bool LocButton::containsPoint(sf::Vector2i coords)
{
	coords -= this->getIntPosition();

	return this->rect.getLocalBounds().contains(static_cast<sf::Vector2f>(coords));
}

void LocButton::setGuiScale(GuiScale scale)
{
	this->scale = scale;
	float sideLen = static_cast<float>(this->getSideLen(this->scale, this->isBig));

	this->rect.setSize(sf::Vector2f(sideLen, sideLen));
	this->setThickness();

	// center icon
	// floor the coordinates to avoid pixel misalignment
	this->icon.setPosition(
		floor((sideLen - this->icon.getLocalBounds().width) / 2),
		floor((sideLen - this->icon.getLocalBounds().height) / 2)
	);
}

void LocButton::setColor(sf::Color color)
{
	this->rect.setOutlineColor(color);

	// TODO somehow change icon tint

	// hover/selected/deselected colors are the same color toned down
	this->colorHover = DIM_COLOR(color, BTN_COLOR_HOVER_FACTOR);
	this->colorBasecampHover = DIM_COLOR(color, BTN_COLOR_BASECAMP_HOVER_FACTOR);
	this->colorBasecamp = DIM_COLOR(color, BTN_COLOR_BASECAMP_FACTOR);
}

void LocButton::setHover(bool hover)
{
	this->hover = hover;
	this->updateState();
}

bool LocButton::getIsBig() const
{
	return this->isBig;
}

void LocButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->rect, states);
	target.draw(this->icon, states);
}
