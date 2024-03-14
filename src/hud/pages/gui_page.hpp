// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <string>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>

#include "../../campaigns/campaign.hpp"
#include "../../consts.hpp"
#include "../clickable.hpp"
#include "../configurable_gui_component.hpp"
#include "../gui_transformable.hpp"

static constexpr uint POS_PAGE_BTNS_Y = 565;
static const sf::Vector2u POS_PAGE_BTN_BOTTOM_1(0, POS_PAGE_BTNS_Y);
static const sf::Vector2u POS_PAGE_BTN_BOTTOM_2(200, POS_PAGE_BTNS_Y);

/**
 * Represents an abstract page to display in GUI (e.g. "Settings") - either in PipBuck or in main menu.
 * TODO Might display additional elements outside PipBuck screen area (e.g. question mark button).
 *
 * Note: since pages are not moved relatively to category, there's no need to subtract parent
 * component position in ::handleLeftClick() and ::handleMouseMove().
 *
 * TODO define an area where all page contents are contained. Then when handling clicks/hover events
 * first check if coordinates are within the defined area. This way, if the point is outside page bounds,
 * we can skip checking controls inside the page altogether.
 */
class GuiPage : public sf::Drawable, public GuiTransformable
{
	protected:
		sf::Text dummy; // TODO delet this
		explicit GuiPage(const std::string& pageTitle);

	public:
		virtual ClickStatus handleLeftClick(sf::Vector2i clickPos)
		{
			return CLICK_NOT_CONSUMED;
		};

		virtual void handleLeftClickUp() {};
		virtual bool handleMouseMove(sf::Vector2i mousePos)
		{
			return false;
		};

		virtual bool setupCampaignInfos()
		{
			return true;
		}

		virtual void unloadCampaignInfos() {}
		void handleSettingsChange() override;
		const std::string pageTitle;
};
