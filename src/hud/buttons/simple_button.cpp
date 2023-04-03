#include "simple_button.hpp"

#include <utility>
#include <string>

#include "../../settings/settings_manager.hpp"
#include "../../util/util.hpp"

#define BTN_TEXT_SMALL_TOP_OFFSET 8U
#define BTN_TEXT_NORMAL_TOP_OFFSET 12U
#define BTN_TEXT_LARGE_TOP_OFFSET 17U

// values from 0 to 255, where 255 is the original color and 0 is black
#define BTN_COLOR_SEL_FACTOR 104
#define BTN_COLOR_UNSEL_FACTOR 68
#define BTN_COLOR_HOVER_FACTOR 140

const sf::Color buttonBgBlack(0x00, 0x0B, 0x06);

SimpleButton::SimpleButton(SimpleButtonSize size, ResourceManager &resMgr, sf::Vector2u position,
						   const std::string &text, std::function<void(void)> callback,
						   ClickStatus consumedStatus) :
	Button(position, std::move(callback), consumedStatus)
{
	this->size = size;

	// text
	this->text.setFont(*resMgr.getFont(FONT_MEDIUM));
	this->text.setString(text);

	this->setGuiScale();
	this->setColor();

	// disabled by default
	this->setSelected(false);
}

void SimpleButton::setThickness()
{
	float thicc;
	switch (SettingsManager::guiScale)
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
	uint w, h, textTopOffset;

	this->text.setCharacterSize(getFontSize(SettingsManager::guiScale, FONT_H3));

	if (SettingsManager::guiScale == GUI_SMALL)
	{
		textTopOffset = BTN_TEXT_SMALL_TOP_OFFSET;

		switch (this->size)
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
	else if (SettingsManager::guiScale == GUI_LARGE)
	{
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

	uint midX = w/2;

	// 0 -- 1
	// |	|
	// 3 -- 2

	// left half
	gradient[0] = sf::Vertex({ 0.F, 0.F }, sf::Color::Transparent);
	gradient[1] = sf::Vertex({ static_cast<float>(midX), 0.F }, buttonBgBlack);
	gradient[2] = sf::Vertex({ static_cast<float>(midX), static_cast<float>(h) }, buttonBgBlack);
	gradient[3] = sf::Vertex({ 0.F, static_cast<float>(h) }, sf::Color::Transparent);

	// right half
	gradient[4] = sf::Vertex({ static_cast<float>(midX), 0.F }, buttonBgBlack);
	gradient[5] = sf::Vertex({ static_cast<float>(w), 0.F }, sf::Color::Transparent);
	gradient[6] = sf::Vertex({ static_cast<float>(w), static_cast<float>(h) }, sf::Color::Transparent);
	gradient[7] = sf::Vertex({ static_cast<float>(midX), static_cast<float>(h) }, buttonBgBlack);

	// center button text
	// to center vertically, we can't use local bounds, as the baselines on different buttons would not match.
	// use a constant top offset instead
	this->text.setPosition(
		static_cast<float>(((w - this->text.getLocalBounds().width) / 2)),
		static_cast<float>((h / 2) - textTopOffset)
	);
}

void SimpleButton::setText(const std::string &text)
{
	this->text.setString(text);
	this->setGuiScale(); // need to re-center text
}

bool SimpleButton::containsPoint(sf::Vector2i coords)
{
	coords -= this->getIntPosition();

	return this->rect.getLocalBounds().contains(static_cast<sf::Vector2f>(coords));
}

void SimpleButton::handleSettingsChange()
{
	this->setColor();
	this->setGuiScale();
}

void SimpleButton::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->rect, states);
	target.draw(this->gradient, states);
	target.draw(this->text, states);
}
