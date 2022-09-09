#include <SFML/Graphics.hpp>
#include <SFML/Audio.hpp>
#include <vector>
#include "hud.hpp"
#include "hover_manager.hpp"
#include "buttons/button.hpp"
#include "../campaigns/campaign.hpp"
#include "pipbuck/pipbuck.hpp"

class MainMenu : public sf::Drawable, public sf::Transformable
{
	private:
		HoverManager hoverMgr;
		sf::Sound soundBtn;
		std::vector<Button> buttons;

	public:
		MainMenu(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, sf::RenderWindow &window, Campaign &campaign, GameState &gameState, PipBuck &pipBuck);
		ClickStatus handleLeftClick(int x, int y);
		void handleMouseMove(int x, int y);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
