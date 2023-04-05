#include "slider_handle.hpp"

#include "../../settings/settings_manager.hpp"
#include "../../util/util.hpp"

#define SLIDER_HANDLE_COLOR_DIM_FACTOR 50
#define SLIDER_HANDLE_THINGY_LINE_HEIGHT 6
#define SLIDER_HANDLE_THINGY_LINES_X 7.5F
#define SLIDER_HANDLE_THINGY_LINE_Y 5

SliderHandle::SliderHandle()
{
	this->outlineRect.setSize({ SLIDER_HANDLE_WIDTH, SLIDER_HANDLE_HEIGHT });
	
	this->outlineRect.setOutlineThickness(1.F);

	for (uint i = 0; i < SLIDER_HANDLE_THINGY_VERT_CNT; i++)
	{
		if (i % 2 == 0)
			this->thingy[i].position = sf::Vector2f(SLIDER_HANDLE_THINGY_LINES_X + i,
													SLIDER_HANDLE_THINGY_LINE_Y);
		else
			this->thingy[i].position = sf::Vector2f(SLIDER_HANDLE_THINGY_LINES_X - 1 + i,
													SLIDER_HANDLE_THINGY_LINE_Y + SLIDER_HANDLE_THINGY_LINE_HEIGHT);
	}

	this->handleSettingsChange();
}

void SliderHandle::handleSettingsChange()
{
	this->outlineRect.setOutlineColor(SettingsManager::hudColor);
	this->outlineRect.setFillColor(DIM_COLOR(SettingsManager::hudColor, SLIDER_HANDLE_COLOR_DIM_FACTOR));

	for (uint i = 0; i < SLIDER_HANDLE_THINGY_VERT_CNT; i++)
	{
		this->thingy[i].color = SettingsManager::hudColor;
	}
}

void SliderHandle::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->outlineRect, states);
	target.draw(this->thingy, states);
}
