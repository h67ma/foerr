// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2024 h67ma <szycikm@gmail.com>

#include "scrollable.hpp"

#include <memory>

#include "../../util/util.hpp"
#include "../hud.hpp"

constexpr float SCROLL_FACTOR = 25; // TODO? make configurable in settings

// arbitrary, doesn't matter that much, as long as it's reasonably large (>= slider height in px).
// ideally it should be = ::bottomScrollLimit, to avoid scaling in ::updateScrollbar() and ::handleScrollbarMoved(),
// but that would require adding support for changing max value in IntSlider. TODO at some point?
constexpr int SCROLLBAR_MAX_VALUE = 5000;

Scrollable::Scrollable(ResourceManager& resMgr, const sf::Vector2f& scrollableAreaSize) :
	scrollableAreaSize(scrollableAreaSize),
	scrollbar(SLIDER_VERTICAL, static_cast<uint>(scrollableAreaSize.y), *resMgr.getFont(FONT_NORMAL), false, 0, 0,
			  SCROLLBAR_MAX_VALUE)
{
	this->scrollbar.setPosition(scrollableAreaSize.x, 0);
	this->setGuiScale();
}

/**
 * Should be called immediately after object's creation (to initialize ::scrollableContentTexture)
 */
void Scrollable::setGuiScale()
{
	sf::Vector2f scaledSize = calculateGuiAwarePoint(this->scrollableAreaSize);

	this->scrollArea.setSize(scaledSize);

	// there's no resize, and it's hard to tell if calling ::create() multiple times leads to creating additional
	// textures indefinitely - better to destroy it to be sure
	this->scrollableContentTexture.reset(nullptr);
	this->scrollableContentTexture = std::make_unique<sf::RenderTexture>();
	this->scrollableContentTexture->create(static_cast<uint>(scaledSize.x), static_cast<uint>(scaledSize.y));
}

/**
 * Renders scrollable content to a texture, which is later drawn in ::draw().
 *
 * @param resize whether the render texture is being resized
 *
 * The intermediate texture is required because content needs to be trimmed to scrollable area size.
 * Note that a trimming mask (+ blend mode) is not required, as we're drawing to a rectangular texture, so contents
 * drawn outside of its area will simply not be visible.
 */
void Scrollable::redrawScrollableContent(bool resize)
{
	sf::RenderStates states;

	this->scrollableContentTexture->clear(sf::Color::Transparent);

	// draw scrollable content at current scroll offset
	states.transform.translate(0, this->scrollOffset);
	this->drawScrollableContent(*this->scrollableContentTexture, states);

	this->scrollableContentTexture->display();
	this->scrollableContentSprite.setTexture(this->scrollableContentTexture->getTexture(), resize);
}

/**
 * Update the total height of scrollable content.
 *
 * @param newHeight height of the resized content
 * @param scrollTo where the content should be scrolled to
 * @param resize whether the scrollable area is being resized
 *
 * Ensures the scroll range and position make sense after this change.
 * Required for scroll limits and scroll bar to behave/display correctly.
 */
void Scrollable::handleScrollableContentHeightChanged(float newHeight, enum ScrollPosition scrollTo, bool resize)
{
	this->scrollableContentHeight = newHeight;

	this->bottomScrollLimit = this->scrollArea.getSize().y - this->scrollableContentHeight;

	// if the content got shorter than scroll area, scroll it to the top
	if (scrollTo == SCROLL_TOP || this->scrollableContentHeight <= this->scrollArea.getSize().y)
		this->scrollOffset = 0;
	// if the content got shorter, but is still taller than scroll area, and was previously scrolled to the
	// bottom, scroll it to the new bottom
	else if (scrollTo == SCROLL_BOTTOM || this->scrollOffset < this->bottomScrollLimit)
		this->scrollOffset = this->bottomScrollLimit;

	// the content height changed, so the content itself also changed - redraw it
	this->redrawScrollableContent(resize);

	this->updateScrollbar();
}

