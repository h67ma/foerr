#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "../hud_transformable.hpp"
#include "../hud.hpp"
#include "../hoverable.hpp"
#include "../clickable.hpp"
#include "../../consts.hpp"

#define BTN_BORDER_THICKNESS_SMALL 1.f
#define BTN_BORDER_THICKNESS_SMALL_SELECTED 2.f
#define BTN_BORDER_THICKNESS_NORMAL 1.f
#define BTN_BORDER_THICKNESS_NORMAL_SELECTED 3.f
#define BTN_BORDER_THICKNESS_LARGE 2.f
#define BTN_BORDER_THICKNESS_LARGE_SELECTED 6.f

class Button : public sf::Drawable, public HudTransformable, public Hoverable, public Clickable
{
	private:
		const ClickStatus consumedStatus;
		const std::function<void(void)> callback = nullptr;

	public:
		Button(GuiScale scale, sf::Vector2u position, std::function<void(void)> callback = nullptr, ClickStatus consumedStatus = CLICK_CONSUMED);
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override;
		virtual bool containsPoint(sf::Vector2i coords) = 0;
		virtual void setGuiScale(GuiScale scale) = 0;
		virtual void setHover(bool hover) = 0;
};
