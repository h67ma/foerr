#include "pipbuck_page_world.hpp"

PipBuckPageWorld::PipBuckPageWorld(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, Campaign &campaign) :
	buttons({
		Button(scale, BTN_NORMAL, hudColor, resMgr, 385, 260, "goto loc 1", [&campaign]() {
			campaign.changeLocation("technical_tunnels");
		}),
		Button(scale, BTN_NORMAL, hudColor, resMgr, 385, 300, "goto loc 2", [&campaign]() {
			campaign.changeLocation("surface");
		})
	})
{
	for (auto &btn : this->buttons)
	{
		this->hoverMgr.addHoverable(&btn);
	}
}

ClickStatus PipBuckPageWorld::handleLeftClick(int x, int y)
{
	for (auto &btn : this->buttons)
	{
		if (btn.handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
			return CLICK_CONSUMED_CLOSE;
	}

	return CLICK_NOT_CONSUMED;
}

std::string PipBuckPageWorld::getLabel()
{
	return "World"; // TODO translate
}

void PipBuckPageWorld::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}
}
