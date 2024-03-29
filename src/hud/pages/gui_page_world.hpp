// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>
#include <unordered_map>

#include <SFML/Graphics/CircleShape.hpp>

#include "../../campaigns/campaign.hpp"
#include "../../resources/resource_manager.hpp"
#include "../../resources/sprite_resource.hpp"
#include "../buttons/location_button.hpp"
#include "../buttons/simple_button.hpp"
#include "../hover_manager.hpp"
#include "../wrappable_text.hpp"
#include "gui_page.hpp"

// valid loc id length is guaranteed to have len > 0, as checked in Campaign::load()
const std::string NO_LOCATION_SELECTED = ""; // NOLINT(readability-redundant-string-init)

/**
 * PipBuck -> Information -> World
 */
class GuiPageWorld : public GuiPage
{
	private:
		ResourceManager& resMgr;
		Campaign& campaign;
		SpriteResource mapBg;
		sf::VertexArray mapBorder = sf::VertexArray(sf::LineStrip, 5);
		sf::VertexArray mapGridLines = sf::VertexArray(sf::Lines, 16); // max 4 horizontal, 4 vertical
		WrappableText locTitle;
		WrappableText locDescription; // also shows recommended lvl, trial count, basecamp, etc.
		sf::CircleShape activeLocIndicator;
		std::unordered_map<std::string, LocButton> mapButtons;
		SimpleButton gotoLocationBtn;
		HoverManager hoverMgr;
		HoverManager mapButtonHoverMgr;

		// represents selection on page, not currently active location
		// NOLINTNEXTLINE(readability-redundant-string-init)
		std::string selectedLocId = NO_LOCATION_SELECTED;

		bool travelButtonAvailable = false;
		void setComponentColors();
		void setupMapDecorations();
		bool mapContainsPoint(sf::Vector2i point);
		void updateActiveIndicator();
		void setGuiScale();
		bool isAnyLocationSelected() const;

	public:
		GuiPageWorld(ResourceManager& resMgr, Campaign& campaign);
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override;
		bool handleMouseMove(sf::Vector2i mousePos) override;
		bool setupCampaignInfos() override;
		void unloadCampaignInfos() override;
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
