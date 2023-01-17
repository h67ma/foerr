#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Status -> Perks
 */
class PipBuckPagePerks : public PipBuckPage
{
	public:
		explicit PipBuckPagePerks(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
