#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Inventory -> Weapons
 */
class PipBuckPageWeapons : public PipBuckPage
{
	public:
		PipBuckPageWeapons(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
