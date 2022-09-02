#include "pipbuck.hpp"
#include "../util/i18n.hpp"
#include "../log.hpp"

PipBuck::PipBuck(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, GameState &gameState) :
	categoryPages({
		{ PIPB_CAT_STATUS, PipBuckCategoryPage(scale, hudColor, resMgr, "STATUS") },
		{ PIPB_CAT_INV, PipBuckCategoryPage(scale, hudColor, resMgr, "INVENTORY") },
		{ PIPB_CAT_INFO, PipBuckCategoryPage(scale, hudColor, resMgr, "INFORMATION") },
		{ PIPB_CAT_MAINMENU, PipBuckCategoryPage(scale, hudColor, resMgr, "MAIN MENU") }
	}),
	categoryButtons({
		{ PIPB_CAT_STATUS, Button(scale, BTN_BIG, hudColor, resMgr, 650, 900, STR_PIPBUCK_STATUS) },
		{ PIPB_CAT_INV, Button(scale, BTN_BIG, hudColor, resMgr, 855, 915, STR_PIPBUCK_INV) },
		{ PIPB_CAT_INFO, Button(scale, BTN_BIG, hudColor, resMgr, 1055, 900, STR_PIPBUCK_INFO) },
		{ PIPB_CAT_MAINMENU, Button(scale, BTN_BIG, hudColor, resMgr, 55, 700, STR_PIPBUCK_MAINMENU) }
	}),
	closeBtn(scale, BTN_BIG, hudColor, resMgr, 55, 800, STR_PIPBUCK_CLOSE, [&gameState](){
		gameState = STATE_PLAYING;
		Log::d(STR_GAME_RESUMED);
	})
{
	this->pipBuckSprite.setTexture(*resMgr.getTexture(PATH_PIPBUCK_OVERLAY));

	for (auto &btn : this->categoryButtons)
	{
		this->hoverMgr.addHoverable(&btn.second);
	}
	this->hoverMgr.addHoverable(&this->closeBtn);

	this->changeCategory(PIPB_CAT_STATUS); // default category
}

void PipBuck::handleScreenResize(uint screenW, uint screenH)
{
	// for now copy the behaviour of Remains pipbuck, i.e. display it unscaled in bottom left corner
	// TODO scale should affect sprite size and individual buttons/labels/etc scale and placement
	this->setPosition(0, static_cast<float>(screenH - this->pipBuckSprite.getLocalBounds().height));
}

void PipBuck::changeCategory(PipBuckCategory cat)
{
	// note: need to use ::at, otherwise we'd need to have a default constructor in Button
	this->categoryButtons.at(selectedCategory).setSelected(false);
	this->categoryButtons.at(cat).setSelected(true);

	selectedCategory = cat;
}

void PipBuck::handleLeftClick(int x, int y)
{
	// need to account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	for (auto &btn : this->categoryButtons)
	{
		if (btn.second.containsPoint(x, y))
		{
			this->changeCategory(btn.first);
			return; // click consumed
		}
	}

	this->closeBtn.maybeHandleLeftClick(x, y);
}

void PipBuck::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->pipBuckSprite, states);

	// note: need to use ::at, otherwise we'd need to have a default constructor in PipBuckCategoryPage
	target.draw(this->categoryPages.at(this->selectedCategory), states);

	for (const auto &btn : this->categoryButtons)
	{
		target.draw(btn.second, states);
	}

	target.draw(this->closeBtn, states);
}
