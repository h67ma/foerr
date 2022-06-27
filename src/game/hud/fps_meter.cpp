#include <math.h>
#include <SFML/Graphics.hpp>
#include "fps_meter.hpp"

FpsMeter::FpsMeter(sf::RenderWindow *window, sf::Font *font, uint fontSize)
{
	this->window = window;
	this->fontSize = fontSize;
	this->text.setFont(*font);
	this->text.setCharacterSize(fontSize);
	this->text.setPosition(2, 2);
	this->text.setFillColor(sf::Color::Green);
	this->text.setString("??");
}

void FpsMeter::updateFontSize(uint newSize)
{
	this->text.setCharacterSize(newSize);
}

void FpsMeter::draw()
{
	this->window->draw(this->text);

	if (this->firstFrame)
	{
		this->firstFrame = false;
		this->elapsedFrames = 0;
		this->clock.restart();
		return;
	}

	this->elapsedFrames++;

	float elapsedTime = this->clock.getElapsedTime().asSeconds();

	// only update the counter every FPS_METER_UPDATE_S seconds
	if (elapsedTime > FPS_METER_UPDATE_S)
	{
		if (elapsedTime > 0) // better safe than sorry
			snprintf(this->buf, FPS_METER_MAX_CHARS, "%.0f", this->elapsedFrames / elapsedTime);
		else
			snprintf(this->buf, FPS_METER_MAX_CHARS, "INF");

		this->text.setString(buf);
		this->firstFrame = true;
	}
}
