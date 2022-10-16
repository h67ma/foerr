#include "hover_manager.hpp"

void HoverManager::addHoverable(Hoverable *hoverable)
{
	this->hoverables.emplace_back(hoverable);
}

/**
 * Searches the collection of hoverable objects and checks if any of
 * them is being hovered over. If so, the found item's hover is enabled
 * and the search stops. This means that only one item can be highlighted
 * by hover at any time.
 *
 * @param x the x mouse coordinate
 * @param y the y mouse coordinate
 * @returns true if an element which is being hovered over was found
 * @returns false if no element managed by this object is being hovered over
 */
bool HoverManager::searchSetHover(int x, int y)
{
	for (auto it = this->hoverables.begin(); it != this->hoverables.end(); it++)
	{
		if ((*it)->containsPoint(x, y))
		{
			this->lastHoveredIdx = static_cast<int>(std::distance(this->hoverables.begin(), it));
			(*it)->setHover(true);
			return true;
		}
	}

	return false;
}

/**
 * Handles mouse move for managed Hoverables.
 *
 * @param x the x mouse coordinate
 * @param y the y mouse coordinate
 * @param relPosition position of the parent component
 * @returns true if any element managed by this object is being hovered over
 * @returns false if no element managed by this object is being hovered over
 */
bool HoverManager::handleMouseMove(int x, int y, sf::Vector2f relPosition)
{
	// account for the component's position
	x -= static_cast<int>(relPosition.x);
	y -= static_cast<int>(relPosition.y);

	if (this->lastHoveredIdx == NO_HOVER_IDX)
	{
		// there was no item which we hovered over previously.
		// need to search through hoverables and check if we entered any of them.
		// if none is found, last hover idx will still remain at NO_HOVER_IDX.
		return this->searchSetHover(x, y);
	}

	// there was an item which was previously hovered-over. there's a good chance
	// it's still being hovered now, so check it before iterating the whole collection
	// of hoverables. if it's still hovered, then we don't need to change anything.
	if (this->hoverables[this->lastHoveredIdx]->containsPoint(x, y))
		return true;

	// previous item lost hover, deselect it
	this->hoverables[this->lastHoveredIdx]->setHover(false);
	
	// now we need to check if any new item gained hover, and if none then set NO_HOVER_IDX.
	if (this->searchSetHover(x, y))
	{
		return true;
	}
	else
	{
		// new hover item not found
		this->lastHoveredIdx = NO_HOVER_IDX;
		return false;
	}
}

/**
 * Removes hover effect from currently hovered over item, if any.
 */
void HoverManager::removeHover()
{
	if (this->lastHoveredIdx == NO_HOVER_IDX)
		return;

	this->hoverables[this->lastHoveredIdx]->setHover(false);
}

void HoverManager::clear()
{
	this->hoverables.clear();
}
