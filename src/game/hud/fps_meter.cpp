#include <math.h>
#include <SFML/Graphics.hpp>
#include "fps_meter.hpp"
#include "../consts.hpp"

FpsMeter::FpsMeter(GuiScale scale, sf::Font &font)
{
	this->text.setFont(font);
	this->setScale(scale);
	this->text.setFillColor(sf::Color::Green);
	this->text.setString("??");
}

void FpsMeter::setScale(GuiScale scale)
{
	switch (scale)
	{
		case GUI_SMALL:
			this->text.setCharacterSize(FONT_SIZE_SMALL);
			break;
		case GUI_LARGE:
			this->text.setCharacterSize(FONT_SIZE_LARGE);
			break;
		case GUI_NORMAL:
		default:
			this->text.setCharacterSize(FONT_SIZE_NORMAL);
	}
}

void FpsMeter::setPosition(ScreenCorner anchor, uint screenW, uint screenH)
{
	switch(anchor)
	{
		case CORNER_TOP_RIGHT:
			this->text.setPosition(screenW - FPS_ANCHOR_NEG_PADDING_RIGHT - this->text.getLocalBounds().width, FPS_ANCHOR_PADDING_TOP);
			break;
		case CORNER_BOTTOM_LEFT:
			this->text.setPosition(FPS_ANCHOR_PADDING_LEFT, screenH - FPS_ANCHOR_NEG_PADDING_BOTTOM - this->text.getLocalBounds().height);
			break;
		case CORNER_BOTTOM_RIGHT:
			this->text.setPosition(screenW - FPS_ANCHOR_NEG_PADDING_RIGHT - this->text.getLocalBounds().width, screenH - FPS_ANCHOR_NEG_PADDING_BOTTOM - this->text.getLocalBounds().height);
			break;
		case CORNER_TOP_LEFT:
		default:
			this->text.setPosition(FPS_ANCHOR_PADDING_LEFT, FPS_ANCHOR_PADDING_TOP);
			break;
	}
}

void FpsMeter::maybeUpdate()
{
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

void FpsMeter::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->text);
}
