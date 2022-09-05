#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"

/**
 * PipBuck -> Main Menu -> Campaign
 */
class PipBuckPageCampaign : public PipBuckPage
{
	public:
		PipBuckPageCampaign(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
