// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include "../../../campaigns/campaign.hpp"
#include "../../../resources/resource_manager.hpp"
#include "../../buttons/simple_button.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Game -> Load
 * or Main Menu -> Load
 */
class PipBuckPageLoad : public PipBuckPage
{
	private:
		SimpleButton exitBtn;
		const bool isInMainMenu;

	public:
		PipBuckPageLoad(ResourceManager& resMgr, bool isInMainMenu);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
