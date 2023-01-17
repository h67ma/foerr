#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Status -> Main
 */
class PipBuckPageMainStatus : public PipBuckPage
{
	public:
		explicit PipBuckPageMainStatus(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
