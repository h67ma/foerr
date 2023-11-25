// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <functional>

#include <SFML/Graphics/Drawable.hpp>

#include "../../consts.hpp"
#include "../clickable.hpp"
#include "../gui_transformable.hpp"
#include "../hoverable.hpp"

#define BTN_BORDER_THICKNESS_SMALL 1.F
#define BTN_BORDER_THICKNESS_SMALL_SELECTED 2.F
#define BTN_BORDER_THICKNESS_NORMAL 1.F
#define BTN_BORDER_THICKNESS_NORMAL_SELECTED 3.F
#define BTN_BORDER_THICKNESS_LARGE 2.F
#define BTN_BORDER_THICKNESS_LARGE_SELECTED 6.F

class Button : public sf::Drawable, public GuiTransformable, public Hoverable, public Clickable
{
	private:
		const ClickStatus consumedStatus;
		const std::function<void(void)> callback = nullptr;

	public:
		explicit Button(sf::Vector2u position, std::function<void(void)> callback = nullptr,
						ClickStatus consumedStatus = CLICK_CONSUMED);
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override;
		bool containsPoint(sf::Vector2i coords) override = 0;
		virtual void setSelected(bool selected) = 0;
};
