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

static void btnRefresh(GuiPageNewGame* page)
{
	Log::i(STR_REFRESHING_CAMPAIGN_LIST);
	page->rebuildCampaignList();
}

GuiPageNewGame::GuiPageNewGame(ResourceManager& resMgr, CursorManager& cursorMgr, sf::RenderWindow& window,
							   Campaign& campaign, GameState& gameState, PipBuck& pipBuck) :
	GuiPage("New Game"),
	refreshButton({ BTN_NORMAL, resMgr, { 400, 815 }, STR_REFRESH, [this]() { btnRefresh(this); } }),
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
	this->campaignListHoverMgr.clear();
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
		this->campaignListHoverMgr += &item.button;
	}

	this->hoverMgr += &this->refreshButton;
}

bool GuiPageNewGame::loadCampaign(const std::string& campaignId)
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
		return false;
	}

	if (!this->pipBuck.setupCampaignInfos())
	{
		Log::e(STR_PIPBUCK_SETUP_FAILED);

		// unroll
		this->campaign.unload();

		return false;
	}

	this->gameState = STATE_PLAYING;

	// TODO query campaign to check what the player is actually pointing at and set proper cursor color
	this->cursorMgr.setCursor(CROSSHAIR_WHITE);

	return true;
}

void GuiPageNewGame::handleSettingsChange()
{
	GuiPage::handleSettingsChange();

	for (auto& item : this->campaignItems)
	{
		item.button.handleSettingsChange();
	}
}

bool GuiPageNewGame::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	if (this->campaignListHoverMgr.handleMouseMove(mousePos))
		return true;

	return this->hoverMgr.handleMouseMove(mousePos);
}

ClickStatus GuiPageNewGame::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	for (auto& item : this->campaignItems)
	{
		ClickStatus status = item.button.handleLeftClick(clickPos);
		if (status != CLICK_NOT_CONSUMED)
		{
			if (!this->loadCampaign(item.campaignId))
			{
				// maybe something changed on disk since the list was initially loaded, e.g. a dir was renamed or
				// removed. refresh campaign list in an attempt to reflect any changes
				Log::w(STR_REFRESHING_CAMPAIGN_LIST);
				this->rebuildCampaignList();
			}

			return CLICK_CONSUMED;
		}
	}

	return this->refreshButton.handleLeftClick(clickPos);
}

void GuiPageNewGame::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto& item : this->campaignItems)
	{
		target.draw(item.button, states);
	}

	target.draw(this->refreshButton, states);
}
