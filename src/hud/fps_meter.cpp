#include "fps_meter.hpp"

#include <math.h>

#include "../settings/settings_manager.hpp"
#include "../util/util.hpp"
#include "../consts.hpp"

FpsMeter::FpsMeter(sf::Font &font, sf::Vector2u windowSize)
{
	this->text.setFont(font);
	this->setGuiScale();
	this->text.setFillColor(sf::Color::Green);
	this->text.setString("??");
	this->setPosition(windowSize);
}

void FpsMeter::setGuiScale()
{
	this->text.setCharacterSize(getFontSize(SettingsManager::guiScale, FONT_H3));
}

void FpsMeter::setPosition(sf::Vector2u windowSize)
{
	switch(SettingsManager::fpsAnchor)
	{
		case CORNER_TOP_RIGHT:
			this->text.setPosition(windowSize.x - FPS_ANCHOR_NEG_PADDING_RIGHT - this->text.getLocalBounds().width, FPS_ANCHOR_PADDING_TOP);
			break;
		case CORNER_BOTTOM_LEFT:
			this->text.setPosition(FPS_ANCHOR_PADDING_LEFT, windowSize.y - FPS_ANCHOR_NEG_PADDING_BOTTOM - this->text.getLocalBounds().height);
			break;
		case CORNER_BOTTOM_RIGHT:
			this->text.setPosition(windowSize.x - FPS_ANCHOR_NEG_PADDING_RIGHT - this->text.getLocalBounds().width, windowSize.y - FPS_ANCHOR_NEG_PADDING_BOTTOM - this->text.getLocalBounds().height);
			break;
		case CORNER_TOP_LEFT:
		default:
			this->text.setPosition(FPS_ANCHOR_PADDING_LEFT, FPS_ANCHOR_PADDING_TOP);
			break;
	}
}

void FpsMeter::tick()
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
