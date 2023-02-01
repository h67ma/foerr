#include <signal.h>
#include <stdlib.h>

#include <functional>
#include <iostream>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>

#include "resources/resource_manager.hpp"
#include "settings/settings_manager.hpp"
#include "window/cursor_manager.hpp"
#include "hud/pipbuck/pipbuck.hpp"
#include "hud/loading_screen.hpp"
#include "campaigns/campaign.hpp"
#include "settings/keymap.hpp"
#include "hud/dev_console.hpp"
#include "hud/fps_meter.hpp"
#include "hud/main_menu.hpp"
#include "window/util.hpp"
#include "util/i18n.hpp"
#include "hud/log.hpp"
#include "consts.hpp"

int main()
{
	GameState gameState = STATE_MAINMENU; // TODO? potentially could be made static

	SettingsManager::setup();

	if (!SettingsManager::generatePathsAndMkdir())
	{
		Log::e(STR_CREATE_DIRS_FAIL);
		exit(1);
	}

	SettingsManager::loadConfig();

	sf::RenderWindow window;
	sf::View gameWorldView({ GAME_AREA_MID_X, GAME_AREA_MID_Y }, { GAME_AREA_WIDTH, GAME_AREA_HEIGHT });
	sf::View hudView;
	sf::Clock animationTimer;
	sf::Clock tickTimer; // for updating physics and such

	recreateWindow(window);

	ResourceManager resManager;

	if (!resManager.loadFonts())
	{
		Log::e(STR_LOAD_FONTS_FAIL);
		window.close();
		exit(1);
	}

	LoadingScreen loadingScreen(resManager, window.getSize());
	window.clear();
	window.draw(loadingScreen);
	window.display();

	if (SettingsManager::debugWriteLogToFile)
		Log::openLogFile(pathCombine(SettingsManager::getGameRootDir(), PATH_LOGFILE));

	Log::setFont(resManager.getFont(FONT_NORMAL));
	Log::setPosition(SettingsManager::logAnchor, window.getSize());
	Log::setWriteLogToFile(SettingsManager::debugWriteLogToFile);
	Log::setPrintMsgs(SettingsManager::debugPrintToStderr);
	Log::setVerboseDebug(SettingsManager::debugVerbose);
	Log::setGuiScale(SettingsManager::guiScale);

	Log::d("Save dir = %s", SettingsManager::getSaveDir().c_str());

	if (!resManager.loadCore())
	{
		Log::e(STR_LOAD_CORE_FAIL);
		window.close();
		exit(1);
	}

	if (!Keymap::setup())
	{
		Log::e(STR_KEYMAP_SETUP_FAIL);
		window.close();
		exit(1);
	}

	Keymap::load();

	FpsMeter fpsMeter(*resManager.getFont(FONT_NORMAL), window.getSize());

	CursorManager cursorMgr;
	if (!cursorMgr.loadCursors())
	{
		Log::e(STR_CURSOR_LOAD_FAIL);
		window.close();
		exit(1);
	}

	cursorMgr.setCursor(window, POINTER);

	Campaign campaign(resManager);
	PipBuck pipBuck(resManager, campaign, gameState);
	if (!pipBuck.setup())
	{
		Log::e(STR_PIPBUCK_SETUP_FAILED);
		window.close();
		exit(1);
	}

	pipBuck.setRadLevel(0.3f); // TODO remove
	MainMenu mainMenu(resManager, window, campaign, gameState, pipBuck);

	DevConsole console(window.getSize(), *resManager.getFont(FONT_FIXED), campaign);

	// this is kinda messy but probably faster than a very long switch-case.
	// for each keypress we have to go through two maps: key -> action, action -> callback.
	// TODO? we could manage with a single map: key -> callback. we'd need a method in Keymap that populates below two
	// (or more) maps (mapping key to callback), which we'd call each time the mapping has changed and on program start.
	// while this would improve performance a bit, it would also significantly complicate things and possibly introduce
	// bugs. for now the current simple solution is fine, until we face issues with keypress response time.
	std::unordered_map<KeyAction, std::function<void(void)>> playingCbs {
		{ ACTION_PIPBUCK_TOGGLE_OPEN, [&pipBuck](){ pipBuck.open(); } },

		{ ACTION_PIPB_GOTO_LOAD, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_LOAD); } },
		{ ACTION_PIPB_GOTO_SAVE, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_SAVE); } },
		{ ACTION_PIPB_GOTO_SETTINGS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_SETTINGS); } },
		{ ACTION_PIPB_GOTO_CONTROLS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_CONTROLS); } },
		{ ACTION_PIPB_GOTO_LOG, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_LOG); } },
		{ ACTION_PIPB_GOTO_STATUS_MAIN, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_STATUS_MAIN); } },
		{ ACTION_PIPB_GOTO_SKILLS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_SKILLS); } },
		{ ACTION_PIPB_GOTO_PERKS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_PERKS); } },
		{ ACTION_PIPB_GOTO_EFFECTS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_EFFECTS); } },
		{ ACTION_PIPB_GOTO_HEALTH, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_HEALTH); } },
		{ ACTION_PIPB_GOTO_WEAPONS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_WEAPONS); } },
		{ ACTION_PIPB_GOTO_ARMOR, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_ARMOR); } },
		{ ACTION_PIPB_GOTO_EQUIPMENT, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_EQUIPMENT); } },
		{ ACTION_PIPB_GOTO_INVENTORY_OTHER, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_INVENTORY_OTHER); } },
		{ ACTION_PIPB_GOTO_AMMO, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_AMMO); } },
		{ ACTION_PIPB_GOTO_MAP, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_MAP); } },
		{ ACTION_PIPB_GOTO_WORLD, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_WORLD); } },
		{ ACTION_PIPB_GOTO_QUESTS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_QUESTS); } },
		{ ACTION_PIPB_GOTO_NOTES, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_NOTES); } },
		{ ACTION_PIPB_GOTO_ENEMIES, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_ENEMIES); } },

		{ ACTION_DEBUG_NAV_LEFT, [&campaign](){
			if (SettingsManager::debugNavigation)
				campaign.gotoRoom(DIR_LEFT);
		} },
		{ ACTION_DEBUG_NAV_RIGHT, [&campaign](){
			if (SettingsManager::debugNavigation)
				campaign.gotoRoom(DIR_RIGHT);
		} },
		{ ACTION_DEBUG_NAV_UP, [&campaign](){
			if (SettingsManager::debugNavigation)
				campaign.gotoRoom(DIR_UP);
		} },
		{ ACTION_DEBUG_NAV_DOWN, [&campaign](){
			if (SettingsManager::debugNavigation)
				campaign.gotoRoom(DIR_DOWN);
		} },
		{ ACTION_DEBUG_NAV_FRONT, [&campaign](){
			if (SettingsManager::debugNavigation)
				campaign.gotoRoom(DIR_FRONT);
		} },
		{ ACTION_DEBUG_NAV_BACK, [&campaign](){
			if (SettingsManager::debugNavigation)
				campaign.gotoRoom(DIR_BACK);
		} },

		{ ACTION_TOGGLE_FULLSCREEN, [&window, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu, &console](){
			toggleFullscreen(window, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console);
		} },

		{ ACTION_DEBUG_TOGGLE_CONSOLE, [&console](){ console.open(); } },
		{ ACTION_DEBUG_REPEAT_LAST_CONSOLE_CMD, [&console, &window, &gameWorldView](){
			console.executeLast(getMousePos(window, gameWorldView));
		} },
	};

	std::unordered_map<KeyAction, std::function<void(void)>> pipBuckCbs {
		{ ACTION_PIPBUCK_TOGGLE_OPEN, [&pipBuck](){ pipBuck.close(); } },

		{ ACTION_PIPB_GOTO_LOAD, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_LOAD); } },
		{ ACTION_PIPB_GOTO_SAVE, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_SAVE); } },
		{ ACTION_PIPB_GOTO_SETTINGS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_SETTINGS); } },
		{ ACTION_PIPB_GOTO_CONTROLS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_CONTROLS); } },
		{ ACTION_PIPB_GOTO_LOG, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_LOG); } },
		{ ACTION_PIPB_GOTO_STATUS_MAIN, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_STATUS_MAIN); } },
		{ ACTION_PIPB_GOTO_SKILLS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_SKILLS); } },
		{ ACTION_PIPB_GOTO_PERKS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_PERKS); } },
		{ ACTION_PIPB_GOTO_EFFECTS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_EFFECTS); } },
		{ ACTION_PIPB_GOTO_HEALTH, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_HEALTH); } },
		{ ACTION_PIPB_GOTO_WEAPONS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_WEAPONS); } },
		{ ACTION_PIPB_GOTO_ARMOR, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_ARMOR); } },
		{ ACTION_PIPB_GOTO_EQUIPMENT, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_EQUIPMENT); } },
		{ ACTION_PIPB_GOTO_INVENTORY_OTHER, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_INVENTORY_OTHER); } },
		{ ACTION_PIPB_GOTO_AMMO, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_AMMO); } },
		{ ACTION_PIPB_GOTO_MAP, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_MAP); } },
		{ ACTION_PIPB_GOTO_WORLD, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_WORLD); } },
		{ ACTION_PIPB_GOTO_QUESTS, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_QUESTS); } },
		{ ACTION_PIPB_GOTO_NOTES, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_NOTES); } },
		{ ACTION_PIPB_GOTO_ENEMIES, [&pipBuck](){ pipBuck.switchToPage(PIPB_PAGE_ENEMIES); } },

		{ ACTION_TOGGLE_FULLSCREEN, [&window, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu, &console](){
			toggleFullscreen(window, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console);
		} },

		{ ACTION_DEBUG_TOGGLE_CONSOLE, [&console](){ console.open(); } },
		{ ACTION_DEBUG_REPEAT_LAST_CONSOLE_CMD, [&console, &window, &gameWorldView](){
			console.executeLast();
		} },
	};

	std::unordered_map<KeyAction, std::function<void(void)>> mainMenuCbs {
		{ ACTION_TOGGLE_FULLSCREEN, [&window, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu, &console](){
			toggleFullscreen(window, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console);
		} },

		{ ACTION_DEBUG_TOGGLE_CONSOLE, [&console](){ console.open(); } },
		{ ACTION_DEBUG_REPEAT_LAST_CONSOLE_CMD, [&console, &window, &gameWorldView](){
			console.executeLast();
		} },
	};


	// TODO remove all comments below

	// howto set gui scale
	//settings.setGuiScale(SETT_GUI_SCALE, GUI_SMALL);
	//setGuiScale() for all buttons
	//fpsMeter.setGuiScale(GUI_SMALL);
	//Log::setGuiScale(GUI_SMALL);


	// TODO delet this: howto change cursor type: cursorMgr.setCursor(window, CROSSHAIR_WHITE);


	// initial size
	windowSizeChanged(window.getSize(), fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console);

	// autoload campaign
	if (SettingsManager::debugAutoloadCampaign != "")
	{
		Log::d(STR_AUTLOADING_CAMPAIGN);

		if (campaign.load(pathCombine("res/campaigns", SettingsManager::debugAutoloadCampaign)) &&
			pipBuck.setupCampaignInfos())
		{
			gameState = STATE_PLAYING;
		}
	}

	// TODO? there's a very minor visual bug that happens if e.g. player hovers over pipbuck btn,
	// closes pipbuck (by pressing the tab btn), moves cursor somewhere else, reopens pipbuck,
	// and the "old" hover is still being shown. fixing this would require saving mouse coords
	// each time mouse moves (yikes), and then calling pipbuck/menu handle mouse move method when
	// either of them is being opened. doesn't seems like something worth fixing tbh.

	tickTimer.restart(); // so that all initialization above won't be counted
	sf::Event event;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			if (console.getIsOpen())
			{
				if (event.type == sf::Event::KeyPressed)
				{
					console.handleKeyPressed(event.key.code, getMousePos(window, gameWorldView));
				}
				else if (event.type == sf::Event::TextEntered)
				{
					console.handleTextEntered(event.text.unicode);
				}

				continue;
			}
			else if (gameState == STATE_PLAYING)
			{
				if (event.type == sf::Event::KeyPressed)
				{
					enum KeyAction action = Keymap::keyToAction(event.key.code);

					auto search = playingCbs.find(action);
					if (search != playingCbs.end())
						search->second();
					else
						campaign.handleKeyUpDown(action, true);

					continue;
				}
				else if (event.type == sf::Event::KeyReleased)
				{
					campaign.handleKeyUpDown(Keymap::keyToAction(event.key.code), false);

					continue;
				}
				else if (event.type == sf::Event::LostFocus)
				{
					if (SettingsManager::pauseOnFocusLoss)
						pipBuck.open(false);

					continue;
				}
			}
			else if (gameState == STATE_PIPBUCK)
			{
				if (event.type == sf::Event::MouseMoved)
				{
					pipBuck.handleMouseMove({ event.mouseMove.x, event.mouseMove.y });

					continue;
				}
				else if (event.type == sf::Event::KeyPressed)
				{
					auto search = pipBuckCbs.find(Keymap::keyToAction(event.key.code));
					if (search != pipBuckCbs.end())
						search->second();

					continue;
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						pipBuck.handleLeftClick({ event.mouseButton.x, event.mouseButton.y });
					}

					continue;
				}
			}
			else if (gameState == STATE_MAINMENU)
			{
				if (event.type == sf::Event::MouseMoved)
				{
					mainMenu.handleMouseMove({ event.mouseMove.x, event.mouseMove.y });

					continue;
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					//sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						mainMenu.handleLeftClick({ event.mouseButton.x, event.mouseButton.y });
					}

					continue;
				}
				else if (event.type == sf::Event::KeyPressed)
				{
					auto search = mainMenuCbs.find(Keymap::keyToAction(event.key.code));
					if (search != mainMenuCbs.end())
						search->second();

					continue;
				}
			}

			// below events apply to all game states
			if (event.type == sf::Event::Closed)
			{
				// TODO autosave if campaign loaded
				Log::d(STR_SHUTTING_DOWN);

				Log::close();
				window.close();
				return 0;
			}
			else if (event.type == sf::Event::Resized)
			{
				windowSizeChanged(window.getSize(), fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console);
			}
		}

		//// update states /////

		// tick timer needs to run on every frame, even if campaign is not active. if it didn't run, then after a pause
		// (e.g. opening the PipBuck), the tick would receive a large value of elapsed microseconds (because the timer
		// didn't update when PipBuck was open), which could lead to very problematic physics problems.
		sf::Time frameDuration = tickTimer.restart();

		if (gameState == STATE_PLAYING && !console.getIsOpen())
			campaign.tick(frameDuration.asMicroseconds());
		else if (gameState == STATE_PIPBUCK)
			pipBuck.updateDraw();

		Log::maybeUpdate();

		if (SettingsManager::showFpsCounter)
			fpsMeter.maybeUpdate();

		// check if it's the time to change all frame-based animations to next frame
		bool drawNextFrame = animationTimer.getElapsedTime().asMilliseconds() >= ANIM_FRAME_DURATION_MS;
		if (drawNextFrame)
			animationTimer.restart();

		window.clear();

		///// draw game world entities /////

		window.setView(gameWorldView);

		if ((gameState == STATE_PLAYING || gameState == STATE_PIPBUCK) && campaign.isLoaded())
		{
			window.draw(campaign);

			if (gameState == STATE_PLAYING && drawNextFrame && !console.getIsOpen())
				campaign.nextFrame();
		}

		///// draw hud /////

		window.setView(hudView);

		if (gameState == STATE_PIPBUCK)
			window.draw(pipBuck);
		else if (gameState == STATE_MAINMENU)
			window.draw(mainMenu);

		if (console.getIsOpen())
			window.draw(console);

		if (SettingsManager::showFpsCounter)
			window.draw(fpsMeter);

		Log::draw(window);

		///// display window /////

		window.display();
	}

	return 0;
}
