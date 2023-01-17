#pragma once

#include <string>
#include <vector>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"
#include "../../buttons/simple_button.hpp"

/**
 * PipBuck -> Main Menu -> Controls
 * TODO or Main Menu -> Controls
 */
class PipBuckPageControls : public PipBuckPage
{
	private:
		std::vector<SimpleButton> buttons;
		sf::Text dummyMapDump; // TODO delet this
		void updateDisplay();

	public:
		explicit PipBuckPageControls(ResourceManager &resMgr);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
