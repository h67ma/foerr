#include "animation.hpp"
/**
 * @param textureImg texture resource containing animation spritesheet
 * @param width width (in px) of a single animation frame
 * @param height height (in px) of a single animation frame
 * @param kinds vector of animation kinds which the spritesheet contains. Order matters.
 */
Animation::Animation(const sf::Texture &texture, uint width, uint height, const std::vector<struct anim_kind_details> kinds)
{
	this->width = width;
	this->height = height;

	this->textureRect = sf::IntRect(0, 0, width, height);

	this->textureHeight = texture.getSize().y;

	this->sprite.setTexture(texture);
	sprite.setTextureRect(this->textureRect);

	uint topOffset = 0;
	bool first = true;
	for (const struct anim_kind_details kind : kinds)
	{
		if (first)
		{
			this->loadedKindTextureWidth = kind.frameCnt * width; // save frame count for the initial/default animation
			first = false;
		}

		this->kinds[kind.kind] = {
			kind.frameCnt,
			topOffset
		};
		topOffset += height;
	}
}

/**
 * Checks if frame should be changed (depending on time of last update) and updates frame if needed.
 */
void Animation::maybeNextFrame()
{
	// TODO? it is possible that the first frame will not be drawn - will this be a visible problem? a simple solution would be to artificially set the timer to >= ANIM_FRAME_DURATION_MS at initialization
	if (this->clock.getElapsedTime().asMilliseconds() < ANIM_FRAME_DURATION_MS)
		return;

	this->textureRect.left += this->width;

	if (this->textureRect.left >= static_cast<int>(this->loadedKindTextureWidth))
		this->textureRect.left = 0;

	this->sprite.setTextureRect(this->textureRect);
	this->clock.restart();
}

/**
 * Switches animation to the specified kind. If the requested kind is not defined for this object,
 * or the texture image is not tall enough, current animation won't be changed and previously
 * selected animation will continue playing.
 * 
 * @param kind animation kind
 * @returns `true` if animation was successfuly changed, `false` otherwise.
 */
bool Animation::setAnimation(AnimationKind kind)
{
	if (this->kinds.find(kind) == this->kinds.end())
		return false; // requested animation kind not found in this object

	struct anim_kind_details_internal details = this->kinds[kind];

	if (details.offsetTop + this->height > this->textureHeight)
		return false; // texture is not tall enough

	this->textureRect.top = details.offsetTop;
	this->loadedKindTextureWidth = details.frameCnt * this->width;
	return true;
}

void Animation::draw(sf::RenderTarget& target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();
	target.draw(this->sprite, states);
}
