#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Status -> Effects
 */
class PipBuckPageEffects : public PipBuckPage
{
	public:
		explicit PipBuckPageEffects(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
