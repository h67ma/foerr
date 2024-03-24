// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>
#include <vector>

#include "../../resources/resource_manager.hpp"
#include "../buttons/simple_button.hpp"
#include "../click_manager.hpp"
#include "../color_selector.hpp"
#include "../hover_manager.hpp"
#include "../sliders/float_slider.hpp"
#include "../wrappable_text.hpp"
#include "gui_page.hpp"

/**
 * PipBuck -> Game -> Settings
 * or Main Menu -> Settings
 */
class GuiPageSettings : public GuiPage
{
	private:
		std::vector<SimpleButton> buttons;
		ColorSelector hudColorSelector;
		FloatSlider guiScaleSlider;
		WrappableText infoText;
		HoverManager hoverMgr;
		ClickManager clickMgr;

		void updateControlsState();

	public:
		explicit GuiPageSettings(ResourceManager& resMgr);
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override;
		void handleLeftClickUp() override;
		bool handleMouseMove(sf::Vector2i mousePos) override;
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
