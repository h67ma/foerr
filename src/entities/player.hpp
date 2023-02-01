#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "../resources/resource_manager.hpp"
#include "../consts.hpp"
#include "animation.hpp"

// TODO we'll probably need something more sophisticated, based on current animation
#define PLAYER_W 60
#define PLAYER_W2 PLAYER_W/2
#define PLAYER_H 60
#define PLAYER_H2 PLAYER_H/2
#define PLAYER_SPRITE_W 130
#define PLAYER_SPRITE_H 130
#define PLAYER_COLLIDER_LEFT (PLAYER_SPRITE_W - PLAYER_W) / 2
#define PLAYER_COLLIDER_TOP 40

// TODO should probably inherit some kind of PhysicsObject or something
class Player : public sf::Drawable, public sf::Transformable
{
	private:
		Animation animation;
		sf::Vector2f velocity{ 0.0f, 0.0f };
		bool facingRight = true;
		sf::IntRect collider {
			PLAYER_COLLIDER_LEFT,
			PLAYER_COLLIDER_TOP,
			PLAYER_W,
			PLAYER_H
		};

	public:
		explicit Player(ResourceManager &resMgr);
		void nextFrame();
		void tick(uint lastFrameDurationUs);
		void stopVertical();
		void stopHorizontal();
		void debugDrawBounds(sf::RenderTarget &target, sf::RenderStates &states) const;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
