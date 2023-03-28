#pragma once

#include <string>

#include "../../../resources/resource_manager.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Information -> Enemies
 */
class PipBuckPageEnemies : public PipBuckPage
{
	public:
		explicit PipBuckPageEnemies(ResourceManager &resMgr);
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
