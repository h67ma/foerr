// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <deque>
#include <string>

#include "../../resources/resource_manager.hpp"
#include "../text_label.hpp"
#include "gui_page.hpp"

/**
 * PipBuck -> Game -> Log
 */
class GuiPageLog : public GuiPage
{
	private:
		std::deque<TextLabel> msgList;
		ResourceManager& resMgr;

	public:
		explicit GuiPageLog(ResourceManager& resMgr);
		void addMsg(const std::string& text, const sf::Color& color);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
