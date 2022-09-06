#include "../pipbuck_category.hpp"

class PipBuckCategoryInventory : public PipBuckCategory
{
	public:
		PipBuckCategoryInventory(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr);
};
