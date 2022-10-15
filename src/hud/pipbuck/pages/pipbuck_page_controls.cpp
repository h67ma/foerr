#include "pipbuck_page_controls.hpp"
#include "../../../util/i18n.hpp"
#include "../../log.hpp"
#include "../../../settings/keymap.hpp"

PipBuckPageControls::PipBuckPageControls(ResourceManager &resMgr, GuiScale scale, sf::Color hudColor) :
	buttons({
		{scale, BTN_NORMAL, hudColor, resMgr, { 400, 815 }, STR_SAVE, []() {
			Keymap::save();
		}},
		{scale, BTN_NORMAL, hudColor, resMgr, { 600, 815 }, STR_RESET_DEFAULT, [this]() {
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
		this->hoverMgr.addHoverable(&btn);
	}

	this->dummyMapDump.setFont(*resMgr.getFont(FONT_NORMAL));
	this->dummyMapDump.setCharacterSize(17);
	this->dummyMapDump.setPosition(400.f, 250.f);
	this->updateDisplay();
}

void PipBuckPageControls::updateDisplay()
{
	// for now only dump the current mapping as text. TODO proper scrollable table
	std::string dump("");

	for (const auto item : Keymap::getkeyToActionMap())
	{
		dump += Keymap::actionToDisplayString(item.second) + " -> " + Keymap::keyToString(item.first) + '\n';
	}

	this->dummyMapDump.setString(dump);
}

ClickStatus PipBuckPageControls::handleLeftClick(int x, int y)
{
	for (auto &btn : this->buttons)
	{
		if (btn.handleLeftClick(x, y) != CLICK_NOT_CONSUMED)
			return CLICK_CONSUMED;
	}

	return CLICK_NOT_CONSUMED;
}

std::string PipBuckPageControls::getLabel()
{
	return "Controls"; // TODO translate
}

void PipBuckPageControls::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	for (auto &btn : this->buttons)
	{
		target.draw(btn, states);
	}

	target.draw(this->dummyMapDump, states);
}