/**
 * Checks if an item in scrollable area should be drawn or not.
 * If its bounds lay entirely outside of the scroll area, then no sense drawing it.
 * Note: only y axis is checked.
 * Note: assumes the scroll area's y = 0.
 */
bool Scrollable::shouldDrawItem(float elemHeight, float elemOffset) const
{
	float elemPos = elemOffset + this->scrollOffset;
	return (elemPos < this->scrollArea.getSize().y) && (elemPos + elemHeight > 0);
}

bool Scrollable::isWithinScrollArea(sf::Vector2f point) const
{
	return this->scrollArea.getLocalBounds().contains(point);
}

float Scrollable::getScrollOffset() const
{
	return this->scrollOffset;
}

void Scrollable::handleScroll(float delta, sf::Vector2i mousePos)
{
	if (this->scrollableContentHeight <= this->scrollArea.getSize().y)
		return; // scrollable content fits entirely inside scroll area - don't scroll

	if ((delta < 0 && this->scrollOffset == this->bottomScrollLimit) || (delta > 0 && this->scrollOffset == 0))
		return; // already scrolled to the max position in the desired direction, nothing to do

	mousePos -= this->getPosition();

	// ignore event if mouse was outside Scrollable's area
	if (!this->isWithinScrollArea(static_cast<sf::Vector2f>(mousePos)))
		return;

	// can't use ::move(), since it would effectively prevent the component to be placed
	// at a desired position (vertically)
	this->scrollOffset += delta * calculateGuiAwareScalar(SCROLL_FACTOR);

	// limit scrolling to the top
	if (this->scrollOffset > 0)
		this->scrollOffset = 0;

	// limit scrolling to the bottom
	if (this->scrollOffset < this->bottomScrollLimit)
		this->scrollOffset = this->bottomScrollLimit;

	// note: as an alternative approach to redrawing the texture after each scroll event, we could instead render all
	// items into a tall texture, and just draw it with some offset (and mask it). however, this could consume a lot of
	// GPU memory if there are a lot of items.
	this->redrawScrollableContent(false);

	this->updateScrollbar();
}

void Scrollable::updateScrollbar()
{
	this->scrollbar.setValue(this->scrollOffset * SCROLLBAR_MAX_VALUE / this->bottomScrollLimit);
}

/**
 * Called when the scrollbar is dragged or clicked.
 */
void Scrollable::handleScrollbarMoved()
{
	// round result for display clarity
	this->scrollOffset = std::round(this->scrollbar.getValue() * this->bottomScrollLimit / SCROLLBAR_MAX_VALUE);
	this->redrawScrollableContent(false);
}

ClickStatus Scrollable::handleLeftClick(sf::Vector2i clickPos)
{
	clickPos -= this->getPosition();

	if (this->scrollbar.handleLeftClick(clickPos))
	{
		this->handleScrollbarMoved();
		return CLICK_CONSUMED;
	}

	return CLICK_NOT_CONSUMED;
}

void Scrollable::handleLeftClickUp()
{
	this->scrollbar.handleLeftClickUp();
}

bool Scrollable::handleMouseMove(sf::Vector2i mousePos)
{
	mousePos -= this->getPosition();

	if (this->scrollbar.handleMouseMove(mousePos))
	{
		this->handleScrollbarMoved();
		return true;
	}

	return false;
}

void Scrollable::handleSettingsChange()
{
	this->handleGuiScaleChange();
	this->scrollbar.handleSettingsChange();
	this->setGuiScale();

	// scroll limit depends on scrollable area size, which can change along with GUI scale.
	// derived class should call ::handleScrollableContentHeightChanged(), which will update scroll limit and redraw
	// scrollable content
	// note: current scroll position would change slightly
}

void Scrollable::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	target.draw(this->scrollableContentSprite, states);
	target.draw(this->scrollbar, states);
}
