// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023-2024 h67ma <szycikm@gmail.com>

#include "slider_handle.hpp"

#include "../../settings/settings_manager.hpp"
#include "../../util/util.hpp"
#include "slider_consts.hpp"

constexpr uchar SLIDER_HANDLE_COLOR_DIM_FACTOR = 50;

constexpr float HANDLE_OUTLINE_THICKNESS = 1;
const sf::Vector2f hHandleOutlineSize(SLIDER_HANDLE_LENGTH, SLIDER_HANDLE_THICKNESS);
const sf::Vector2f vHandleOutlineSize(SLIDER_HANDLE_THICKNESS, SLIDER_HANDLE_LENGTH);

constexpr float SLIDER_HANDLE_THINGY_LINE_LENGTH = 7;
constexpr float SLIDER_HANDLE_THINGY_LINES_X = 7.5;
constexpr float SLIDER_HANDLE_THINGY_LINE_Y = 4;

SliderHandle::SliderHandle(enum SliderOrientation orientation) : orientation(orientation)
{
	this->outlineRect.setOutlineThickness(HANDLE_OUTLINE_THICKNESS);

	this->handleSettingsChange();
}

void SliderHandle::handleSettingsChange()
{
	this->outlineRect.setOutlineColor(SettingsManager::hudColor);
	this->outlineRect.setFillColor(DIM_COLOR(SettingsManager::hudColor, SLIDER_HANDLE_COLOR_DIM_FACTOR));

	if (this->orientation == SLIDER_HORIZONTAL)
	{
		this->outlineRect.setSize(calculateGuiAwarePoint(hHandleOutlineSize));

		for (uint i = 0; i < SLIDER_HANDLE_THINGY_VERT_CNT; i++)
		{
			this->thingy[i].color = SettingsManager::hudColor;

			if (i % 2 == 0)
				this->thingy[i].position = sf::Vector2f((SLIDER_HANDLE_THINGY_LINES_X + i) * SettingsManager::guiScale,
														SLIDER_HANDLE_THINGY_LINE_Y * SettingsManager::guiScale);
			else
				this->thingy[i].position =
					sf::Vector2f((SLIDER_HANDLE_THINGY_LINES_X - 1 + i) * SettingsManager::guiScale,
								 (SLIDER_HANDLE_THINGY_LINE_Y + SLIDER_HANDLE_THINGY_LINE_LENGTH) *
									 SettingsManager::guiScale);
		}
	}
	else
	{
		this->outlineRect.setSize(calculateGuiAwarePoint(vHandleOutlineSize));

		for (uint i = 0; i < SLIDER_HANDLE_THINGY_VERT_CNT; i++)
		{
			this->thingy[i].color = SettingsManager::hudColor;

			if (i % 2 == 0)
				this->thingy[i].position = sf::Vector2f(SLIDER_HANDLE_THINGY_LINE_Y * SettingsManager::guiScale,
														(SLIDER_HANDLE_THINGY_LINES_X + i) * SettingsManager::guiScale);
			else
				this->thingy[i].position =
					sf::Vector2f((SLIDER_HANDLE_THINGY_LINE_Y + SLIDER_HANDLE_THINGY_LINE_LENGTH) *
									 SettingsManager::guiScale,
								 (SLIDER_HANDLE_THINGY_LINES_X - 1 + i) * SettingsManager::guiScale);
		}
	}
}

void SliderHandle::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->outlineRect, states);
	target.draw(this->thingy, states);
}
