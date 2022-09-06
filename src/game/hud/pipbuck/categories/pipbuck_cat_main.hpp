#include "../pipbuck_category.hpp"
#include "../../../campaigns/campaign.hpp"

class PipBuckCategoryMain : public PipBuckCategory
{
	public:
		PipBuckCategoryMain(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign, GameState &gameState);
};
