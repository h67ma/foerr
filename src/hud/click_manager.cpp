#include "click_manager.hpp"

void ClickManager::addClickable(Clickable *clickable)
{
	this->clickables.emplace_back(clickable);
}

/**
 * Handles left mouse click for managed Clickables.
 *
 * @param x x mouse coordinate
 * @param y y mouse coordinate
 * @param relPosition position of the parent component
 * @return click status if click was consumed by managed Clickable, or CLICK_NOT_CONSUMED if none consumed the click
 */
ClickStatus ClickManager::handleLeftClick(int x, int y, sf::Vector2f relPosition)
{
	// account for the component's position
	x -= static_cast<int>(relPosition.x);
	y -= static_cast<int>(relPosition.y);

	for (auto &clickable : this->clickables)
	{
		if (clickable->handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
			return CLICK_CONSUMED;
	}

	return CLICK_NOT_CONSUMED;
}

void ClickManager::clear()
{
	this->clickables.clear();
}
