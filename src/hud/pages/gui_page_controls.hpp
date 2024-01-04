// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>
#include <vector>

#include "../../resources/resource_manager.hpp"
#include "../buttons/simple_button.hpp"
#include "../wrappable_text.hpp"
#include "gui_page.hpp"

/**
 * PipBuck -> Game -> Controls
 * or Main Menu -> Controls
 */
class PipBuckPageControls : public PipBuckPage
{
	private:
		std::vector<SimpleButton> buttons;
		WrappableText dummyMapDump; // TODO delet this
		void updateDisplay();

	public:
		explicit PipBuckPageControls(ResourceManager& resMgr);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
