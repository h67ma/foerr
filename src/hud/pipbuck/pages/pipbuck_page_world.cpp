#include "pipbuck_page_world.hpp"
#include <string>
#include <memory>
#include "../../../settings/settings_manager.hpp"
#include "../../../util/util.hpp"
#include "../../../util/i18n.hpp"

// relative to the page area
#define WORLD_MAP_X 400
#define WORLD_MAP_Y 260

#define DESCRIPTION_TEXT_WIDTH 430

#define MAP_GRID_SPACING 110
#define SQRT_2 1.414213562f

PipBuckPageWorld::PipBuckPageWorld(ResourceManager &resMgr, Campaign &campaign) :
	PipBuckPage("World"), // TODO translate
	resMgr(resMgr),
	campaign(campaign),
	gotoLocationBtn(BTN_NORMAL, resMgr, { 1000, 815 }, "Travel", [this](){
		auto search = this->mapButtons.find(this->selectedLocId);
		if (search == this->mapButtons.end())
			return;

		// TODO display a loading screen
		if (!this->campaign.changeLocation(this->selectedLocId))
			return;

		this->updateActiveIndicator();

		// reset selection
		// no need to reset texts as now they are not shown anyway
		search->second.setSelected(false);
		this->selectedLocId = NO_LOCATION_SELECTED;
		this->travelButtonAvailable = false;
	})
{
	this->mapBg.setPosition(WORLD_MAP_X, WORLD_MAP_Y);

	this->locTitle.setFont(*resMgr.getFont(FONT_MEDIUM));
	this->locTitle.setPosition(970, 260);
	this->locTitle.setFillColor(SettingsManager::hudColor);

	this->locDescription.setFont(*resMgr.getFont(FONT_NORMAL));
	this->locDescription.setPosition(970, 300);
	this->locDescription.setFillColor(SettingsManager::hudColor);

	this->activeLocIndicator.setOutlineColor(SettingsManager::hudColor);
	this->activeLocIndicator.setFillColor(sf::Color::Transparent);
	this->activeLocIndicator.setOutlineThickness(2.f);

	this->setGuiScale();

	this->hoverMgr += &this->gotoLocationBtn;
}

bool PipBuckPageWorld::mapContainsPoint(sf::Vector2i point)
{
	return this->mapBg.getGlobalBounds().contains(static_cast<sf::Vector2f>(point));
}

void PipBuckPageWorld::updateActiveIndicator()
{
	auto search = this->mapButtons.find(this->campaign.getCurrentLocation()->getId());
	if (search == this->mapButtons.end())
		return;

	uint halfSide = LocButton::getSideLen(search->second.getIsBig()) / 2;
	float radius = halfSide * SQRT_2;
	float offset = radius - halfSide;

	this->activeLocIndicator.setPosition(search->second.getPosition() - sf::Vector2f(offset, offset));
	this->activeLocIndicator.setRadius(radius);
}

ClickStatus PipBuckPageWorld::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getIntPosition();

	// if click was outside map area, then no point in checking map buttons
	if (this->mapContainsPoint(clickPos))
	{
		auto oldBtnSearch = this->mapButtons.find(this->selectedLocId);

		for (auto &btn : this->mapButtons)
		{
			if (btn.second.handleLeftClick(clickPos) != CLICK_NOT_CONSUMED)
			{
				if (oldBtnSearch != this->mapButtons.end())
				{
					if (oldBtnSearch->first == btn.first)
						return CLICK_CONSUMED; // selected location btn is already highlighted, no need to do anything

					// deselect old map btn
					oldBtnSearch->second.setSelected(false);
				}

				this->selectedLocId = btn.first;

				const std::shared_ptr<Location> selectedLoc = this->campaign.getLocation(this->selectedLocId);
				if (selectedLoc != nullptr)
				{
					this->locTitle.setString(selectedLoc->getTitle());

					std::string description = selectedLoc->getDescription();

					uint recLvl = selectedLoc->getRecommendedLevel();
					if (recLvl != REC_LVL_EMPTY)
						description += litSprintf(STR_RECOMMENDED_LVL, recLvl);

					this->locDescription.setString(description, DESCRIPTION_TEXT_WIDTH);
				}

				// select new btn
				btn.second.setSelected(true);

				// prevent traveling to the location which is already active
				this->travelButtonAvailable = this->selectedLocId != this->campaign.getCurrentLocation()->getId();

				return CLICK_CONSUMED;
			}
		}
	}

	if (this->travelButtonAvailable &&
		this->gotoLocationBtn.handleLeftClick(clickPos) != CLICK_NOT_CONSUMED)
	{
		this->hoverMgr.removeHover(); // otherwise the "travel" btn will be highlighted when it next appears
		return CLICK_CONSUMED_CLOSE;
	}

	return CLICK_NOT_CONSUMED;
}

