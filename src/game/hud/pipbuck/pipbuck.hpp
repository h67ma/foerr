#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "../buttons/simple_button.hpp"
#include "../../resource_manager.hpp"
#include "../hover_manager.hpp"
#include "../hud.hpp"
#include "pipbuck_category.hpp"
#include "../campaigns/campaign.hpp"
#include "../settings/settings_manager.hpp"
#include "../../sound_resource.hpp"
#include "../../sprite_resource.hpp"

/**
 * In-game menu, containing both inventory/character info/map/etc. and settings.
 * PipBuck contains 4 *categories*, each of them containing 5 *pages*. Some pages
 * (settings, controls) can also be acccessed via main menu.
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
 * we can avoid defining some methods like ::setupCampaignInfos() in PipBuckPage,
 * and instead define them only in pages that actually use it.
 */
class PipBuck : public sf::Drawable, public sf::Transformable
{
	private:
		ResourceManager &resMgr;
		HoverManager hoverMgr;
		SpriteResource pipBuckSprite;
		sf::ConvexShape radIndicator;
		sf::Clock timer;
		float radIndicatorLevel = 0.f;
		uint selectedCategory = 0;
		std::vector<PipBuckCategory> categories;
		std::vector<SimpleButton> categoryButtons;
		SimpleButton closeBtn;
		SoundResource soundOpenClose;
		SoundResource soundCategoryBtn;
		GameState &gameState;
		void changeCategory(uint idx);

	public:
		PipBuck(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign, GameState &gameState, SettingsManager &settings);
		void handleScreenResize(uint screenW, uint screenH);
		ClickStatus handleLeftClick(int x, int y);
		void handleMouseMove(int x, int y);
		bool setupCampaignInfos();
		void unloadCampaignInfos();
		void open(bool sound=true);
		void close();
		void setRadLevel(float rads);
		void nextFrame();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
