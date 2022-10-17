#include "click_manager.hpp"

/**
 * Adds a Clickable to the collection of Clickables tracked by this object.
 *
 * @param clickable a Clickable to add
 */
void ClickManager::operator +=(Clickable *clickable)
{
	this->clickables.emplace_back(clickable);
}

/**
 * Handles left mouse click for managed Clickables.
 *
 * @param clickPos click position (parent position already subtracted)
 * @return click status if click was consumed by managed Clickable, or CLICK_NOT_CONSUMED if none consumed the click
 */
ClickStatus ClickManager::handleLeftClick(sf::Vector2i clickPos)
{
	for (auto &clickable : this->clickables)
	{
		if (clickable->handleLeftClick(clickPos) != CLICK_NOT_CONSUMED)
			return CLICK_CONSUMED;
	}

	return CLICK_NOT_CONSUMED;
}

void ClickManager::clear()
{
	this->clickables.clear();
}
