#include <signal.h>
#include <stdlib.h>
#include <iostream>
#include <functional>
#include <SFML/System/Clock.hpp>
#include <SFML/Window/Event.hpp>
#include "window/cursor_manager.hpp"
#include "util/i18n.hpp"
#include "consts.hpp"
#include "hud/log.hpp"
#include "settings/settings_manager.hpp"
#include "resources/resource_manager.hpp"
#include "hud/fps_meter.hpp"
#include "hud/buttons/simple_button.hpp" // TODO delet this
#include "entities/animation.hpp" // TODO delet this
#include "campaigns/campaign.hpp"
#include "window/util.hpp"
#include "hud/hover_manager.hpp" // TODO delet this
#include "hud/pipbuck/pipbuck.hpp"
#include "hud/main_menu.hpp"
#include "hud/loading_screen.hpp"
#include "settings/keymap.hpp"

int main()
{
	GameState gameState = STATE_MAINMENU;

	if (!SettingsManager::generatePathsAndMkdir())
	{
		Log::e(STR_CREATE_DIRS_FAIL);
		exit(1);
	}

	SettingsManager settings;
	settings.loadConfig();

	sf::RenderWindow window;
	std::vector<SimpleButton*> buttons; // TODO delet this
	std::vector<Animation*> animations; // TODO delet this
	sf::View gameWorldView({ GAME_AREA_MID_X, GAME_AREA_MID_Y }, { GAME_AREA_WIDTH, GAME_AREA_HEIGHT });
	sf::View hudView;
	sf::Clock animationTimer;

	recreateWindow(window, settings);

	GuiScale initialScale = settings.getGuiScale(SETT_GUI_SCALE);
	uint initialFxVol = settings.getUint(SETT_FX_VOLUME);
	sf::Color hudColor = settings.getColor(SETT_HUD_COLOR);
	uint transitionTimeMs = settings.getUint(SETT_ROOM_TRANSITION_DURATION_MS);

	ResourceManager resManager;

	if (!resManager.loadFonts())
	{
		Log::e(STR_LOAD_FONTS_FAIL);
		window.close();
		exit(1);
	}

	LoadingScreen loadingScreen(initialScale, hudColor, resManager, window.getSize());
	window.clear();
	window.draw(loadingScreen);
	window.display();

	if (settings.getBool(SETT_WRITE_LOG_TO_FILE))
		Log::openLogFile(pathCombine(SettingsManager::getGameRootDir(), PATH_LOGFILE));

	Log::setFont(resManager.getFont(FONT_NORMAL));
	Log::setPosition(settings.getScreenCorner(SETT_ANCHOR_LOG), window.getSize());
	Log::setWriteLogToFile(settings.getBool(SETT_WRITE_LOG_TO_FILE));
	Log::setPrintMsgs(settings.getBool(SETT_PRINT_MSGS));
	Log::setVerboseDebug(settings.getBool(SETT_VERBOSE_DEBUG));
	Log::setGuiScale(initialScale);

	Log::d("Save dir = %s", settings.getSaveDir().c_str());

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

	FpsMeter fpsMeter(initialScale, *resManager.getFont(FONT_NORMAL));
	fpsMeter.setPosition(settings.getScreenCorner(SETT_ANCHOR_FPS), window.getSize());

	CursorManager cursorMgr;
	if (!cursorMgr.loadCursors(settings.getBool(SETT_PREFER_CUSTOM_CURSOR)))
	{
		Log::e(STR_CURSOR_LOAD_FAIL);
		window.close();
		exit(1);
	}

	cursorMgr.setCursor(window, POINTER);

	Campaign campaign(resManager);
	HoverManager hoverMgr; // TODO delet this
	PipBuck pipBuck(initialScale, hudColor, initialFxVol, resManager, campaign, gameState, settings);
	if (!pipBuck.setup())
	{
		Log::e(STR_PIPBUCK_SETUP_FAILED);
		window.close();
		exit(1);
	}

	pipBuck.setRadLevel(0.3f); // TODO remove
	MainMenu mainMenu(initialScale, hudColor, initialFxVol, transitionTimeMs, resManager, window, campaign, gameState,
					  pipBuck);

	// this is kinda messy but probably faster than a very long switch-case.
	// for each keypress we have to go through two maps: key -> action, action -> callback.
	// TODO? we could manage with a single map: key -> callback. we'd need a method in Keymap that populates below two
	// (or more) maps (mapping key to callback), which we'd call each time the mapping has changed and on program start.
	// while this would improve performance a bit, it would also significantly complicate things and possibly introduce
	// bugs. for now the current simple solution is fine, until we face issues with keypress response time.
	std::unordered_map<KeyAction, std::function<void(void)>> playingCbs {
		{ ACTION_PIPBUCK_TOGGLE_OPEN, [&pipBuck](){
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_LOAD, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_LOAD);
		} },
		{ ACTION_PIPB_GOTO_SAVE, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SAVE);
		} },
		{ ACTION_PIPB_GOTO_SETTINGS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SETTINGS);
		} },
		{ ACTION_PIPB_GOTO_CONTROLS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_CONTROLS);
		} },
		{ ACTION_PIPB_GOTO_LOG, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_LOG);
		} },
		{ ACTION_PIPB_GOTO_STATUS_MAIN, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_STATUS_MAIN);
		} },
		{ ACTION_PIPB_GOTO_SKILLS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SKILLS);
		} },
		{ ACTION_PIPB_GOTO_PERKS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_PERKS);
		} },
		{ ACTION_PIPB_GOTO_EFFECTS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_EFFECTS);
		} },
		{ ACTION_PIPB_GOTO_HEALTH, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_HEALTH);
		} },
		{ ACTION_PIPB_GOTO_WEAPONS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_WEAPONS);
		} },
		{ ACTION_PIPB_GOTO_ARMOR, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_ARMOR);
		} },
		{ ACTION_PIPB_GOTO_EQUIPMENT, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_EQUIPMENT);
		} },
		{ ACTION_PIPB_GOTO_INVENTORY_OTHER, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_INVENTORY_OTHER);
		} },
		{ ACTION_PIPB_GOTO_AMMO, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_AMMO);
		} },
		{ ACTION_PIPB_GOTO_MAP, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_MAP);
		} },
		{ ACTION_PIPB_GOTO_WORLD, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_WORLD);
		} },
		{ ACTION_PIPB_GOTO_QUESTS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_QUESTS);
		} },
		{ ACTION_PIPB_GOTO_NOTES, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_NOTES);
		} },
		{ ACTION_PIPB_GOTO_ENEMIES, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_ENEMIES);
		} },
		{ ACTION_TOGGLE_FULLSCREEN, [&window, &settings, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu](){
			toggleFullscreen(window, settings, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu);
		} },
	};

	std::unordered_map<KeyAction, std::function<void(void)>> pipBuckCbs {
		{ ACTION_PIPBUCK_TOGGLE_OPEN, [&pipBuck](){
			pipBuck.close();
		} },
		{ ACTION_PIPB_GOTO_LOAD, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_LOAD);
		} },
		{ ACTION_PIPB_GOTO_SAVE, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SAVE);
		} },
		{ ACTION_PIPB_GOTO_SETTINGS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SETTINGS);
		} },
		{ ACTION_PIPB_GOTO_CONTROLS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_CONTROLS);
		} },
		{ ACTION_PIPB_GOTO_LOG, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_LOG);
		} },
		{ ACTION_PIPB_GOTO_STATUS_MAIN, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_STATUS_MAIN);
		} },
		{ ACTION_PIPB_GOTO_SKILLS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SKILLS);
		} },
		{ ACTION_PIPB_GOTO_PERKS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_PERKS);
		} },
		{ ACTION_PIPB_GOTO_EFFECTS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_EFFECTS);
		} },
		{ ACTION_PIPB_GOTO_HEALTH, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_HEALTH);
		} },
		{ ACTION_PIPB_GOTO_WEAPONS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_WEAPONS);
		} },
		{ ACTION_PIPB_GOTO_ARMOR, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_ARMOR);
		} },
		{ ACTION_PIPB_GOTO_EQUIPMENT, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_EQUIPMENT);
		} },
		{ ACTION_PIPB_GOTO_INVENTORY_OTHER, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_INVENTORY_OTHER);
		} },
		{ ACTION_PIPB_GOTO_AMMO, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_AMMO);
		} },
		{ ACTION_PIPB_GOTO_MAP, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_MAP);
		} },
		{ ACTION_PIPB_GOTO_WORLD, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_WORLD);
		} },
		{ ACTION_PIPB_GOTO_QUESTS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_QUESTS);
		} },
		{ ACTION_PIPB_GOTO_NOTES, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_NOTES);
		} },
		{ ACTION_PIPB_GOTO_ENEMIES, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_ENEMIES);
		} },
		{ ACTION_TOGGLE_FULLSCREEN, [&window, &settings, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu](){
			toggleFullscreen(window, settings, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu);
		} },
	};

	std::unordered_map<KeyAction, std::function<void(void)>> mainMenuCbs {
		{ ACTION_TOGGLE_FULLSCREEN, [&window, &settings, &fpsMeter, &hudView, &gameWorldView, &pipBuck, &mainMenu](){
			toggleFullscreen(window, settings, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu);
		} }
	};


	// TODO remove all comments below

	// howto set gui scale
	//settings.setGuiScale(SETT_GUI_SCALE, GUI_SMALL);
	//setGuiScale() for all buttons
	//fpsMeter.setGuiScale(GUI_SMALL);
	//Log::setGuiScale(GUI_SMALL);


	Animation *mchavi = new Animation(resManager.getTexture("res/entities/mchavi.png"), { 130, 130 }, {
		{ ANIM_STAND, 1 },
		{ ANIM_TROT, 17 },
		{ ANIM_GALLOP, 8 },
		{ ANIM_JUMP, 16 },
		{ ANIM_DIE_GROUND, 20 },
		{ ANIM_DIE_AIR, 13 },
		{ ANIM_TK_HOLD, 8 },
		{ ANIM_SWIM, 24 },
		{ ANIM_CLIMB, 12 },
		{ ANIM_WALK, 24 },
	});
	mchavi->setPosition(350, 100);
	animations.push_back(mchavi);

	SimpleButton mchavi1(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 60 }, "stand", [&mchavi]() {
		mchavi->setAnimation(ANIM_STAND);
	});
	buttons.push_back(&mchavi1);
	hoverMgr += &mchavi1;

	SimpleButton mchavi2(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 90 }, "walk", [&mchavi]() {
		mchavi->setAnimation(ANIM_WALK);
	});
	buttons.push_back(&mchavi2);
	hoverMgr += &mchavi2;

	SimpleButton mchavi3(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 120 }, "trot", [&mchavi]() {
		mchavi->setAnimation(ANIM_TROT);
	});
	buttons.push_back(&mchavi3);
	hoverMgr += &mchavi3;

	SimpleButton mchavi4(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 150 }, "gallop", [&mchavi]() {
		mchavi->setAnimation(ANIM_GALLOP);
	});
	buttons.push_back(&mchavi4);
	hoverMgr += &mchavi4;

	SimpleButton mchavi5(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 180 }, "jump", [&mchavi]() {
		mchavi->setAnimation(ANIM_JUMP);
	});
	buttons.push_back(&mchavi5);
	hoverMgr += &mchavi5;

	SimpleButton mchavi6(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 210 }, "die ground", [&mchavi]() {
		mchavi->setAnimation(ANIM_DIE_GROUND);
	});
	buttons.push_back(&mchavi6);
	hoverMgr += &mchavi6;

	SimpleButton mchavi7(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 240 }, "die air", [&mchavi]() {
		mchavi->setAnimation(ANIM_DIE_AIR);
	});
	buttons.push_back(&mchavi7);
	hoverMgr += &mchavi7;

	SimpleButton mchavi8(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 270 }, "tk hold", [&mchavi]() {
		mchavi->setAnimation(ANIM_TK_HOLD);
	});
	buttons.push_back(&mchavi8);
	hoverMgr += &mchavi8;

	SimpleButton mchavi9(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 300 }, "swim", [&mchavi]() {
		mchavi->setAnimation(ANIM_SWIM);
	});
	buttons.push_back(&mchavi9);
	hoverMgr += &mchavi9;

	SimpleButton mchavi10(initialScale, BTN_NORMAL, hudColor, resManager, { 100, 330 }, "climb", [&mchavi]() {
		mchavi->setAnimation(ANIM_CLIMB);
	});
	buttons.push_back(&mchavi10);
	hoverMgr += &mchavi10;

	mchavi->setAnimation(ANIM_SWIM);



	Animation *fire = new Animation(resManager.getTexture("res/entities/fire.png"), { 50, 67 }, {
		{ANIM_STATIC, 17}
	});
	fire->setPosition(350, 180);
	animations.push_back(fire);

	Animation *fire2 = new Animation(resManager.getTexture("res/entities/fire.png"), { 50, 67 }, {
		{ANIM_STATIC, 17}
	});
	fire2->setPosition(400, 180);
	animations.push_back(fire2);


	// TODO delet this: howto change cursor type: cursorMgr.setCursor(window, CROSSHAIR_WHITE);


	// debug room navigation
	sf::Text debugCoords("(?, ?, ?)", *resManager.getFont(FONT_FIXED), 30);
	debugCoords.setPosition(900, 793);
	debugCoords.setFillColor(hudColor);

	SimpleButton btnRoomLeft(initialScale, BTN_NARROW, hudColor, resManager, { 750, 800 }, "<",
		[&campaign, &debugCoords]() {
			campaign.gotoRoom(DIR_LEFT);
			debugCoords.setString(litSprintf("(%d, %d, %d)",
											 campaign.getPlayerRoomCoords().x,
											 campaign.getPlayerRoomCoords().y,
											 campaign.getPlayerRoomCoords().z));
		});
	buttons.push_back(&btnRoomLeft);
	hoverMgr += &btnRoomLeft;

	SimpleButton btnRoomRight(initialScale, BTN_NARROW, hudColor, resManager, { 1050, 800 }, ">",
		[&campaign, &debugCoords]() {
			campaign.gotoRoom(DIR_RIGHT);
			debugCoords.setString(litSprintf("(%d, %d, %d)",
											 campaign.getPlayerRoomCoords().x,
											 campaign.getPlayerRoomCoords().y,
											 campaign.getPlayerRoomCoords().z));
		});
	buttons.push_back(&btnRoomRight);
	hoverMgr += &btnRoomRight;

	SimpleButton btnRoomUp(initialScale, BTN_NARROW, hudColor, resManager, { 900, 750 }, "/\\",
		[&campaign, &debugCoords]() {
			campaign.gotoRoom(DIR_UP);
			debugCoords.setString(litSprintf("(%d, %d, %d)",
											 campaign.getPlayerRoomCoords().x,
											 campaign.getPlayerRoomCoords().y,
											 campaign.getPlayerRoomCoords().z));
		});
	buttons.push_back(&btnRoomUp);
	hoverMgr += &btnRoomUp;

	SimpleButton btnRoomDown(initialScale, BTN_NARROW, hudColor, resManager, { 900, 850 }, "\\/",
		[&campaign, &debugCoords]() {
			campaign.gotoRoom(DIR_DOWN);
			debugCoords.setString(litSprintf("(%d, %d, %d)",
											 campaign.getPlayerRoomCoords().x,
											 campaign.getPlayerRoomCoords().y,
											 campaign.getPlayerRoomCoords().z));
		});
	buttons.push_back(&btnRoomDown);
	hoverMgr += &btnRoomDown;

	SimpleButton btnRoomFront(initialScale, BTN_NARROW, hudColor, resManager, { 1050, 750 }, "fore",
		[&campaign, &debugCoords]() {
			campaign.gotoRoom(DIR_FRONT);
			debugCoords.setString(litSprintf("(%d, %d, %d)",
											 campaign.getPlayerRoomCoords().x,
											 campaign.getPlayerRoomCoords().y,
											 campaign.getPlayerRoomCoords().z));
		});
	buttons.push_back(&btnRoomFront);
	hoverMgr += &btnRoomFront;

	SimpleButton btnRoomBack(initialScale, BTN_NARROW, hudColor, resManager, { 1050, 850 }, "back",
		[&campaign, &debugCoords]() {
			campaign.gotoRoom(DIR_BACK);
			debugCoords.setString(litSprintf("(%d, %d, %d)",
											 campaign.getPlayerRoomCoords().x,
											 campaign.getPlayerRoomCoords().y,
											 campaign.getPlayerRoomCoords().z));
		});
	buttons.push_back(&btnRoomBack);
	hoverMgr += &btnRoomBack;



	// initial size
	windowSizeChanged(window.getSize(), settings, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu);

	// autoload campaign
	std::string autoLoadPath = settings.getText(SETT_AUTOLOAD_CAMPAIGN);
	if (autoLoadPath != "")
	{
		Log::d(STR_AUTLOADING_CAMPAIGN);

		if (campaign.load(pathCombine("res/campaigns", autoLoadPath), transitionTimeMs) && pipBuck.setupCampaignInfos())
		{
			gameState = STATE_PLAYING;
			debugCoords.setString(litSprintf("(%d, %d, %d)",
											 campaign.getPlayerRoomCoords().x,
											 campaign.getPlayerRoomCoords().y,
											 campaign.getPlayerRoomCoords().z));
		}
	}

	// TODO? there's a very minor visual bug that happens if e.g. player hovers over pipbuck btn,
	// closes pipbuck (by pressing the tab btn), moves cursor somewhere else, reopens pipbuck,
	// and the "old" hover is still being shown. fixing this would require saving mouse coords
	// each time mouse moves (yikes), and then calling pipbuck/menu handle mouse move method when
	// either of them is being opened. doesn't seems like something worth fixing tbh.
	sf::Event event;
	while (window.isOpen())
	{
		while (window.pollEvent(event))
		{
			// TODO? probably logic for different game states could be handled by objects such as pipbuck/mainmenu/campaign, etc. instead of here
			if (gameState == STATE_PLAYING)
			{
				if (event.type == sf::Event::MouseMoved)
				{
					hoverMgr.handleMouseMove({ event.mouseMove.x, event.mouseMove.y });
				}
				else if (event.type == sf::Event::KeyPressed)
				{
					auto search = playingCbs.find(Keymap::keyToAction(event.key.code));
					if (search != playingCbs.end())
						search->second();
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					//sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						for (Button* btn : buttons)
						{
							if (btn->handleLeftClick({ event.mouseButton.x, event.mouseButton.y }) != CLICK_NOT_CONSUMED)
								break;
						}
					}
				}
				else if (event.type == sf::Event::LostFocus)
				{
					if (settings.getBool(SETT_PAUSE_ON_FOCUS_LOSS))
						pipBuck.open(false);
				}
			}
			else if (gameState == STATE_PIPBUCK)
			{
				if (event.type == sf::Event::MouseMoved)
				{
					pipBuck.handleMouseMove({ event.mouseMove.x, event.mouseMove.y });
				}
				else if (event.type == sf::Event::KeyPressed)
				{
					auto search = pipBuckCbs.find(Keymap::keyToAction(event.key.code));
					if (search != pipBuckCbs.end())
						search->second();
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					//sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						pipBuck.handleLeftClick({ event.mouseButton.x, event.mouseButton.y });
					}
				}
			}
			else if (gameState == STATE_MAINMENU)
			{
				if (event.type == sf::Event::MouseMoved)
				{
					mainMenu.handleMouseMove({ event.mouseMove.x, event.mouseMove.y });
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					//sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						mainMenu.handleLeftClick({ event.mouseButton.x, event.mouseButton.y });
					}
				}
				else if (event.type == sf::Event::KeyPressed)
				{
					auto search = mainMenuCbs.find(Keymap::keyToAction(event.key.code));
					if (search != mainMenuCbs.end())
						search->second();
				}
			}

			// below events apply to all game states
			if (event.type == sf::Event::Closed)
			{
				// TODO autosave if campaign loaded
				Log::d(STR_SHUTTING_DOWN);

				for (Animation* animation : animations)
				{
					delete animation;
				}

				Log::close();
				window.close();
				return 0;
			}
			else if (event.type == sf::Event::Resized)
			{
				windowSizeChanged(window.getSize(), settings, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu);
			}
			// TODO probably useful for text entry
			//else if (event.type == sf::Event::TextEntered)
			//{
			//	if (event.text.unicode < 128)
			//		Log::d("ASCII character typed: %c", static_cast<char>(event.text.unicode));
			//}
		}

		window.clear();

		// game world entities
		window.setView(gameWorldView);

		if ((gameState == STATE_PLAYING || gameState == STATE_PIPBUCK) && campaign.isLoaded())
			window.draw(campaign); // TODO ultimately campaign should contain pipbuck and decide to display it or not based on game state

		// used to determine speed of frame-based animations (i.e. loaded from spritesheets)
		bool drawNextFrame = animationTimer.getElapsedTime().asMilliseconds() >= ANIM_FRAME_DURATION_MS;
		if (drawNextFrame)
			animationTimer.restart();

		if (gameState == STATE_PLAYING || gameState == STATE_PIPBUCK)
		{
			for(Animation* animation : animations)
			{
				if (gameState == STATE_PLAYING && drawNextFrame)
					animation->nextFrame();

				window.draw(*animation);
			}
		}

		// TODO something like this probably, campaign will call its kids which will call individual animations at the end
		//if (gameState == STATE_PLAYING)
		//	campaign.maybeNextFrame();

		// hud
		window.setView(hudView);

		if (gameState == STATE_PLAYING)
		{
			// TODO delet this
			for (Button* btn : buttons)
			{
				window.draw(*btn);
			}
			window.draw(debugCoords);

			campaign.updateState();
		}
		else if (gameState == STATE_PIPBUCK)
		{
			pipBuck.updateDraw();
			window.draw(pipBuck);
		}
		else if (gameState == STATE_MAINMENU)
			window.draw(mainMenu);

		Log::maybeUpdate();
		Log::draw(window);

		if (settings.getBool(SETT_SHOW_FPS_COUNTER))
		{
			// the clock will be initialized at program start either way, but fps won't be calculated if disabled
			fpsMeter.maybeUpdate();
			window.draw(fpsMeter);
		}

		window.display();
	}

	return 0;
}
