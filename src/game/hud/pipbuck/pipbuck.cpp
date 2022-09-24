#include <math.h>
#include "pipbuck.hpp"
#include "../util/i18n.hpp"
#include "../log.hpp"
#include "categories/pipbuck_cat_status.hpp"
#include "categories/pipbuck_cat_inventory.hpp"
#include "categories/pipbuck_cat_info.hpp"
#include "categories/pipbuck_cat_main.hpp"
#include "../util/util.hpp"

PipBuck::PipBuck(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr, Campaign &campaign, GameState &gameState, SettingsManager &settings) :
	resMgr(resMgr),
	gameState(gameState),
	campaign(campaign),
	categories { // order matters
		PipBuckCategoryStatus(scale, hudColor, fxVolume, resMgr),
		PipBuckCategoryInventory(scale, hudColor, fxVolume, resMgr),
		PipBuckCategoryInfo(scale, hudColor, fxVolume, resMgr, campaign),
		PipBuckCategoryMain(scale, hudColor, fxVolume, resMgr, campaign, gameState, settings)
	},
	categoryButtons { // order matters
		{ scale, BTN_BIG, hudColor, resMgr, { 650, 900 }, STR_PIPBUCK_STATUS },
		{ scale, BTN_BIG, hudColor, resMgr, { 855, 915 }, STR_PIPBUCK_INV },
		{ scale, BTN_BIG, hudColor, resMgr, { 1055, 900 }, STR_PIPBUCK_INFO },
		{ scale, BTN_BIG, hudColor, resMgr, { 55, 700 }, STR_PIPBUCK_MAINMENU }
	},
	closeBtn(scale, BTN_BIG, hudColor, resMgr, { 55, 800 }, STR_PIPBUCK_CLOSE, [this](){
		this->close();
	}),
	soundOpenClose(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_OPENCLOSE)),
	soundCategoryBtn(resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGECHANGE)),
	pipBuckSprite(resMgr.getTexture(PATH_TXT_PIPBUCK_OVERLAY))
{

	this->soundOpenClose.get().setVolume(static_cast<float>(fxVolume));
	this->soundCategoryBtn.get().setVolume(static_cast<float>(fxVolume));

	this->radIndicator.setPointCount(3);
	this->radIndicator.setOrigin(5.f, 0.f);
	this->radIndicator.setPosition(130.f, 190.f);
	this->radIndicator.setPoint(0, { 0.f, 0.f });
	this->radIndicator.setPoint(1, { 10.f, 0.f });
	this->radIndicator.setPoint(2, { 5.f, 60.f });
	this->radIndicator.setFillColor(sf::Color::Black);

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
	this->setPosition(0, static_cast<float>(screenH - this->pipBuckSprite.get().getLocalBounds().height));
}

/**
 * Opens PipBuck by changing game state and plays the open/close sound.
 */
void PipBuck::open(bool sound)
{
	this->gameState = STATE_PIPBUCK;
	if (sound)
		this->soundOpenClose.get().play();
	Log::d(STR_GAME_PAUSED);
}

/**
 * Closes PipBuck by changing game state and plays the open/close sound.
 */
void PipBuck::close()
{
	this->gameState = STATE_PLAYING;
	this->soundOpenClose.get().play();
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
	else if (catResult == CLICK_CONSUMED_CLOSE)
	{
		this->close();
		return CLICK_CONSUMED;
	}
	else if (catResult == CLICK_CONSUMED_UNLOAD)
	{
		// note: we could unload campaign inside the page (inside btn callback),
		// but then we'd have to clear resmgr two times (unload campaign + clear resmgr,
		// unload pipbuck infos, clear resmgr)
		this->unloadCampaignInfos();
		this->campaign.unload();
		this->gameState = STATE_MAINMENU;
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
				this->soundCategoryBtn.get().play();
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
		{
			this->unloadCampaignInfos();
			this->resMgr.cleanUnused();
			return false;
		}
	}

	return true;
}

void PipBuck::unloadCampaignInfos()
{
	Log::d(STR_PIPBUCK_UNLOADING_CAMPAIGN);

	for (auto &cat : this->categories)
	{
		cat.unloadCampaignInfos();
	}
}

/**
 * Sets the value displayed by "hardware" rad indicator.
 *
 * @param rads amount of rads, in fraction (1.0 = max)
 */
void PipBuck::setRadLevel(float rads)
{
	if (rads > 1.f)
		this->radIndicatorLevel = 1.f;
	else
		this->radIndicatorLevel = rads;
}

/**
 * Don't expect this function to deploy smooth jazz.
 */
double PipBuck::getSmoothNoise(double time)
{
	return sin(time * 7) + cos(time * 12) - cos(time * 9);
}

void PipBuck::updateDraw()
{
	this->radIndicator.setRotation(
		this->radIndicatorLevel * -180 +
		static_cast<float>(this->getSmoothNoise(this->timer.getElapsedTime().asSeconds())/2)
	);
}

void PipBuck::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->pipBuckSprite.sprite, states);
	target.draw(this->radIndicator, states);

	target.draw(this->categories[this->selectedCategory], states);

	for (const auto &btn : this->categoryButtons)
	{
		target.draw(btn, states);
	}

	target.draw(this->closeBtn, states);
}
