#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Information -> Notes
 */
class PipBuckPageNotes : public PipBuckPage
{
	public:
		explicit PipBuckPageNotes(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
