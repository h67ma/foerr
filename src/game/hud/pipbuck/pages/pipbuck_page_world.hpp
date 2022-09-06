#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"
#include "../../button.hpp"
#include "../../../campaigns/campaign.hpp"

/**
 * PipBuck -> Information -> World
 */
class PipBuckPageWorld : public PipBuckPage
{
	private:
		std::vector<Button> buttons;

	public:
		PipBuckPageWorld(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign);
		ClickStatus handleLeftClick(int x, int y) override;
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
