#include "pipbuck_category.hpp"

#define PIPB_PAGE_BTNS_X_START 385
#define PIPB_PAGE_BTNS_X_DISTANCE 140

/**
 * Note: defaultPage *must* be the key of one of the pages, or else ::setup() will fail.
 */
PipBuckCategory::PipBuckCategory(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, PipBuckPageType defaultPage, std::unordered_map<PipBuckPageType, std::shared_ptr<PipBuckPage>> pages) :
	selectedPage(defaultPage),
	pages(pages),
	soundPageChange(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGECHANGE)),
	soundClick(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGE_CLICK))
{
	this->soundPageChange.get().setVolume(static_cast<float>(fxVolume));
	this->soundClick.get().setVolume(static_cast<float>(fxVolume));

	// create buttons for switching pages
	uint x = PIPB_PAGE_BTNS_X_START;
	for (const auto &page : this->pages)
	{
		this->pageButtons.emplace(page.first, SimpleButton(scale, BTN_NARROW, hudColor, resMgr, { x, 210 }, page.second->getLabel()));
		x += PIPB_PAGE_BTNS_X_DISTANCE;
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
	if (!this->changePage(this->selectedPage))
		return false;

	for (auto &btn : this->pageButtons)
	{
		this->hoverMgr += &btn.second;
	}

	return true;
}

bool PipBuckCategory::changePage(PipBuckPageType pageType)
{
	auto found = this->pages.find(pageType);
	if (found == this->pages.end())
		return false;

	// ::pageButtons map has the same keys as ::pages map
	// ...famous last words, right? let's hope not
	this->pageButtons.at(this->selectedPage).setSelected(false);
	this->pageButtons.at(pageType).setSelected(true);

	this->selectedPage = pageType;
	return true;
}

ClickStatus PipBuckCategory::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getIntPosition();

	ClickStatus pageResult = this->pages.at(this->selectedPage)->handleLeftClick(clickPos);
	if (pageResult != CLICK_NOT_CONSUMED)
	{
		// basically we want every page click to play the same sound
		// so there's no point in having each page keep its own sf::Sound for that.
		// except the case when pipbuck is being closed, then pipbuck sound should play
		if (pageResult != CLICK_CONSUMED_CLOSE)
			this->soundClick.get().play();

		return pageResult;
	}

	for (auto btn : this->pageButtons)
	{
		if (btn.second.containsPoint(clickPos))
		{
			if (btn.first != this->selectedPage)
			{
				this->changePage(btn.first);
				this->soundPageChange.get().play();
			}
			return CLICK_CONSUMED;
		}
	}

	return CLICK_NOT_CONSUMED;
}

bool PipBuckCategory::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getIntPosition();

	if (this->pages.at(this->selectedPage)->handleMouseMove(mousePos))
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
	for (const auto &page : this->pages)
	{
		if (!page.second->setupCampaignInfos())
			return false;
	}

	return true;
}

void PipBuckCategory::unloadCampaignInfos()
{
	for (const auto &page : this->pages)
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
			return PIPB_CAT_MAIN;
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

void PipBuckCategory::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(*this->pages.at(this->selectedPage), states);

	for (const auto &btn : this->pageButtons)
	{
		target.draw(btn.second, states);
	}
}
