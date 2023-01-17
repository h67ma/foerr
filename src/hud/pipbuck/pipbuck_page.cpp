#include "pipbuck_page.hpp"

PipBuckPage::PipBuckPage(const std::string &pageTitle) : pageTitle(pageTitle) {}

ClickStatus PipBuckPage::handleLeftClick(sf::Vector2i clickPos)
{
	return this->clickMgr.handleLeftClick(clickPos);
}

bool PipBuckPage::handleMouseMove(sf::Vector2i mousePos)
{
	return this->hoverMgr.handleMouseMove(mousePos);
}
