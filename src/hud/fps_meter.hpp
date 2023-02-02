#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Text.hpp>
#include <SFML/System/Clock.hpp>

#define FPS_METER_MAX_CHARS 6 // yes, we CAN go beyond 4 digits with fps limit off
#define FPS_METER_UPDATE_S 0.25

#define FPS_ANCHOR_PADDING_LEFT 3
#define FPS_ANCHOR_NEG_PADDING_RIGHT 5
#define FPS_ANCHOR_PADDING_TOP 0
#define FPS_ANCHOR_NEG_PADDING_BOTTOM 10

class FpsMeter : public sf::Drawable
{
	private:
		sf::Text text;
		uint elapsedFrames;
		bool firstFrame = true;
		char buf[FPS_METER_MAX_CHARS];
		sf::Clock clock;

	public:
		FpsMeter(sf::Font &font, sf::Vector2u windowSize);
		void setGuiScale();
		void setPosition(sf::Vector2u windowSize);
		void tick();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
