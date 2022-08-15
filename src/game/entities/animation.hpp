#pragma once

#include <unordered_map>
#include <SFML/Graphics.hpp>
#include "../consts.hpp"

enum AnimationKind
{
	ANIM_STAND,
	ANIM_WALK,
	ANIM_TROT,
	ANIM_GALLOP,
	ANIM_JUMP,
	ANIM_CROUCH,
	ANIM_CLIMB,
	ANIM_TK_HOLD,
	ANIM_SWIM,
	ANIM_DIE_AIR,
	ANIM_DIE_GROUND,
	ANIM_STATIC,
	ANIM_OPEN,
};

struct anim_kind_details
{
	AnimationKind kind;
	uint frameCnt;
};

struct anim_kind_details_internal
{
	uint frameCnt;
	uint offsetTop;
};

class Animation : public sf::Drawable, public sf::Transformable
{
	private:
		uint textureHeight;
		sf::Sprite sprite;
		sf::IntRect textureRect;
		sf::Clock clock;
		uint loadedKindTextureWidth;
		uint width;
		uint height;
		std::unordered_map<AnimationKind, struct anim_kind_details_internal> kinds;

	public:
		Animation(const sf::Texture &texture, uint width, uint height, const std::vector<struct anim_kind_details> kinds);
		bool loadTexture();
		void maybeNextFrame();
		bool setAnimation(AnimationKind kind);
		virtual void draw(sf::RenderTarget& target, sf::RenderStates states) const;
};
