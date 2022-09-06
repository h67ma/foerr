#pragma once

#include <SFML/Graphics.hpp>
#include "../hover_manager.hpp"
#include "../../consts.hpp"

/**
 * Represents an abstract PipBuck page (e.g. Armor).
 * TODO Might display additional elements outside PipBuck screen area (e.g. question mark button).
 *
 * TODO define an area where all page contents are contained. Then when handling clicks/hover events
 * first check if coordinates are within the defined area. This way, if the point is outside page bounds,
 * we can skip checking controls inside the page altogether.
 */
class PipBuckPage : public sf::Drawable, public sf::Transformable
{
	protected:
		HoverManager hoverMgr;
		sf::Text dummy; // TODO delet this

	public:
		virtual ClickStatus handleLeftClick(int x, int y) { return CLICK_NOT_CONSUMED; };
		virtual bool handleMouseMove(int x, int y);
		virtual std::string getLabel() = 0;
};
