// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <vector>

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>

#include "../campaigns/campaign.hpp"
#include "../resources/sound_resource.hpp"
#include "../window/cursor_manager.hpp"
#include "buttons/simple_button.hpp"
#include "click_manager.hpp"
#include "configurable_gui_component.hpp"
#include "hover_manager.hpp"
#include "pipbuck/pipbuck.hpp"
#include "text_label.hpp"

class MainMenu : public sf::Drawable, public sf::Transformable, public ConfigurableGuiComponent
{
	private:
		HoverManager hoverMgr;
		ClickManager clickMgr;
		SoundResource btnSound;
		std::vector<SimpleButton> buttons;
		TextLabel versionText;
		TextLabel licenseText;

	public:
		MainMenu(ResourceManager& resMgr, CursorManager& cursorMgr, sf::RenderWindow& window, Campaign& campaign,
				 GameState& gameState, PipBuck& pipBuck);
		void handleLeftClick(sf::Vector2i clickPos);
		void handleMouseMove(sf::Vector2i mousePos);
		void handleScreenResize(sf::Vector2u newSize);
		void handleSettingsChange() override;
		void draw(sf::RenderTarget& target, sf::RenderStates states) const override;
};
