#pragma once

#include <vector>
#include "clickable.hpp"

/**
 * Manages click-related behaviour of a collection of clickable elements.
 *
 * About the usage of raw pointers: see the note in hover_manager.hpp.
 */
class ClickManager
{
	private:
		std::vector<Clickable*> clickables;

	public:
		void addClickable(Clickable *clickable);
		ClickStatus handleLeftClick(sf::Vector2i clickPos);
		void clear();
};
