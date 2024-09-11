// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2024 h67ma <szycikm@gmail.com>

#pragma once

#include <deque>

#include "../../consts.hpp"
#include "../../resources/resource_manager.hpp"
#include "../text_label.hpp"
#include "list_view.hpp"

/**
 * A ListView which items are one-line text elements.
 * Allows setting a different text color for each item.
 * Items are stored in a deque, because they are likely to be added/removed (e.g. in case of log)
 */
class TextListView : public ListView<TextLabel, StringAndColor, std::deque>
{
	private:
		TextLabel viewHolder;
		const TextLabel& bindViewHolder(const StringAndColor& item) override;

	public:
		TextListView(ResourceManager& resMgr, enum FontType fontType, uint fontSize,
					 const sf::Vector2f& scrollableAreaSize, std::deque<StringAndColor>& items);
		void handleSettingsChange() override;
};
