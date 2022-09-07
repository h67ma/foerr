#include "pipbuck_page_world.hpp"

// relative to the page area
#define WORLD_MAP_X 400
#define WORLD_MAP_Y 260

PipBuckPageWorld::PipBuckPageWorld(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign) :
	resMgr(resMgr),
	campaign(campaign),
	guiScale(scale),
	hudColor(hudColor)
{
	this->mapBg.setPosition(WORLD_MAP_X, WORLD_MAP_Y);
}

ClickStatus PipBuckPageWorld::handleLeftClick(int x, int y)
{
	if (this->mapBg.getGlobalBounds().contains(x - this->getPosition().x, y - this->getPosition().y))
	{
		// if click was outside map area, then no point in checking it
		for (auto &btn : this->mapButtons)
		{
			if (btn.handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
				return CLICK_CONSUMED_CLOSE;
		}
	}

	return CLICK_NOT_CONSUMED;
}

bool PipBuckPageWorld::handleMouseMove(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	if (this->mapBg.getGlobalBounds().contains(static_cast<float>(x), static_cast<float>(y)))
	{
		// if hover was outside map area, then no point in checking it
		if (this->mapButtonHoverMgr.handleMouseMove(x, y) != CLICK_NOT_CONSUMED)
			return CLICK_CONSUMED;
	}

	return this->hoverMgr.handleMouseMove(x, y);
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
			loc.second->getTitle(),
			[this, &loc](){
				this->campaign.changeLocation(loc.first);
			}
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
}
