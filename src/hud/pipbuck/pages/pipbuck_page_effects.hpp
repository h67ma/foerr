#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Status -> Effects
 */
class PipBuckPageEffects : public PipBuckPage
{
	public:
		explicit PipBuckPageEffects(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
