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
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
