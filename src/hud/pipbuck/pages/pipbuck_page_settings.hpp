#pragma once

#include <string>
#include <vector>

#include "../../../resources/resource_manager.hpp"
#include "../../buttons/simple_button.hpp"
#include "../../color_selector.hpp"
#include "../pipbuck_page.hpp"

/**
 * PipBuck -> Main Menu -> Settings
 * TODO or Main Menu -> Settings
 */
class PipBuckPageSettings : public PipBuckPage
{
	private:
		std::vector<SimpleButton> buttons;
		ColorSelector hudColorSelector;
		sf::Text infoText;

	public:
		explicit PipBuckPageSettings(ResourceManager &resMgr);
		ClickStatus handleLeftClick(sf::Vector2i clickPos) override;
		void handleLeftClickUp() override;
		bool handleMouseMove(sf::Vector2i mousePos) override;
		void handleSettingsChange() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
