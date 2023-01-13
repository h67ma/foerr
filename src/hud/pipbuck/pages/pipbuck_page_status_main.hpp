#pragma once

#include <string>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Status -> Main
 */
class PipBuckPageMainStatus : public PipBuckPage
{
	public:
		explicit PipBuckPageMainStatus(ResourceManager &resMgr);
		std::string getLabel() const override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
