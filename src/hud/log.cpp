#include "log.hpp"

#define LOG_UPDATE_FREQUENCY_MS 200U

#define LOG_ANCHOR_PADDING_LEFT 7U
#define LOG_ANCHOR_NEG_PADDING_RIGHT 7U
#define LOG_ANCHOR_PADDING_TOP 2U
#define LOG_ANCHOR_NEG_PADDING_BOTTOM 7U

sf::Font* Log::font = nullptr;
ScreenCorner Log::anchor;
sf::Vector2u Log::windowSize;

// default values are only relevant until SettingsManager is loaded
bool Log::writeLogToFile = false;
bool Log::printMsgs = true;
bool Log::verboseDebug = true;
GuiScale Log::scale = GUI_NORMAL;
uint Log::fontGap;

std::list<std::unique_ptr<LogElementText>> Log::history;
sf::Clock Log::clock;

std::ofstream Log::logFile;

void Log::setFont(sf::Font *font)
{
	Log::font = font;
}

// TODO when we'll have the ability to modify settings during runtime,
// we need to call this when setting SETT_WRITE_LOG_TO_FILE = true
void Log::setPosition(ScreenCorner anchor, sf::Vector2u windowSize)
{
	Log::anchor = anchor;
	Log::windowSize = windowSize;
}

void Log::openLogFile(std::string logFilePath)
{
	Log::logFile.open(logFilePath);
}

void Log::setWriteLogToFile(bool writeLogToFile)
{
	Log::writeLogToFile = writeLogToFile;
}

void Log::setPrintMsgs(bool printMsgs)
{
	Log::printMsgs = printMsgs;
}

void Log::setVerboseDebug(bool verboseDebug)
{
	Log::verboseDebug = verboseDebug;
}

void Log::setGuiScale(GuiScale scale)
{
	Log::scale = scale;
	Log::fontGap = getFontGap(scale, FONT_H3);
	for (const auto &item : Log::history)
	{
		item->setGuiScale(Log::scale);
	}
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

	if ((Log::history.empty() || Log::clock.getElapsedTime().asMilliseconds() < LOG_UPDATE_FREQUENCY_MS) && !force)
		return;

	// remove items which were in the log for longer than LOG_ELEMENT_LIFE_TIME_S
	Log::history.remove_if([](const auto &item){ return item->isTimeUp(); });

	// initial offset from top/bottom
	if (Log::anchor == CORNER_BOTTOM_LEFT || Log::anchor == CORNER_BOTTOM_RIGHT)
		y = Log::windowSize.y - static_cast<uint>(Log::history.size()) * Log::fontGap - LOG_ANCHOR_NEG_PADDING_BOTTOM;

	for (auto &item : Log::history)
	{
		if (Log::anchor == CORNER_TOP_RIGHT || Log::anchor == CORNER_BOTTOM_RIGHT)
			x = Log::windowSize.x - static_cast<uint>(item->getLocalBounds().width) - LOG_ANCHOR_NEG_PADDING_RIGHT;

		item->setPosition(static_cast<float>(x), static_cast<float>(y));

		y += Log::fontGap;
	}

	Log::clock.restart();
}

void Log::draw(sf::RenderTarget &target)
{
	for (const auto &item : Log::history)
	{
		target.draw(*item);
	}
}

/**
 * Writes a formatted message to log file.
 */
void Log::logToFile(const char* prefix, std::string msg)
{
	Log::logFile << prefix << msg << std::endl;
}

/**
 * Writes a formatted message to stderr.
 *
 * Use only if no Log object is available (writing to hud or log file is preferred).
 */
void Log::logStderr(const char* prefix, std::string msg)
{
	std::cerr << prefix << msg << std::endl;
}

// TODO when we'll have the ability to modify settings during runtime,
// we need to call this when setting SETT_WRITE_LOG_TO_FILE = false
void Log::close()
{
	Log::logFile.close();
}
