#include "player.hpp"

#include <SFML/Graphics/RectangleShape.hpp>
#include <SFML/Graphics/CircleShape.hpp>

#include "../settings/settings_manager.hpp"
// TODO #include "../settings/keymap.hpp"

// no worries, float can handle precision at this order of magnitude
#define MAX_VELOCITY 0.0004f
#define MAX_VELOCITY_SPRINT 0.0008f
#define VELOCITY_INCREMENT 0.00005f

// TODO we'll probably need something more sophisticated, based on current animation
#define PLAYER_W 130
#define PLAYER_H 130
#define PLAYER_CENTER_X PLAYER_W/2
#define PLAYER_CENTER_Y PLAYER_H/2

Player::Player(ResourceManager &resMgr) :
	// TODO actual animation
	animation(resMgr.getTexture("res/entities/mchavi.png"), { PLAYER_W, PLAYER_H }, {
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
	}),
	// TODO read keys from Keymap
	keyLeft(sf::Keyboard::A),
	keyRight(sf::Keyboard::D),
	keyUp(sf::Keyboard::W),
	keyDown(sf::Keyboard::S),
	keySprint(sf::Keyboard::LShift)
{
	// TODO actual animation
	this->animation.setAnimation(ANIM_SWIM);

	this->setOrigin(PLAYER_CENTER_X, PLAYER_CENTER_Y);
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
	float maxHVelocity = sf::Keyboard::isKeyPressed(this->keySprint) ? MAX_VELOCITY_SPRINT : MAX_VELOCITY;

	if (sf::Keyboard::isKeyPressed(this->keyLeft))
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
	else if (sf::Keyboard::isKeyPressed(this->keyRight))
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
	if (sf::Keyboard::isKeyPressed(this->keyUp))
	{
		this->velocity.y -= VELOCITY_INCREMENT;
		if (this->velocity.y < -MAX_VELOCITY)
			this->velocity.y = -MAX_VELOCITY;
	}
	else if (sf::Keyboard::isKeyPressed(this->keyDown))
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

	this->move(this->velocity * static_cast<float>(lastFrameDurationUs));
}

void Player::debugDrawBounds(sf::RenderTarget &target, sf::RenderStates &states) const
{
	sf::RectangleShape debugBox;
	debugBox.setFillColor(sf::Color::Transparent);
	debugBox.setOutlineThickness(1.f);
	debugBox.setOutlineColor(sf::Color::White);
	debugBox.setSize({ PLAYER_W, PLAYER_H });
	target.draw(debugBox, states);

	sf::CircleShape originPoint;
	originPoint.setFillColor(sf::Color::White);
	originPoint.setRadius(2.f);
	originPoint.setPosition(PLAYER_CENTER_X - 1, PLAYER_CENTER_Y - 1);
	target.draw(originPoint, states);
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();

	target.draw(this->animation, states);

	if (SettingsManager::debugBoundingBoxes)
		this->debugDrawBounds(target, states);
}
