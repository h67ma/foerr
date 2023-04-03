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

	public:
		explicit TrapezeShape(struct trapeze_data data);
		std::size_t getPointCount() const override;
		sf::Vector2f getPoint(std::size_t index) const override;
		bool contains(sf::Vector2f point) const;
};
