#pragma once

#include "../pipbuck_page.hpp"

#include "../../../resources/resource_manager.hpp"
#include <string>

/**
 * PipBuck -> Information -> Map
 */
class PipBuckPageMap : public PipBuckPage
{
	public:
		explicit PipBuckPageMap(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
