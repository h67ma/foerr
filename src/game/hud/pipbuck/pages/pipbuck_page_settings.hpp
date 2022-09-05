#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"

/**
 * PipBuck -> Main Menu -> Settings
 */
class PipBuckPageSettings : public PipBuckPage
{
	public:
		PipBuckPageSettings(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
