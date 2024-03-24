// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "simple_button.hpp"

#include <cmath>

#include <string>
#include <utility>

#include "../../settings/settings_manager.hpp"
#include "../../util/util.hpp"

constexpr uint BTN_TEXT_TOP_OFFSET = 12;

// 255 is the original color, 0 is black
constexpr uchar BTN_COLOR_SEL_FACTOR = 104;
constexpr uchar BTN_COLOR_UNSEL_FACTOR = 68;
constexpr uchar BTN_COLOR_HOVER_FACTOR = 140;

const sf::Color COLOR_BUTTON_BG_BLACK(0x00, 0x0B, 0x06);

SimpleButton::SimpleButton(SimpleButtonSize size, ResourceManager& resMgr, const sf::Vector2u& position,
						   const std::string& text, std::function<void(void)> callback, ClickStatus consumedStatus) :
	Button(position, std::move(callback), consumedStatus),
	text(text, *resMgr.getFont(FONT_MEDIUM), FONT_H3)
{
	this->size = size;

	this->setGuiScale();
	this->setColor();

	// disabled by default
	this->setSelected(false);
}

void SimpleButton::setThickness()
{
	float thicc = this->selected ? BTN_BORDER_THICKNESS_NORMAL_SELECTED : BTN_BORDER_THICKNESS_NORMAL;

	this->rect.setOutlineThickness(calculateGuiAwareScalar(thicc));
}

/**
 * Sets the correct button state (rect background color)
 * based on whether the button is selected and/or being hovered.
 */
void SimpleButton::updateState()
{
	if (this->hover)
		this->rect.setFillColor(this->colorHover);
	else
		this->rect.setFillColor(this->selected ? this->colorSelected : this->colorUnselected);

	this->setThickness(); // thickness changes between selected/deselected state
}

void SimpleButton::setSelected(bool selected)
{
	this->selected = selected;
	this->updateState();
}

void SimpleButton::setHover(bool hover)
{
	this->hover = hover;
	this->updateState();
}

void SimpleButton::setColor()
{
	// rectangle border
	this->rect.setOutlineColor(SettingsManager::hudColor);

	// text
	this->text.setFillColor(SettingsManager::hudColor);

	// hover/selected/deselected colors are the same color toned down
	this->colorHover = DIM_COLOR(SettingsManager::hudColor, BTN_COLOR_HOVER_FACTOR);
	this->colorSelected = DIM_COLOR(SettingsManager::hudColor, BTN_COLOR_SEL_FACTOR);
	this->colorUnselected = DIM_COLOR(SettingsManager::hudColor, BTN_COLOR_UNSEL_FACTOR);

	this->updateState();
}

void SimpleButton::setGuiScale()
{
	sf::Vector2u dim;

	this->text.handleSettingsChange();

	switch (this->size)
	{
		case BTN_BIG:
			dim.x = 128;
			dim.y = 65;
			break;
		case BTN_NARROW:
			dim.x = 131;
			dim.y = 27;
			break;
		case BTN_NORMAL:
		default:
			dim.x = 187;
			dim.y = 27;
	}

	dim.x *= SettingsManager::guiScale;
	dim.y *= SettingsManager::guiScale;

	this->rect.setSize(static_cast<sf::Vector2f>(dim));
	this->setThickness();

	// gradient fill (transparent - almost black - transparent)
	// this->rect color will "shine" through left & right side, this way we don't have to prepare multiple gradients

	float midX = std::round(dim.x / 2);

	// 0 -- 1
	// |	|
	// 3 -- 2

	// left half
	gradient[0] = sf::Vertex({ 0.F, 0.F }, sf::Color::Transparent);
	gradient[1] = sf::Vertex({ midX, 0.F }, COLOR_BUTTON_BG_BLACK);
	gradient[2] = sf::Vertex({ midX, static_cast<float>(dim.y) }, COLOR_BUTTON_BG_BLACK);
	gradient[3] = sf::Vertex({ 0.F, static_cast<float>(dim.y) }, sf::Color::Transparent);

	// right half
	gradient[4] = sf::Vertex({ midX, 0.F }, COLOR_BUTTON_BG_BLACK);
	gradient[5] = sf::Vertex({ static_cast<float>(dim.x), 0.F }, sf::Color::Transparent);
	gradient[6] = sf::Vertex({ static_cast<float>(dim.x), static_cast<float>(dim.y) }, sf::Color::Transparent);
	gradient[7] = sf::Vertex({ midX, static_cast<float>(dim.y) }, COLOR_BUTTON_BG_BLACK);

	// center button text
	// to center vertically, we can't use local bounds, as the baselines on different buttons would not match.
	// use a constant top offset instead
	this->text.setPosition(std::round(dim.x - this->text.getLocalBounds().width) / 2,
						   std::round((dim.y / 2) - (SettingsManager::guiScale * BTN_TEXT_TOP_OFFSET)));
}

void SimpleButton::setText(const std::string& text)
{
	this->text.setString(text);
	this->setGuiScale(); // need to re-center text
}

bool SimpleButton::containsPoint(sf::Vector2i coords)
{
	coords -= this->getPosition();

	return this->rect.getLocalBounds().contains(static_cast<sf::Vector2f>(coords));
}

void SimpleButton::handleSettingsChange()
{
	this->handleGuiScaleChange();
	this->setColor();
	this->setGuiScale();
}

void SimpleButton::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->rect, states);
	target.draw(this->gradient, states);
	target.draw(this->text, states);
}
