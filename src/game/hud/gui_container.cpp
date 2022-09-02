#include "gui_container.hpp"

void GuiContainer::handleMouseMove(int x, int y)
{
	// need to account for this component's position
	this->hoverMgr.handleMouseMove(x - static_cast<int>(this->getPosition().x),
								   y - static_cast<int>(this->getPosition().y));
}
