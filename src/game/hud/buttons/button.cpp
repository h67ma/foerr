#include "buttons/simple_button.hpp"
#include "../consts.hpp"
#include "../util/util.hpp"

Button::Button(GuiScale scale, uint x, uint y, std::function<void(void)> callback)
{
	this->callback = callback;
	
	this->setPosition(static_cast<float>(x), static_cast<float>(y));
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
