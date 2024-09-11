// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page_log.hpp"

#include <string>

#include "../hud.hpp"

constexpr uint LOG_HISTORY_MAX_LENGTH = 1000; // TODO? make configurable via settings

GuiPageLog::GuiPageLog(ResourceManager& resMgr) :
	GuiPage("Log"), // TODO translate
	resMgr(resMgr)
{
}

void GuiPageLog::addMsg(const StringAndColor& strAndColor)
{
	// obviously a temporary solution. should be a scrollable list.
	sf::Vector2f position = sf::Vector2f(20, 20 + (this->msgList.size() * 30));

	this->msgList.emplace_back(strAndColor.first, *resMgr.getFont(FONT_NORMAL), FONT_H3, strAndColor.second, position);

	// remove oldest message if limit is reached
	if (this->msgList.size() > LOG_HISTORY_MAX_LENGTH)
		this->msgList.pop_front();
}

void GuiPageLog::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	for (const auto& msg : this->msgList)
	{
		target.draw(msg, states);
	}
}
