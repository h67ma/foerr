#pragma once

#include <list>
#include <string>
#include <SFML/Graphics.hpp>
#include "util.hpp"
#include "settings.hpp"
#include "log_element_text.hpp"

#define HISTORY_LEN 100

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
			return sf::Color(64, 64, 64);
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

class Log
{
	private:
		sf::RenderWindow *window;
		sf::Font *font;
		Settings *settings;
		std::list<LogElementText> history;

	public:
		Log(sf::RenderWindow *window, sf::Font *font, Settings *settings);
		template<typename... T> void log(LogMsgType msgType, const char *fmt, T... args);
		void logToFile(LogMsgType msgType, std::string msg);
		void updateFontSize();
		void draw();
};

/**
 * Adds a message to the game log. Both `fmt` and `args` must not contain newlines.
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

	if (this->settings->getBool(SETT_WRITE_DEBUG_LOG_TO_FILE))
		this->logToFile(msgType, formatted);

	if (msgType == LOG_DEBUG && !this->settings->getBool(SETT_DISPLAY_DEBUG_MSGS_IN_LOG))
		return;

	LogElementText logElem(formatted, this->font, this->settings->getUint(SETT_NORMAL_FONT_SIZE), logMsgTypeToColor(msgType));

	this->history.push_back(logElem);
}
