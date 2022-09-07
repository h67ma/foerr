#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"
#include "../../button.hpp"
#include "../../../campaigns/campaign.hpp"

#define NO_LOCATION_SELECTED -1

/**
 * PipBuck -> Information -> World
 */
class PipBuckPageWorld : public PipBuckPage
{
	private:
		ResourceManager &resMgr;
		Campaign &campaign;
		GuiScale guiScale;
		sf::Color hudColor;
		sf::Sprite mapBg;
		std::vector<Button> mapButtons;
		std::vector<std::string> locationIds;
		Button gotoLocationBtn;
		HoverManager mapButtonHoverMgr;
		int selectedLocationIdx = NO_LOCATION_SELECTED;

	public:
		PipBuckPageWorld(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign);
		ClickStatus handleLeftClick(int x, int y) override;
		bool handleMouseMove(int x, int y) override;
		std::string getLabel() override;
		bool setupCampaignInfos() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
