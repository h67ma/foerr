#include "pipbuck_cat_info.hpp"
#include "../pages/pipbuck_page_map.hpp"
#include "../pages/pipbuck_page_quests.hpp"
#include "../pages/pipbuck_page_world.hpp"
#include "../pages/pipbuck_page_notes.hpp"
#include "../pages/pipbuck_page_enemies.hpp"

PipBuckCategoryInfo::PipBuckCategoryInfo(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign) :
	PipBuckCategory(scale, hudColor, fxVolume, resMgr) {
		this->pages = { // order matters
			std::make_shared<PipBuckPageMap>(resMgr),
			std::make_shared<PipBuckPageWorld>(scale, hudColor, resMgr, campaign),
			std::make_shared<PipBuckPageQuests>(resMgr),
			std::make_shared<PipBuckPageNotes>(resMgr),
			std::make_shared<PipBuckPageEnemies>(resMgr)
		};
	}
