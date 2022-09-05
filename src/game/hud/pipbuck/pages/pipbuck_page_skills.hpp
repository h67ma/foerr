#include <SFML/Graphics.hpp>
#include "../pipbuck_page.hpp"
#include "../../../resource_manager.hpp"

/**
 * PipBuck -> Status -> Skills
 */
class PipBuckPageSkills : public PipBuckPage
{
	public:
		PipBuckPageSkills(ResourceManager &resMgr);
		std::string getLabel() override;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
