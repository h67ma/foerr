// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2023 h67ma <szycikm@gmail.com>

#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include <SFML/Window/Mouse.hpp>

#include "../hud/dev_console.hpp"
#include "../hud/fps_meter.hpp"
#include "../hud/main_menu.hpp"
#include "../hud/pipbuck/pipbuck.hpp"

void recreateWindow(sf::RenderWindow& window);
void windowSizeChanged(sf::Vector2u windowSize, FpsMeter& fpsMeter, sf::View& hudView, sf::View& gameWorldView,
					   PipBuck& pipBuck, MainMenu& mainMenu, DevConsole& console);
void toggleFullscreen(sf::RenderWindow& window, FpsMeter& fpsMeter, sf::View& hudView, sf::View& gameWorldView,
					  PipBuck& pipBuck, MainMenu& mainMenu, DevConsole& console);

/**
 * Returns the coordinates of current mouse postion in world units.
 */
inline sf::Vector2f getMousePos(const sf::RenderWindow& window, const sf::View& view)
{
	return window.mapPixelToCoords(sf::Mouse::getPosition(window), view);
}
