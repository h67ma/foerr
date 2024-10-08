// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>
#include <vector>

#include "../../resources/resource_manager.hpp"
#include "../buttons/simple_button.hpp"
#include "../click_manager.hpp"
#include "../hover_manager.hpp"
#include "../scrollable/text_list_view.hpp"
#include "gui_page.hpp"

/**
 * PipBuck -> Game -> Controls
 * or Main Menu -> Controls
 */
class GuiPageControls : public GuiPage
{
	private:
		std::vector<SimpleButton> buttons;
		HoverManager hoverMgr;
		ClickManager clickMgr;
		std::deque<StringAndColor> dumpList;
		TextListView mappingDump; // TODO allow changing keybinds
		void updateDisplay();

	public:
		explicit GuiPageControls(ResourceManager& resMgr);
		bool handleMouseMove(sf::Vector2i mousePos) override;
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override;
		void handleLeftClickUp() override;
		void handleScroll(float delta, sf::Vector2i mousePos) override;
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
