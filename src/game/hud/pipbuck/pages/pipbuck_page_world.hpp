#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"
#include "../../../sprite_resource.hpp"
#include "../../buttons/simple_button.hpp"
#include "../../buttons/location_button.hpp"
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
		SpriteResource mapBg;
		sf::VertexArray mapBorder = sf::VertexArray(sf::LineStrip, 5);
		sf::VertexArray mapGridLines = sf::VertexArray(sf::Lines, 16); // max 4 horizontal, 4 vertical
		sf::Text locTitle;
		sf::Text locDescription;
		std::vector<LocButton> mapButtons;
		SimpleButton gotoLocationBtn;
		HoverManager mapButtonHoverMgr;
		int selectedLocationIdx = NO_LOCATION_SELECTED; // note: different from the one in Campaign (only represents selection on page)
		void setupMapDecorations();

	public:
		PipBuckPageWorld(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign);
		ClickStatus handleLeftClick(int x, int y) override;
		bool handleMouseMove(int x, int y) override;
		std::string getLabel() override;
		bool setupCampaignInfos() override;
		void unloadCampaignInfos() override;
		void setGuiScale(GuiScale scale);
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
