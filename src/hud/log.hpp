// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <fstream>
#include <functional>
#include <iostream>
#include <list>
#include <string>

#include <SFML/Graphics/Color.hpp>
#include <SFML/Graphics/Font.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/System/Clock.hpp>

#include "../consts.hpp"
#include "../settings/settings_manager.hpp"
#include "../util/util.hpp"
#include "log_element_text.hpp"

#define LOG_PREFIX_ERROR "[ERRO] "
#define LOG_PREFIX_WARNING "[WARN] "
#define LOG_PREFIX_INFO "[INFO] "
#define LOG_PREFIX_DEBUG "[DEBG] "
#define LOG_PREFIX_VERBOSE "[VERB] "

using msg_add_function = std::function<void(const StringAndColor&)>;

class Log
{
	private:
		static sf::Font* font;
		static sf::Vector2u windowSize;

		static uint fontGap;
		static std::list<LogElementText> hudHistory;
		static sf::Clock clock;
		static std::ofstream logFile;
		static msg_add_function msgAddedCallback;
		static void logToFile(const char* prefix, const std::string& msg);
		static void logStderr(const char* prefix, const std::string& msg);

	public:
		static void setup();
		static void setFont(sf::Font* font);
		static void setMsgAddedCallback(const msg_add_function& callback);
		static void handleScreenResize(sf::Vector2u windowSize);
		static void handleSettingsChange();
		static void openLogFile();
		static void tick(bool force = false);
		static void draw(sf::RenderTarget& target);
		static void close();

		/**
		 * Adds a formatted message to the game log. Both `fmt` and `args` must not contain newlines.
		 */
		template<typename... T>
		static void log(const char* prefix, sf::Color color, bool hideInGui, const char* fmt, T... args)
		{
			// TODO problem: translations will be loaded from file and can specify arbitrary (incorrect) format strings,
			// e.g. "number format %s" which is supposed to load a %d, not %s. this can lead to crashes and potentially
			// memory exploits. for now it will do, but in the future replace it with some custom implementation that
			// detects incorrect format string. e.g. log("user defined %f string %d", "sd", "format", 3000) - "sd"
			// meaning the format string *must* contain %s followed by %d. otherwise it will print an error msg. another
			// approach would be to check user format strings as they are loaded from file. this could be pretty
			// tedious, as we'd have to check every single string

			std::string formatted = litSprintf(fmt, args...);

			if (SettingsManager::debugPrintToStderr)
				Log::logStderr(prefix, formatted);

			if (SettingsManager::debugWriteLogToFile)
				Log::logToFile(prefix, formatted);

			// don't display debug msgs in hud
			if (hideInGui)
				return;

			if (Log::font != nullptr)
			{
				Log::hudHistory.emplace_back(formatted, *Log::font, color);
				Log::tick(true);
			}

			if (msgAddedCallback != nullptr)
				msgAddedCallback({ formatted, color });
		}

		/**
		 * Logs a critical message.
		 */
		template<typename... T>
		static inline void e(const char* fmt, T... args)
		{
			log(LOG_PREFIX_ERROR, sf::Color::Red, false, fmt, args...);
		}

		/**
		 * Logs a warning message.
		 */
		template<typename... T>
		static inline void w(const char* fmt, T... args)
		{
			log(LOG_PREFIX_WARNING, sf::Color::Yellow, false, fmt, args...);
		}

		/**
		 * Logs a message.
		 */
		template<typename... T>
		static inline void i(const char* fmt, T... args)
		{
			log(LOG_PREFIX_INFO, sf::Color::White, false, fmt, args...);
		}

		/**
		 * Logs a debug message. Not visible in game hud.
		 */
		template<typename... T>
		static inline void d(const char* fmt, T... args)
		{
			log(LOG_PREFIX_DEBUG, sf::Color::Cyan, true, fmt, args...);
		}

		/**
		 * Logs a verbose message. Not visible in game hud. SETT_VERBOSE_DEBUG needs to be enabled.
		 */
		template<typename... T>
		static void v(const char* fmt, T... args)
		{
			if (!SettingsManager::debugVerbose)
				return;

			log(LOG_PREFIX_VERBOSE, sf::Color::Blue, true, fmt, args...);
		}
};
