#pragma once

#include <vector>
#include "hoverable.hpp"

#define NO_HOVER_IDX -1

/**
 * Manages hover-related behaviour of a collection of hoverable elements.
 *
 * About the usage of raw pointers:
 * Storing raw pointers *can* cause problematic situations if the addresses
 * of target elements change. One particular case of this is when initializing/
 * adding items to a collection. Object which wants to use the hover manager adds
 * all elements which it wants to respond to hover events to hover manager via
 * ::addHoverable() in its constructor. But as the object is added to collectoin,
 * an implicit copy happens, causing actual hoverables to have different addresses.
 * Because of this risk, it is recommended to NOT add items to hover manager in
 * constructor and instead use a post-construction method, e.g. ::setup(), in which
 * hoverable items are added to hover manager. If the object is moved, hover manager
 * should be reset (::clear()) and re-initialized after object is copyed.
 * Another approach might be to delete the implicit copy constructor from object which
 * uses hover manager and e.g. use emplace_back() to store a collection of objects, but
 * this is a bit limited as it requires all objects in collection to be of the same type.
 *
 * An alternate solution could be to use shared pointers, forcing objects that want to
 * track hover to store hoverable elements as shared pointers, thus ensuring we're
 * storing an address to something that exists. This can get annoying though, and also
 * will probably be a bit less efficient, which is not without meaning, as hover events
 * happen very frequently.
 */
class HoverManager
{
	private:
		int lastHoveredIdx = NO_HOVER_IDX;
		std::vector<Hoverable*> hoverables;
		bool searchSetHover(sf::Vector2i mousePos);

	public:
		void operator +=(Hoverable *hoverable);
		bool handleMouseMove(sf::Vector2i mousePos);
		void removeHover();
		void clear();
};
