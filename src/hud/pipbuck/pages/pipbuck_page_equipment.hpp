#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Inventory -> Equipment
 */
class PipBuckPageEquipment : public PipBuckPage
{
	public:
		explicit PipBuckPageEquipment(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
