#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"
#include "../../../settings/settings_manager.hpp"
#include "../../buttons/simple_button.hpp"

/**
 * PipBuck -> Main Menu -> Settings
 * TODO or Main Menu -> Settings
 */
class PipBuckPageSettings : public PipBuckPage
{
	private:
		std::vector<SimpleButton> buttons;

	public:
		PipBuckPageSettings(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, SettingsManager &settings);
		ClickStatus handleLeftClick(int x, int y) override;
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
