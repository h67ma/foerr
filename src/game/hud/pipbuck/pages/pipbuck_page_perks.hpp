#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Status -> Perks
 */
class PipBuckPagePerks : public PipBuckPage
{
	public:
		PipBuckPagePerks(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
