#include "pipbuck.hpp"
#include "../util/i18n.hpp"
#include "../log.hpp"

PipBuck::PipBuck(GuiScale scale, sf::Color hudColor, ResourceManager& resMgr) :
	resMgr(resMgr),
	catStatusBtn(scale, BTN_BIG, hudColor, *resMgr.getFont(FONT_MEDIUM), 650, 900, STR_PIPBUCK_STATUS),
	catInvBtn(scale, BTN_BIG, hudColor, *resMgr.getFont(FONT_MEDIUM), 855, 915, STR_PIPBUCK_INV),
	catInfoBtn(scale, BTN_BIG, hudColor, *resMgr.getFont(FONT_MEDIUM), 1055, 900, STR_PIPBUCK_INFO),
	closeBtn(scale, BTN_BIG, hudColor, *resMgr.getFont(FONT_MEDIUM), 55, 750, STR_PIPBUCK_CLOSE),
	statusCategoryPage(scale, hudColor, resMgr, "STATUS"),
	invCategoryPage(scale, hudColor, resMgr, "INVENTORY"),
	infoCategoryPage(scale, hudColor, resMgr, "INFORMATION")
{
	sf::Texture *pipbuckOverlay = resMgr.getTexture(PATH_PIPBUCK_OVERLAY);

	this->pipBuckSprite.setTexture(*pipbuckOverlay);

	this->hoverMgr.addHoverable(&catStatusBtn);
	this->hoverMgr.addHoverable(&catInvBtn);
	this->hoverMgr.addHoverable(&catInfoBtn);
	this->hoverMgr.addHoverable(&closeBtn);

	this->changeCategory(PIPB_CAT_STATUS); // default category
}

void PipBuck::handleScreenResize(uint screenW, uint screenH)
{
	// for now copy the behaviour of Remains pipbuck, i.e. display it unscaled in bottom left corner
	// TODO scale should affect sprite size and individual buttons/labels/etc scale and placement
	this->setPosition(0, static_cast<float>(screenH - this->pipBuckSprite.getLocalBounds().height));
}

void PipBuck::handleMouseMove(int x, int y)
{
	// need to account for this component's position
	this->hoverMgr.handleMouseMove(x - static_cast<int>(this->getPosition().x),
								   y - static_cast<int>(this->getPosition().y));
}

/**
 * Changes active category.
 * If the requested category is already active, nothing will happen.
 *
 * @param cat new category to set
 */
void PipBuck::changeCategory(PipBuckCategory cat)
{
	if (cat == PIPB_CAT_STATUS && selectedCategory != PIPB_CAT_STATUS)
	{
		this->catStatusBtn.setSelected(true);
		this->catInvBtn.setSelected(false);
		this->catInfoBtn.setSelected(false);
	}
	else if (cat == PIPB_CAT_INV && selectedCategory != PIPB_CAT_INV)
	{
		this->catStatusBtn.setSelected(false);
		this->catInvBtn.setSelected(true);
		this->catInfoBtn.setSelected(false);
	}
	else if (cat == PIPB_CAT_INFO && selectedCategory != PIPB_CAT_INFO)
	{
		this->catStatusBtn.setSelected(false);
		this->catInvBtn.setSelected(false);
		this->catInfoBtn.setSelected(true);
	}

	selectedCategory = cat;
}

/**
 * Handles left mouse click event.
 *
 * @param x x click coordinate
 * @param y y click coordinate
 * @return `true` if game should be unpaused (owner should hide PipBuck menu)
 * @return `false` if the game should remain paused (owner should leave PipBuck open)
 */
bool PipBuck::handleLeftClick(int x, int y)
{
	// need to account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	if (this->catStatusBtn.maybeHandleLeftClick(x, y))
		this->changeCategory(PIPB_CAT_STATUS);
	else if (this->catInvBtn.maybeHandleLeftClick(x, y))
		this->changeCategory(PIPB_CAT_INV);
	else if (this->catInfoBtn.maybeHandleLeftClick(x, y))
		this->changeCategory(PIPB_CAT_INFO);
	else if (this->closeBtn.maybeHandleLeftClick(x, y))
		return true;

	return false;
}

void PipBuck::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->pipBuckSprite, states);

	target.draw(this->catStatusBtn, states);
	target.draw(this->catInvBtn, states);
	target.draw(this->catInfoBtn, states);
	target.draw(this->closeBtn, states);

	if (this->selectedCategory == PIPB_CAT_STATUS)
		target.draw(this->statusCategoryPage, states);
	else if (this->selectedCategory == PIPB_CAT_INV)
		target.draw(this->invCategoryPage, states);
	else if (this->selectedCategory == PIPB_CAT_INFO)
		target.draw(this->infoCategoryPage, states);
}
