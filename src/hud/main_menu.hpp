#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/RenderWindow.hpp>
#include <vector>
#include "hud.hpp"
#include "hover_manager.hpp"
#include "click_manager.hpp"
#include "buttons/simple_button.hpp"
#include "../campaigns/campaign.hpp"
#include "pipbuck/pipbuck.hpp"
#include "../resources/sound_resource.hpp"

class MainMenu : public sf::Drawable, public sf::Transformable
{
	private:
		HoverManager hoverMgr;
		ClickManager clickMgr;
		SoundResource btnSound;
		std::vector<SimpleButton> buttons;

	public:
		MainMenu(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, sf::RenderWindow &window, Campaign &campaign, GameState &gameState, PipBuck &pipBuck);
		void handleLeftClick(sf::Vector2i clickPos);
		void handleMouseMove(sf::Vector2i mousePos);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};