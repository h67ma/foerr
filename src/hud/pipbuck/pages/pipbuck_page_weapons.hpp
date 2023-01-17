#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Inventory -> Weapons
 */
class PipBuckPageWeapons : public PipBuckPage
{
	public:
		explicit PipBuckPageWeapons(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
