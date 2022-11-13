#pragma once

#include <list>
#include <string>
#include <iostream>
#include <fstream>
#include <memory>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>
#include "../util/util.hpp"
#include "log_element_text.hpp"
#include "../consts.hpp"
#include "../hud/hud.hpp"

#define LOG_PREFIX_ERROR	"[ERRO] "
#define LOG_PREFIX_WARNING	"[WARN] "
#define LOG_PREFIX_INFO		"[INFO] "
#define LOG_PREFIX_DEBUG	"[DEBG] "
#define LOG_PREFIX_VERBOSE	"[VERB] "

class Log
{
	private:
		static sf::Font *font;
		static ScreenCorner anchor;
		static sf::Vector2u windowSize;
		static bool writeLogToFile;
		static bool printMsgs;
		static bool verboseDebug;
		static GuiScale scale;
		static uint fontGap;
		static std::list<std::unique_ptr<LogElementText>> history;
		static sf::Clock clock;
		static std::ofstream logFile;
		static void logToFile(const char* prefix, std::string msg);
		static void logStderr(const char* prefix, std::string msg);

	public:
		static void setFont(sf::Font *font);
		static void setPosition(ScreenCorner anchor, sf::Vector2u windowSize);
		static void openLogFile(std::string logFilePath);
		static void setWriteLogToFile(bool writeLogToFile);
		static void setPrintMsgs(bool printMsgs);
		static void setVerboseDebug(bool verboseDebug);
		static void setGuiScale(GuiScale scale);
		static void maybeUpdate(bool force = false);
		static void draw(sf::RenderTarget &target);
		static void close();

		/**
		 * Adds a formatted message to the game log. Both `fmt` and `args` must not contain newlines.
		 */
		template<typename... T>
		static void log(const char* prefix, sf::Color color, bool hideInGui, const char *fmt, T... args)
		{
			// TODO problem: translations will be loaded from file and can specify arbitrary (incorrect) format strings,
			// e.g. "number format %s" which is supposed to load a %d, not %s. this can lead to crashes and potentially memory exploits.
			// for now it will do, but in the future replace it with some custom implementation that detects incorrect format string.
			// e.g. log("user defined %f string %d", "sd", "format", 3000) - "sd" meaning the format string *must* contain %s followed by %d.
			// otherwise it will print an error msg.
			// another approach would be to check user format strings as they are loaded from file. this could be pretty tedious, as we'd have to check every single string

			std::string formatted = litSprintf(fmt, args...);

			if (Log::printMsgs)
				Log::logStderr(prefix, formatted);

			if (Log::writeLogToFile)
				Log::logToFile(prefix, formatted);

			// don't display debug msgs in hud, or any messages whatsoever when gui is not initialized
			if (hideInGui || Log::font == nullptr)
				return;

			Log::history.emplace_back(std::make_unique<LogElementText>(formatted, *Log::font, Log::scale, color));
			maybeUpdate(true);
		}

		/**
		 * Logs a critical message.
		 */
		template<typename... T>
		static inline void e(const char *fmt, T... args)
		{
			log(LOG_PREFIX_ERROR, sf::Color::Red, false, fmt, args...);
		}

		/**
		 * Logs a warning message.
		 */
		template<typename... T>
		static inline void w(const char *fmt, T... args)
		{
			log(LOG_PREFIX_WARNING, sf::Color::Yellow, false, fmt, args...);
		}

		/**
		 * Logs a message.
		 */
		template<typename... T>
		static inline void i(const char *fmt, T... args)
		{
			log(LOG_PREFIX_INFO, sf::Color::White, false, fmt, args...);
		}

		/**
		 * Logs a debug message. Not visible in game hud.
		 */
		template<typename... T>
		static inline void d(const char *fmt, T... args)
		{
			log(LOG_PREFIX_DEBUG, sf::Color::Cyan, true, fmt, args...);
		}

		/**
		 * Logs a verbose message. Not visible in game hud. SETT_VERBOSE_DEBUG needs to be enabled.
		 */
		template<typename... T>
		static void v(const char *fmt, T... args)
		{
			if (!Log::verboseDebug)
				return;

			log(LOG_PREFIX_VERBOSE, sf::Color::Blue, true, fmt, args...);
		}
};
