#include "log.hpp"

Log::Log(sf::RenderWindow *window, sf::Font *font, Settings *settings)
{
	this->window = window;
	this->font = font;
	this->settings = settings;
}

void Log::updateFontSize()
{
	for (LogElementText &item : this->history)
	{
		item.setCharacterSize(this->settings->getUint(SETT_NORMAL_FONT_SIZE)); // TODO do we actually operate on list elements, or on iterator?
	}
}

/**
 * Draws message log items.
 */
void Log::draw()
{
	uint x = 5; // constant offset from left, will be enough for CORNER_*_LEFT
	uint y = 2; // constant offset from top, will be enough for CORNER_TOP_*
	uint timesUpCnt = 0;
	ScreenCorner anchor = this->settings->getScreenCorner(SETT_ANCHOR_LOG);
	uint lineHeight = this->settings->getUint(SETT_NORMAL_FONT_SIZE_WITH_GAP);

	// initial offset from top/bottom
	if (anchor == CORNER_BOTTOM_LEFT || anchor == CORNER_BOTTOM_RIGHT)
		y = this->window->getSize().y - this->history.size() * lineHeight - 2;

	for (LogElementText &item : this->history)
	{
		if (anchor == CORNER_TOP_RIGHT || anchor == CORNER_BOTTOM_RIGHT)
			x = this->window->getSize().x - item.getLocalBounds().width - 7;

		item.setPosition(x, y);
		this->window->draw(item);

		y += lineHeight;

		if (item.isTimeUp())
			timesUpCnt++;
	}

	for (uint i = 0; i < timesUpCnt; i++)
		this->history.pop_front();
}
