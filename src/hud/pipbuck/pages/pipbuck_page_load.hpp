#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../../buttons/simple_button.hpp"
#include "../../../campaigns/campaign.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Main Menu -> Load
 */
class PipBuckPageLoad : public PipBuckPage
{
	private:
		SimpleButton exitBtn;

	public:
		explicit PipBuckPageLoad(ResourceManager &resMgr);
		void handleSettingsChange() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
