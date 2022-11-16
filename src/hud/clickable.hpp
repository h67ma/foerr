#pragma once

#include <SFML/System/Vector2.hpp>

enum ClickStatus
{
	CLICK_NOT_CONSUMED,
	CLICK_CONSUMED,			// omnomnom
	CLICK_CONSUMED_CLOSE,	// special status used to signal that PipBuck should be closed
	CLICK_CONSUMED_UNLOAD,	// special status used to signal that PipBuck should unload its campaign infos
};

class Clickable
{
	public:
		virtual bool containsPoint(sf::Vector2i coords) = 0;
		virtual ClickStatus handleLeftClick(sf::Vector2i clickPos) = 0;
};
