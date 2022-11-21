#include "util.hpp"
#include "../util/i18n.hpp"
#include "../hud/log.hpp"
#include "../consts.hpp"

void recreateWindow(sf::RenderWindow &window, SettingsManager &settings)
{
	// TODO settings activity should detect valid video modes and list them, with the standard "confirm in 15 seconds"
	// dialog after changing resolution.
	// the selected mode should be saved to settings file.
	// when loading settings, game should check if the specified video mode exists - sf::VideoMode::isValid()
	// when settings are uninitialized, game should pick *some* mode - either best, or "safe"

	sf::ContextSettings context;
	context.antialiasingLevel = settings.getUint(SETT_AA);

	if (settings.getBool(SETT_FULLSCREEN_ENABLED))
		// TODO support overriding fullscreen resolution via settings
		window.create(sf::VideoMode::getDesktopMode(),
					  STR_WINDOW_TITLE, sf::Style::Fullscreen, context);
	else
		window.create(sf::VideoMode(settings.getUint(SETT_WINDOW_WIDTH),
									settings.getUint(SETT_WINDOW_HEIGHT)),
					  STR_WINDOW_TITLE, sf::Style::Default, context);

	if (settings.getBool(SETT_FPS_LIMIT_ENABLED))
		window.setFramerateLimit(settings.getUint(SETT_FPS_LIMIT));

	window.setVerticalSyncEnabled(settings.getBool(SETT_FAKE_VSYNC_ENABLED));
}

/**
 * Compares the aspect ratio of the window to the aspect ratio of the view,
 * and sets the view's viewport accordingly in order to archieve a letterbox effect.
 * A new view (with a new viewport set) is returned.
 *
 * Stolen from https://github.com/SFML/SFML/wiki/Source%3A-Letterbox-effect-using-a-view & slightly modified.
 * Really, this should've been build into SFML.
 */
void setLetterboxView(sf::View &view, sf::Vector2u windowSize)
{
	float windowRatio = windowSize.x / static_cast<float>(windowSize.y);
	float viewRatio = view.getSize().x / static_cast<float>(view.getSize().y);
	float sizeX = 1;
	float sizeY = 1;
	float posX = 0;
	float posY = 0;

	if (windowRatio < viewRatio)
	{
		// black bars on top and bottom
		sizeY = windowRatio / viewRatio;
		posY = (1 - sizeY) / 2.f;
	}
	else
	{
		// black bars on left and right side
		sizeX = viewRatio / windowRatio;
		posX = (1 - sizeX) / 2.f;
	}

	view.setViewport(sf::FloatRect(posX, posY, sizeX, sizeY));
}

void windowSizeChanged(sf::Vector2u windowSize, SettingsManager &settings, FpsMeter &fpsMeter, sf::View &hudView, sf::View &gameWorldView, PipBuck &pipBuck, MainMenu &mainMenu)
{
	// update position of dockable elements
	Log::setPosition(settings.getScreenCorner(SETT_ANCHOR_LOG), windowSize);
	fpsMeter.setPosition(settings.getScreenCorner(SETT_ANCHOR_FPS), windowSize);
	pipBuck.handleScreenResize(windowSize);
	mainMenu.handleScreenResize(windowSize);

	// update views
	hudView.reset({ 0.f, 0.f, static_cast<float>(windowSize.x), static_cast<float>(windowSize.y) });
	setLetterboxView(gameWorldView, windowSize);
}

void toggleFullscreen(sf::RenderWindow &window, SettingsManager &settings, FpsMeter &fpsMeter, sf::View &hudView, sf::View &gameWorldView, PipBuck &pipBuck, MainMenu &mainMenu)
{
	if (settings.getBool(SETT_FULLSCREEN_ENABLED))
	{
		settings.setBool(SETT_FULLSCREEN_ENABLED, false);
		Log::d(STR_WINDOW_WINDOWED);
	}
	else
	{
		settings.setBool(SETT_FULLSCREEN_ENABLED, true);
		Log::d(STR_WINDOW_FULLSCREEN);
	}

	recreateWindow(window, settings);
	windowSizeChanged(window.getSize(), settings, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu);
}
