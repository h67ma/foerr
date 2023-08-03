#include "player.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "../settings/settings_manager.hpp"
#include "../settings/keymap.hpp"

// no worries, float can handle precision at this order of magnitude
constexpr float MAX_VELOCITY = 0.0004;
constexpr float MAX_VELOCITY_SPRINT = 0.0008;
constexpr float PLAYER_ACCELERATION = 0.000000004;

Player::Player(ResourceManager &resMgr) :
	// TODO actual animation
	animation(resMgr.getTexture("res/entities/mchavi.png"), { PLAYER_SPRITE_W, PLAYER_SPRITE_H }, {
		{ ANIM_STAND, 1 },
		{ ANIM_TROT, 17 },
		{ ANIM_GALLOP, 8 },
		{ ANIM_JUMP, 16 },
		{ ANIM_DIE_GROUND, 20 },
		{ ANIM_DIE_AIR, 13 },
		{ ANIM_TK_HOLD, 8 },
		{ ANIM_SWIM, 24 },
		{ ANIM_CLIMB, 12 },
		{ ANIM_WALK, 24 },
	})
{
	// TODO actual animation
	this->animation.setAnimation(ANIM_SWIM);

	this->setOrigin(this->collider.left + PLAYER_W2, this->collider.top + PLAYER_H2);
}

void Player::nextFrame()
{
	this->animation.nextFrame();
}

/**
 * Updates Player velocity based on previous velocity and currently pressed keys.
 *
 * Velocity should be updated two times on every frame, like so:
 *	velocity += deltaTime * acc / 2
 *	position.x += old.x * speed * deltaTime // etc for .y
 *	velocity += deltaTime * acc / 2
 * See https://www.youtube.com/watch?v=yGhfUcPjXuE for a nice explanation.
 *
 * Calling this twice is not the best solution, but it's not that much better if we calculate deltaV here and change
 * velocity in the caller, as we also have to check for speed limit, and also there's sprint, so it ends up being messy
 * and not that much more efficient. Let's stick with current approach for now as at least it's more readable.
 */
void Player::updateVelocity(uint lastFrameDurationUs)
{
	float maxHVelocity = Keymap::isSprintHeld() ? MAX_VELOCITY_SPRINT : MAX_VELOCITY;

	float velocityIncrement = lastFrameDurationUs * PLAYER_ACCELERATION / 2;

	if (Keymap::isLeftHeld())
	{
		this->velocity.x -= velocityIncrement;

		// cap velocity
		this->velocity.x = std::max(-maxHVelocity, this->velocity.x);

		if (this->facingRight)
		{
			this->setScale(-1.F, 1.F);
			this->facingRight = false;
		}
	}
	else if (Keymap::isRightHeld())
	{
		this->velocity.x += velocityIncrement;

		// cap velocity
		this->velocity.x = std::min(maxHVelocity, this->velocity.x);

		if (!this->facingRight)
		{
			this->setScale(1.F, 1.F);
			this->facingRight = true;
		}
	}
	else if (std::abs(this->velocity.x) < velocityIncrement)
		this->velocity.x = 0;
	else if (this->velocity.x > 0)
		this->velocity.x -= velocityIncrement;
	else if (this->velocity.x < 0)
		this->velocity.x += velocityIncrement;

	// X & Y axes can be controlled independently
	if (Keymap::isUpHeld())
	{
		this->velocity.y -= velocityIncrement;

		// cap velocity
		this->velocity.y = std::max(-MAX_VELOCITY, this->velocity.y);
	}
	else if (Keymap::isDownHeld())
	{
		this->velocity.y += velocityIncrement;

		// cap velocity
		this->velocity.y = std::min(MAX_VELOCITY, this->velocity.y);
	}
	else if (std::abs(this->velocity.y) < velocityIncrement)
		this->velocity.y = 0;
	else if (this->velocity.y > 0)
		this->velocity.y -= velocityIncrement;
	else if (this->velocity.y < 0)
		this->velocity.y += velocityIncrement;
}

/**
 * Zeroes Player velocity vertically.
 */
void Player::stopVertical()
{
	this->velocity.y = 0;
}

/**
 * Zeroes Player velocity horizontally.
 */
void Player::stopHorizontal()
{
	this->velocity.x = 0;
}

const sf::Vector2f& Player::getVelocity() const
{
	return this->velocity;
}

/**
 * Draws a debug overlay consisting of bounding box and origin point.
 *
 * This is done in a function in order not to waste stack memory while in normal (non-debug) operation.
 */
void Player::debugDrawBounds(sf::RenderTarget &target, sf::RenderStates &states) const
{
	sf::RectangleShape debugBox;
	debugBox.setFillColor(sf::Color::Transparent);
	debugBox.setOutlineThickness(1.F);
	debugBox.setOutlineColor(sf::Color::White);
	debugBox.setSize(sf::Vector2f(this->collider.width, this->collider.height));
	debugBox.setPosition(sf::Vector2f(this->collider.left, this->collider.top));
	target.draw(debugBox, states);

	sf::CircleShape originPoint;
	originPoint.setFillColor(sf::Color::White);
	originPoint.setRadius(2.F);
	originPoint.setOrigin(1.F, 1.F);
	originPoint.setPosition(this->getOrigin());
	target.draw(originPoint, states);
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->animation, states);

	if (SettingsManager::debugBoundingBoxes)
		this->debugDrawBounds(target, states);
}
