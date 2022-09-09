#include "pipbuck.hpp"
#include "../util/i18n.hpp"
#include "../log.hpp"
#include "categories/pipbuck_cat_status.hpp"
#include "categories/pipbuck_cat_inventory.hpp"
#include "categories/pipbuck_cat_info.hpp"
#include "categories/pipbuck_cat_main.hpp"

PipBuck::PipBuck(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign, GameState &gameState, SettingsManager &settings) :
	gameState(gameState),
	categories { // order matters
		PipBuckCategoryStatus(scale, hudColor, fxVolume, resMgr),
		PipBuckCategoryInventory(scale, hudColor, fxVolume, resMgr),
		PipBuckCategoryInfo(scale, hudColor, fxVolume, resMgr, campaign),
		PipBuckCategoryMain(scale, hudColor, fxVolume, resMgr, campaign, gameState, settings)
	},
	categoryButtons { // order matters
		SimpleButton(scale, BTN_BIG, hudColor, resMgr, 650, 900, STR_PIPBUCK_STATUS),
		SimpleButton(scale, BTN_BIG, hudColor, resMgr, 855, 915, STR_PIPBUCK_INV),
		SimpleButton(scale, BTN_BIG, hudColor, resMgr, 1055, 900, STR_PIPBUCK_INFO),
		SimpleButton(scale, BTN_BIG, hudColor, resMgr, 55, 700, STR_PIPBUCK_MAINMENU)
	},
	closeBtn(scale, BTN_BIG, hudColor, resMgr, 55, 800, STR_PIPBUCK_CLOSE, [this](){
		this->close();
	})
{
	this->pipBuckSprite.setTexture(*resMgr.getTexture(PATH_TXT_PIPBUCK_OVERLAY, true));

	this->soundOpenClose.setBuffer(*resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_OPENCLOSE, true));
	this->soundCategoryBtn.setBuffer(*resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGECHANGE, true));
	this->soundOpenClose.setVolume(static_cast<float>(fxVolume));
	this->soundCategoryBtn.setVolume(static_cast<float>(fxVolume));

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

ClickStatus PipBuck::handleLeftClick(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	ClickStatus catResult = this->categories[this->selectedCategory].handleLeftClick(x, y);
	if (catResult == CLICK_CONSUMED)
		return CLICK_CONSUMED;
	else if(catResult == CLICK_CONSUMED_CLOSE)
	{
		this->close();
		return CLICK_CONSUMED;
	}

	for (auto it = this->categoryButtons.begin(); it != this->categoryButtons.end(); it++)
	{
		if (it->containsPoint(x, y))
		{
			uint idx = static_cast<uint>(std::distance(this->categoryButtons.begin(), it));
			if (idx != this->selectedCategory)
			{
				this->changeCategory(idx);
				this->soundCategoryBtn.play();
			}
			return CLICK_CONSUMED;
		}
	}

	return this->closeBtn.handleLeftClick(x, y);
}

void PipBuck::handleMouseMove(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	if (this->categories[this->selectedCategory].handleMouseMove(x, y))
		return; // hover "consumed"

	this->hoverMgr.handleMouseMove(x, y);
}

/**
 * Setups the PipBuck pages according to campaign data.
 * Locations in the campaign should be already loaded.
 *
 * @return true if setup was successful
 * @return false if there was an error during setup
 */
bool PipBuck::setupCampaignInfos()
{
	for (auto &cat : this->categories)
	{
		if (!cat.setupCampaignInfos())
			return false;
	}

	return true;
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
