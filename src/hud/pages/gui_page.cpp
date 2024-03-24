// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include "gui_page.hpp"

GuiPage::GuiPage(const std::string& pageTitle) : pageTitle(pageTitle) {}

void GuiPage::handleSettingsChange()
{
	this->handleGuiScaleChange();
}
