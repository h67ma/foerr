// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2024 h67ma <szycikm@gmail.com>

#include "text_list_view.hpp"

#include "../../settings/settings_manager.hpp"

constexpr float TEXT_V_MARGIN = 5;

TextListView::TextListView(ResourceManager& resMgr, enum FontType fontType, uint fontSize,
						   const sf::Vector2f& scrollableAreaSize, std::deque<StringAndColor>& items) :
	ListView(resMgr, scrollableAreaSize, resMgr.getFont(fontType)->getLineSpacing(fontSize) + TEXT_V_MARGIN, items),
	viewHolder(*resMgr.getFont(fontType), fontSize)
{
}

const TextLabel& TextListView::bindViewHolder(const StringAndColor& item)
{
	this->viewHolder.setString(item.first);
	this->viewHolder.setFillColor(item.second);

	return this->viewHolder;
}

void TextListView::handleSettingsChange()
{
	this->viewHolder.handleSettingsChange();
	ListView::handleSettingsChange();
}
