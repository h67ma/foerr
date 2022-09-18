#include "animation.hpp"
/**
 * @param texture texture resource containing animation spritesheet
 * @param width width (in px) of a single animation frame
 * @param height height (in px) of a single animation frame
 * @param kinds vector of animation kinds which the spritesheet contains. Order matters.
 */
Animation::Animation(std::shared_ptr<sf::Texture> texture, uint width, uint height, const std::vector<struct anim_kind_details> kinds) :
	sprite(texture),
	textureRect(0, 0, width, height),
	width(width),
	height(height)
{
	this->textureHeight = texture->getSize().y;

	sprite.get().setTextureRect(this->textureRect);

	uint topOffset = 0;
	bool first = true;
	for (const struct anim_kind_details kind : kinds)
	{
		if (first)
		{
			// initial/default animation
			this->loadedKindTextureWidth = kind.frameCnt * width;
			this->loadedKindIsSingleFrame = kind.frameCnt == 1;
			first = false;
		}

		this->kinds[kind.kind] = {
			kind.frameCnt,
			topOffset
		};
		topOffset += height;
	}
}

void Animation::moveTexture()
{
	this->textureRect.left += this->width;

	if (this->textureRect.left >= static_cast<int>(this->loadedKindTextureWidth))
		this->textureRect.left = 0;

	this->sprite.get().setTextureRect(this->textureRect);
}

void Animation::nextFrame()
{
	if (this->loadedKindIsSingleFrame)
		return; // no need to do anything for "static animation"

	this->moveTexture();
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
	this->loadedKindIsSingleFrame = details.frameCnt == 1;

	if (this->loadedKindIsSingleFrame)
		this->moveTexture(); // the only txt move that will happen for "static animation"

	return true;
}

void Animation::draw(sf::RenderTarget &target, sf::RenderStates states) const
{
	states.transform *= this->getTransform();
	target.draw(this->sprite.sprite, states);
}
