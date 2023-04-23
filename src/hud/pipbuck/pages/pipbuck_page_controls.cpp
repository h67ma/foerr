#include "pipbuck_page_controls.hpp"

#include <string>

#include "../../../settings/keymap.hpp"
#include "../../../util/i18n.hpp"
#include "../../log.hpp"

PipBuckPageControls::PipBuckPageControls(ResourceManager &resMgr) :
	PipBuckPage("Controls"), // TODO translate
	buttons({
		{ BTN_NORMAL, resMgr, { 400, 815 }, STR_SAVE, []() {
			Keymap::save();
		}},
		{ BTN_NORMAL, resMgr, { 600, 815 }, STR_RESET_DEFAULT, [this]() {
			// TODO display confirm dialog
			Keymap::resetToDefault();
			this->updateDisplay();
			Log::i(STR_KEYMAP_RESETTED);
			Keymap::save();
		}}
	})
{
	for (auto &btn : this->buttons)
	{
		this->hoverMgr += &btn;
		this->clickMgr += &btn;
	}

	this->dummyMapDump.setFont(*resMgr.getFont(FONT_NORMAL));
	this->dummyMapDump.setCharacterSize(17);
	this->dummyMapDump.setPosition(400.F, 250.F);
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
	for (auto &btn : this->buttons)
	{
		btn.handleSettingsChange();
	}

	this->dummyMapDump.handleSettingsChange();
}

void PipBuckPageControls::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (const auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->dummyMapDump, states);
}
