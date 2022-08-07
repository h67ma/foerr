#pragma once

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <SFML/Graphics.hpp>
#include "../util/util.hpp"
#include "log_element_text.hpp"
#include "../consts.h"
#include "../hud/hud.hpp"

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

inline sf::Color logMsgTypeToColor(LogMsgType msgType)
{
	switch (msgType)
	{
		case LOG_ERROR:
			return sf::Color::Red;
			break;
		case LOG_WARNING:
			return sf::Color::Yellow;
			break;
		case LOG_DEBUG:
			return sf::Color(80, 80, 80);
			break;
		case LOG_INFO:
		default:
			return sf::Color::White;
	}
}

inline const char* logMsgTypeToPrefix(LogMsgType msgType)
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

class Log : public sf::Drawable
{
	private:
		sf::Font *font;
		ScreenCorner anchor;
		uint windowW;
		uint windowH;
		bool writeLogToFile;
		bool displayDebugMsgsInLog;
		std::list<LogElementText> history;
		sf::Clock clock;
		void logToFile(LogMsgType msgType, std::string msg);

		// empty file will be created even if user has disabled writing to log file.
		// if user enables writing to log mid-game we'll have somewhere to write to.
		// otherwise we'd have to check if the file exists every time we write.
		std::ofstream logFile = std::ofstream(PATH_LOGFILE);

	public:
		Log(sf::Font *font);
		template<typename... T> void log(LogMsgType msgType, const char *fmt, T... args);
		template<typename... T> static void logStderr(LogMsgType msgType, const char *fmt, T... args);
		void setPosition(ScreenCorner anchor, uint windowW, uint windowH);
		void setWriteLogToFile(bool writeLogToFile);
		void setDisplayDebugMsgsInLog(bool displayDebugMsgsInLog);
		void maybeUpdate(bool force=false);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
		~Log();
};

/**
 * Adds a formatted message to the game log. Both `fmt` and `args` must not contain newlines.
 */
template<typename... T>
void Log::log(LogMsgType msgType, const char *fmt, T... args)
{
	// TODO problem: translations will be loaded from file and can specify arbitrary (incorrect) format strings,
	// e.g. "number format %s" which is supposed to load a %d, not %s. this can lead to crashes and potentially memory exploits.
	// for now it will do, but in the future replace it with some custom implementation that detects incorrect format string.
	// e.g. log("user defined %f string %d", "sd", "format", 3000) - "sd" meaning the format string *must* contain %s followed by %d.
	// otherwise it will print an error msg.
	// another approach would be to check user format strings as they are loaded from file. this could be pretty tedious, as we'd have to check every single string

	std::string formatted = litSprintf(fmt, args...);

	if (this->writeLogToFile)
		this->logToFile(msgType, formatted);

	if (msgType == LOG_DEBUG && !this->displayDebugMsgsInLog)
		return;

	LogElementText logElem(formatted, this->font, FONT_SIZE_NORMAL, logMsgTypeToColor(msgType));

	this->history.push_back(logElem);
	this->maybeUpdate(true);
}

/**
 * Writes a formatted message to stderr.
 *
 * Use only if no Log object is available (writing to hud or log file is preferred).
 */
template<typename... T>
void Log::logStderr(LogMsgType msgType, const char *fmt, T... args)
{
	std::cerr << '[' << logMsgTypeToPrefix(msgType) << "] " << litSprintf(fmt, args...) << std::endl;
}
