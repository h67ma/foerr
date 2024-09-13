// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2024 h67ma <szycikm@gmail.com>

#pragma once

#include <stdexcept>

#include <SFML/System/Vector2.hpp>

#include "../../resources/resource_manager.hpp"
#include "../../util/util.hpp"
#include "../clickable.hpp"
#include "../hud.hpp"
#include "scrollable.hpp"

/**
 * A generic scrollable component, displaying a vertically stacked collection of items. All items are the same type of
 * component (can be a text label, image, table row, etc.).
 *
 * Items are displayed based on data (@tparam Data) stored in a collection (@tparam Container). Note that a view holder
 * (@tparam Holder) component is *not* created for each data item in the collection. Instead, one view holder is being
 * reused to draw all items.
 *
 * @tparam Holder component used to display a single item
 * @tparam Data struct/class holding a single item's data
 * @tparam Container implementation of the collection which stores data items
 */
template<typename Holder, typename Data, template<typename, typename> class Container>
class ListView : public Scrollable
{
	private:
		const float itemHeight; // the height of a single item
		Container<Data, std::allocator<Data>>& items;

		void scrollableContentHeightChanged(bool resize)
		{
			float totalHeight = calculateGuiAwareScalar(this->itemHeight) * static_cast<float>(this->items.size());
			this->handleScrollableContentHeightChanged(totalHeight, SCROLL_BOTTOM, resize);
		}

	protected:
		virtual const Holder& bindViewHolder(const Data& item) = 0;

	public:
		ListView(ResourceManager& resMgr, const sf::Vector2f& scrollableAreaSize, float itemHeight,
				 Container<Data, std::allocator<Data>>& items) :
			Scrollable(resMgr, scrollableAreaSize),
			itemHeight(itemHeight),
			items(items)
		{
		}

		/**
		 * Click handler that does not support detecting clicks on items (only scrollbar).
		 */
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override
		{
			return Scrollable::handleLeftClick(clickPos);
		}

		/**
		 * Handles a click event on list. Finds the clicked item and returns it (via param).
		 *
		 * @param clickPos click position
		 * @param clickedData output param containing the item's data, if an item was clicked. Otherwise unmodified
		 * @returns CLICK_CONSUMED if an item was clicked, CLICK_NOT_CONSUMED otherwise
		 */
		ClickStatus handleLeftClick(sf::Vector2i clickPos, Data& clickedData)
		{
			clickPos -= this->getPosition();

			ClickStatus status = Scrollable::handleLeftClick(clickPos);
			if (status != CLICK_NOT_CONSUMED)
				return status;

			if (!this->isWithinScrollArea(static_cast<sf::Vector2f>(clickPos)))
				return CLICK_NOT_CONSUMED;

			// click was inside the scroll area.
			// find the selected item, assuming the item's width is equal to scroll area width.

			std::size_t selectedItemIdx =
				(-this->getScrollOffset() + clickPos.y) / calculateGuiAwareScalar(this->itemHeight);
			try
			{
				clickedData = items.at(selectedItemIdx);
			}
			catch (const std::out_of_range& ex)
			{
				return CLICK_NOT_CONSUMED;
			}

			return CLICK_CONSUMED;
		}

		void handleItemsChanged()
		{
			this->scrollableContentHeightChanged(false);
		}

		void handleSettingsChange() override
		{
			Scrollable::handleSettingsChange();

			this->scrollableContentHeightChanged(true);
		}

		/**
		 * Draws the collection of items as vertical list.
		 * Only draws items that are currently visible inside the scrollable area.
		 */
		void drawScrollableContent(sf::RenderTarget& target, sf::RenderStates states) override
		{
			float offset = 0; // sadly can't reuse the y offset from render states
			float scaledItemHeight = calculateGuiAwareScalar(this->itemHeight);

			for (const auto& item : this->items)
			{
				if (this->shouldDrawItem(scaledItemHeight, offset))
					target.draw(this->bindViewHolder(item), states);

				// move down regardless if the item was actually drawn or not,
				// as the next item might qualify for drawing.
				// this happens when the list is scrolled a bit.
				offset += scaledItemHeight;
				states.transform.translate(0, scaledItemHeight);
			}
		}
};
