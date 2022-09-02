#pragma once

#include <SFML/Graphics/Transformable.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include "hover_manager.hpp"

class GuiContainer : public sf::Drawable, public sf::Transformable
{
	protected:
		HoverManager hoverMgr;

	public:
		void handleMouseMove(int x, int y);
};
