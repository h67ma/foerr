#pragma once

#include <SFML/Graphics.hpp>

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
		sf::Text dummy; // TODO delet this

	public:
		virtual std::string getLabel() = 0;
};
