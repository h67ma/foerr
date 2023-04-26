#include "hud.hpp"

#include "../settings/settings_manager.hpp"

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
