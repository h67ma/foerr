// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2024 h67ma <szycikm@gmail.com>

#include "gui_page_new_game.hpp"

#include <filesystem>

#include "../../consts.hpp"
#include "../../util/i18n.hpp"
#include "../loading_screen.hpp"
#include "../log.hpp"

constexpr uint BTN_POS_LEFT = 400;
constexpr uint BTN_POS_TOP = 260;
constexpr uint BTN_POS_HEIGHT = 35;

GuiPageNewGame::GuiPageNewGame(ResourceManager& resMgr, CursorManager& cursorMgr, sf::RenderWindow& window,
							   Campaign& campaign, GameState& gameState, PipBuck& pipBuck) :
	GuiPage("New Game"),
	resMgr(resMgr), // TODO translate
	cursorMgr(cursorMgr),
	window(window),
	campaign(campaign),
	gameState(gameState),
	pipBuck(pipBuck)
{
	this->rebuildCampaignList();
}

void GuiPageNewGame::rebuildCampaignList()
{
	this->hoverMgr.clear();
	this->campaignItems.clear();

	uint i = 0;
	for (const auto& entry : std::filesystem::directory_iterator(PATH_CAMPAIGNS))
	{
		if (!entry.is_directory())
			continue;

		std::string campaignId = entry.path().stem();
		SimpleButton btn(BTN_NORMAL, this->resMgr, sf::Vector2u(BTN_POS_LEFT, BTN_POS_TOP + BTN_POS_HEIGHT * i),
						 campaignId);

		this->campaignItems.emplace_back(campaignId, btn);
		i++;
	}

	// this is kinda nasty because vector can relocate objects when it's modified.
	// we don't modify the vector (at least don't add/delete items) at any point though (except on a complete rebuild),
	// so it should be fine.
	for (auto& item : this->campaignItems)
	{
		this->hoverMgr += &item.button;
	}
}

void GuiPageNewGame::loadCampaign(const std::string& campaignId)
{
	// this is a pretty terrible way of showing a loading screen, but it will do for now
	// TODO load on thread, display loading screen in main loop with a progress bar
	LoadingScreen loadingScreen(this->resMgr, this->window.getSize());
	this->window.clear();
	this->window.draw(loadingScreen);
	this->window.display();

	if (!this->campaign.load(pathCombine(PATH_CAMPAIGNS, campaignId)))
	{
		Log::e(STR_CAMPAIGN_LOAD_FAILED);
		return;
	}

	if (!this->pipBuck.setupCampaignInfos())
	{
		Log::e(STR_PIPBUCK_SETUP_FAILED);
		return;
	}

	this->gameState = STATE_PLAYING;

	// TODO query campaign to check what the player is actually pointing at and set proper cursor color
	this->cursorMgr.setCursor(CROSSHAIR_WHITE);
}

void GuiPageNewGame::handleSettingsChange()
{
	GuiPage::handleSettingsChange();

	for (auto& item : this->campaignItems)
	{
		item.button.handleSettingsChange();
	}
}

ClickStatus GuiPageNewGame::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	for (auto& item : this->campaignItems)
	{
		ClickStatus status = item.button.handleLeftClick(clickPos);
		if (status != CLICK_NOT_CONSUMED)
		{
			this->loadCampaign(item.campaignId);
			return CLICK_CONSUMED;
		}
	}

	return CLICK_NOT_CONSUMED;
}

void GuiPageNewGame::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto& item : this->campaignItems)
	{
		target.draw(item.button, states);
	}
}
