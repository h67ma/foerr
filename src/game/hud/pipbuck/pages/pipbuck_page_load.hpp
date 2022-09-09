#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"
#include "../../buttons/simple_button.hpp"
#include "../../../campaigns/campaign.hpp"

/**
 * PipBuck -> Main Menu -> Load
 */
class PipBuckPageLoad : public PipBuckPage
{
	private:
		std::vector<SimpleButton> buttons;

	public:
		PipBuckPageLoad(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign, GameState &gameState);
		ClickStatus handleLeftClick(int x, int y) override;
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
