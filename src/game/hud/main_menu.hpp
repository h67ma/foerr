#include <SFML/Graphics.hpp>
#include <vector>
#include "hud.hpp"
#include "hover_manager.hpp"
#include "button.hpp"
#include "../campaigns/campaign.hpp"

class MainMenu : public sf::Drawable, public sf::Transformable
{
	private:
		HoverManager hoverMgr;
		std::vector<Button> buttons;

	public:
		MainMenu(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, sf::RenderWindow &window, Campaign &campaign, GameState &gameState);
		ClickStatus handleLeftClick(int x, int y);
		void handleMouseMove(int x, int y);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
