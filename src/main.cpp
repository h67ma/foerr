// SPDX-License-Identifier: GPL-3.0-only
//
// (c) 2022-2024 h67ma <szycikm@gmail.com>

#include <csignal>
#include <cstdlib>

#include <functional>
#include <iostream>

#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include <SFML/Window/Mouse.hpp>

#include "campaigns/campaign.hpp"
#include "consts.hpp"
#include "hud/dev_console.hpp"
#include "hud/fps_meter.hpp"
#include "hud/loading_screen.hpp"
#include "hud/log.hpp"
#include "hud/main_menu/main_menu.hpp"
#include "hud/pipbuck/pipbuck.hpp"
#include "hud/sliders/slider.hpp"
#include "resources/resource_manager.hpp"
#include "settings/keymap.hpp"
#include "settings/settings_manager.hpp"
#include "util/i18n.hpp"
#include "window/cursor_manager.hpp"
#include "window/util.hpp"

/**
 * Let all applicable components know that settings have changed and they need to update things like GUI scale, colors,
 * etc.
 */
static void propagateSettings(PipBuck& pipBuck, MainMenu& mainMenu, FpsMeter& fpsMeter, DevConsole& console,
							  const sf::Vector2u& newWindowSize)
{
	Slider::calculateCoeffs();
	Log::handleSettingsChange();
	pipBuck.handleSettingsChange();
	pipBuck.handleScreenResize(newWindowSize);
	mainMenu.handleSettingsChange();
	fpsMeter.handleSettingsChange();
	console.handleSettingsChange();
	console.handleScreenResize(newWindowSize);
}

