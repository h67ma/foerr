#include "location_button.hpp"

#include <cmath>

#include <utility>

#include "../../settings/settings_manager.hpp"

// values from 0 to 255, where 255 is the original color and 0 is black
#define BTN_COLOR_BASECAMP_FACTOR 90
#define BTN_COLOR_BASECAMP_HOVER_FACTOR 120
#define BTN_COLOR_HOVER_FACTOR 60

LocButton::LocButton(bool isBig, bool isBaseCamp, sf::Vector2u position, std::shared_ptr<sf::Texture> iconTexture) :
	Button(position, nullptr, CLICK_CONSUMED),
	icon(std::move(iconTexture))
{
	this->isBaseCamp = isBaseCamp;
	this->isBig = isBig;

	this->setGuiScale();
	this->setColor();
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

uint LocButton::getSideLen(bool big)
{
	return big ? (50 * SettingsManager::guiScale) : (40 * SettingsManager::guiScale);
}

void LocButton::setThickness()
{
	float thicc = this->selected ? (BTN_BORDER_THICKNESS_NORMAL_SELECTED * SettingsManager::guiScale) :
								   (BTN_BORDER_THICKNESS_NORMAL * SettingsManager::guiScale);

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

void LocButton::setGuiScale()
{
	float sideLen = static_cast<float>(this->getSideLen(this->isBig));

	this->rect.setSize(sf::Vector2f(sideLen, sideLen));
	this->setThickness();

	this->icon.setScale(SettingsManager::guiScale, SettingsManager::guiScale);

	// center icon
	// floor the coordinates to avoid pixel misalignment
	this->icon.setPosition(
		floor((sideLen - this->icon.getGlobalBounds().width) / 2),
		floor((sideLen - this->icon.getGlobalBounds().height) / 2)
	);
}

void LocButton::setColor()
{
	this->rect.setOutlineColor(SettingsManager::hudColor);

	this->icon.setColor(SettingsManager::hudColor);

	// hover/selected/deselected colors are the same color toned down
	this->colorHover = DIM_COLOR(SettingsManager::hudColor, BTN_COLOR_HOVER_FACTOR);
	this->colorBasecampHover = DIM_COLOR(SettingsManager::hudColor, BTN_COLOR_BASECAMP_HOVER_FACTOR);
	this->colorBasecamp = DIM_COLOR(SettingsManager::hudColor, BTN_COLOR_BASECAMP_FACTOR);
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

void LocButton::handleSettingsChange()
{
	this->setColor();
	this->setGuiScale();
	this->updateState();
}

void LocButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->rect, states);
	target.draw(this->icon, states);
}
