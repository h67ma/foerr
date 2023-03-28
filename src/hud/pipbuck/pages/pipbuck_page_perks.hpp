#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Status -> Perks
 */
class PipBuckPagePerks : public PipBuckPage
{
	public:
		explicit PipBuckPagePerks(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
