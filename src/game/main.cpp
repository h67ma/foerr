#include <iostream>
//#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
//#include <unistd.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Audio.hpp>
#include "window/window_cursor.hpp"
#include "util/i18n.hpp"
#include "consts.h"
#include "hud/log.hpp"
#include "settings/settings_manager.hpp"
#include "resource_manager.hpp"
#include "hud/fps_meter.hpp"
#include "hud/button.hpp"
#include "entities/animation.hpp"

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

void recreateWindow(sf::Window *window, SettingsManager *settings)
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
	SettingsManager settings;
	settings.loadConfig();

	sf::RenderWindow window;
	uint windowW, windowH;
	sf::Font fontMedium;
	sf::Font fontNormal;
	std::list<Button*> buttons;
	std::vector<Animation*> fires;
	Animation* fire;

	// TODO find a platform-independent way to display stack trace on crash
	//signal(SIGSEGV, stackTraceHandler);

	recreateWindow(&window, &settings); // actually create it for the first time, details
	windowW = window.getSize().x;
	windowH = window.getSize().y;

	if (!fontNormal.loadFromFile(PATH_FONT_NORMAL))
	{
		Log::logStderr(LOG_ERROR, STR_FONT_LOAD_FAIL);
		exit(1);
	}

	if (!fontMedium.loadFromFile(PATH_FONT_MEDIUM))
	{
		Log::logStderr(LOG_ERROR, STR_FONT_LOAD_FAIL);
		exit(1);
	}

	Log log(&fontNormal);
	log.setPosition(settings.getScreenCorner(SETT_ANCHOR_LOG), windowW, windowH);

	ResourceManager resManager(&log);

	FpsMeter fpsMeter(&fontNormal, FONT_SIZE_NORMAL);
	fpsMeter.setPosition(settings.getScreenCorner(SETT_ANCHOR_FPS), windowW, windowH);
	
	WindowCursor cursor(&log);
	if (!cursor.loadCursors(settings.getBool(SETT_PREFER_CUSTOM_CURSOR)))
	{
		log.log(LOG_ERROR, STR_CURSOR_LOAD_FAIL);
		exit(1);
	}

	cursor.setCursor(&window, POINTER);

	sf::Text dummyTab;
	dummyTab.setFont(fontMedium);
	dummyTab.setPosition(200, 300);
	dummyTab.setString("Weapons");



	Button tab1Btn(100, 100, BTN_NARROW, "Weapons", &fontMedium);
	Button tab2Btn(240, 100, BTN_NARROW, "Armor", &fontMedium);
	Button tab3Btn(380, 100, BTN_NARROW, "Equipment", &fontMedium);
	
	tab1Btn.setCallback([&dummyTab, &tab1Btn, &tab2Btn, &tab3Btn]() {
		tab1Btn.setSelected(true);
		tab2Btn.setSelected(false);
		tab3Btn.setSelected(false);
		dummyTab.setString("Weapons");
	});

	tab2Btn.setCallback([&dummyTab, &tab1Btn, &tab2Btn, &tab3Btn]() {
		tab1Btn.setSelected(false);
		tab2Btn.setSelected(true);
		tab3Btn.setSelected(false);
		dummyTab.setString("Armor");
	});

	tab3Btn.setCallback([&dummyTab, &tab1Btn, &tab2Btn, &tab3Btn]() {
		tab1Btn.setSelected(false);
		tab2Btn.setSelected(false);
		tab3Btn.setSelected(true);
		dummyTab.setString("Equipment");
	});

	tab1Btn.setSelected(true);

	buttons.push_back(&tab1Btn);
	buttons.push_back(&tab2Btn);
	buttons.push_back(&tab3Btn);

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("res/audio/13_skill.wav"))
		return -1;

	sf::Sound sound;
	sound.setBuffer(buffer);

	Button debugBtn(400, 400, BTN_BIG, "DEBUG", &fontMedium, [&sound]() {
		sound.play();
	});
	buttons.push_back(&debugBtn);

	Button saveBtn(100, 500, BTN_NORMAL, "Save config", &fontMedium, [&settings]() {
		settings.saveConfig();
	});
	buttons.push_back(&saveBtn);

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
					fpsMeter.setPosition(settings.getScreenCorner(SETT_ANCHOR_FPS), event.size.width, event.size.height);
					log.setPosition(settings.getScreenCorner(SETT_ANCHOR_LOG), event.size.width, event.size.height);
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
				case sf::Event::MouseButtonPressed:
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						for(std::list<Button*>::iterator btn = buttons.begin(); btn != buttons.end(); btn++)
						{
							if ((*btn)->maybeHandleClick(event.mouseButton.x, event.mouseButton.y))
								break; // click consumed, no need to check other buttons
						}
					}

					// excercie: set selected point on fire
					fire = new Animation(*resManager.getImage(IMG_FIRE), 50, 67);
					fire->setPosition(event.mouseButton.x, event.mouseButton.y);
					fires.push_back(fire);
					break;
				default:
					break;
			}
		}

		window.clear();

		// entities
		for(Animation* fire : fires)
		{
			fire->maybeNextFrame();
			window.draw(*fire);
		}

		// hud
		for(std::list<Button*>::iterator btn = buttons.begin(); btn != buttons.end(); btn++)
		{
			(*btn)->draw(&window);
		}

		window.draw(dummyTab);

		log.maybeUpdate();
		window.draw(log);

		if (settings.getBool(SETT_SHOW_FPS_COUNTER))
		{
			// the clock will be initialized at program start either way, but fps won't be calculated if disabled
			fpsMeter.maybeUpdate();
			window.draw(fpsMeter);
		}

		window.display();
	}

	for(Animation* fire : fires)
	{
		delete fire;
	}

	return 0;
}
