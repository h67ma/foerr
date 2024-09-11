// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <deque>
#include <string>

#include "../../consts.hpp"
#include "../../resources/resource_manager.hpp"
#include "../scrollable/text_list_view.hpp"
#include "gui_page.hpp"

/**
 * PipBuck -> Game -> Log
 */
class GuiPageLog : public GuiPage
{
	private:
		std::deque<StringAndColor> msgList;
		TextListView logListView;

	public:
		explicit GuiPageLog(ResourceManager& resMgr);
		void addMsg(const StringAndColor& strAndColor);
		void handleScroll(float delta, sf::Vector2i mousePos) override;
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
