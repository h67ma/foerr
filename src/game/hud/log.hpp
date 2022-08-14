#pragma once

#include <list>
#include <string>
#include <iostream>
#include <SFML/Graphics.hpp>
#include "../util/util.hpp"
#include "log_element_text.hpp"
#include "../consts.hpp"
#include "../hud/hud.hpp"

/**
 * This is pretty bad, but still better than passing the Log object
 * everywhere we need to log something...
 * pls don't access members prefixed with `_` :)
 */
namespace Log
{
#define LOG_HISTORY_LEN 100
#define LOG_UPDATE_FREQUENCY_MS 200

#define LOG_ANCHOR_PADDING_LEFT 7
#define LOG_ANCHOR_NEG_PADDING_RIGHT 7
#define LOG_ANCHOR_PADDING_TOP 2
#define LOG_ANCHOR_NEG_PADDING_BOTTOM 7

	enum LogMsgType
	{
		LOG_ERROR,
		LOG_WARNING,
		LOG_INFO,
		LOG_DEBUG,
	};

	inline sf::Color _logMsgTypeToColor(LogMsgType msgType)
	{
		switch (msgType)
		{
			case LOG_ERROR:
				return sf::Color::Red;
				break;
			case LOG_WARNING:
				return sf::Color::Yellow;
				break;
			case LOG_INFO:
			default:
				// LOG_DEBUG are never shown in hud
				return sf::Color::White;
		}
	}

	inline const char* _logMsgTypeToPrefix(LogMsgType msgType)
	{
		switch (msgType)
		{
			case LOG_ERROR:
				return "ERROR";
				break;
			case LOG_WARNING:
				return "WARN";
				break;
			case LOG_DEBUG:
				return "DEBUG";
				break;
			case LOG_INFO:
			default:
				return "INFO";
		}
	}

	extern sf::Font *_font;
	extern bool _writeLogToFile;
	extern bool _printDebugMsgs;
	extern std::list<LogElementText> _history;

	void _logToFile(LogMsgType msgType, std::string msg);
	void _logStderr(LogMsgType msgType, std::string msg);

	void setFont(sf::Font *font);
	void setPosition(ScreenCorner anchor, uint windowW, uint windowH);
	void setWriteLogToFile(bool writeLogToFile);
	void setPrintDebugMsgs(bool printDebugMsgs);
	void maybeUpdate(bool force=false);
	void draw(sf::RenderTarget& target);
	void close();

	/**
	 * Adds a formatted message to the game log. Both `fmt` and `args` must not contain newlines.
	 */
	template<typename... T>
	void log(LogMsgType msgType, const char *fmt, T... args)
	{
		// TODO problem: translations will be loaded from file and can specify arbitrary (incorrect) format strings,
		// e.g. "number format %s" which is supposed to load a %d, not %s. this can lead to crashes and potentially memory exploits.
		// for now it will do, but in the future replace it with some custom implementation that detects incorrect format string.
		// e.g. log("user defined %f string %d", "sd", "format", 3000) - "sd" meaning the format string *must* contain %s followed by %d.
		// otherwise it will print an error msg.
		// another approach would be to check user format strings as they are loaded from file. this could be pretty tedious, as we'd have to check every single string

		std::string formatted = litSprintf(fmt, args...);

		if (_printDebugMsgs)
			_logStderr(msgType, formatted);

		if (_writeLogToFile)
			_logToFile(msgType, formatted);

		// don't display debug msgs in hud, or any messages whatsoever when gui is not initialized
		if (msgType == LOG_DEBUG || _font == nullptr)
			return;

		LogElementText logElem(formatted, _font, FONT_SIZE_NORMAL, _logMsgTypeToColor(msgType));

		_history.push_back(logElem);
		maybeUpdate(true);
	}
}
