// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Inventory -> Equipment
 */
class PipBuckPageEquipment : public PipBuckPage
{
	public:
		explicit PipBuckPageEquipment(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
