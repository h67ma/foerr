#pragma once

#include <string>
#include <vector>

#include "../../../resources/resource_manager.hpp"
#include "../../buttons/simple_button.hpp"
#include "../pipbuck_page.hpp"

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
		void handleSettingsChange() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
