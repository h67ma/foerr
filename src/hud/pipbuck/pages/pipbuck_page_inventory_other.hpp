#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Inventory -> Other
 */
class PipBuckPageInventoryOther : public PipBuckPage
{
	public:
		explicit PipBuckPageInventoryOther(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
