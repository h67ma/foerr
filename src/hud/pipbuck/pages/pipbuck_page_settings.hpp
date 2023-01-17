#pragma once

#include <string>
#include <vector>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"
#include "../../buttons/simple_button.hpp"

/**
 * PipBuck -> Main Menu -> Settings
 * TODO or Main Menu -> Settings
 */
class PipBuckPageSettings : public PipBuckPage
{
	private:
		std::vector<SimpleButton> buttons;
		sf::Text infoText;

	public:
		explicit PipBuckPageSettings(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
