#pragma once

#include <SFML/Graphics/Drawable.hpp>
#include <SFML/Window/Keyboard.hpp>
#include <SFML/System/Vector2.hpp>

#include "../resources/resource_manager.hpp"
#include "../consts.hpp"
#include "animation.hpp"

// TODO should probably inherit some kind of PhysicsObject or something
class Player : public sf::Drawable, public sf::Transformable
{
	private:
		Animation animation;
		sf::Vector2f velocity{ 0.0f, 0.0f };
		bool facingRight = true;

		// keys used for movement will be held and need to be very responsive, so they are not handled by events,
		// rather by directly checking if a given key is currently pressed in ::tick()
		sf::Keyboard::Key keyLeft;
		sf::Keyboard::Key keyRight;
		sf::Keyboard::Key keyUp;
		sf::Keyboard::Key keyDown;
		sf::Keyboard::Key keySprint;

	public:
		explicit Player(ResourceManager &resMgr);
		void nextFrame();
		void tick(uint lastFrameDurationUs);
		void debugDrawBounds(sf::RenderTarget &target, sf::RenderStates &states) const;
		virtual void draw(sf::RenderTarget &target, sf::RenderStates states) const;
};
