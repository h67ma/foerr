// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page_log.hpp"

#include <string>

#include "../hud.hpp"

constexpr uint LOG_HISTORY_MAX_LENGTH = 1000; // TODO? make configurable via settings

GuiPageLog::GuiPageLog(ResourceManager& resMgr) :
	GuiPage("Log"), // TODO translate
	logListView(resMgr, FONT_NORMAL, FONT_SPAN, { FULL_PAGE_WIDTH_SANS_SCROLLBAR, FULL_PAGE_HEIGHT }, this->msgList)
{
}

void GuiPageLog::addMsg(const StringAndColor& strAndColor)
{
	this->msgList.emplace_back(strAndColor);

	// remove oldest message if limit is reached
	if (this->msgList.size() > LOG_HISTORY_MAX_LENGTH)
		this->msgList.pop_front();

	this->logListView.handleItemsChanged(SCROLL_BOTTOM);
}

void GuiPageLog::handleScroll(float delta, sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	this->logListView.handleScroll(delta, mousePos);
}

void GuiPageLog::handleSettingsChange()
{
	GuiPage::handleSettingsChange();

	this->logListView.handleSettingsChange();
}

ClickStatus GuiPageLog::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	return this->logListView.handleLeftClick(clickPos);
}

void GuiPageLog::handleLeftClickUp()
{
	this->logListView.handleLeftClickUp();
}

bool GuiPageLog::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	return this->logListView.handleMouseMove(mousePos);
}

void GuiPageLog::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->logListView, states);
}
