#include "log.hpp"
#include "util.hpp"

Log::Log(sf::RenderWindow *window, sf::Font *font, Settings *settings)
{
	this->window = window;
	this->font = font;
	this->settings = settings;
}

/**
 * Writes a message to log file.
 */
void Log::logToFile(LogMsgType msgType, std::string msg)
{
	// TODO actually write to log file
	std::cerr << '[' << logMsgTypeToPrefix(msgType) << "] " << msg << std::endl;
}

void Log::updateFontSize()
{
	for (LogElementText &item : this->history)
	{
		item.setCharacterSize(this->settings->normalFontSize.val); // TODO do we actually operate on list elements, or on iterator?
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

	// initial offset from top/bottom
	if (this->settings->logAnchor.val == CORNER_BOTTOM_LEFT || this->settings->logAnchor.val == CORNER_BOTTOM_RIGHT)
		y = this->window->getSize().y - this->history.size() * this->settings->normalFontSizeWithGap.val - 2;

	for (LogElementText &item : this->history)
	{
		if (this->settings->logAnchor.val == CORNER_TOP_RIGHT || this->settings->logAnchor.val == CORNER_BOTTOM_RIGHT)
			x = this->window->getSize().x - item.getLocalBounds().width - 7;

		item.setPosition(x, y);
		this->window->draw(item);

		y += this->settings->normalFontSizeWithGap.val;

		if (item.isTimeUp())
			timesUpCnt++;
	}

	for (uint i = 0; i < timesUpCnt; i++)
		this->history.pop_front();
}
