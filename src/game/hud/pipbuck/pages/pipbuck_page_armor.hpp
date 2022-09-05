#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"

/**
 * PipBuck -> Inventory -> Armor
 */
class PipBuckPageArmor : public PipBuckPage
{
	public:
		PipBuckPageArmor(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
