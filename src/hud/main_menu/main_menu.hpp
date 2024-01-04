// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#pragma once

#include <unordered_map>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../../campaigns/campaign.hpp"
#include "../../resources/sound_resource.hpp"
#include "../../window/cursor_manager.hpp"
#include "../buttons/simple_button.hpp"
#include "../configurable_gui_component.hpp"
#include "../hover_manager.hpp"
#include "../pages/gui_page.hpp"
#include "../pipbuck/pipbuck.hpp"
#include "../text_label.hpp"
#include "main_menu_page.hpp"

/**
 * Main Menu reuses PipBuck pages. They are displayed similarly to how it is done in PipBuckCategory.
 * Some menu items (buttons) don't have an associated page - instead they have a callback (makes sense e.g. for "quit",
 * or "continue").
 */
class MainMenu : public sf::Drawable, public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		HoverManager hoverMgr;
		SoundResource btnSound;
		MainMenuPageType selectedPageType = MAINM_PAGE_NONE;
		std::shared_ptr<PipBuckPage> selectedPage = nullptr;
		std::unordered_map<MainMenuPageType, SimpleButton> buttons;
		const std::unordered_map<MainMenuPageType, std::shared_ptr<PipBuckPage>> pages;
		TextLabel versionText;
		TextLabel licenseText;
		sf::VertexArray pageFrame;

		void changeActiveButton(MainMenuPageType newPageType);
		void changePage(MainMenuPageType newPageType);
		void setupPageFrameStatic();
		void setupPageFrameSelectedItem(float selectedItemOffset);

	public:
		MainMenu(ResourceManager& resMgr, CursorManager& cursorMgr, sf::RenderWindow& window, Campaign& campaign,
				 GameState& gameState, PipBuck& pipBuck);
		ClickStatus handleLeftClick(sf::Vector2i clickPos);
		void handleLeftClickUp();
		void handleMouseMove(sf::Vector2i mousePos);
		void handleScreenResize(sf::Vector2u newSize);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
