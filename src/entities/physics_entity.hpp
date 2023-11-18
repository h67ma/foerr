// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2023 h67ma <szycikm@gmail.com>

#include <SFML/System/Vector2.hpp>

#include "../consts.hpp"

constexpr float ACCELERATION_GRAVITY = 0.000000003;

enum MovementMode
{
	MOVM_WALK,				// walk/sprint left/right, jump, double jump
// TODO	MOVM_SELF_LEVITATION,	// free movement in all 4 directions. jump button held
// TODO	MOVM_SWIM,				// free movement in all 4 directions. instead of jump, go up
	MOVM_FLY,				// functionally the same as MOVM_SWIM
};
