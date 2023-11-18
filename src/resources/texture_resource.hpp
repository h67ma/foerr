// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <memory>

#include <SFML/Graphics/Texture.hpp>

/**
 * Used to track usage of resources in ResourceManager (shared ptr must be kept
 * in order not to unload Texture on ::cleanUnused() when it's actually being used).
 *
 * Note: this type exists to improve readability by suggesting that a texture is managed by Resource Manager.
 */

typedef std::shared_ptr<sf::Texture> TextureResource;
