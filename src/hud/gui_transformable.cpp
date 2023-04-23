#include "gui_transformable.hpp"

#include "../settings/settings_manager.hpp"

/**
 * Input position is multiplied by GUI scale in both dimensions and rounded to integer resolution to avoid blurriness
 * in fonts and shapes.
 */
sf::Vector2f GuiTransformable::calculateGuiAwarePosition(sf::Vector2f position)
{
	position.x = std::round(position.x * SettingsManager::guiScale);
	position.y = std::round(position.y * SettingsManager::guiScale);

	return position;
}

void GuiTransformable::setPosition(float x, float y)
{
	this->origPos.x = x;
	this->origPos.y = y;

	this->handleGuiScaleChange();
}

void GuiTransformable::setPosition(const sf::Vector2f& position)
{
	this->origPos = position;

	this->handleGuiScaleChange();
}

/**
 * Updates the position using saved "original position" and current GUI scale.
 *
 * Should be called by components inheriting this in ::handleSettingsChange().
 */
void GuiTransformable::handleGuiScaleChange()
{
	sf::Transformable::setPosition(calculateGuiAwarePosition(this->origPos));
}
