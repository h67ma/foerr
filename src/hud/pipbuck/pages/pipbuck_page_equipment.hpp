#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Inventory -> Equipment
 */
class PipBuckPageEquipment : public PipBuckPage
{
	public:
		PipBuckPageEquipment(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
