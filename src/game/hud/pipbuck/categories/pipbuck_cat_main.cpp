#include "pipbuck_cat_main.hpp"
#include "../pages/pipbuck_page_load.hpp"
#include "../pages/pipbuck_page_save.hpp"
#include "../pages/pipbuck_page_settings.hpp"
#include "../pages/pipbuck_page_controls.hpp"
#include "../pages/pipbuck_page_log.hpp"

PipBuckCategoryMain::PipBuckCategoryMain(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign, GameState &gameState, SettingsManager &settings) :
	PipBuckCategory(scale, hudColor, fxVolume, resMgr) {
		this->pages = { // order matters
			std::make_shared<PipBuckPageLoad>(scale, hudColor, resMgr, campaign, gameState),
			std::make_shared<PipBuckPageSave>(resMgr),
			std::make_shared<PipBuckPageSettings>(scale, hudColor, resMgr, settings),
			std::make_shared<PipBuckPageControls>(resMgr),
			std::make_shared<PipBuckPageLog>(resMgr)
		};
	}
