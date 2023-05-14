#include "hud.hpp"

#include "../settings/settings_manager.hpp"

/**
 * Returns the vertical offset used for centering text.
 * The return value should be added to text Y coordinate.
 * The output is *not* adjusted to GUI scale.
 */
float getFontVOffset(uint fontSize)
{
	return static_cast<float>(fontSize) / -4;
}

/**
 * Input vector is multiplied by GUI scale in both dimensions and rounded to integer resolution to avoid blurriness
 * in fonts and shapes. Can be used for positions as well as sizes.
 */
sf::Vector2f calculateGuiAwarePoint(sf::Vector2f point)
{
	point.x = std::round(point.x * SettingsManager::guiScale);
	point.y = std::round(point.y * SettingsManager::guiScale);

	return point;
}

/**
 * Multiplies a float value by GUI scale and rounds to integer resolution. Should be used e.g. for calculating
 * border thickness.
 */
float calculateGuiAwareScalar(float value)
{
	return std::round(value * SettingsManager::guiScale);
}
