#include "pipbuck_page.hpp"

ClickStatus PipBuckPage::handleLeftClick(int x, int y)
{
	return this->clickMgr.handleLeftClick(x, y, this->getPosition());
}

bool PipBuckPage::handleMouseMove(int x, int y)
{
	return this->hoverMgr.handleMouseMove(x, y, this->getPosition());
}
