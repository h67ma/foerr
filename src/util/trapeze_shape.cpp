// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#include "trapeze_shape.hpp"

TrapezeShape::TrapezeShape(const struct trapeze_data &data) : data(data)
{
	this->updateCoeffs();
	this->update();
}

TrapezeShape::TrapezeShape(float topStartX, float bottomStartX, float topWidth, float bottomWidth, float height) :
	data { topStartX, bottomStartX, topWidth, bottomWidth, height }
{
	this->updateCoeffs();
	this->update();
}

void TrapezeShape::updateCoeffs()
{
	this->leftCoeff = this->data.bottomStartX - this->data.topStartX;
	this->rightCoeff = this->leftCoeff + this->data.bottomWidth - this->data.topWidth;
}

std::size_t TrapezeShape::getPointCount() const
{
	return 4;
}

sf::Vector2f TrapezeShape::getPoint(std::size_t index) const
{
	switch (index)
	{
		default:
		case 0: return sf::Vector2f(this->data.topStartX, 0);
		case 1: return sf::Vector2f(this->data.topStartX + this->data.topWidth, 0);
		case 2: return sf::Vector2f(this->data.bottomStartX + this->data.bottomWidth, this->data.height);
		case 3: return sf::Vector2f(this->data.bottomStartX, this->data.height);
	}
}

bool TrapezeShape::contains(sf::Vector2f point) const
{
	// vertically
	if (point.y < 0 || point.y > this->data.height)
		return false;

	// horizontal calculations are derived from the two-point form equation of a line

	// horizontally - left side
	if ((this->leftCoeff * point.y) -
		((point.x - this->data.topStartX) * this->data.height) > 0)
		return false;

	// horizontally - right side
	if ((this->rightCoeff * point.y) -
		((point.x - this->data.topStartX - this->data.topWidth) * this->data.height) < 0)
		return false;

	return true;
}

struct trapeze_data TrapezeShape::getData() const
{
	return this->data;
}

void TrapezeShape::setData(const struct trapeze_data &data)
{
	this->data = data;
	this->updateCoeffs();
	this->update();
}
