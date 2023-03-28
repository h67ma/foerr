#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Information -> Quests
 */
class PipBuckPageQuests : public PipBuckPage
{
	public:
		explicit PipBuckPageQuests(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
