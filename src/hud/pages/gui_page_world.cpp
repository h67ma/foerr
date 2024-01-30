// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page_world.hpp"

#include <memory>
#include <string>

#include "../../settings/settings_manager.hpp"
#include "../../util/i18n.hpp"
#include "../../util/util.hpp"

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

GuiPageWorld::GuiPageWorld(ResourceManager& resMgr, Campaign& campaign) :
	GuiPage("World"), // TODO translate
	resMgr(resMgr),
	campaign(campaign),
	gotoLocationBtn(BTN_NORMAL, resMgr, gotoLocationBtnPos, "Travel",
					[this]()
					{
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
					}),
	locTitle(*resMgr.getFont(FONT_MEDIUM), FONT_H2, locTitlePos),
	locDescription(*resMgr.getFont(FONT_NORMAL), FONT_SPAN, locDescriptionPos)
{
	this->activeLocIndicator.setFillColor(sf::Color::Transparent);
	this->activeLocIndicator.setOutlineThickness(ACTIVE_LOC_INDICATOR_OUTLINE_THICKNESS);

	this->setComponentColors();
	this->setGuiScale();

	this->hoverMgr += &this->gotoLocationBtn;
}

bool GuiPageWorld::mapContainsPoint(sf::Vector2i point)
{
	return this->mapBg.getGlobalBounds().contains(static_cast<sf::Vector2f>(point));
}

void GuiPageWorld::updateActiveIndicator()
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

ClickStatus GuiPageWorld::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	// if click was outside map area, then no point in checking map buttons
	if (this->mapContainsPoint(clickPos))
	{
		auto oldBtnSearch = this->mapButtons.find(this->selectedLocId);

		for (auto& btn : this->mapButtons)
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

	if (this->travelButtonAvailable && this->gotoLocationBtn.handleLeftClick(clickPos) != CLICK_NOT_CONSUMED)
	{
		this->hoverMgr.removeHover(); // otherwise the "travel" btn will be highlighted when it next appears
		return CLICK_CONSUMED_CLOSE;
	}

	return CLICK_NOT_CONSUMED;
}

bool GuiPageWorld::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	// if hover was outside map area, then no point in checking map buttons
	if (this->mapContainsPoint(mousePos))
	{
		if (this->mapButtonHoverMgr.handleMouseMove(mousePos))
			return true;
	}

	if (this->isAnyLocationSelected() && this->travelButtonAvailable)
		return this->hoverMgr.handleMouseMove(mousePos);

	return false;
}

void GuiPageWorld::setComponentColors()
{
	this->mapBg.setColor(SettingsManager::hudColor);
	this->locTitle.setFillColor(SettingsManager::hudColor);
	this->locDescription.setFillColor(SettingsManager::hudColor);
	this->activeLocIndicator.setOutlineColor(SettingsManager::hudColor);
}

void GuiPageWorld::setupMapDecorations()
{
	sf::Color hudColor = SettingsManager::hudColor;
	float mapW = this->mapBg.getLocalBounds().width;
	float mapH = this->mapBg.getLocalBounds().height;

	sf::Vector2f topLeft = calculateGuiAwarePoint({ mapPos.x, mapPos.y - 1 });
	sf::Vector2f topRight = calculateGuiAwarePoint({ mapPos.x + mapW + 1, mapPos.y - 1 });
	sf::Vector2f bottomRight = calculateGuiAwarePoint({ mapPos.x + mapW + 1, mapPos.y + mapH });
	sf::Vector2f bottomLeft = calculateGuiAwarePoint({ mapPos.x, mapPos.y + mapH });

	this->mapBorder[0] = sf::Vertex(topLeft, hudColor);
	this->mapBorder[1] = sf::Vertex(topRight, hudColor);
	this->mapBorder[2] = sf::Vertex(bottomRight, hudColor);
	this->mapBorder[3] = sf::Vertex(bottomLeft, hudColor);
	this->mapBorder[4] = this->mapBorder[0];

	// set transparency for grid lines
	hudColor.a = GRID_LINES_TRANSPARENCY;

	float pos = mapPos.x + MAP_GRID_SPACING;
	for (uint i = 0; i < 8; i += 2)
	{
		if (pos - mapPos.x > mapW)
			break; // just in case someone will make a smaller map

		sf::Vector2f top = calculateGuiAwarePoint({ pos, mapPos.y });
		sf::Vector2f bottom = calculateGuiAwarePoint({ pos, mapPos.y + mapH });

		mapGridLines[i] = sf::Vertex(top, hudColor);
		mapGridLines[i + 1] = sf::Vertex(bottom, hudColor);
		pos += MAP_GRID_SPACING;
	}

	pos = mapPos.y + MAP_GRID_SPACING;

	for (uint i = 8; i < 16; i += 2)
	{
		if (pos - mapPos.y > mapH)
			break; // just in case someone will make a smaller map

		sf::Vector2f left = calculateGuiAwarePoint({ mapPos.x, pos });
		sf::Vector2f right = calculateGuiAwarePoint({ mapPos.x + mapW, pos });

		mapGridLines[i] = sf::Vertex(left, hudColor);
		mapGridLines[i + 1] = sf::Vertex(right, hudColor);
		pos += MAP_GRID_SPACING;
	}
}

