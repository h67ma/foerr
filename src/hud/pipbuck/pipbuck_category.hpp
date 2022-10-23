#pragma once

#include <vector>
#include <map>
#include <unordered_map>
#include <SFML/Graphics/Drawable.hpp>
#include "../hud_transformable.hpp"
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
class PipBuckCategory : public sf::Drawable, public HudTransformable
{
	private:
		HoverManager hoverMgr;
		PipBuckPageType selectedPage;
		std::unordered_map<PipBuckPageType, SimpleButton> pageButtons;
		SoundResource soundPageChange;
		SoundResource soundClick;
		const std::unordered_map<PipBuckPageType, std::shared_ptr<PipBuckPage>> pages;

	public:
		PipBuckCategory(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, PipBuckPageType defaultPage, std::map<PipBuckPageType, std::shared_ptr<PipBuckPage>> pages);
		bool setup();
		ClickStatus handleLeftClick(sf::Vector2i clickPos);
		bool handleMouseMove(sf::Vector2i mousePos);
		bool changePage(PipBuckPageType pageType);
		bool setupCampaignInfos();
		void unloadCampaignInfos();
		static PipBuckCategoryType pageTypeToCategoryType(PipBuckPageType pageType);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
