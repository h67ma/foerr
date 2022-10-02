#include "simple_button.hpp"
#include "../../util/util.hpp"

Button::Button(GuiScale scale, sf::Vector2u position, std::function<void(void)> callback)
{
	this->callback = callback;

	// we use uints because we don't want to have buttons
	// "in between", because it may cause edges to blur
	this->setPosition(static_cast<sf::Vector2f>(position));
}

void Button::setCallback(std::function<void(void)> callback)
{
	this->callback = callback;
}

/**
 * Checks if click was placed inside button area.
 * If it was, and the callback function exists, the callback will be called.
*/
ClickStatus Button::handleLeftClick(int x, int y)
{
	if (!this->containsPoint(x, y))
		return CLICK_NOT_CONSUMED;

	if (this->callback != nullptr)
		this->callback();

	return CLICK_CONSUMED;
}
