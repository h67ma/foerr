#include <SFML/Graphics.hpp>
#include "../settings/settings_manager.hpp"
#include "../hud/fps_meter.hpp"
#include "../hud/pipbuck/pipbuck.hpp"

void recreateWindow(sf::RenderWindow &window, SettingsManager &settings);
void windowSizeChanged(sf::RenderWindow &window, SettingsManager &settings, FpsMeter &fpsMeter, sf::View &hudView, sf::View &gameWorldView, PipBuck &pipBuck);
