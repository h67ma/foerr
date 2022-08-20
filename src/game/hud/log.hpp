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
#define LOG_UPDATE_FREQUENCY_MS 200U

#define LOG_ANCHOR_PADDING_LEFT 7U
#define LOG_ANCHOR_NEG_PADDING_RIGHT 7U
#define LOG_ANCHOR_PADDING_TOP 2U
#define LOG_ANCHOR_NEG_PADDING_BOTTOM 7U

#define LOG_PREFIX_ERROR	"[ERRO] "
#define LOG_PREFIX_WARNING	"[WARN] "
#define LOG_PREFIX_INFO		"[INFO] "
#define LOG_PREFIX_DEBUG	"[DEBG] "
#define LOG_PREFIX_VERBOSE	"[VERB] "

	extern sf::Font *_font;
	extern bool _writeLogToFile;
	extern bool _printMsgs;
	extern bool _verboseDebug;
	extern GuiScale _scale;
	extern std::list<std::unique_ptr<LogElementText>> _history;

	void _logToFile(const char* prefix, std::string msg);
	void _logStderr(const char* prefix, std::string msg);

	void setFont(sf::Font *font);
	void setPosition(ScreenCorner anchor, uint windowW, uint windowH);
	void setWriteLogToFile(bool writeLogToFile);
	void setPrintMsgs(bool printMsgs);
	void setVerboseDebug(bool verboseDebug);
	void setScale(GuiScale scale);
	void maybeUpdate(bool force=false);
	void draw(sf::RenderTarget& target);
	void close();

	/**
	 * Adds a formatted message to the game log. Both `fmt` and `args` must not contain newlines.
	 */
	template<typename... T>
	void log(const char* prefix, sf::Color color, bool hideInGui, const char *fmt, T... args)
	{
		// TODO problem: translations will be loaded from file and can specify arbitrary (incorrect) format strings,
		// e.g. "number format %s" which is supposed to load a %d, not %s. this can lead to crashes and potentially memory exploits.
		// for now it will do, but in the future replace it with some custom implementation that detects incorrect format string.
		// e.g. log("user defined %f string %d", "sd", "format", 3000) - "sd" meaning the format string *must* contain %s followed by %d.
		// otherwise it will print an error msg.
		// another approach would be to check user format strings as they are loaded from file. this could be pretty tedious, as we'd have to check every single string

		std::string formatted = litSprintf(fmt, args...);

		if (_printMsgs)
			_logStderr(prefix, formatted);

		if (_writeLogToFile)
			_logToFile(prefix, formatted);

		// don't display debug msgs in hud, or any messages whatsoever when gui is not initialized
		if (hideInGui || _font == nullptr)
			return;

		LogElementText *logElem = new LogElementText(formatted, *_font, _scale, color);

		_history.push_back(std::unique_ptr<LogElementText>(logElem));
		maybeUpdate(true);
	}

	/**
	 * Logs a critical message.
	 */
	template<typename... T>
	inline void e(const char *fmt, T... args)
	{
		log(LOG_PREFIX_ERROR, sf::Color::Red, false, fmt, args...);
	}

	/**
	 * Logs a warning message.
	 */
	template<typename... T>
	inline void w(const char *fmt, T... args)
	{
		log(LOG_PREFIX_WARNING, sf::Color::Yellow, false, fmt, args...);
	}

	/**
	 * Logs a message.
	 */
	template<typename... T>
	inline void i(const char *fmt, T... args)
	{
		log(LOG_PREFIX_INFO, sf::Color::White, false, fmt, args...);
	}

	/**
	 * Logs a debug message. Not visible in game hud.
	 */
	template<typename... T>
	inline void d(const char *fmt, T... args)
	{
		log(LOG_PREFIX_DEBUG, sf::Color::Cyan, true, fmt, args...);
	}

	/**
	 * Logs a verbose message. Not visible in game hud. SETT_VERBOSE_DEBUG needs to be enabled.
	 */
	template<typename... T>
	void v(const char *fmt, T... args)
	{
		if (!_verboseDebug)
			return;

		log(LOG_PREFIX_VERBOSE, sf::Color::Blue, true, fmt, args...);
	}
}
