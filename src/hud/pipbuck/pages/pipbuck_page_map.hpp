#pragma once

#include "../pipbuck_page.hpp"
#include <string>
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Information -> Map
 */
class PipBuckPageMap : public PipBuckPage
{
	public:
		PipBuckPageMap(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
