#include "pipbuck_cat_main.hpp"
#include "../pages/pipbuck_page_campaign.hpp"
#include "../pages/pipbuck_page_save.hpp"
#include "../pages/pipbuck_page_settings.hpp"
#include "../pages/pipbuck_page_controls.hpp"
#include "../pages/pipbuck_page_log.hpp"

PipBuckCategoryMain::PipBuckCategoryMain(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign, GameState &gameState) :
	PipBuckCategory(scale, hudColor, fxVolume, resMgr) {
		this->pages = { // order matters
			std::make_shared<PipBuckPageCampaign>(scale, hudColor, resMgr, campaign, gameState),
			std::make_shared<PipBuckPageSave>(resMgr),
			std::make_shared<PipBuckPageSettings>(resMgr),
			std::make_shared<PipBuckPageControls>(resMgr),
			std::make_shared<PipBuckPageLog>(resMgr)
		};
	}
