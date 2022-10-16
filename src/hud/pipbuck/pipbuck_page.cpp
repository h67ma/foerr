#include "pipbuck_page.hpp"

ClickStatus PipBuckPage::handleLeftClick(int x, int y)
{
	return this->clickMgr.handleLeftClick(x, y, this->getPosition());
}

bool PipBuckPage::handleMouseMove(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	return this->hoverMgr.handleMouseMove(x, y);
}
