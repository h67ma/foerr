// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include "../../campaigns/campaign.hpp"
#include "../../resources/resource_manager.hpp"
#include "../../resources/sound_resource.hpp"
#include "../buttons/simple_button.hpp"
#include "../clickable.hpp"
#include "../configurable_gui_component.hpp"
#include "../hover_manager.hpp"
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
class PipBuckCategory : public sf::Drawable, public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		HoverManager hoverMgr;
		PipBuckPageType selectedPage;
		std::unordered_map<PipBuckPageType, SimpleButton> pageButtons;
		SoundResource soundPageChange;
		SoundResource soundClick;
		const std::unordered_map<PipBuckPageType, std::shared_ptr<PipBuckPage>> pages;

	public:
		PipBuckCategory(ResourceManager& resMgr, PipBuckPageType defaultPage,
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
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
