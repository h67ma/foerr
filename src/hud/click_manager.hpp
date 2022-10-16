#pragma once

#include <vector>
#include <SFML/System/Vector2.hpp>
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
		ClickStatus handleLeftClick(int x, int y, sf::Vector2f relPosition);
		void clear();
};
