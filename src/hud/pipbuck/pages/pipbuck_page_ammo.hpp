#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Inventory -> Ammunition
 */
class PipBuckPageAmmo : public PipBuckPage
{
	public:
		explicit PipBuckPageAmmo(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
