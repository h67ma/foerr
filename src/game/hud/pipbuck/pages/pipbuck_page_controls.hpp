#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"

/**
 * PipBuck -> Main Menu -> Controls
 * TODO or Main Menu -> Controls
 */
class PipBuckPageControls : public PipBuckPage
{
	public:
		PipBuckPageControls(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
