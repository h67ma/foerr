#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Inventory -> Armor
 */
class PipBuckPageArmor : public PipBuckPage
{
	public:
		explicit PipBuckPageArmor(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
