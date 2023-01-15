#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Text.hpp>
#include "../resources/resource_manager.hpp"

class LoadingScreen : public sf::Drawable
{
	private:
		sf::Text loadingText;

	public:
		LoadingScreen(ResourceManager &resMgr, sf::Vector2u windowSize);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
