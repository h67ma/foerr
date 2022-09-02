#include "pipbuck_category.hpp"

PipBuckCategoryPage::PipBuckCategoryPage(GuiScale scale, sf::Color hudColor, ResourceManager& resMgr, std::string dummyText) :
	page1Btn(scale, BTN_NARROW, hudColor, *resMgr.getFont(FONT_MEDIUM), 385, 210, "kappa1"),
	page2Btn(scale, BTN_NARROW, hudColor, *resMgr.getFont(FONT_MEDIUM), 525, 210, "kappa2"),
	page3Btn(scale, BTN_NARROW, hudColor, *resMgr.getFont(FONT_MEDIUM), 665, 210, "kappa3"),
	page4Btn(scale, BTN_NARROW, hudColor, *resMgr.getFont(FONT_MEDIUM), 805, 210, "kappa4"),
	page5Btn(scale, BTN_NARROW, hudColor, *resMgr.getFont(FONT_MEDIUM), 945, 210, "kappa5")
{
	this->dummyText.setFont(*resMgr.getFont(FONT_NORMAL));
	this->dummyText.setString(dummyText);
	this->dummyText.setPosition(800, 500);
}

void PipBuckCategoryPage::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->page1Btn, states);
	target.draw(this->page2Btn, states);
	target.draw(this->page3Btn, states);
	target.draw(this->page4Btn, states);
	target.draw(this->page5Btn, states);

	target.draw(this->dummyText, states);
}
