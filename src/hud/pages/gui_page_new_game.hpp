// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>
#include <vector>

#include <SFML/Graphics/RenderWindow.hpp>

#include "../../resources/resource_manager.hpp"
#include "../../window/cursor_manager.hpp"
#include "../buttons/simple_button.hpp"
#include "../pipbuck/pipbuck.hpp"
#include "gui_page.hpp"

/**
 * Main Menu -> New Game
 */
class GuiPageNewGame : public GuiPage
{
	private:
		struct list_item
		{
				std::string campaignId;
				SimpleButton button;
				list_item(std::string& campaignId, SimpleButton& button) :
					campaignId(std::move(campaignId)),
					button(std::move(button))
				{
				}
		};

		HoverManager campaignListHoverMgr;
		std::vector<struct GuiPageNewGame::list_item> campaignItems; // TODO use some sort of scrollable list instead
		SimpleButton refreshButton;
		ResourceManager& resMgr;
		CursorManager& cursorMgr;
		sf::RenderWindow& window;
		Campaign& campaign;
		GameState& gameState;
		PipBuck& pipBuck;

		bool loadCampaign(const std::string& campaignId);

	public:
		GuiPageNewGame(ResourceManager& resMgr, CursorManager& cursorMgr, sf::RenderWindow& window, Campaign& campaign,
					   GameState& gameState, PipBuck& pipBuck);
		void rebuildCampaignList();
		void handleSettingsChange() override;
		bool handleMouseMove(sf::Vector2i mousePos) override;
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
