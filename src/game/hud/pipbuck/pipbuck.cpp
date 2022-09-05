#include "pipbuck.hpp"
#include "../util/i18n.hpp"
#include "../log.hpp"
#include "categories/pipbuck_cat_status.hpp"
#include "categories/pipbuck_cat_inventory.hpp"
#include "categories/pipbuck_cat_info.hpp"
#include "categories/pipbuck_cat_main.hpp"

PipBuck::PipBuck(GuiScale scale, sf::Color hudColor, ResourceManager &resMgr, GameState &gameState) :
	gameState(gameState),
	categories { // order matters
		PipBuckCategoryStatus(scale, hudColor, resMgr),
		PipBuckCategoryInventory(scale, hudColor, resMgr),
		PipBuckCategoryInfo(scale, hudColor, resMgr),
		PipBuckCategoryMain(scale, hudColor, resMgr)
	},
	categoryButtons { // order matters
		Button(scale, BTN_BIG, hudColor, resMgr, 650, 900, STR_PIPBUCK_STATUS),
		Button(scale, BTN_BIG, hudColor, resMgr, 855, 915, STR_PIPBUCK_INV),
		Button(scale, BTN_BIG, hudColor, resMgr, 1055, 900, STR_PIPBUCK_INFO),
		Button(scale, BTN_BIG, hudColor, resMgr, 55, 700, STR_PIPBUCK_MAINMENU)
	},
	closeBtn(scale, BTN_BIG, hudColor, resMgr, 55, 800, STR_PIPBUCK_CLOSE, [this](){
		this->close();
	})
{
	this->pipBuckSprite.setTexture(*resMgr.getTexture(PATH_TXT_PIPBUCK_OVERLAY));
	this->soundOpenClose.setBuffer(*resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_OPENCLOSE));

	for (auto &cat : this->categories)
	{
		cat.setup();
	}

	for (auto &btn : this->categoryButtons)
	{
		this->hoverMgr.addHoverable(&btn);
	}
	this->hoverMgr.addHoverable(&this->closeBtn);

	this->changeCategory(this->selectedCategory); // default category
}

void PipBuck::handleScreenResize(uint screenW, uint screenH)
{
	// for now copy the behaviour of Remains pipbuck, i.e. display it unscaled in bottom left corner
	// TODO scale should affect sprite size and individual buttons/labels/etc scale and placement
	this->setPosition(0, static_cast<float>(screenH - this->pipBuckSprite.getLocalBounds().height));
}

/**
 * Opens PipBuck by changing game state and plays the open/close sound.
 */
void PipBuck::open(bool sound)
{
	this->gameState = STATE_PIPBUCK;
	if (sound)
		this->soundOpenClose.play();
	Log::d(STR_GAME_PAUSED);
}

/**
 * Closes PipBuck by changing game state and plays the open/close sound.
 */
void PipBuck::close()
{
	this->gameState = STATE_PLAYING;
	this->soundOpenClose.play();
	Log::d(STR_GAME_RESUMED);
}

void PipBuck::changeCategory(uint idx)
{
	this->categoryButtons[this->selectedCategory].setSelected(false);
	this->categoryButtons[idx].setSelected(true);

	this->selectedCategory = idx;
}

void PipBuck::handleLeftClick(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	if (this->categories[this->selectedCategory].handleLeftClick(x, y))
		return; // click consumed

	for (auto it = this->categoryButtons.begin(); it != this->categoryButtons.end(); it++)
	{
		if (it->containsPoint(x, y))
		{
			uint idx = static_cast<uint>(std::distance(this->categoryButtons.begin(), it));
			this->changeCategory(idx);
			return; // click consumed
		}
	}

	this->closeBtn.maybeHandleLeftClick(x, y);
}

void PipBuck::handleMouseMove(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	// TODO if one of them "consumed" hover then no need to call the other one
	this->categories[this->selectedCategory].handleMouseMove(x, y);
	this->hoverMgr.handleMouseMove(x, y);
}

void PipBuck::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->pipBuckSprite, states);

	target.draw(this->categories[this->selectedCategory], states);

	for (const auto &btn : this->categoryButtons)
	{
		target.draw(btn, states);
	}

	target.draw(this->closeBtn, states);
}