int main()
{
	GameState gameState = STATE_MAINMENU; // TODO? potentially could be made static

	SettingsManager::setup();
	Log::setup(); // must be called after SettingsManager::setup() and before SettingsManager::loadConfig()

	if (!SettingsManager::generatePathsAndMkdir())
	{
		Log::e(STR_CREATE_DIRS_FAIL);
		exit(1);
	}

	SettingsManager::loadConfig();
	Log::openLogFile();

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

	Log::setFont(resManager.getFont(FONT_NORMAL));
	Log::handleSettingsChange();
	Log::handleScreenResize(window.getSize());

	Log::d("Save dir = %s", SettingsManager::getSaveDir().c_str());

	// initial coefficient calculation for all sliders
	Slider::calculateCoeffs();

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

	CursorManager cursorMgr(window);
	if (!cursorMgr.loadCursors())
	{
		Log::e(STR_CURSOR_LOAD_FAIL);
		window.close();
		exit(1);
	}

	Campaign campaign(resManager);
	PipBuck pipBuck(resManager, cursorMgr, campaign, gameState);
	if (!pipBuck.setup())
	{
		Log::e(STR_PIPBUCK_SETUP_FAILED);
		window.close();
		exit(1);
	}

	// hook up the Log PipBuck page to receive log messages
	Log::setMsgAddedCallback([&pipBuck](const StringAndColor& strAndColor) { pipBuck.addLogMessage(strAndColor); });

	MainMenu mainMenu(resManager, cursorMgr, window, campaign, gameState, pipBuck);

	DevConsole console(window.getSize(), *resManager.getFont(FONT_FIXED), campaign);

	// this is kinda messy but probably faster than a very long switch-case.
	// for each keypress we have to go through two maps: key -> action, action -> callback.
	// TODO? we could manage with a single map: key -> callback. we'd need a method in Keymap that populates below two
	// (or more) maps (mapping key to callback), which we'd call each time the mapping has changed and on program start.
	// while this would improve performance a bit, it would also significantly complicate things and possibly introduce
	// bugs. for now the current simple solution is fine, until we face issues with keypress response time.
	std::unordered_map<KeyAction, std::function<void(void)>> playingCbs {
		{ ACTION_PIPBUCK_TOGGLE_OPEN, [&pipBuck, &window]() { pipBuck.open(sf::Mouse::getPosition(window)); } },

		{ ACTION_PIPB_GOTO_LOAD,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_LOAD, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_SAVE,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_SAVE, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_SETTINGS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_SETTINGS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_CONTROLS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_CONTROLS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_LOG,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_LOG, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_STATUS_MAIN,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_STATUS_MAIN, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_SKILLS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_SKILLS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_PERKS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_PERKS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_EFFECTS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_EFFECTS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_HEALTH,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_HEALTH, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_WEAPONS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_WEAPONS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_ARMOR,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_ARMOR, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_EQUIPMENT,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_EQUIPMENT, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_INVENTORY_OTHER,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_INVENTORY_OTHER, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_AMMO,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_AMMO, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_MAP,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_MAP, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_WORLD,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_WORLD, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_QUESTS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_QUESTS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_NOTES,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_NOTES, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_ENEMIES,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_ENEMIES, sf::Mouse::getPosition(window)); } },

		{ ACTION_DEBUG_NAV_LEFT,
		  [&campaign]()
		  {
			  if (SettingsManager::debugNavigation)
				  campaign.gotoRoom(DIR_LEFT);
		  } },
		{ ACTION_DEBUG_NAV_RIGHT,
		  [&campaign]()
		  {
			  if (SettingsManager::debugNavigation)
				  campaign.gotoRoom(DIR_RIGHT);
		  } },
		{ ACTION_DEBUG_NAV_UP,
		  [&campaign]()
		  {
			  if (SettingsManager::debugNavigation)
				  campaign.gotoRoom(DIR_UP);
		  } },
		{ ACTION_DEBUG_NAV_DOWN,
		  [&campaign]()
		  {
			  if (SettingsManager::debugNavigation)
				  campaign.gotoRoom(DIR_DOWN);
		  } },
		{ ACTION_DEBUG_NAV_FRONT,
		  [&campaign]()
		  {
			  if (SettingsManager::debugNavigation)
				  campaign.gotoRoom(DIR_FRONT);
		  } },
		{ ACTION_DEBUG_NAV_BACK,
		  [&campaign]()
		  {
			  if (SettingsManager::debugNavigation)
				  campaign.gotoRoom(DIR_BACK);
		  } },

		{ ACTION_TOGGLE_FULLSCREEN, [&window, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu, &console]()
		  { toggleFullscreen(window, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console); } },

		{ ACTION_DEBUG_TOGGLE_CONSOLE, [&console]() { console.open(); } },
		{ ACTION_DEBUG_REPEAT_LAST_CONSOLE_CMD,
		  [&console, &window, &gameWorldView]() { console.executeLast(getMousePos(window, gameWorldView)); } },
	};

	std::unordered_map<KeyAction, std::function<void(void)>> pipBuckCbs {
		{ ACTION_PIPBUCK_TOGGLE_OPEN, [&pipBuck]() { pipBuck.close(); } },

		{ ACTION_PIPB_GOTO_LOAD,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_LOAD, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_SAVE,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_SAVE, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_SETTINGS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_SETTINGS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_CONTROLS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_CONTROLS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_LOG,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_LOG, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_STATUS_MAIN,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_STATUS_MAIN, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_SKILLS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_SKILLS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_PERKS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_PERKS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_EFFECTS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_EFFECTS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_HEALTH,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_HEALTH, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_WEAPONS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_WEAPONS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_ARMOR,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_ARMOR, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_EQUIPMENT,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_EQUIPMENT, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_INVENTORY_OTHER,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_INVENTORY_OTHER, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_AMMO,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_AMMO, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_MAP,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_MAP, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_WORLD,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_WORLD, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_QUESTS,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_QUESTS, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_NOTES,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_NOTES, sf::Mouse::getPosition(window)); } },
		{ ACTION_PIPB_GOTO_ENEMIES,
		  [&pipBuck, &window]() { pipBuck.switchToPage(PIPB_PAGE_ENEMIES, sf::Mouse::getPosition(window)); } },

		{ ACTION_TOGGLE_FULLSCREEN, [&window, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu, &console]()
		  { toggleFullscreen(window, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console); } },

		{ ACTION_DEBUG_TOGGLE_CONSOLE, [&console]() { console.open(); } },
		{ ACTION_DEBUG_REPEAT_LAST_CONSOLE_CMD, [&console]() { console.executeLast(); } },
	};

	std::unordered_map<KeyAction, std::function<void(void)>> mainMenuCbs {
		{ ACTION_TOGGLE_FULLSCREEN, [&window, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu, &console]()
		  { toggleFullscreen(window, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console); } },

		{ ACTION_DEBUG_TOGGLE_CONSOLE, [&console]() { console.open(); } },
		{ ACTION_DEBUG_REPEAT_LAST_CONSOLE_CMD, [&console]() { console.executeLast(); } },
	};

	// initial size
	windowSizeChanged(window.getSize(), fpsMeter, hudView, gameWorldView, pipBuck, mainMenu, console);

	cursorMgr.setCursor(POINTER);

	// autoload campaign
	if (!SettingsManager::debugAutoloadCampaign.empty())
	{
		Log::d(STR_AUTLOADING_CAMPAIGN);

		if (campaign.load(SettingsManager::debugAutoloadCampaign) && pipBuck.setupCampaignInfos())
		{
			gameState = STATE_PLAYING;

			// TODO query campaign to check what the player is actually pointing at and set proper cursor color
			cursorMgr.setCursor(CROSSHAIR_WHITE);
		}
	}

	tickTimer.restart(); // so that all initialization above won't be counted
	sf::Event event;
	enum KeyAction action;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			// key up/down needs to be handled for all game states in order to always keep information about currently
			// pressed keys up to date. note that in all key up/down events after this, there's no need to translate
			// keycode to action, as it's already done here
			if (event.type == sf::Event::KeyPressed)
			{
				action = Keymap::keyToAction(event.key.code);
				if (Keymap::handleKeyUpDown(action, true))
					continue;
			}
			else if (event.type == sf::Event::KeyReleased)
			{
				action = Keymap::keyToAction(event.key.code);
				if (Keymap::handleKeyUpDown(action, false))
					continue;
			}

			if (console.getIsOpen())
			{
				if (event.type == sf::Event::KeyPressed)
				{
					console.handleKeyPressed(event.key.code, getMousePos(window, gameWorldView));
					continue;
				}

				if (event.type == sf::Event::TextEntered)
				{
					console.handleTextEntered(event.text.unicode);
					continue;
				}
			}
			else if (gameState == STATE_PLAYING)
			{
				if (event.type == sf::Event::KeyPressed)
				{
					auto search = playingCbs.find(action);
					if (search != playingCbs.end())
						search->second();

					continue;
				}

				if (event.type == sf::Event::LostFocus)
				{
					if (SettingsManager::pauseOnFocusLoss)
						pipBuck.open(sf::Mouse::getPosition(window), false);

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

				if (event.type == sf::Event::KeyPressed)
				{
					auto search = pipBuckCbs.find(action);
					if (search != pipBuckCbs.end())
						search->second();

					continue;
				}

				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == sf::Mouse::Left &&
						pipBuck.handleLeftClick({ event.mouseButton.x, event.mouseButton.y }) ==
							CLICK_CONSUMED_SETTINGS_CHANGED)
					{
						propagateSettings(pipBuck, mainMenu, fpsMeter, console, window.getSize());
					}

					continue;
				}

				if (event.type == sf::Event::MouseButtonReleased)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
						pipBuck.handleLeftClickUp();

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

				if (event.type == sf::Event::MouseButtonPressed)
				{
					if (event.mouseButton.button == sf::Mouse::Left &&
						mainMenu.handleLeftClick({ event.mouseButton.x, event.mouseButton.y }) ==
							CLICK_CONSUMED_SETTINGS_CHANGED)
					{
						propagateSettings(pipBuck, mainMenu, fpsMeter, console, window.getSize());
					}

					continue;
				}

				if (event.type == sf::Event::MouseButtonReleased)
				{
					if (event.mouseButton.button == sf::Mouse::Left)
						mainMenu.handleLeftClickUp();

					continue;
				}

				if (event.type == sf::Event::KeyPressed)
				{
					auto search = mainMenuCbs.find(action);
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

			if (event.type == sf::Event::Resized)
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
			pipBuck.tick();

		Log::tick();

		if (SettingsManager::showFpsCounter)
			fpsMeter.tick();

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
