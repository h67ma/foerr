#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Inventory -> Weapons
 */
class PipBuckPageWeapons : public PipBuckPage
{
	public:
		explicit PipBuckPageWeapons(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
