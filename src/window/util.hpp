#pragma once

#include <SFML/Graphics/RenderWindow.hpp>
#include <SFML/Graphics/View.hpp>
#include "../hud/fps_meter.hpp"
#include "../hud/pipbuck/pipbuck.hpp"
#include "../hud/main_menu.hpp"

void recreateWindow(sf::RenderWindow &window);
void windowSizeChanged(sf::Vector2u windowSize, FpsMeter &fpsMeter, sf::View &hudView, sf::View &gameWorldView,
					   PipBuck &pipBuck, MainMenu &mainMenu);
void toggleFullscreen(sf::RenderWindow &window, FpsMeter &fpsMeter, sf::View &hudView, sf::View &gameWorldView,
					  PipBuck &pipBuck, MainMenu &mainMenu);
