#include <SFML/Graphics.hpp>
#include <vector>
#include "hud.hpp"
#include "hover_manager.hpp"
#include "buttons/simple_button.hpp"
#include "../campaigns/campaign.hpp"
#include "pipbuck/pipbuck.hpp"
#include "../resources/sound_resource.hpp"

class MainMenu : public sf::Drawable, public sf::Transformable
{
	private:
		HoverManager hoverMgr;
		SoundResource btnSound;
		std::vector<SimpleButton> buttons;

	public:
		MainMenu(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, sf::RenderWindow &window, Campaign &campaign, GameState &gameState, PipBuck &pipBuck);
		ClickStatus handleLeftClick(int x, int y);
		void handleMouseMove(int x, int y);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
