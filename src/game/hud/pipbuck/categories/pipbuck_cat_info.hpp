#include "../pipbuck_category.hpp"
#include "../../../campaigns/campaign.hpp"

class PipBuckCategoryInfo : public PipBuckCategory
{
	public:
		PipBuckCategoryInfo(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign);
};
