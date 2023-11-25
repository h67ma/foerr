// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#include "pipbuck_page_controls.hpp"

#include <string>

#include "../../../settings/keymap.hpp"
#include "../../../util/i18n.hpp"
#include "../../log.hpp"

PipBuckPageControls::PipBuckPageControls(ResourceManager& resMgr) :
	PipBuckPage("Controls"), // TODO translate
	buttons({ { BTN_NORMAL, resMgr, { 400, 815 }, STR_SAVE, []() { Keymap::save(); } },
			  { BTN_NORMAL,
				resMgr,
				{ 600, 815 },
				STR_RESET_DEFAULT,
				[this]()
				{
					// TODO display confirm dialog
					Keymap::resetToDefault();
					this->updateDisplay();
					Log::i(STR_KEYMAP_RESETTED);
					Keymap::save();
				} } }),
	dummyMapDump(*resMgr.getFont(FONT_NORMAL), 17U, { 400.F, 250.F })
{
	for (auto& btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->updateDisplay();
}

void PipBuckPageControls::updateDisplay()
{
	// for now only dump the current mapping as text. TODO proper scrollable table
	std::string dump("");

	for (const auto item : Keymap::getKeyToActionMap())
	{
		dump += Keymap::actionToDisplayString(item.second) + " -> " + Keymap::keyToString(item.first) + '\n';
	}

	this->dummyMapDump.setString(dump);
}

void PipBuckPageControls::handleSettingsChange()
{
	for (auto& btn : this->buttons)
	{
		btn.handleSettingsChange();
	}

	this->dummyMapDump.handleSettingsChange();
}

void PipBuckPageControls::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	for (const auto& btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->dummyMapDump, states);
}
