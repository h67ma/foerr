#include <fstream>
#include "log.hpp"

Log::Log(sf::Font *font)
{
	this->font = font;
}

void Log::setPosition(ScreenCorner anchor, uint windowW, uint windowH)
{
	this->anchor = anchor;
	this->windowW = windowW;
	this->windowH = windowH;
}

void Log::setWriteLogToFile(bool writeLogToFile)
{
	this->writeLogToFile = writeLogToFile;
}

void Log::setDisplayDebugMsgsInLog(bool displayDebugMsgsInLog)
{
	this->displayDebugMsgsInLog = displayDebugMsgsInLog;
}

/**
 * Removes old items from history.
 * If update should happen (based on private clock) and there are items in history, calculate their positions.
 * 
 * @param force update even if there was an update in last `LOG_UPDATE_FREQUENCY_MS`
 */
void Log::maybeUpdate(bool force)
{
	uint x = LOG_ANCHOR_PADDING_LEFT; // constant offset from left, will be enough for CORNER_*_LEFT
	uint y = LOG_ANCHOR_PADDING_TOP; // constant offset from top, will be enough for CORNER_TOP_*
	uint timesUpCnt = 0;

	if ((this->history.empty() || this->clock.getElapsedTime().asMilliseconds() < LOG_UPDATE_FREQUENCY_MS) && !force)
		return;

	// remove items which were in the log for longer than LOG_ELEMENT_LIFE_TIME_S
	this->history.remove_if([](LogElementText& item){ return item.isTimeUp(); });

	// initial offset from top/bottom
	if (this->anchor == CORNER_BOTTOM_LEFT || this->anchor == CORNER_BOTTOM_RIGHT)
		y = this->windowH - this->history.size() * FONT_SIZE_NORMAL_WITH_GAP - LOG_ANCHOR_NEG_PADDING_BOTTOM;

	for (LogElementText &item : this->history)
	{
		if (this->anchor == CORNER_TOP_RIGHT || this->anchor == CORNER_BOTTOM_RIGHT)
			x = this->windowW - item.getLocalBounds().width - LOG_ANCHOR_NEG_PADDING_RIGHT;

		item.setPosition(x, y);

		y += FONT_SIZE_NORMAL_WITH_GAP;
	}

	this->clock.restart();
}

void Log::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const LogElementText &item : this->history)
	{
		target.draw(item);
	}
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
