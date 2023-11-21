// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include "../pipbuck_page.hpp"

#include "../../../resources/resource_manager.hpp"
#include <string>

/**
 * PipBuck -> Information -> Map
 */
class PipBuckPageMap : public PipBuckPage
{
	public:
		explicit PipBuckPageMap(ResourceManager& resMgr);
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
