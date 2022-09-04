#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include "../button.hpp"
#include "../../resource_manager.hpp"
#include "../hover_manager.hpp"
#include "../hud.hpp"
#include "pipbuck_category.hpp"
#include "gui_container.hpp"

enum PipBuckCategory
{
	PIPB_CAT_STATUS,
	PIPB_CAT_INV,
	PIPB_CAT_INFO,
	PIPB_CAT_MAINMENU,
};

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
 */
class PipBuck : public GuiContainer
{
	private:
		sf::Sprite pipBuckSprite;
		PipBuckCategory selectedCategory = PIPB_CAT_STATUS;
		std::unordered_map<PipBuckCategory, PipBuckCategoryPage> categoryPages;
		std::unordered_map<PipBuckCategory, Button> categoryButtons;
		Button closeBtn;
		sf::Sound soundOpenClose;
		GameState &gameState;
		void changeCategory(PipBuckCategory cat);

	public:
		PipBuck(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, GameState &gameState);
		void handleScreenResize(uint screenW, uint screenH);
		void handleLeftClick(int x, int y);
		void open(bool sound=true);
		void close();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
