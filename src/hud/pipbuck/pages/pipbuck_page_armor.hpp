#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Inventory -> Armor
 */
class PipBuckPageArmor : public PipBuckPage
{
	public:
		explicit PipBuckPageArmor(ResourceManager &resMgr);
		std::string getLabel() const override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
