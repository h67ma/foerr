#include "../pipbuck_category.hpp"

class PipBuckCategoryInfo : public PipBuckCategory
{
	public:
		PipBuckCategoryInfo(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr);
};
