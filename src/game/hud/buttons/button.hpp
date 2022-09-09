#pragma once

#include <functional>
#include <SFML/Graphics.hpp>
#include "hud.hpp"
#include "hoverable.hpp"
#include "../consts.hpp"

#define BTN_BORDER_THICKNESS_SMALL 1.f
#define BTN_BORDER_THICKNESS_SMALL_SELECTED 2.f
#define BTN_BORDER_THICKNESS_NORMAL 1.f
#define BTN_BORDER_THICKNESS_NORMAL_SELECTED 3.f
#define BTN_BORDER_THICKNESS_LARGE 2.f
#define BTN_BORDER_THICKNESS_LARGE_SELECTED 6.f

class Button : public sf::Drawable, public sf::Transformable, public Hoverable
{
	private:
		std::function<void(void)> callback = nullptr;

	public:
		Button(GuiScale scale, uint x, uint y, std::function<void(void)> callback = nullptr);
		void setCallback(std::function<void(void)> callback);
		ClickStatus handleLeftClick(int x, int y);
		virtual bool containsPoint(int x, int y) = 0;
		virtual void setGuiScale(GuiScale scale) = 0;
		virtual void setHover(bool hover) = 0;
};
