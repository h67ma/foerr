// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "pipbuck_category.hpp"

#include <memory>

#include "../../settings/settings_manager.hpp"

constexpr uint PIPB_PAGE_BTNS_X_START = 385;
constexpr uint PIPB_PAGE_BTNS_X_DISTANCE = 140;
static const sf::Vector2f POS_PAGE(400, 250);

/**
 * Note: defaultPage *must* be the key of one of the pages, or else ::setup() will fail.
 * Note: pages can't be passed as an unordered map, because order is important for creating buttons.
 * However nothing prevents internal pages and buttons maps from being unordered.
 */
PipBuckCategory::PipBuckCategory(ResourceManager& resMgr, PipBuckPageType defaultPage,
								 std::map<PipBuckPageType, std::shared_ptr<GuiPage>> pages) :
	selectedPageType(defaultPage),
	pages(pages.begin(), pages.end()),
	soundPageChange(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGECHANGE)),
	soundClick(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGE_CLICK))
{
	float initialVolume = static_cast<float>(SettingsManager::fxVolume);
	this->soundPageChange.setVolume(initialVolume);
	this->soundClick.setVolume(initialVolume);

	// create buttons for switching pages
	uint x = PIPB_PAGE_BTNS_X_START;
	for (auto& page : pages)
	{
		this->pageButtons.emplace(page.first, SimpleButton(BTN_NARROW, resMgr, { x, 210 }, page.second->pageTitle));
		x += PIPB_PAGE_BTNS_X_DISTANCE;

		page.second->setPosition(POS_PAGE);
	}
}

/**
 * Setups buttons stored in this object to respond to hover events.
 *
 * Also selects the default page.
 *
 * Must be called after constructing the object. If the object is
 * moved in memory, there *will* be problems (because of hover mgr).
 *
 * @return true if setup was successful
 * @return false if setup failed
 */
bool PipBuckCategory::setup()
{
	// set default page
	if (!this->changePage(this->selectedPageType))
		return false;

	for (auto& btn : this->pageButtons)
	{
		this->hoverMgr += &btn.second;
	}

	return true;
}

PipBuckPageType PipBuckCategory::getSelectedPageType() const
{
	return this->selectedPageType;
}

void PipBuckCategory::changeActiveButton(PipBuckPageType newPageType)
{
	auto foundOld = this->pageButtons.find(this->selectedPageType);
	if (foundOld != this->pageButtons.end())
	{
		// found previously selected button, deselect it
		foundOld->second.setSelected(false);
	}

	auto foundNew = this->pageButtons.find(newPageType);
	if (foundNew != this->pageButtons.end())
	{
		// found new button, select it
		foundNew->second.setSelected(true);
	}
}

bool PipBuckCategory::changePage(PipBuckPageType newPageType)
{
	auto foundPage = this->pages.find(newPageType);
	if (foundPage == this->pages.end())
		return false;

	this->changeActiveButton(newPageType);

	this->selectedPageType = newPageType;
	this->selectedPage = foundPage->second;

	return true;
}

ClickStatus PipBuckCategory::handleLeftClick(sf::Vector2i clickPos)
{
	if (this->selectedPage == nullptr)
		return CLICK_NOT_CONSUMED;

	clickPos -= this->getPosition();

	ClickStatus pageResult = this->selectedPage->handleLeftClick(clickPos);
	if (pageResult != CLICK_NOT_CONSUMED)
	{
		// basically we want every page click to play the same sound
		// so there's no point in having each page keep its own sf::Sound for that.
		// except the case when pipbuck is being closed, then pipbuck sound should play
		if (pageResult != CLICK_CONSUMED_CLOSE)
			this->soundClick.play();

		return pageResult;
	}

	for (auto btn : this->pageButtons)
	{
		if (btn.second.containsPoint(clickPos))
		{
			if (btn.first != this->selectedPageType)
			{
				this->changePage(btn.first);
				this->soundPageChange.play();
			}
			return CLICK_CONSUMED;
		}
	}

	return CLICK_NOT_CONSUMED;
}

void PipBuckCategory::handleLeftClickUp()
{
	if (this->selectedPage != nullptr)
		this->selectedPage->handleLeftClickUp();
}

bool PipBuckCategory::handleMouseMove(sf::Vector2i mousePos)
{
	if (this->selectedPage == nullptr)
		return false;

	mousePos -= this->getPosition();

	if (this->selectedPage->handleMouseMove(mousePos))
		return true;

	return this->hoverMgr.handleMouseMove(mousePos);
}

/**
 * Setups the PipBuck pages according to campaign data.
 * Locations in the campaign should be already loaded.
 *
 * @return true if setup was successful
 * @return false if there was an error during setup
 */
bool PipBuckCategory::setupCampaignInfos()
{
	for (const auto& page : this->pages)
	{
		if (!page.second->setupCampaignInfos())
			return false;
	}

	return true;
}

void PipBuckCategory::unloadCampaignInfos()
{
	for (const auto& page : this->pages)
	{
		page.second->unloadCampaignInfos();
	}
}

PipBuckCategoryType PipBuckCategory::pageTypeToCategoryType(PipBuckPageType pageType)
{
	switch (pageType)
	{
		case PIPB_PAGE_LOAD:
		case PIPB_PAGE_SAVE:
		case PIPB_PAGE_SETTINGS:
		case PIPB_PAGE_CONTROLS:
		case PIPB_PAGE_LOG:
			return PIPB_CAT_GAME;
		case PIPB_PAGE_STATUS_MAIN:
		case PIPB_PAGE_SKILLS:
		case PIPB_PAGE_PERKS:
		case PIPB_PAGE_EFFECTS:
		case PIPB_PAGE_HEALTH:
			return PIPB_CAT_STATUS;
		case PIPB_PAGE_WEAPONS:
		case PIPB_PAGE_ARMOR:
		case PIPB_PAGE_EQUIPMENT:
		case PIPB_PAGE_INVENTORY_OTHER:
		case PIPB_PAGE_AMMO:
			return PIPB_CAT_INVENTORY;
		case PIPB_PAGE_MAP:
		case PIPB_PAGE_WORLD:
		case PIPB_PAGE_QUESTS:
		case PIPB_PAGE_NOTES:
		case PIPB_PAGE_ENEMIES:
			return PIPB_CAT_INFO;
		default:
			return PIPB_CAT_NO_CAT;
	}
}

void PipBuckCategory::handleSettingsChange()
{
	for (auto& btn : this->pageButtons)
	{
		btn.second.handleSettingsChange();
	}

	for (const auto& page : this->pages)
	{
		page.second->handleSettingsChange();
	}
}

void PipBuckCategory::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	if (this->selectedPage != nullptr)
	{
		target.draw(*this->selectedPage, states);
	}

	for (const auto& btn : this->pageButtons)
	{
		target.draw(btn.second, states);
	}
}
