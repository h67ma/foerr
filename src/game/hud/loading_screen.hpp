#include <SFML/Graphics.hpp>
#include "../consts.hpp"
#include "../resource_manager.hpp"
#include "hud.hpp"

class LoadingScreen : public sf::Drawable
{
	private:
		sf::Text loadingText;

	public:
		LoadingScreen(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, uint screenW, uint screenH);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
