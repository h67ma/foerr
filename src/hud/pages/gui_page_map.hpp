// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include "../../resources/resource_manager.hpp"
#include "gui_page.hpp"

/**
 * PipBuck -> Information -> Map
 */
class GuiPageMap : public GuiPage
{
	public:
		explicit GuiPageMap(ResourceManager& resMgr);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
