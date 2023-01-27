#include "player.hpp"

Player::Player(ResourceManager &resMgr) :
	// TODO actual animation
	animation(resMgr.getTexture("res/entities/mchavi.png"), { 130, 130 }, {
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
}

void Player::nextFrame()
{
	this->animation.nextFrame();
}

void Player::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	target.draw(this->animation, states);
}
