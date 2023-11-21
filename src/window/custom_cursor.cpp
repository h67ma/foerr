// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "custom_cursor.hpp"

#include "../settings/settings_manager.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

CustomCursor::CustomCursor(const std::string& path, sf::Vector2u hotPoint, sf::Cursor::Type fallback) :
	path(path),
	hotPoint(hotPoint),
	fallbackCursor(fallback)
{
	// "In a sandwich, it's not the bread that's the most delicious. It's the cucumber inside."
}

/**
 * Tries to load a cursor from image file. If image is not found, or
 * there are other load errors, a fallback (system) cursor will be loaded instead.
 *
 * @returns whether load succeeded (fallback cursors are acceptable)
 */
bool CustomCursor::load()
{
	if (!SettingsManager::preferCustomCursor)
		return this->loadFromSystem(this->fallbackCursor);

	sf::Image img;

	if (!img.loadFromFile(this->path)) {
		Log::w(STR_CURSOR_LOAD_IMG_ERR, this->path.c_str());
		return this->loadFromSystem(this->fallbackCursor);
	}

	if (!this->loadFromPixels(img.getPixelsPtr(), img.getSize(), this->hotPoint)) {
		Log::w(STR_CURSOR_LOAD_PIXELS_ERR, this->path.c_str());
		return this->loadFromSystem(this->fallbackCursor);
	}

	return true;
}
