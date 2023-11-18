// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>
#include <SFML/Graphics/Rect.hpp>

#include "../resources/resource_manager.hpp"
#include "../campaigns/room_cell.hpp"
#include "physics_entity.hpp"
#include "../util/util.hpp"
#include "../consts.hpp"
#include "animation.hpp"

// TODO we'll probably need something more sophisticated, based on current animation
constexpr uint PLAYER_W = 60;
constexpr uint PLAYER_CELL_W = uintDivCeil(PLAYER_W, CELL_SIDE_LEN);
constexpr uint PLAYER_W2 = PLAYER_W / 2;
constexpr uint PLAYER_H = 60;
constexpr uint PLAYER_CELL_H = uintDivCeil(PLAYER_H, CELL_SIDE_LEN);
constexpr uint PLAYER_H2 = PLAYER_H / 2;
constexpr uint PLAYER_SPRITE_W = 130;
constexpr uint PLAYER_SPRITE_H = 130;
constexpr uint PLAYER_COLLIDER_LEFT = (PLAYER_SPRITE_W - PLAYER_W) / 2;
constexpr uint PLAYER_COLLIDER_TOP = 40;

// TODO should probably inherit some kind of PhysicsObject or something
class Player : public sf::Drawable, public sf::Transformable
{
	private:
		Animation animation;
		sf::Vector2f velocity{ 0.F, 0.F };
		bool facingRight = true;
		sf::IntRect collider {
			PLAYER_COLLIDER_LEFT,
			PLAYER_COLLIDER_TOP,
			PLAYER_W,
			PLAYER_H
		};
		enum MovementMode movementMode = MOVM_WALK;

	public:
		explicit Player(ResourceManager &resMgr);
		void nextFrame();
		void updateVelocity(uint lastFrameDurationUs);
		void stopVertical();
		void stopHorizontal();
		void debugToggleFlight();
		const sf::Vector2f& getVelocity() const;
		void debugDrawBounds(sf::RenderTarget &target, sf::RenderStates &states) const;
		void draw(sf::RenderTarget &target, sf::RenderStates states) const override;
};
