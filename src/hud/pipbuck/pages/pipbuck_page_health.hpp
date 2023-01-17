#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Status -> Health
 */
class PipBuckPageHealth : public PipBuckPage
{
	public:
		explicit PipBuckPageHealth(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
