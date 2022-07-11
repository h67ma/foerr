#include <fstream>
#include "log.hpp"

Log::Log(sf::Font *font, SettingsManager *settings)
{
	this->font = font;
	this->settings = settings;
}

void Log::updateFontSize()
{
	for (LogElementText &item : this->history)
	{
		item.setCharacterSize(FONT_SIZE_NORMAL); // TODO do we actually operate on list elements, or on iterator?
	}
}

/**
 * Draws message log items.
 * TODO this should be calculated every 1s or 0.25s or whatever, then only draw the items.
 */
void Log::draw(sf::RenderWindow *window)
{
	uint x = LOG_ANCHOR_PADDING_LEFT; // constant offset from left, will be enough for CORNER_*_LEFT
	uint y = LOG_ANCHOR_PADDING_TOP; // constant offset from top, will be enough for CORNER_TOP_*
	uint timesUpCnt = 0;
	ScreenCorner anchor = this->settings->getScreenCorner(SETT_ANCHOR_LOG);

	// initial offset from top/bottom
	if (anchor == CORNER_BOTTOM_LEFT || anchor == CORNER_BOTTOM_RIGHT)
		y = window->getSize().y - this->history.size() * FONT_SIZE_NORMAL_WITH_GAP - LOG_ANCHOR_NEG_PADDING_BOTTOM;

	for (LogElementText &item : this->history)
	{
		if (anchor == CORNER_TOP_RIGHT || anchor == CORNER_BOTTOM_RIGHT)
			x = window->getSize().x - item.getLocalBounds().width - LOG_ANCHOR_NEG_PADDING_RIGHT;

		item.setPosition(x, y);
		window->draw(item);

		y += FONT_SIZE_NORMAL_WITH_GAP;

		if (item.isTimeUp())
			timesUpCnt++;
	}

	// remove items which were in the log for longer than LOG_ELEMENT_LIFE_TIME_S
	for (uint i = 0; i < timesUpCnt; i++)
		this->history.pop_front();
}

/**
 * Writes a formatted message to log file.
 */
void Log::logToFile(LogMsgType msgType, std::string msg)
{
	this->logFile << '[' << logMsgTypeToPrefix(msgType) << "] " << msg << std::endl;
}

Log::~Log()
{
	this->logFile.close();
}
