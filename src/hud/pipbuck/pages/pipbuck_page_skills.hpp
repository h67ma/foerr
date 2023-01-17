#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Status -> Skills
 */
class PipBuckPageSkills : public PipBuckPage
{
	public:
		explicit PipBuckPageSkills(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
