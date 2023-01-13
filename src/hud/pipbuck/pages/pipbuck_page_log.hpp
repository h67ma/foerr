#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Main Menu -> Log
 */
class PipBuckPageLog : public PipBuckPage
{
	public:
		explicit PipBuckPageLog(ResourceManager &resMgr);
		std::string getLabel() const override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
