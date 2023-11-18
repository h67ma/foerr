// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <functional>
#include <string>

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Color.hpp>

#include "../../resources/resource_manager.hpp"
#include "../text_label.hpp"
#include "../../consts.hpp"
#include "../hud.hpp"
#include "button.hpp"

enum SimpleButtonSize
{
	BTN_NORMAL,
	BTN_NARROW,
	BTN_BIG,
};

class SimpleButton : public Button
{
	private:
		SimpleButtonSize size;
		bool selected = false;
		bool hover = false;
		sf::Color colorHover; // hover background color
		sf::Color colorSelected; // selected background color
		sf::Color colorUnselected; // unselected background color
		sf::RectangleShape rect;
		TextLabel text;
		sf::VertexArray gradient = sf::VertexArray(sf::Quads, 8);
		void setThickness();
		void updateState();
		void setColor();
		void setGuiScale();

	public:
		SimpleButton(SimpleButtonSize size, ResourceManager &resMgr, const sf::Vector2u &position,
					 const std::string &text = "Button", std::function<void(void)> callback = nullptr,
					 ClickStatus consumedStatus = CLICK_CONSUMED);
		bool containsPoint(sf::Vector2i coords) override;
		void setHover(bool hover) override;
		void setSelected(bool selected) override;
		void setText(const std::string &text);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
