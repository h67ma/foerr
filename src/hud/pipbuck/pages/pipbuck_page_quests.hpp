#pragma once

#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Information -> Quests
 */
class PipBuckPageQuests : public PipBuckPage
{
	public:
		PipBuckPageQuests(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
