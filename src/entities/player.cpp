#include "player.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "../settings/settings_manager.hpp"

// no worries, float can handle precision at this order of magnitude
#define MAX_VELOCITY 0.0004f
#define MAX_VELOCITY_SPRINT 0.0008f
#define VELOCITY_INCREMENT 0.00005f

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
 * Updates Player position based on velocity (which is affected by currently pressed keys) and last frame time.
 */
void Player::tick(uint lastFrameDurationUs)
{
	///// check the state of keys, calculate current velocity /////

	float maxHVelocity = this->sprintHeld ? MAX_VELOCITY_SPRINT : MAX_VELOCITY;

	if (this->leftHeld)
	{
		this->velocity.x -= VELOCITY_INCREMENT;
		if (this->velocity.x < -maxHVelocity)
			this->velocity.x = -maxHVelocity;

		if (this->facingRight)
		{
			this->setScale(-1.0f, 1.0f);
			this->facingRight = false;
		}
	}
	else if (this->rightHeld)
	{
		this->velocity.x += VELOCITY_INCREMENT;
		if (this->velocity.x > maxHVelocity)
			this->velocity.x = maxHVelocity;

		if (!this->facingRight)
		{
			this->setScale(1.0f, 1.0f);
			this->facingRight = true;
		}
	}
	else if (std::abs(this->velocity.x) < VELOCITY_INCREMENT)
		this->velocity.x = 0;
	else if (this->velocity.x > 0)
		this->velocity.x -= VELOCITY_INCREMENT;
	else if (this->velocity.x < 0)
		this->velocity.x += VELOCITY_INCREMENT;

	// X & Y axes can be controlled independently
	if (this->upHeld)
	{
		this->velocity.y -= VELOCITY_INCREMENT;
		if (this->velocity.y < -MAX_VELOCITY)
			this->velocity.y = -MAX_VELOCITY;
	}
	else if (this->downHeld)
	{
		this->velocity.y += VELOCITY_INCREMENT;
		if (this->velocity.y > MAX_VELOCITY)
			this->velocity.y = MAX_VELOCITY;
	}
	else if (std::abs(this->velocity.y) < VELOCITY_INCREMENT)
		this->velocity.y = 0;
	else if (this->velocity.y > 0)
		this->velocity.y -= VELOCITY_INCREMENT;
	else if (this->velocity.y < 0)
		this->velocity.y += VELOCITY_INCREMENT;

	///// move the player /////

	this->move(this->velocity * static_cast<float>(lastFrameDurationUs));
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

/**
 * Handles key up or key down event.
 *
 * @param action the action associated with key pressed
 * @param down true for key down event, false for key up event
 */
void Player::handleKeyUpDown(enum KeyAction action, bool down)
{
	switch (action)
	{
		case ACTION_PLAYER_MOVE_LEFT:
			this->leftHeld = down;
			break;
		case ACTION_PLAYER_MOVE_RIGHT:
			this->rightHeld = down;
			break;
		case ACTION_PLAYER_MOVE_UP:
			this->upHeld = down;
			break;
		case ACTION_PLAYER_MOVE_DOWN:
			this->downHeld = down;
			break;
		case ACTION_PLAYER_SPRINT:
			this->sprintHeld = down;
			break;
		default:
			break;
	}
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
	debugBox.setOutlineThickness(1.f);
	debugBox.setOutlineColor(sf::Color::White);
	debugBox.setSize(sf::Vector2f(this->collider.width, this->collider.height));
	debugBox.setPosition(sf::Vector2f(this->collider.left, this->collider.top));
	target.draw(debugBox, states);

	sf::CircleShape originPoint;
	originPoint.setFillColor(sf::Color::White);
	originPoint.setRadius(2.f);
	originPoint.setOrigin(1.f, 1.f);
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
