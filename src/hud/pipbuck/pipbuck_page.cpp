// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "pipbuck_page.hpp"

PipBuckPage::PipBuckPage(const std::string& pageTitle) : pageTitle(pageTitle) {}

ClickStatus PipBuckPage::handleLeftClick(sf::Vector2i clickPos)
{
	return this->clickMgr.handleLeftClick(clickPos);
}

bool PipBuckPage::handleMouseMove(sf::Vector2i mousePos)
{
	return this->hoverMgr.handleMouseMove(mousePos);
}
