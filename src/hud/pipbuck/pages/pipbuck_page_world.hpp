#pragma once

#include <unordered_map>
#include <string>

#include <SFML/Graphics/CircleShape.hpp>

#include "../../../resources/resource_manager.hpp"
#include "../../../resources/sprite_resource.hpp"
#include "../../buttons/location_button.hpp"
#include "../../buttons/simple_button.hpp"
#include "../../../campaigns/campaign.hpp"
#include "../../wrappable_text.hpp"
#include "../pipbuck_page.hpp"

// valid loc id length is guaranteed to have len > 0, as checked in Campaign::load()
#define NO_LOCATION_SELECTED ""

/**
 * PipBuck -> Information -> World
 */
class PipBuckPageWorld : public PipBuckPage
{
	private:
		ResourceManager &resMgr;
		Campaign &campaign;
		SpriteResource mapBg;
		sf::VertexArray mapBorder = sf::VertexArray(sf::LineStrip, 5);
		sf::VertexArray mapGridLines = sf::VertexArray(sf::Lines, 16); // max 4 horizontal, 4 vertical
		WrappableText locTitle;
		WrappableText locDescription; // also shows recommended lvl, trial count, basecamp, etc.
		sf::CircleShape activeLocIndicator;
		std::unordered_map<std::string, LocButton> mapButtons;
		SimpleButton gotoLocationBtn;
		HoverManager mapButtonHoverMgr;
		std::string selectedLocId = NO_LOCATION_SELECTED; // represents selection on page, not currently active location
		bool travelButtonAvailable = false;
		void setComponentColors();
		void setupMapDecorations();
		bool mapContainsPoint(sf::Vector2i point);
		void updateActiveIndicator();
		void setGuiScale();

	public:
		PipBuckPageWorld(ResourceManager &resMgr, Campaign &campaign);
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override;
		bool handleMouseMove(sf::Vector2i mousePos) override;
		bool setupCampaignInfos() override;
		void unloadCampaignInfos() override;
		void handleSettingsChange() override;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
