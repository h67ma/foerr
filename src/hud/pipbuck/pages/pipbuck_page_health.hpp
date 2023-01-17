#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Status -> Health
 */
class PipBuckPageHealth : public PipBuckPage
{
	public:
		explicit PipBuckPageHealth(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
