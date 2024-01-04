// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics/ConvexShape.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>
#include <SFML/System/Clock.hpp>

#include "../../campaigns/campaign.hpp"
#include "../../resources/resource_manager.hpp"
#include "../../resources/sound_resource.hpp"
#include "../../resources/sprite_resource.hpp"
#include "../../settings/settings_manager.hpp"
#include "../../window/cursor_manager.hpp"
#include "../buttons/simple_button.hpp"
#include "../clickable.hpp"
#include "../configurable_gui_component.hpp"
#include "../hover_manager.hpp"
#include "../hud.hpp"
#include "pipbuck_category.hpp"
#include "pipbuck_page.hpp"

/**
 * In-game menu, containing both inventory/character info/map/etc. and settings.
 * PipBuck contains 4 *categories*, each of them containing 5 *pages*. Some pages
 * (settings, controls, load) can also be accessed via main menu.
 *
 * The convention is to keep all categories and their child pages in memory.
 * This way we can switch between them instantaneously, without the need to
 * deallocate old page and allocate new one. As a bonus, state of all pages is
 * preserved, unless we explicitly want to e.g. scroll list to top when page is opened.
 *
 * TODO what about updating states of individual pages? We should NOT update
 * states of inactive (invisible) pages; instead only update them on page open.
 *
 * Possible refactor: store each category (and each page in categories) as member,
 * not in an array & store pointers to pages in vector for convenience. This way
 * we can avoid defining some methods like ::setupCampaignInfos() in GuiPage,
 * and instead define them only in pages that actually use it.
 */
class PipBuck : public sf::Drawable, public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		ResourceManager& resMgr;
		Campaign& campaign;
		HoverManager hoverMgr;
		CursorManager& cursorMgr;
		SpriteResource pipBuckSprite;
		SpriteResource pipBuckScreenRadialGrad; // note: can be generated purely by SFML, but it's hard to get right
		sf::ConvexShape radIndicator;
		sf::RectangleShape screenBackgroundGray;
		sf::Texture screenBackgroundStripesTxt;
		sf::Sprite screenBackgroundStripes;
		sf::Clock timer;
		float radIndicatorLevel = 0.F;
		PipBuckCategoryType selectedCategory = PIPB_CAT_STATUS;
		std::unordered_map<PipBuckCategoryType, PipBuckCategory> categories;
		std::unordered_map<PipBuckCategoryType, std::unique_ptr<Button>> categoryButtons;
		SimpleButton closeBtn;
		SoundResource soundOpenClose;
		SoundResource soundCategoryBtn;
		GameState& gameState;
		bool changeCategory(PipBuckCategoryType categoryType);
		static double getSmoothNoise(double time);
		void updateCampaignInfos();
		void setScreenTint();
		void setupRadIndicator();
		void setRadIndicatorScale();
		void setupScreenBackground();
		void setScreenBackgroundScale();
		void setGuiScale();
		void setAudioVolume();
		void setRadLevel(float rads);

	public:
		PipBuck(ResourceManager& resMgr, CursorManager& cursorMgr, Campaign& campaign, GameState& gameState);
		void handleScreenResize(sf::Vector2u windowSize);
		ClickStatus handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		void handleMouseMove(sf::Vector2i mousePos);
		bool setupCampaignInfos();
		void unloadCampaignInfos();
		void open(sf::Vector2i mousePos, bool sound = true);
		bool switchToPage(PipBuckPageType pageType, sf::Vector2i mousePos);
		void close();
		bool setup();
		void tick();
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
