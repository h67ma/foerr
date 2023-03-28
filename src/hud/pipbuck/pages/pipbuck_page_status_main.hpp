#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Status -> Main
 */
class PipBuckPageMainStatus : public PipBuckPage
{
	public:
		explicit PipBuckPageMainStatus(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
