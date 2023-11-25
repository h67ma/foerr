// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "hover_manager.hpp"

/**
 * Adds a Hoverable to the collection of Hoverables tracked by this object.
 *
 * @param hoverable a Hoverable to add
 */
void HoverManager::operator+=(Hoverable* hoverable)
{
	this->hoverables.emplace_back(hoverable);
}

/**
 * Searches the collection of hoverable objects and checks if any of
 * them is being hovered over. If so, the found item's hover is enabled
 * and the search stops. This means that only one item can be highlighted
 * by hover at any time.
 *
 * @param mousePos mouse position (parent position already subtracted)
 * @returns true if an element which is being hovered over was found
 * @returns false if no element managed by this object is being hovered over
 */
bool HoverManager::searchSetHover(sf::Vector2i mousePos)
{
	for (auto it = this->hoverables.begin(); it != this->hoverables.end(); it++)
	{
		if ((*it)->containsPoint(mousePos))
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
 * @param mousePos mouse position (parent position already subtracted)
 * @returns true if any element managed by this object is being hovered over
 * @returns false if no element managed by this object is being hovered over
 */
bool HoverManager::handleMouseMove(sf::Vector2i mousePos)
{
	if (this->lastHoveredIdx == NO_HOVER_IDX)
	{
		// there was no item which we hovered over previously.
		// need to search through hoverables and check if we entered any of them.
		// if none is found, last hover idx will still remain at NO_HOVER_IDX.
		return this->searchSetHover(mousePos);
	}

	// there was an item which was previously hovered-over. there's a good chance
	// it's still being hovered now, so check it before iterating the whole collection
	// of hoverables. if it's still hovered, then we don't need to change anything.
	if (this->hoverables[this->lastHoveredIdx]->containsPoint(mousePos))
		return true;

	// previous item lost hover, deselect it
	this->hoverables[this->lastHoveredIdx]->setHover(false);

	// now we need to check if any new item gained hover, and if none then set NO_HOVER_IDX.
	if (this->searchSetHover(mousePos))
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
