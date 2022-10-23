#pragma once

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
		PipBuckPageControls(ResourceManager &resMgr, GuiScale scale, sf::Color hudColor);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
