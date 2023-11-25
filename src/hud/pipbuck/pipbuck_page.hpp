// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include "../../campaigns/campaign.hpp"
#include "../../consts.hpp"
#include "../click_manager.hpp"
#include "../configurable_gui_component.hpp"
#include "../hover_manager.hpp"

enum PipBuckPageType
{
	PIPB_PAGE_LOAD,
	PIPB_PAGE_SAVE,
	PIPB_PAGE_SETTINGS,
	PIPB_PAGE_CONTROLS,
	PIPB_PAGE_LOG,
	PIPB_PAGE_STATUS_MAIN,
	PIPB_PAGE_SKILLS,
	PIPB_PAGE_PERKS,
	PIPB_PAGE_EFFECTS,
	PIPB_PAGE_HEALTH,
	PIPB_PAGE_WEAPONS,
	PIPB_PAGE_ARMOR,
	PIPB_PAGE_EQUIPMENT,
	PIPB_PAGE_INVENTORY_OTHER,
	PIPB_PAGE_AMMO,
	PIPB_PAGE_MAP,
	PIPB_PAGE_WORLD,
	PIPB_PAGE_QUESTS,
	PIPB_PAGE_NOTES,
	PIPB_PAGE_ENEMIES,
};

/**
 * Represents an abstract PipBuck page (e.g. Armor).
 * TODO Might display additional elements outside PipBuck screen area (e.g. question mark button).
 *
 * Note: since pages are not moved relatively to category, there's no need to subtract parent
 * component position in ::handleLeftClick() and ::handleMouseMove().
 *
 * TODO define an area where all page contents are contained. Then when handling clicks/hover events
 * first check if coordinates are within the defined area. This way, if the point is outside page bounds,
 * we can skip checking controls inside the page altogether.
 */
class PipBuckPage : public sf::Drawable, public ConfigurableGuiComponent
{
	protected:
		HoverManager hoverMgr;
		ClickManager clickMgr; // TODO delet if too many pages will implement a custom ::handleLeftClick()
		sf::Text dummy; // TODO delet this
		explicit PipBuckPage(const std::string& pageTitle);

	public:
		virtual ClickStatus handleLeftClick(sf::Vector2i clickPos);
		virtual void handleLeftClickUp() {};
		virtual bool handleMouseMove(sf::Vector2i mousePos);
		virtual bool setupCampaignInfos()
		{
			return true;
		}
		virtual void unloadCampaignInfos() {}
		void handleSettingsChange() override {}
		const std::string pageTitle;
};
