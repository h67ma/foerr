#include "pipbuck_page_campaign.hpp"
#include "../../../util/i18n.hpp"

PipBuckPageCampaign::PipBuckPageCampaign(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign, GameState &gameState) :
	buttons({
		Button(scale, BTN_NORMAL, hudColor, resMgr, 385, 260, STR_EXIT_TO_MAIN_MENU, [&campaign, &gameState, &resMgr]() {
			// TODO also save game before unloading campaign
			campaign.unload(resMgr);
			gameState = STATE_MAINMENU;
		})
	})
{
	for (auto &btn : this->buttons)
	{
		this->hoverMgr.addHoverable(&btn);
	}
}

ClickStatus PipBuckPageCampaign::handleLeftClick(int x, int y)
{
	for (auto &btn : this->buttons)
	{
		if (btn.handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
			return CLICK_CONSUMED;
	}

	return CLICK_NOT_CONSUMED;
}

std::string PipBuckPageCampaign::getLabel()
{
	return "Campaign"; // TODO translate
}

void PipBuckPageCampaign::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}
}
