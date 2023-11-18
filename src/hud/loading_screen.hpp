// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

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
