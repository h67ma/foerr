#pragma once

#include <vector>
#include <SFML/Graphics.hpp>
#include "../../resource_manager.hpp"
#include "../hud.hpp"
#include "../buttons/simple_button.hpp"
#include "../hover_manager.hpp"
#include "pipbuck_page.hpp"
#include "../../campaigns/campaign.hpp"
#include "../../sound_resource.hpp"

/**
 * Represents a single PipBuck category (e.g. "Status").
 * Contains 5 pages.
 */
class PipBuckCategory : public sf::Drawable, public sf::Transformable
{
	private:
		HoverManager hoverMgr;
		uint selectedPage = 0;
		std::vector<SimpleButton> pageButtons;
		SoundResource soundPageChange;
		SoundResource soundClick;
		void changePage(uint idx);

	protected:
		std::vector<std::shared_ptr<PipBuckPage>> pages;

	public:
		PipBuckCategory(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr);
		void setup();
		ClickStatus handleLeftClick(int x, int y);
		bool handleMouseMove(int x, int y);
		bool setupCampaignInfos();
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
