#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Information -> Quests
 */
class PipBuckPageQuests : public PipBuckPage
{
	public:
		explicit PipBuckPageQuests(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
