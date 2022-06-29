#include <iostream>
//#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
//#include <unistd.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include "window/window_cursor.hpp"
#include "i18n.hpp"
#include "consts.h"
#include "hud/log.hpp"
#include "settings/settings.hpp"
#include "hud/fps_meter.hpp"

//void stackTraceHandler(int sig) {
//	void *array[STACKTRACE_MAX_CNT];
//	size_t size;
//
//	// get void*'s for all entries on the stack
//	size = backtrace(array, STACKTRACE_MAX_CNT);
//
//	// print out all the frames to stderr
//	std::cerr << "Error: signal " << sig << std::endl;
//	backtrace_symbols_fd(array, size, STDERR_FILENO);
//	exit(1);
//}

void recreateWindow(sf::Window *window, Settings *settings)
{
	// TODO settings activity should detect valid video modes and list them, with the standard "confirm in 15 seconds" dialog after changing resolution.
	// the selected mode should be saved to settings file.
	// when loading settings, game should check if the specified video mode exists - sf::VideoMode::isValid()
	// when settings are uninitialized, game should pick *some* mode - either best, or "safe"

	if (settings->getBool(SETT_FULLSCREEN_ENABLED))
		// TODO support overriding fullscreen resolution via settings
		window->create(sf::VideoMode::getDesktopMode(), STR_WINDOW_TITLE, sf::Style::Fullscreen);
	else
		window->create(sf::VideoMode(1280, 720), STR_WINDOW_TITLE);

	if (settings->getBool(SETT_FPS_LIMIT_ENABLED))
		window->setFramerateLimit(settings->getUint(SETT_FPS_LIMIT));

	window->setVerticalSyncEnabled(settings->getBool(SETT_FAKE_VSYNC_ENABLED));
}

int main()
{
	Settings settings;
	//settings.saveConfig();
	settings.loadConfig();
	sf::RenderWindow window;
	sf::Font font;

	// TODO find a platform-independent way to display stack trace on crash
	//signal(SIGSEGV, stackTraceHandler);

	recreateWindow(&window, &settings); // actually create it for the first time, details

	if (!font.loadFromFile(PATH_FONT))
	{
		Log::logStderr(LOG_ERROR, STR_FONT_LOAD_FAIL);
		exit(1);
	}

	Log log(&window, &font, &settings);

	FpsMeter fpsMeter(&window, &font, settings.getUint(SETT_NORMAL_FONT_SIZE));
	
	WindowCursor cursor(&window, &log);
	if (!cursor.loadCursors(settings.getBool(SETT_PREFER_CUSTOM_CURSOR)))
	{
		log.log(LOG_ERROR, STR_CURSOR_LOAD_FAIL);
		exit(1);
	}

	cursor.setCursor(POINTER);

	sf::CircleShape circle(100.f);
	circle.setFillColor(sf::Color::Green);

	sf::RectangleShape rectangle(sf::Vector2f(120.f, 50.f));
	rectangle.setFillColor(sf::Color::Blue);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			switch (event.type)
			{
				case sf::Event::Closed:
					window.close();
					break;
				case sf::Event::Resized:
					window.setView(sf::View(sf::FloatRect(0.f, 0.f, event.size.width, event.size.height)));
					break;
				case sf::Event::LostFocus:
					// TODO actually pause game
					log.log(LOG_DEBUG, "Game lost focus");
					break;
				case sf::Event::TextEntered:
					if (event.text.unicode < 128)
						log.log(LOG_DEBUG, "ASCII character typed: %c", static_cast<char>(event.text.unicode));
					break;
				case sf::Event::KeyPressed:
					switch (event.key.code)
					{
						case sf::Keyboard::F11:
							if (settings.getBool(SETT_FULLSCREEN_ENABLED))
							{
								settings.setBool(SETT_FULLSCREEN_ENABLED, false);
								log.log(LOG_DEBUG, "Switching to windowed mode");
							}
							else
							{
								settings.setBool(SETT_FULLSCREEN_ENABLED, true);
								log.log(LOG_DEBUG, "Switching to fullscreen mode");
							}

							recreateWindow(&window, &settings);
							break;
						default:
							break;
					}
					break;
				default:
					break;
			}
		}

		window.clear();

		window.draw(circle);
		window.draw(rectangle);

		// gui
		log.draw();

		if (settings.getBool(SETT_SHOW_FPS_COUNTER))
			fpsMeter.draw(); // also won't count frame times when disabled, but the clock will be initialized at program start either way

		window.display();
	}

	return 0;
}
