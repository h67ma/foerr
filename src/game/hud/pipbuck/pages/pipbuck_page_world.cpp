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
			this->selectedLocationIdx < this->campaign.getLocations().size())
		{
			this->campaign.changeLocation(this->selectedLocationIdx);

			// TODO un-highlight old location & hightlight new one

			// reset selection
			// no need to reset texts as now they are not shown anyway
			this->mapButtons[this->selectedLocationIdx].setSelected(false);
			this->selectedLocationIdx = NO_LOCATION_SELECTED;
		}
	})
{
	this->mapBg.setPosition(WORLD_MAP_X, WORLD_MAP_Y);

	this->locTitle.setFont(*resMgr.getFont(FONT_MEDIUM));
	this->locTitle.setPosition(970, 260);
	this->locTitle.setFillColor(hudColor);

	this->locDescription.setFont(*resMgr.getFont(FONT_NORMAL));
	this->locDescription.setPosition(970, 300);
	this->locDescription.setFillColor(hudColor);

	switch (scale)
	{
		case GUI_SMALL:
			this->locTitle.setCharacterSize(FONT_SIZE_SMALL);
			this->locDescription.setCharacterSize(FONT_SIZE_SMALL);
			break;
		case GUI_LARGE:
			this->locTitle.setCharacterSize(FONT_SIZE_LARGE);
			this->locDescription.setCharacterSize(FONT_SIZE_LARGE);
			break;
		case GUI_NORMAL:
		default:
			this->locTitle.setCharacterSize(FONT_SIZE_NORMAL);
			this->locDescription.setCharacterSize(FONT_SIZE_NORMAL);
	}
	

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
					this->selectedLocationIdx < this->campaign.getLocations().size())
					this->mapButtons[this->selectedLocationIdx].setSelected(false);

				this->selectedLocationIdx = static_cast<int>(std::distance(this->mapButtons.begin(), it));
				this->locTitle.setString(this->campaign.getLocations()[this->selectedLocationIdx].getTitle());
				this->locDescription.setString(this->campaign.getLocations()[this->selectedLocationIdx].getDescription());

				// select new btn
				it->setSelected(true);

				return CLICK_CONSUMED;
			}
		}
	}

	if (this->selectedLocationIdx != NO_LOCATION_SELECTED &&
		this->selectedLocationIdx != this->campaign.getCurrentLocationIdx() &&
		this->gotoLocationBtn.handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
	{
		this->hoverMgr.removeHover(); // otherwise the "travel" btn will be highlighted when it next appears
		return CLICK_CONSUMED_CLOSE;
	}

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

	if (this->selectedLocationIdx != NO_LOCATION_SELECTED && this->selectedLocationIdx != this->campaign.getCurrentLocationIdx())
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

	for (auto &loc : this->campaign.getLocations())
	{
		mapButtons.emplace_back(
			this->guiScale,
			BTN_NORMAL,
			this->hudColor,
			this->resMgr,
			WORLD_MAP_X + loc.getWorldMapX(),
			WORLD_MAP_Y + loc.getWorldMapY(),
			loc.getTitle()
		);
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
	{
		target.draw(this->locTitle, states);
		target.draw(this->locDescription, states);

		if (this->selectedLocationIdx != this->campaign.getCurrentLocationIdx())
			target.draw(this->gotoLocationBtn, states);
	}
}
