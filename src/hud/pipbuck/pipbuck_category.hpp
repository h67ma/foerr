#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "../../resources/resource_manager.hpp"
#include "../hud.hpp"
#include "../buttons/simple_button.hpp"
#include "../hover_manager.hpp"
#include "../clickable.hpp"
#include "pipbuck_page.hpp"
#include "../../campaigns/campaign.hpp"
#include "../../resources/sound_resource.hpp"

enum PipBuckCategoryType
{
	PIPB_CAT_MAIN,
	PIPB_CAT_STATUS,
	PIPB_CAT_INVENTORY,
	PIPB_CAT_INFO,
	PIPB_CAT_NO_CAT,
};

/**
 * Represents a single PipBuck category (e.g. "Status").
 * Contains 5 pages.
 */
class PipBuckCategory : public sf::Drawable, public sf::Transformable
{
	private:
		HoverManager hoverMgr;
		PipBuckPageType selectedPage;
		std::unordered_map<PipBuckPageType, SimpleButton> pageButtons;
		SoundResource soundPageChange;
		SoundResource soundClick;
		const std::unordered_map<PipBuckPageType, std::shared_ptr<PipBuckPage>> pages;

	public:
		PipBuckCategory(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, PipBuckPageType defaultPage, std::unordered_map<PipBuckPageType, std::shared_ptr<PipBuckPage>> pages);
		bool setup();
		ClickStatus handleLeftClick(int x, int y);
		bool handleMouseMove(int x, int y);
		bool changePage(PipBuckPageType pageType);
		bool setupCampaignInfos();
		void unloadCampaignInfos();
		static PipBuckCategoryType pageTypeToCategoryType(PipBuckPageType pageType);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
