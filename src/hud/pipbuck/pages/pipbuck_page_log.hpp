#include "../pipbuck_page.hpp"
#include "../../../resources/resource_manager.hpp"

/**
 * PipBuck -> Main Menu -> Log
 */
class PipBuckPageLog : public PipBuckPage
{
	public:
		PipBuckPageLog(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
