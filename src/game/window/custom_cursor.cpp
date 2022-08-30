#include "custom_cursor.hpp"
#include "../hud/log.hpp"
#include "../util/i18n.hpp"

CustomCursor::CustomCursor(const char* path, uint hotX, uint hotY, sf::Cursor::Type fallback)
{
	this->path = path;
	this->hotX = hotX;
	this->hotY = hotY;
	this->fallbackCursor = fallback;
}

/**
 * Tries to load a cursor from image file. If image is not found, or
 * there are other load errors, a fallback (system) cursor will be loaded instead.
 *
 * @returns whether load succeeded (fallback cursors are acceptable)
 */
bool CustomCursor::load(bool preferCustom)
{
	if (!preferCustom)
	{
		return this->cursor.loadFromSystem(this->fallbackCursor);
	}

	sf::Image img;
	bool loadSucc;

	loadSucc = img.loadFromFile(this->path);
	if (!loadSucc) {
		Log::w(STR_CURSOR_LOAD_IMG_ERR, this->path);
		return this->cursor.loadFromSystem(this->fallbackCursor);
	}

	loadSucc = this->cursor.loadFromPixels(img.getPixelsPtr(), img.getSize(), sf::Vector2u(this->hotX, this->hotY));
	if (!loadSucc) {
		Log::w(STR_CURSOR_LOAD_PIXELS_ERR, this->path);
		return this->cursor.loadFromSystem(this->fallbackCursor);
	}

	return true;
}

sf::Cursor& CustomCursor::getCursor()
{
	return this->cursor;
}
