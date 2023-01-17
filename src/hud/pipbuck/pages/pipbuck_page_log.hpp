#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Main Menu -> Log
 */
class PipBuckPageLog : public PipBuckPage
{
	public:
		explicit PipBuckPageLog(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
