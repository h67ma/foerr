// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "log.hpp"

#define LOG_UPDATE_FREQUENCY_MS 200U

#define LOG_ANCHOR_PADDING_LEFT 7U
#define LOG_ANCHOR_NEG_PADDING_RIGHT 7U
#define LOG_ANCHOR_PADDING_TOP 2U
#define LOG_ANCHOR_NEG_PADDING_BOTTOM 7U

sf::Font* Log::font = nullptr;
sf::Vector2u Log::windowSize;

uint Log::fontGap;

std::list<std::unique_ptr<LogElementText>> Log::hudHistory;
sf::Clock Log::clock;

std::ofstream Log::logFile;

/**
 * Sets *temporary* SettingsManager settings, which are relevant only in the short window of time between when
 * SettingsManager has initialized its settings (and reset them to actual default values), and SettingsManager loads
 * settings from a file. A few settings are used in Log::log and need to have specific values, mostly so that messages
 * are always displayed in stdout during settings load, so that it can be debugged.
 */
void Log::setup()
{
	SettingsManager::debugWriteLogToFile = false;
	SettingsManager::debugPrintToStderr = true;
	SettingsManager::debugVerbose = true;
	SettingsManager::guiScale = 1.F;
}

void Log::setFont(sf::Font *font)
{
	Log::font = font;
}

void Log::handleScreenResize(sf::Vector2u windowSize)
{
	Log::windowSize = windowSize;
}

void Log::handleSettingsChange()
{
	Log::fontGap = Log::font->getLineSpacing(static_cast<uint>(SettingsManager::guiScale * FONT_H3));
	for (const auto& item : Log::hudHistory)
	{
		item->handleSettingsChange();
	}
}

/**
 * Should be called after SettingsManager is already loaded its settings.
 * TODO call this when setting SettingsManager::debugWriteLogToFile = true
 */
void Log::openLogFile()
{
	if (!SettingsManager::debugWriteLogToFile)
		return;

	const std::string logFilePath = pathCombine(SettingsManager::getGameRootDir(), PATH_LOGFILE);
	Log::logFile.open(logFilePath);
}

/**
 * Removes old items from history.
 * If update should happen (based on private clock) and there are items in history, calculate their positions.
 *
 * @param force update even if there was an update in last `LOG_UPDATE_FREQUENCY_MS`
 */
void Log::tick(bool force)
{
	uint x = LOG_ANCHOR_PADDING_LEFT; // constant offset from left, will be enough for CORNER_*_LEFT
	uint y = LOG_ANCHOR_PADDING_TOP; // constant offset from top, will be enough for CORNER_TOP_*
	uint timesUpCnt = 0;

	if ((Log::hudHistory.empty() || Log::clock.getElapsedTime().asMilliseconds() < LOG_UPDATE_FREQUENCY_MS) && !force)
		return;

	// remove items which were in the log for longer than LOG_ELEMENT_LIFE_TIME_S
	Log::hudHistory.remove_if([](const auto& item){ return item->isTimeUp(); });

	// initial offset from top/bottom
	if (SettingsManager::logAnchor == CORNER_BOTTOM_LEFT || SettingsManager::logAnchor == CORNER_BOTTOM_RIGHT)
		y = Log::windowSize.y - static_cast<uint>(Log::hudHistory.size()) * Log::fontGap - LOG_ANCHOR_NEG_PADDING_BOTTOM;

	for (auto& item : Log::hudHistory)
	{
		if (SettingsManager::logAnchor == CORNER_TOP_RIGHT || SettingsManager::logAnchor == CORNER_BOTTOM_RIGHT)
			x = Log::windowSize.x - static_cast<uint>(item->getLocalBounds().width) - LOG_ANCHOR_NEG_PADDING_RIGHT;

		item->setPosition(static_cast<float>(x), static_cast<float>(y));

		y += Log::fontGap;
	}

	Log::clock.restart();
}

void Log::draw(sf::RenderTarget& target)
{
	for (const auto& item : Log::hudHistory)
	{
		target.draw(*item);
	}
}

/**
 * Writes a formatted message to log file.
 */
void Log::logToFile(const char* prefix, const std::string& msg)
{
	// TODO log file should always contain messages in english, not in selected language, for easier debugging
	Log::logFile << prefix << msg << std::endl;
}

/**
 * Writes a formatted message to stderr.
 *
 * Use only if no Log object is available (writing to hud or log file is preferred).
 */
void Log::logStderr(const char* prefix, const std::string& msg)
{
	std::cerr << prefix << msg << std::endl;
}

// TODO when we'll have the ability to modify settings during runtime,
// we need to call this when setting SETT_WRITE_LOG_TO_FILE = false
void Log::close()
{
	Log::logFile.close();
}
