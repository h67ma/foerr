#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"
#include "../../buttons/simple_button.hpp"
#include "../../../campaigns/campaign.hpp"

/**
 * PipBuck -> Main Menu -> Load
 */
class PipBuckPageLoad : public PipBuckPage
{
	private:
		SimpleButton exitBtn;

	public:
		PipBuckPageLoad(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign, GameState &gameState);
		std::string getLabel() const override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
