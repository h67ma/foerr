#include "pipbuck_category.hpp"

PipBuckCategory::PipBuckCategory(GuiScale scale, sf::Color hudColor, uint fxVolume, ResourceManager &resMgr) :
	pageButtons { // order matters
		Button(scale, BTN_NARROW, hudColor, resMgr, 385, 210),
		Button(scale, BTN_NARROW, hudColor, resMgr, 525, 210),
		Button(scale, BTN_NARROW, hudColor, resMgr, 665, 210),
		Button(scale, BTN_NARROW, hudColor, resMgr, 805, 210),
		Button(scale, BTN_NARROW, hudColor, resMgr, 945, 210)
	}
{
	this->soundPageChange.setBuffer(*resMgr.getSoundBuffer(PATH_AUD_PIPBUCK_PAGECHANGE));
	this->soundPageChange.setVolume(static_cast<float>(fxVolume));

	this->changePage(this->selectedPage); // default page
}

/**
 * Setups buttons stored in this object to respond to hover events.
 * Must be called after constructing the object. If the object is
 * moved in memory, there *will* be problems.
 *
 * Also setups button labels.
 */
void PipBuckCategory::setup()
{
	for (auto &btn : this->pageButtons)
	{
		this->hoverMgr.addHoverable(&btn);
	}

	// this is not very safe, but considering we'll always have
	// exactly 5 buttons and 5 pages, it should be ok
	for (uint i = 0; i < this->pages.size(); i++)
	{
		this->pageButtons[i].setText(this->pages[i]->getLabel());
	}
}

void PipBuckCategory::changePage(uint idx)
{
	this->pageButtons[this->selectedPage].setSelected(false);
	this->pageButtons[idx].setSelected(true);

	this->selectedPage = idx;
}

/**
 * @param x the x mouse coordinate
 * @param y the y mouse coordinate
 * @return true if click was consumed
 * @return false if click was not consumed
 */
bool PipBuckCategory::handleLeftClick(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	for (auto it = this->pageButtons.begin(); it != this->pageButtons.end(); it++)
	{
		if (it->containsPoint(x, y))
		{
			uint idx = static_cast<uint>(std::distance(this->pageButtons.begin(), it));
			if (idx != this->selectedPage)
			{
				this->changePage(idx);
				this->soundPageChange.play();
			}
			return true; // click consumed
		}
	}

	return false;
}

bool PipBuckCategory::handleMouseMove(int x, int y)
{
	// account for this component's position
	x -= static_cast<int>(this->getPosition().x);
	y -= static_cast<int>(this->getPosition().y);

	return this->hoverMgr.handleMouseMove(x, y);
}

void PipBuckCategory::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(*this->pages[this->selectedPage], states);

	for (auto &btn : this->pageButtons)
	{
		target.draw(btn, states);
	}
}
