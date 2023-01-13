#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Information -> Enemies
 */
class PipBuckPageEnemies : public PipBuckPage
{
	public:
		explicit PipBuckPageEnemies(ResourceManager &resMgr);
		std::string getLabel() const override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
