#include "../pipbuck_category.hpp"

class PipBuckCategoryStatus : public PipBuckCategory
{
	public:
		PipBuckCategoryStatus(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr);
};