#include <fstream>
#include "log.hpp"

namespace Log
{
	sf::Font *_font = nullptr;
	ScreenCorner _anchor;
	uint _windowW;
	uint _windowH;

	// default values are only relevant until SettingsManager is loaded
	bool _writeLogToFile = false;
	bool _printMsgs = true;
	bool _verboseDebug = true;
	GuiScale _scale = GUI_NORMAL;
	uint _fontGap;

	std::list<std::unique_ptr<LogElementText>> _history;
	sf::Clock _clock;

	// empty file will be created even if user has disabled writing to log file.
	// if user enables writing to log mid-game we'll have somewhere to write to.
	// otherwise we'd have to check if the file exists every time we write.
	std::ofstream _logFile = std::ofstream(PATH_LOGFILE);

	void setFont(sf::Font *font)
	{
		_font = font;
	}

	void setPosition(ScreenCorner anchor, uint windowW, uint windowH)
	{
		_anchor = anchor;
		_windowW = windowW;
		_windowH = windowH;
	}

	void setWriteLogToFile(bool writeLogToFile)
	{
		_writeLogToFile = writeLogToFile;
	}

	void setPrintMsgs(bool printMsgs)
	{
		_printMsgs = printMsgs;
	}

	void setVerboseDebug(bool verboseDebug)
	{
		_verboseDebug = verboseDebug;
	}

	void setGuiScale(GuiScale scale)
	{
		_scale = scale;
		_fontGap = getFontGap(scale, FONT_H3);
		for (const auto &item : _history)
		{
			item->setGuiScale(_scale);
		}
	}

	/**
	 * Removes old items from history.
	 * If update should happen (based on private clock) and there are items in history, calculate their positions.
	 *
	 * @param force update even if there was an update in last `LOG_UPDATE_FREQUENCY_MS`
	 */
	void maybeUpdate(bool force)
	{
		uint x = LOG_ANCHOR_PADDING_LEFT; // constant offset from left, will be enough for CORNER_*_LEFT
		uint y = LOG_ANCHOR_PADDING_TOP; // constant offset from top, will be enough for CORNER_TOP_*
		uint timesUpCnt = 0;

		if ((_history.empty() || _clock.getElapsedTime().asMilliseconds() < LOG_UPDATE_FREQUENCY_MS) && !force)
			return;

		// remove items which were in the log for longer than LOG_ELEMENT_LIFE_TIME_S
		_history.remove_if([](const auto &item){ return item->isTimeUp(); });

		// initial offset from top/bottom
		if (_anchor == CORNER_BOTTOM_LEFT || _anchor == CORNER_BOTTOM_RIGHT)
			y = _windowH - static_cast<uint>(_history.size()) * _fontGap - LOG_ANCHOR_NEG_PADDING_BOTTOM;

		for (auto &item : _history)
		{
			if (_anchor == CORNER_TOP_RIGHT || _anchor == CORNER_BOTTOM_RIGHT)
				x = _windowW - static_cast<uint>(item->getLocalBounds().width) - LOG_ANCHOR_NEG_PADDING_RIGHT;

			item->setPosition(static_cast<float>(x), static_cast<float>(y));

			y += _fontGap;
		}

		_clock.restart();
	}

	void draw(sf::RenderTarget &target)
	{
		for (const auto &item : _history)
		{
			target.draw(*item);
		}
	}

	/**
	 * Writes a formatted message to log file.
	 */
	void _logToFile(const char* prefix, std::string msg)
	{
		_logFile << prefix << msg << std::endl;
	}

	/**
	 * Writes a formatted message to stderr.
	 *
	 * Use only if no Log object is available (writing to hud or log file is preferred).
	 */
	void _logStderr(const char* prefix, std::string msg)
	{
		std::cerr << prefix << msg << std::endl;
	}

	void close()
	{
		_logFile.close();
	}
}
