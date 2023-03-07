#pragma once

#include <unordered_map>
#include <vector>
#include <memory>
#include <map>

#include <SFML/Graphics/Drawable.hpp>

#include "../../resources/resource_manager.hpp"
#include "../../resources/sound_resource.hpp"
#include "../buttons/simple_button.hpp"
#include "../../campaigns/campaign.hpp"
#include "../hud_transformable.hpp"
#include "../hover_manager.hpp"
#include "../clickable.hpp"
#include "pipbuck_page.hpp"

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
		PipBuckCategory(ResourceManager &resMgr, PipBuckPageType defaultPage,
						std::map<PipBuckPageType, std::shared_ptr<PipBuckPage>> pages);
		bool setup();
		PipBuckPageType getSelectedPage() const;
		ClickStatus handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		bool changePage(PipBuckPageType pageType);
		bool setupCampaignInfos();
		void unloadCampaignInfos();
		static PipBuckCategoryType pageTypeToCategoryType(PipBuckPageType pageType);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
