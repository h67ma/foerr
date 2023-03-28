#pragma once

#include <vector>

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/Text.hpp>

#include "../resources/sound_resource.hpp"
#include "configurable_hud_component.hpp"
#include "buttons/simple_button.hpp"
#include "../campaigns/campaign.hpp"
#include "pipbuck/pipbuck.hpp"
#include "hover_manager.hpp"
#include "click_manager.hpp"

class MainMenu : public sf::Drawable, public sf::Transformable, public ConfigurableHudComponent
{
	private:
		HoverManager hoverMgr;
		ClickManager clickMgr;
		SoundResource btnSound;
		std::vector<SimpleButton> buttons;
		sf::Text versionText;

	public:
		MainMenu(ResourceManager &resMgr, sf::RenderWindow &window, Campaign &campaign, GameState &gameState,
				 PipBuck &pipBuck);
		void handleLeftClick(sf::Vector2i clickPos);
		void handleMouseMove(sf::Vector2i mousePos);
		void handleScreenResize(sf::Vector2u newSize);
		void handleSettingsChange() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
