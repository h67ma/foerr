// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/RenderTarget.hpp>

#include "../../resources/resource_manager.hpp"
#include "../../resources/sprite_resource.hpp"
#include "../../util/trapeze_shape.hpp"
#include "../hud.hpp"
#include "../text_label.hpp"
#include "button.hpp"

constexpr struct trapeze_data PIPBUCK_CAT_BTN_LEFT
{
		0, 22, 160, 160, 78
};
constexpr struct trapeze_data PIPBUCK_CAT_BTN_MID
{
		0, 23, 223, 179, 78
};
constexpr struct trapeze_data PIPBUCK_CAT_BTN_RIGHT
{
		22, 0, 160, 160, 78
};

class PipBuckCategoryButton : public Button
{
	private:
		bool selected = false;
		bool hover = false;
		const struct trapeze_data trapezeData;
		TrapezeShape trapeze;
		TextLabel text;
		void updateState();
		void centerText();
		SpriteResource hoverImgSprite;

	public:
		PipBuckCategoryButton(ResourceManager& resMgr, sf::Vector2u position, const struct trapeze_data& shape,
							  const std::string& text, const std::string& hoverImgPath);
		bool containsPoint(sf::Vector2i coords) override;
		void setHover(bool hover) override;
		void setSelected(bool selected) override;
		void setText(const std::string& text);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
