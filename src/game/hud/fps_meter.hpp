#pragma once

#include <SFML/Graphics.hpp>
#include "../consts.h"
#include "hud.hpp"

#define FPS_METER_MAX_CHARS 6 // yes, we CAN go beyond 4 digits with fps limit off
#define FPS_METER_UPDATE_S 0.25

#define FPS_ANCHOR_PADDING_LEFT 3
#define FPS_ANCHOR_NEG_PADDING_RIGHT 5
#define FPS_ANCHOR_PADDING_TOP 0
#define FPS_ANCHOR_NEG_PADDING_BOTTOM 10

class FpsMeter
{
	private:
		sf::Text text;
		uint elapsedFrames;
		bool firstFrame = true;
		char buf[FPS_METER_MAX_CHARS];
		sf::Clock clock;
		ScreenCorner anchor;
	public:
		FpsMeter(sf::Font *font, uint fontSize, ScreenCorner anchor);
		void setFontSize(uint newSize);
		void setAnchor(ScreenCorner anchor);
		void updatePosition(uint w, uint h);
		void draw(sf::RenderWindow *window);
};
