#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "../button.hpp"
#include "../../resource_manager.hpp"
#include "../hover_manager.hpp"
#include "../hud.hpp"
#include "pipbuck_category.hpp"

enum PipBuckCategory
{
	PIPB_CAT_STATUS,
	PIPB_CAT_INV,
	PIPB_CAT_INFO,
	PIPB_CAT_MAINMENU,
	_PIPB_CAT_CNT
};

/**
 * In-game menu, containing both inventory/character info/map/etc. and settings.
 * PipBuck contains 4 *categories*, each of them containing 5 *pages*. Some pages
 * (settings, controls) can also be acccessed via main menu.
 */
class PipBuck : public sf::Drawable, public sf::Transformable
{
	private:
		sf::Sprite pipBuckSprite;
		HoverManager hoverMgr;
		std::vector<Button> miscButtons;
		PipBuckCategory selectedCategory;
		PipBuckCategoryPage statusCategoryPage;
		PipBuckCategoryPage invCategoryPage;
		PipBuckCategoryPage infoCategoryPage;
		PipBuckCategoryPage mainMenuCategoryPage;
		Button catStatusBtn;
		Button catInvBtn;
		Button catInfoBtn;
		Button catMainMenuBtn;
		Button closeBtn;
		void changeCategory(PipBuckCategory cat);

	public:
		PipBuck(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr);
		void handleScreenResize(uint screenW, uint screenH);
		void handleMouseMove(int x, int y);
		bool handleLeftClick(int x, int y);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
