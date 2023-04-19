#pragma once

#include <functional>

#include <SFML/Graphics/Drawable.hpp>

#include "../hud_transformable.hpp"
#include "../hoverable.hpp"
#include "../clickable.hpp"
#include "../../consts.hpp"

#define BTN_BORDER_THICKNESS_SMALL 1.F
#define BTN_BORDER_THICKNESS_SMALL_SELECTED 2.F
#define BTN_BORDER_THICKNESS_NORMAL 1.F
#define BTN_BORDER_THICKNESS_NORMAL_SELECTED 3.F
#define BTN_BORDER_THICKNESS_LARGE 2.F
#define BTN_BORDER_THICKNESS_LARGE_SELECTED 6.F

class Button : public sf::Drawable, public HudTransformable, public Hoverable, public Clickable
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
