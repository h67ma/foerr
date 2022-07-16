#include "animation.hpp"

Animation::Animation(sf::Image textureImg, uint width, uint height)
{
	this->width = width;
	this->height = height;

	this->textureRect = sf::IntRect(0, 0, width, height);

	this->texture.loadFromImage(textureImg);

	this->textureWidth = this->texture.getSize().x;

	this->texture.setSmooth(true);
	this->sprite.setTexture(this->texture);
	sprite.setTextureRect(this->textureRect);
}

/**
 * Checks if frame should be changed (depending on time of last update) and updates frame if needed.
 * TODO? it is possible that the first frame will not be drawn - will this be a visible problem? a simple solution would be to artificially set the timer to >= ANIM_FRAME_DURATION_MS at initialization
 */
void Animation::maybeNextFrame()
{
	if (this->clock.getElapsedTime().asMilliseconds() < ANIM_FRAME_DURATION_MS)
		return;

	this->textureRect.left += this->width;

	if (this->textureRect.left >= this->textureWidth)
		this->textureRect.left = 0;

	this->sprite.setTextureRect(this->textureRect);
	this->clock.restart();
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();
	target.draw(this->sprite, states);
}
