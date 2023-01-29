#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/System/Vector2.hpp>

#include "../resources/resource_manager.hpp"
#include "../settings/keymap.hpp"
#include "../consts.hpp"
#include "animation.hpp"

// TODO should probably inherit some kind of PhysicsObject or something
class Player : public sf::Drawable, public sf::Transformable
{
	private:
		Animation animation;
		sf::Vector2f velocity{ 0.0f, 0.0f };
		bool facingRight = true;

		// flags used to determine if a key is being pressed. this is pretty lame, but for a different approach
		// (directly checking keyboard state via sf::Keyboard::isKeyPressed()), Keymap would need to be changed quite
		// a lot. for now this will do.
		bool rightHeld = false;
		bool leftHeld = false;
		bool upHeld = false;
		bool downHeld = false;
		bool sprintHeld = false;

	public:
		explicit Player(ResourceManager &resMgr);
		void nextFrame();
		void tick(uint lastFrameDurationUs);
		void handleKeyUpDown(enum KeyAction action, bool down);
		void debugDrawBounds(sf::RenderTarget &target, sf::RenderStates &states) const;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
