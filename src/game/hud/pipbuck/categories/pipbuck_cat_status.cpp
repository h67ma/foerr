#include "pipbuck_cat_status.hpp"
#include "../pages/pipbuck_page_status_main.hpp"
#include "../pages/pipbuck_page_skills.hpp"
#include "../pages/pipbuck_page_perks.hpp"
#include "../pages/pipbuck_page_effects.hpp"
#include "../pages/pipbuck_page_health.hpp"

PipBuckCategoryStatus::PipBuckCategoryStatus(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr) :
	PipBuckCategory(scale, hudColor, fxVolume, resMgr) {
		this->pages = { // order matters
			std::make_shared<PipBuckPageMainStatus>(resMgr),
			std::make_shared<PipBuckPageSkills>(resMgr),
			std::make_shared<PipBuckPagePerks>(resMgr),
			std::make_shared<PipBuckPageEffects>(resMgr),
			std::make_shared<PipBuckPageHealth>(resMgr)
		};
	}
