#include "hover_manager.hpp"

void HoverManager::addHoverable(Hoverable *hoverable)
{
	this->hoverables.push_back(hoverable);
}

void HoverManager::handleMouseMove(int x, int y)
{
	// note: we could reuse the searching loop, but it just makes the code look more complicated

	if (this->lastHoveredIdx == NO_HOVER_IDX)
	{
		// there was no item which we hovered over previously.
		// need to search through hoverables and check if we entered any of them.
		// if none is found, last hover idx will still remain at NO_HOVER.
		for (auto it = this->hoverables.begin(); it != this->hoverables.end(); it++)
		{
			if ((*it)->containsPoint(x, y))
			{
				this->lastHoveredIdx = static_cast<int>(std::distance(this->hoverables.begin(), it));
				(*it)->setHover(true);
				break;
			}
		}
	}
	// there was an item which was previously hovered-over. there's a good chance
	// it's still being hovered now, so check it before iterating the whole collection
	// of hoverables. if it's still hovered, then we don't need to change anything.
	else if (!this->hoverables[this->lastHoveredIdx]->containsPoint(x, y))
	{
		// previous item lost hover. we need to check if any new item gained hover,
		// and if none then set NO_HOVER as last idx.

		this->hoverables[this->lastHoveredIdx]->setHover(false);

		bool foundNewHover = false;
		for (auto it = this->hoverables.begin(); it != this->hoverables.end(); it++)
		{
			if ((*it)->containsPoint(x, y))
			{
				this->lastHoveredIdx = static_cast<int>(std::distance(this->hoverables.begin(), it));
				(*it)->setHover(true);
				foundNewHover = true;
				break;
			}
		}

		if (!foundNewHover)
			this->lastHoveredIdx = NO_HOVER_IDX;
	}
}