bool GuiPageWorld::setupCampaignInfos()
{
	this->unloadCampaignInfos();

	std::string worldMapBgPath = pathCombine(PATH_CAMPAIGNS, this->campaign.getId(), PATH_WORLD_MAP);

	std::shared_ptr<sf::Texture> mapBgTxt = this->resMgr.getTexture(worldMapBgPath, false);
	if (mapBgTxt == nullptr)
		return false;

	this->mapBg.setTexture(mapBgTxt);
	this->setupMapDecorations();

	for (const auto& loc : this->campaign.getLocations())
	{
		std::shared_ptr<sf::Texture> iconTxt = this->resMgr.getTexture(loc.second->getWorldMapIconId(), false);
		if (iconTxt == nullptr)
		{
			this->unloadCampaignInfos();
			return false;
		}

		this->mapButtons.try_emplace(loc.first, loc.second->isWorldMapIconBig(), loc.second->isBasecamp(),
									 mapPos + loc.second->getWorldMapCoords(), iconTxt);
	}

	this->updateActiveIndicator();

	for (auto& btn : this->mapButtons)
	{
		this->mapButtonHoverMgr += &btn.second;
	}

	return true;
}

void GuiPageWorld::unloadCampaignInfos()
{
	this->mapBg.clearPtr();
	this->mapButtonHoverMgr.clear();
	this->mapButtons.clear();
	this->selectedLocId = NO_LOCATION_SELECTED;
	this->travelButtonAvailable = false;
}

void GuiPageWorld::setGuiScale()
{
	this->locTitle.handleSettingsChange();
	this->locDescription.handleSettingsChange();
	this->mapBg.setPosition(mapPos * SettingsManager::guiScale);
	this->mapBg.setScale(SettingsManager::guiScale, SettingsManager::guiScale);

	this->updateActiveIndicator();
}

bool GuiPageWorld::isAnyLocationSelected() const
{
	return this->selectedLocId != NO_LOCATION_SELECTED; // NOLINT(readability-container-size-empty)
}

void GuiPageWorld::handleSettingsChange()
{
	GuiPage::handleSettingsChange();

	this->gotoLocationBtn.handleSettingsChange();

	for (auto& btn : this->mapButtons)
	{
		btn.second.handleSettingsChange();
	}

	this->locTitle.handleSettingsChange();
	this->locDescription.handleSettingsChange();

	this->setGuiScale();
	this->setupMapDecorations();
	this->setComponentColors();
}

void GuiPageWorld::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->mapBg, states);
	target.draw(this->mapGridLines, states);
	target.draw(this->mapBorder, states);

	for (const auto& btn : this->mapButtons)
	{
		target.draw(btn.second, states);
	}

	target.draw(this->activeLocIndicator, states);

	if (this->isAnyLocationSelected())
	{
		target.draw(this->locTitle, states);
		target.draw(this->locDescription, states);

		if (this->travelButtonAvailable)
			target.draw(this->gotoLocationBtn, states);
	}
}