bool PipBuckPageWorld::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getIntPosition();

	// if hover was outside map area, then no point in checking map buttons
	if (this->mapContainsPoint(mousePos))
	{
		if (this->mapButtonHoverMgr.handleMouseMove(mousePos))
			return true;
	}

	if (this->selectedLocId != NO_LOCATION_SELECTED && this->travelButtonAvailable)
		return this->hoverMgr.handleMouseMove(mousePos);

	return false;
}

void PipBuckPageWorld::setupMapDecorations()
{
	sf::Color hudColor = SettingsManager::hudColor;
	uint mapW = static_cast<uint>(this->mapBg.getLocalBounds().width);
	uint mapH = static_cast<uint>(this->mapBg.getLocalBounds().height);

	mapBorder[0] = sf::Vertex({ static_cast<float>(WORLD_MAP_X), static_cast<float>(WORLD_MAP_Y - 1) },
							  hudColor);
	mapBorder[1] = sf::Vertex({ static_cast<float>(WORLD_MAP_X + mapW + 1), static_cast<float>(WORLD_MAP_Y - 1) },
							  hudColor);
	mapBorder[2] = sf::Vertex({ static_cast<float>(WORLD_MAP_X + mapW + 1), static_cast<float>(WORLD_MAP_Y + mapH) },
							  hudColor);
	mapBorder[3] = sf::Vertex({ static_cast<float>(WORLD_MAP_X - 1), static_cast<float>(WORLD_MAP_Y + mapH) },
							  hudColor);
	// not sure why, but the left bottom corner needs to be moved 1px to the left, otherwise there's a blank pixel
	mapBorder[4] = mapBorder[0];

	// set transparency for grid lines
	hudColor.a = 0x40;

	float pos = WORLD_MAP_X + MAP_GRID_SPACING;
	for (uint i = 0; i < 8; i += 2)
	{
		if (pos - WORLD_MAP_X > mapW)
			break; // just in case someone will make a smaller map

		mapGridLines[i] = sf::Vertex({ pos, static_cast<float>(WORLD_MAP_Y) }, hudColor);
		mapGridLines[i + 1] = sf::Vertex({ pos, static_cast<float>(WORLD_MAP_Y + mapH) }, hudColor);
		pos += MAP_GRID_SPACING;
	}

	pos = WORLD_MAP_Y + MAP_GRID_SPACING;

	for (uint i = 8; i < 16; i += 2)
	{
		if (pos - WORLD_MAP_Y > mapH)
			break; // just in case someone will make a smaller map

		mapGridLines[i] = sf::Vertex({ static_cast<float>(WORLD_MAP_X), pos }, hudColor);
		mapGridLines[i + 1] = sf::Vertex({ static_cast<float>(WORLD_MAP_X + mapW), pos }, hudColor);
		pos += MAP_GRID_SPACING;
	}
}

bool PipBuckPageWorld::setupCampaignInfos()
{
	this->unloadCampaignInfos();

	std::shared_ptr<sf::Texture> mapBgTxt = this->resMgr.getTexture(this->campaign.getWorldMapBackground());
	if (mapBgTxt == nullptr)
		return false;

	this->mapBg.setTexture(mapBgTxt);
	this->setupMapDecorations();

	for (const auto &loc : this->campaign.getLocations())
	{
		std::shared_ptr<sf::Texture> iconTxt = this->resMgr.getTexture(loc.second->getWorldMapIconId());
		if (iconTxt == nullptr)
		{
			this->unloadCampaignInfos();
			return false;
		}

		this->mapButtons.try_emplace(
			loc.first,
			loc.second->isWorldMapIconBig(),
			loc.second->isBasecamp(),
			sf::Vector2u(WORLD_MAP_X + loc.second->getWorldMapCoords().x,
						 WORLD_MAP_Y + loc.second->getWorldMapCoords().y),
			iconTxt);
	}

	this->updateActiveIndicator();

	for (auto &btn : this->mapButtons)
	{
		this->mapButtonHoverMgr += &btn.second;
	}

	return true;
}

void PipBuckPageWorld::unloadCampaignInfos()
{
	this->mapBg.clearPtr();
	this->mapButtonHoverMgr.clear();
	this->mapButtons.clear();
	this->selectedLocId = NO_LOCATION_SELECTED;
	this->travelButtonAvailable = false;
}

void PipBuckPageWorld::setGuiScale()
{
	this->locTitle.setCharacterSize(getFontSize(SettingsManager::guiScale, FONT_H2));
	this->locDescription.setCharacterSize(getFontSize(SettingsManager::guiScale, FONT_SPAN));

	if (this->selectedLocId != NO_LOCATION_SELECTED)
		this->updateActiveIndicator();
}

void PipBuckPageWorld::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->mapBg, states);
	target.draw(this->mapGridLines, states);
	target.draw(this->mapBorder, states);

	for (const auto &btn : this->mapButtons)
	{
		target.draw(btn.second, states);
	}

	target.draw(this->activeLocIndicator, states);

	if (this->selectedLocId != NO_LOCATION_SELECTED)
	{
		target.draw(this->locTitle, states);
		target.draw(this->locDescription, states);

		if (travelButtonAvailable)
			target.draw(this->gotoLocationBtn, states);
	}
}
