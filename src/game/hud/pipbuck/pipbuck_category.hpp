#include <SFML/Graphics.hpp>
#include "../../resource_manager.hpp"
#include "../hud.hpp"
#include "../button.hpp"

/**
 * Represents a single PipBuck category page (e.g. "Status").
 * Contains 5 pages.
 */
class PipBuckCategoryPage : public sf::Drawable, public sf::Transformable
{
	private:
		Button page1Btn;
		Button page2Btn;
		Button page3Btn;
		Button page4Btn;
		Button page5Btn;
		sf::Text dummyText; // TODO replace with actual pages & switch between them like the same as pipbuck-category

	public:
		PipBuckCategoryPage(GuiScale scale, sf::Color hudColor, ResourceManager& resMgr, std::string dummyText);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
