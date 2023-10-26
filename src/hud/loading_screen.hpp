#pragma once

#include <SFML/Graphics/RenderTarget.hpp>
#include <SFML/Graphics/Drawable.hpp>

#include "../resources/resource_manager.hpp"
#include "text_label.hpp"

class LoadingScreen : public sf::Drawable
{
	private:
		TextLabel loadingText;

	public:
		LoadingScreen(ResourceManager &resMgr, sf::Vector2u windowSize);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
