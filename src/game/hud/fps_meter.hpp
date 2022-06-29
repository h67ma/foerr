#pragma once

#include <SFML/Graphics.hpp>
#include "../consts.h"

#define FPS_METER_MAX_CHARS 6 // yes, we CAN go beyond 4 digits with fps limit off
#define FPS_METER_UPDATE_S 0.25

class FpsMeter
{
	private:
		sf::RenderWindow *window;
		sf::Text text;
		uint fontSize;
		uint elapsedFrames;
		bool firstFrame = true;
		char buf[FPS_METER_MAX_CHARS];
		sf::Clock clock;
	public:
		FpsMeter(sf::RenderWindow *window, sf::Font *font, uint fontSize);
		void updateFontSize(uint newSize);
		void draw();
};
