#include "pipbuck_page_world.hpp"

#include <string>
#include <memory>

#include "../../../settings/settings_manager.hpp"
#include "../../../util/util.hpp"
#include "../../../util/i18n.hpp"

// all relative to the page area
const sf::Vector2f mapPos(400, 260);
const sf::Vector2u gotoLocationBtnPos(1000, 815);
const sf::Vector2f locTitlePos(970, 260);
const sf::Vector2f locDescriptionPos(970, 300);

constexpr float ACTIVE_LOC_INDICATOR_OUTLINE_THICKNESS = 2;
constexpr uchar GRID_LINES_TRANSPARENCY = 0x40;
constexpr uint DESC_TEXT_WIDTH = 430;
constexpr float MAP_GRID_SPACING = 110;
constexpr float SQRT2 = 1.414213562;

PipBuckPageWorld::PipBuckPageWorld(ResourceManager &resMgr, Campaign &campaign) :
	PipBuckPage("World"), // TODO translate
	resMgr(resMgr),
	campaign(campaign),
	gotoLocationBtn(BTN_NORMAL, resMgr, gotoLocationBtnPos, "Travel", [this](){
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
	this->locTitle.setFont(*resMgr.getFont(FONT_MEDIUM));
	this->locTitle.setPosition(locTitlePos);

	this->locDescription.setFont(*resMgr.getFont(FONT_NORMAL));
	this->locDescription.setPosition(locDescriptionPos);

	this->activeLocIndicator.setFillColor(sf::Color::Transparent);
	this->activeLocIndicator.setOutlineThickness(ACTIVE_LOC_INDICATOR_OUTLINE_THICKNESS);

	this->setComponentColors();
	this->setGuiScale();

	this->hoverMgr += &this->gotoLocationBtn;
}

bool PipBuckPageWorld::mapContainsPoint(sf::Vector2i point)
{
	return this->mapBg.getGlobalBounds().contains(static_cast<sf::Vector2f>(point));
}

void PipBuckPageWorld::updateActiveIndicator()
{
	std::shared_ptr<Location> loc = this->campaign.getCurrentLocation();
	if (loc == nullptr)
		return;

	auto search = this->mapButtons.find(loc->getId());
	if (search == this->mapButtons.end())
		return;

	uint halfSide = LocButton::getSideLen(search->second.getIsBig()) / 2;
	float radius = halfSide * SQRT2;
	float offset = radius - halfSide;

	this->activeLocIndicator.setPosition(search->second.getPosition() - sf::Vector2f(offset, offset));
	this->activeLocIndicator.setRadius(radius);
}

ClickStatus PipBuckPageWorld::handleLeftClick(sf::Vector2i clickPos)
{
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

					this->locDescription.setString(description, DESC_TEXT_WIDTH);
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

void PipBuckPageWorld::setComponentColors()
{
	this->mapBg.setColor(SettingsManager::hudColor);
	this->locTitle.setFillColor(SettingsManager::hudColor);
	this->locDescription.setFillColor(SettingsManager::hudColor);
	this->activeLocIndicator.setOutlineColor(SettingsManager::hudColor);
}

void PipBuckPageWorld::setupMapDecorations()
{
	sf::Color hudColor = SettingsManager::hudColor;
	float mapW = this->mapBg.getLocalBounds().width;
	float mapH = this->mapBg.getLocalBounds().height;

	sf::Vector2f topLeft(std::round(mapPos.x * SettingsManager::guiScale),
						 std::round((mapPos.y - 1) * SettingsManager::guiScale));
	sf::Vector2f topRight(std::round((mapPos.x + mapW + 1) * SettingsManager::guiScale),
						  std::round((mapPos.y - 1) * SettingsManager::guiScale));
	sf::Vector2f bottomRight(std::round((mapPos.x + mapW + 1) * SettingsManager::guiScale),
							 std::round((mapPos.y + mapH) * SettingsManager::guiScale));
	// not sure why, but the left bottom corner needs to be moved 1px to the left, otherwise there's a blank pixel
	sf::Vector2f bottomLeft(std::round((mapPos.x - 1) * SettingsManager::guiScale),
							std::round((mapPos.y + mapH) * SettingsManager::guiScale));

	mapBorder[0] = sf::Vertex(topLeft, hudColor);
	mapBorder[1] = sf::Vertex(topRight, hudColor);
	mapBorder[2] = sf::Vertex(bottomRight, hudColor);
	mapBorder[3] = sf::Vertex(bottomLeft, hudColor);
	mapBorder[4] = mapBorder[0];

	// set transparency for grid lines
	hudColor.a = GRID_LINES_TRANSPARENCY;

	float pos = mapPos.x + MAP_GRID_SPACING;
	for (uint i = 0; i < 8; i += 2)
	{
		if (pos - mapPos.x > mapW)
			break; // just in case someone will make a smaller map

		sf::Vector2f top(std::round(pos * SettingsManager::guiScale),
						 std::round(mapPos.y * SettingsManager::guiScale));
		sf::Vector2f bottom(std::round(pos * SettingsManager::guiScale),
							std::round((mapPos.y + mapH) * SettingsManager::guiScale));

		mapGridLines[i] = sf::Vertex(top, hudColor);
		mapGridLines[i + 1] = sf::Vertex(bottom, hudColor);
		pos += MAP_GRID_SPACING;
	}

	pos = mapPos.y + MAP_GRID_SPACING;

	for (uint i = 8; i < 16; i += 2)
	{
		if (pos - mapPos.y > mapH)
			break; // just in case someone will make a smaller map

		sf::Vector2f left(std::round(mapPos.x * SettingsManager::guiScale),
						  std::round(pos * SettingsManager::guiScale));
		sf::Vector2f right(std::round((mapPos.x + mapW) * SettingsManager::guiScale),
						   std::round(pos * SettingsManager::guiScale));

		mapGridLines[i] = sf::Vertex(left, hudColor);
		mapGridLines[i + 1] = sf::Vertex(right, hudColor);
		pos += MAP_GRID_SPACING;
	}
}

bool PipBuckPageWorld::setupCampaignInfos()
{
	this->unloadCampaignInfos();

	std::shared_ptr<sf::Texture> mapBgTxt = this->resMgr.getTexture(this->campaign.getWorldMapBackground(), false);
	if (mapBgTxt == nullptr)
		return false;

	this->mapBg.setTexture(mapBgTxt);
	this->setupMapDecorations();

	for (const auto &loc : this->campaign.getLocations())
	{
		std::shared_ptr<sf::Texture> iconTxt = this->resMgr.getTexture(loc.second->getWorldMapIconId(), false);
		if (iconTxt == nullptr)
		{
			this->unloadCampaignInfos();
			return false;
		}

		this->mapButtons.try_emplace(
			loc.first,
			loc.second->isWorldMapIconBig(),
			loc.second->isBasecamp(),
			mapPos + loc.second->getWorldMapCoords(),
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
	this->locTitle.setCharacterSize(static_cast<uint>(SettingsManager::guiScale * FONT_H2));
	this->locDescription.setCharacterSize(static_cast<uint>(SettingsManager::guiScale * FONT_SPAN));
	this->mapBg.setPosition(mapPos * SettingsManager::guiScale);
	this->mapBg.setScale(SettingsManager::guiScale, SettingsManager::guiScale);

	this->updateActiveIndicator();
}

void PipBuckPageWorld::handleSettingsChange()
{
	this->gotoLocationBtn.handleSettingsChange();

	for (auto &btn : this->mapButtons)
	{
		btn.second.handleSettingsChange();
	}

	this->locTitle.handleSettingsChange();
	this->locDescription.handleSettingsChange();

	this->setGuiScale();
	this->setupMapDecorations();
	this->setComponentColors();
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
