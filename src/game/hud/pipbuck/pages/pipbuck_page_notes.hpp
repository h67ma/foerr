#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"

/**
 * PipBuck -> Information -> Notes
 */
class PipBuckPageNotes : public PipBuckPage
{
	public:
		PipBuckPageNotes(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};