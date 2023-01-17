#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Inventory -> Ammunition
 */
class PipBuckPageAmmo : public PipBuckPage
{
	public:
		explicit PipBuckPageAmmo(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
