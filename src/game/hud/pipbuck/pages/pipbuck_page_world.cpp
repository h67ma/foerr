#include "pipbuck_page_world.hpp"

// relative to the page area
#define WORLD_MAP_X 400
#define WORLD_MAP_Y 260

PipBuckPageWorld::PipBuckPageWorld(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign) :
	resMgr(resMgr),
	campaign(campaign),
	guiScale(scale),
	hudColor(hudColor),
	gotoLocationBtn(scale, BTN_NORMAL, hudColor, resMgr, 1000, 815, "Travel", [this](){
		// TODO display loading screen
		if (this->selectedLocationIdx != NO_LOCATION_SELECTED &&
			this->selectedLocationIdx < this->locationIds.size())
		{
			this->campaign.changeLocation(this->locationIds[this->selectedLocationIdx]);

			// reset map buttons
			this->mapButtons[this->selectedLocationIdx].setSelected(false);
			this->selectedLocationIdx = NO_LOCATION_SELECTED;
		}
	})
{
	this->mapBg.setPosition(WORLD_MAP_X, WORLD_MAP_Y);

	this->hoverMgr.addHoverable(&this->gotoLocationBtn);
}

ClickStatus PipBuckPageWorld::handleLeftClick(int x, int y)
{
	// if click was outside map area, then no point in checking map buttons
	if (this->mapBg.getGlobalBounds().contains(x - this->getPosition().x, y - this->getPosition().y))
	{
		for (auto it = this->mapButtons.begin(); it != this->mapButtons.end(); it++)
		{
			if (it->handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
			{
				// deselect old map btn if any
				if (this->selectedLocationIdx != NO_LOCATION_SELECTED &&
					this->selectedLocationIdx < this->locationIds.size())
					this->mapButtons[this->selectedLocationIdx].setSelected(false);

				this->selectedLocationIdx = static_cast<int>(std::distance(this->mapButtons.begin(), it));

				// select new btn
				it->setSelected(true);

				return CLICK_CONSUMED;
			}
		}
	}

	if (this->selectedLocationIdx != NO_LOCATION_SELECTED &&
		this->gotoLocationBtn.handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
		return CLICK_CONSUMED_CLOSE;

	return CLICK_NOT_CONSUMED;
}

bool PipBuckPageWorld::handleMouseMove(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	// if hover was outside map area, then no point in checking map buttons
	if (this->mapBg.getGlobalBounds().contains(static_cast<float>(x), static_cast<float>(y)))
	{
		if (this->mapButtonHoverMgr.handleMouseMove(x, y))
			return true;
	}

	if (this->selectedLocationIdx != NO_LOCATION_SELECTED)
		return this->hoverMgr.handleMouseMove(x, y);

	return false;
}

std::string PipBuckPageWorld::getLabel()
{
	return "World"; // TODO translate
}

bool PipBuckPageWorld::setupCampaignInfos()
{
	sf::Texture *mapBgTxt = this->resMgr.getTexture(this->campaign.getWorldMapBackground());
	if (mapBgTxt == nullptr)
		return false;

	this->mapBg.setTexture(*mapBgTxt);

	this->selectedLocationIdx = NO_LOCATION_SELECTED;
	this->mapButtonHoverMgr.clear();
	this->mapButtons.clear();

	for (const auto &loc : this->campaign.getLocations())
	{
		mapButtons.emplace_back(
			this->guiScale,
			BTN_NORMAL,
			this->hudColor,
			this->resMgr,
			WORLD_MAP_X + loc.second->getWorldMapX(),
			WORLD_MAP_Y + loc.second->getWorldMapY(),
			loc.second->getTitle()
		);

		// keep track of location ids in a separate vector
		locationIds.emplace_back(loc.first);
	}

	for (auto &btn : this->mapButtons)
	{
		this->mapButtonHoverMgr.addHoverable(&btn);
	}

	return true;
}

void PipBuckPageWorld::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->mapBg, states);

	for (auto &btn : this->mapButtons)
	{
		target.draw(btn, states);
	}

	if (this->selectedLocationIdx != NO_LOCATION_SELECTED)
		target.draw(this->gotoLocationBtn, states);
}
