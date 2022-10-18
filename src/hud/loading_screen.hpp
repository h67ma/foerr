#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../consts.hpp"
#include "../resources/resource_manager.hpp"
#include "hud.hpp"

class LoadingScreen : public sf::Drawable
{
	private:
		sf::Text loadingText;

	public:
		LoadingScreen(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, uint screenW, uint screenH);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
