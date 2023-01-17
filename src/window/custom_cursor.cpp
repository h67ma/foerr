#include "custom_cursor.hpp"

#include "../settings/settings_manager.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"

CustomCursor::CustomCursor(const std::string &path, sf::Vector2u hotPoint, sf::Cursor::Type fallback) :
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
		return this->cursor.loadFromSystem(this->fallbackCursor);

	sf::Image img;
	bool loadSucc;

	loadSucc = img.loadFromFile(this->path);
	if (!loadSucc) {
		Log::w(STR_CURSOR_LOAD_IMG_ERR, this->path.c_str());
		return this->cursor.loadFromSystem(this->fallbackCursor);
	}

	loadSucc = this->cursor.loadFromPixels(img.getPixelsPtr(), img.getSize(), this->hotPoint);
	if (!loadSucc) {
		Log::w(STR_CURSOR_LOAD_PIXELS_ERR, this->path.c_str());
		return this->cursor.loadFromSystem(this->fallbackCursor);
	}

	return true;
}

const sf::Cursor& CustomCursor::getCursor() const
{
	return this->cursor;
}
