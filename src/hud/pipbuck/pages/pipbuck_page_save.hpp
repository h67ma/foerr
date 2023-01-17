#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Main Menu -> Save
 */
class PipBuckPageSave : public PipBuckPage
{
	public:
		explicit PipBuckPageSave(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
