#include "hud_transformable.hpp"

#include "../settings/settings_manager.hpp"

/**
 * Input position is multiplied by GUI scale in both dimensions and rounded to integer resolution to avoid blurriness
 * in fonts and shapes.
 */
sf::Vector2f HudTransformable::calculateGuiAwarePosition(sf::Vector2f position)
{
	position.x = std::round(position.x * SettingsManager::guiScale);
	position.y = std::round(position.y * SettingsManager::guiScale);

	return position;
}

void HudTransformable::setPosition(float x, float y)
{
	this->origPos.x = x;
	this->origPos.y = y;

	this->handleGuiScaleChange();
}

void HudTransformable::setPosition(const sf::Vector2f& position)
{
	this->origPos = position;

	this->handleGuiScaleChange();
}

/**
 * Updates the position using saved "original position" and current GUI scale.
 *
 * Should be called by components inheriting this in ::handleSettingsChange().
 */
void HudTransformable::handleGuiScaleChange()
{
	sf::Transformable::setPosition(calculateGuiAwarePosition(this->origPos));
}
