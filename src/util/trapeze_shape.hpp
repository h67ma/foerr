// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/Graphics/Shape.hpp>

struct trapeze_data
{
		float topStartX;
		float bottomStartX;
		float topWidth;
		float bottomWidth;
		float height;
};

template<typename T>
constexpr struct trapeze_data operator*(const struct trapeze_data& lhs, T rhs)
{
	return trapeze_data { lhs.topStartX * rhs, lhs.bottomStartX * rhs, lhs.topWidth * rhs, lhs.bottomWidth * rhs,
						  lhs.height * rhs };
}

/**
 * Represents a trapeze-shaped polygon, compatible with SFML API.
 *
 * Supports checking if a point is inside the shape, not by using a bounding rectangle, but rather by checking if the
 * point is actually inside the shape.
 */
class TrapezeShape : public sf::Shape
{
	private:
		struct trapeze_data data;
		float leftCoeff;
		float rightCoeff;
		void updateCoeffs();

	public:
		explicit TrapezeShape(const struct trapeze_data& data);
		explicit TrapezeShape(float topStartX, float bottomStartX, float topWidth, float bottomWidth, float height);
		std::size_t getPointCount() const override;
		sf::Vector2f getPoint(std::size_t index) const override;
		bool contains(sf::Vector2f point) const;
		struct trapeze_data getData() const;
		void setData(const struct trapeze_data& data);
};
