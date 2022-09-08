#include "pipbuck_page_world.hpp"

// relative to the page area
#define WORLD_MAP_X 400
#define WORLD_MAP_Y 260
#define MAP_GRID_SPACING 110

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
			this->locTitle.setCharacterSize(FONT_SIZE_H2_SMALL);
			this->locDescription.setCharacterSize(FONT_SIZE_SPAN_SMALL);
			break;
		case GUI_LARGE:
			this->locTitle.setCharacterSize(FONT_SIZE_H2_LARGE);
			this->locDescription.setCharacterSize(FONT_SIZE_SPAN_LARGE);
			break;
		case GUI_NORMAL:
		default:
			this->locTitle.setCharacterSize(FONT_SIZE_H2_NORMAL);
			this->locDescription.setCharacterSize(FONT_SIZE_SPAN_NORMAL);
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

void PipBuckPageWorld::setupMapDecorations()
{
	uint mapW = static_cast<uint>(this->mapBg.getLocalBounds().width);
	uint mapH = static_cast<uint>(this->mapBg.getLocalBounds().height);

	mapBorder[0] = sf::Vertex({ static_cast<float>(WORLD_MAP_X), static_cast<float>(WORLD_MAP_Y - 1) }, this->hudColor);
	mapBorder[1] = sf::Vertex({ static_cast<float>(WORLD_MAP_X + mapW + 1), static_cast<float>(WORLD_MAP_Y - 1) }, this->hudColor);
	mapBorder[2] = sf::Vertex({ static_cast<float>(WORLD_MAP_X + mapW + 1), static_cast<float>(WORLD_MAP_Y + mapH) }, this->hudColor);
	mapBorder[3] = sf::Vertex({ static_cast<float>(WORLD_MAP_X - 1), static_cast<float>(WORLD_MAP_Y + mapH) }, this->hudColor);
	// not sure why, but the left bottom corner needs to be moved 1px to the left, otherwise there's a blank pixel
	mapBorder[4] = mapBorder[0];

	sf::Color gridColor = this->hudColor;
	gridColor.a = 0x40;
	
	float pos = WORLD_MAP_X + MAP_GRID_SPACING;
	for (uint i = 0; i < 8; i += 2)
	{
		if (pos - WORLD_MAP_X > mapW)
			break; // just in case someone will make a smaller map

		mapGridLines[i] = sf::Vertex({ pos, static_cast<float>(WORLD_MAP_Y) }, gridColor);
		mapGridLines[i + 1] = sf::Vertex({ pos, static_cast<float>(WORLD_MAP_Y + mapH) }, gridColor);
		pos += MAP_GRID_SPACING;
	}

	pos = WORLD_MAP_Y + MAP_GRID_SPACING;

	for (uint i = 8; i < 16; i += 2)
	{
		if (pos - WORLD_MAP_Y > mapH)
			break; // just in case someone will make a smaller map

		mapGridLines[i] = sf::Vertex({ static_cast<float>(WORLD_MAP_X), pos }, gridColor);
		mapGridLines[i + 1] = sf::Vertex({ static_cast<float>(WORLD_MAP_X + mapW), pos }, gridColor);
		pos += MAP_GRID_SPACING;
	}
}

bool PipBuckPageWorld::setupCampaignInfos()
{
	sf::Texture *mapBgTxt = this->resMgr.getTexture(this->campaign.getWorldMapBackground());
	if (mapBgTxt == nullptr)
		return false;

	this->mapBg.setTexture(*mapBgTxt);
	this->setupMapDecorations();

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
	target.draw(this->mapGridLines, states);
	target.draw(this->mapBorder, states);

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
