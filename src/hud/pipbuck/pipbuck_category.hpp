// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <map>
#include <memory>
#include <unordered_map>
#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Transformable.hpp>

#include "../../campaigns/campaign.hpp"
#include "../../resources/resource_manager.hpp"
#include "../../resources/sound_resource.hpp"
#include "../buttons/simple_button.hpp"
#include "../clickable.hpp"
#include "../configurable_gui_component.hpp"
#include "../hover_manager.hpp"
#include "../pages/gui_page.hpp"
#include "pipbuck_page.hpp"

enum PipBuckCategoryType
{
	PIPB_CAT_GAME,
	PIPB_CAT_STATUS,
	PIPB_CAT_INVENTORY,
	PIPB_CAT_INFO,
	PIPB_CAT_NO_CAT,
};

/**
 * Represents a single PipBuck category (e.g. "Status").
 * Contains 5 pages.
 */
class PipBuckCategory : public sf::Drawable, public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		HoverManager hoverMgr;
		PipBuckPageType selectedPageType;
		std::shared_ptr<GuiPage> selectedPage = nullptr;
		std::unordered_map<PipBuckPageType, SimpleButton> pageButtons;
		SoundResource soundPageChange;
		SoundResource soundClick;
		const std::unordered_map<PipBuckPageType, std::shared_ptr<GuiPage>> pages;
		void changeActiveButton(PipBuckPageType newPageType);

	public:
		PipBuckCategory(ResourceManager& resMgr, PipBuckPageType defaultPage,
						std::map<PipBuckPageType, std::shared_ptr<GuiPage>> pages);
		bool setup();
		PipBuckPageType getSelectedPageType() const;
		ClickStatus handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		bool handleMouseMove(sf::Vector2i mousePos);
		bool changePage(PipBuckPageType newPageType);
		bool setupCampaignInfos();
		void unloadCampaignInfos();
		static PipBuckCategoryType pageTypeToCategoryType(PipBuckPageType pageType);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;

		/**
		 * Finds a page and casts it to specific type.
		 * Return value can be a nullptr if page was not found or if the cast failed.
		 */
		template<typename T>
		T* getPage(PipBuckPageType type) const
		{
			auto foundPage = this->pages.find(type);
			if (foundPage == this->pages.end())
				return nullptr;

			// dynamic_pointer_cast with template type is C++20
			return dynamic_cast<T*>(foundPage->second.get());
		}
};
