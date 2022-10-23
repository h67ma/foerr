#include <iostream>
#include <functional>
//#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
//#include <unistd.h>

#include <SFML/System/Clock.hpp>
#include <SFML/Graphics/RenderTexture.hpp>
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

int main()
{
	GameState gameState = STATE_MAINMENU;
	SettingsManager settings;
	settings.loadConfig();

	sf::RenderWindow window;
	std::vector<SimpleButton*> buttons; // TODO delet this
	std::vector<Animation*> animations; // TODO delet this
	sf::View gameWorldView({ GAME_AREA_MID_X, GAME_AREA_MID_Y }, { GAME_AREA_WIDTH, GAME_AREA_HEIGHT });
	sf::View hudView;
	sf::Clock animationTimer;

	// TODO find a platform-independent way to display stack trace on crash
	//signal(SIGSEGV, stackTraceHandler);

	recreateWindow(window, settings);

	GuiScale initialScale = settings.getGuiScale(SETT_GUI_SCALE);
	uint initialFxVol = settings.getUint(SETT_FX_VOLUME);
	sf::Color hudColor = settings.getColor(SETT_HUD_COLOR);

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

	Log::setFont(resManager.getFont(FONT_NORMAL));
	Log::setPosition(settings.getScreenCorner(SETT_ANCHOR_LOG), window.getSize());
	Log::setWriteLogToFile(settings.getBool(SETT_WRITE_LOG_TO_FILE));
	Log::setPrintMsgs(settings.getBool(SETT_PRINT_MSGS));
	Log::setVerboseDebug(settings.getBool(SETT_VERBOSE_DEBUG));
	Log::setGuiScale(initialScale);

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
	MainMenu mainMenu(initialScale, hudColor, initialFxVol, resManager, window, campaign, gameState, pipBuck);

	// this is kinda messy but probably faster than a very long switch-case.
	// for each keypress we have to go through two maps: key -> action, action -> callback.
	// TODO? maybe we could manage with a single map key -> callback. maybe a function that
	// populates cbs map (this time mapping key to callback), which we call each time the
	// mapping has changed?
	std::unordered_map<KeyAction, std::function<void(void)>> playingCbs {
		{ ACTION_PIPBUCK_TOGGLE_OPEN, [&pipBuck](){
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_LOAD, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_LOAD, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_SAVE, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SAVE, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_SETTINGS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SETTINGS, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_CONTROLS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_CONTROLS, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_LOG, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_LOG, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_STATUS_MAIN, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_STATUS_MAIN, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_SKILLS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_SKILLS, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_PERKS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_PERKS, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_EFFECTS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_EFFECTS, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_HEALTH, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_HEALTH, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_WEAPONS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_WEAPONS, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_ARMOR, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_ARMOR, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_EQUIPMENT, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_EQUIPMENT, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_INVENTORY_OTHER, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_INVENTORY_OTHER, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_AMMO, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_AMMO, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_MAP, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_MAP, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_WORLD, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_WORLD, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_QUESTS, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_QUESTS, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_NOTES, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_NOTES, true);
			pipBuck.open();
		} },
		{ ACTION_PIPB_GOTO_ENEMIES, [&pipBuck](){
			pipBuck.switchToPage(PIPB_PAGE_ENEMIES, true);
			pipBuck.open();
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
	mchavi->setPosition(1400, 100);
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
	fire->setPosition(1400, 180);
	animations.push_back(fire);

	Animation *fire2 = new Animation(resManager.getTexture("res/entities/fire.png"), { 50, 67 }, {
		{ANIM_STATIC, 17}
	});
	fire2->setPosition(1450, 180);
	animations.push_back(fire2);



	SimpleButton cursor1(initialScale, BTN_NARROW, hudColor, resManager, { 1100, 100 }, "pointer", [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, POINTER);
	});
	buttons.push_back(&cursor1);
	hoverMgr += &cursor1;

	SimpleButton cursor2(initialScale, BTN_NARROW, hudColor, resManager, { 1100, 150 }, "x white", [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_WHITE);
	});
	buttons.push_back(&cursor2);
	hoverMgr += &cursor2;

	SimpleButton cursor3(initialScale, BTN_NARROW, hudColor, resManager, { 1100, 200 }, "x yellow", [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_YELLOW);
	});
	buttons.push_back(&cursor3);
	hoverMgr += &cursor3;

	SimpleButton cursor4(initialScale, BTN_NARROW, hudColor, resManager, { 1100, 250 }, "x red", [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_RED);
	});
	buttons.push_back(&cursor4);
	hoverMgr += &cursor4;



	// debug room navigation
	sf::Text debugCoords("(?, ?)", *resManager.getFont(FONT_FIXED), 30);
	debugCoords.setPosition(570, 493);
	debugCoords.setFillColor(hudColor);

	SimpleButton btnRoomLeft(initialScale, BTN_NARROW, hudColor, resManager, { 430, 500 }, "<", [&campaign, &debugCoords]() {
		campaign.gotoRoom(DIR_LEFT);
		debugCoords.setString(litSprintf("(%u, %u)", campaign.getPlayerRoomCoords().x, campaign.getPlayerRoomCoords().y));
	});
	buttons.push_back(&btnRoomLeft);
	hoverMgr += &btnRoomLeft;

	SimpleButton btnRoomRight(initialScale, BTN_NARROW, hudColor, resManager, { 670, 500 }, ">", [&campaign, &debugCoords]() {
		campaign.gotoRoom(DIR_RIGHT);
		debugCoords.setString(litSprintf("(%u, %u)", campaign.getPlayerRoomCoords().x, campaign.getPlayerRoomCoords().y));
	});
	buttons.push_back(&btnRoomRight);
	hoverMgr += &btnRoomRight;

	SimpleButton btnRoomUp(initialScale, BTN_NARROW, hudColor, resManager, { 550, 450 }, "/\\", [&campaign, &debugCoords]() {
		campaign.gotoRoom(DIR_UP);
		debugCoords.setString(litSprintf("(%u, %u)", campaign.getPlayerRoomCoords().x, campaign.getPlayerRoomCoords().y));
	});
	buttons.push_back(&btnRoomUp);
	hoverMgr += &btnRoomUp;

	SimpleButton btnRoomDown(initialScale, BTN_NARROW, hudColor, resManager, { 550, 550 }, "\\/", [&campaign, &debugCoords]() {
		campaign.gotoRoom(DIR_DOWN);
		debugCoords.setString(litSprintf("(%u, %u)", campaign.getPlayerRoomCoords().x, campaign.getPlayerRoomCoords().y));
	});
	buttons.push_back(&btnRoomDown);
	hoverMgr += &btnRoomDown;



	sf::RenderTexture renderTxt;
	if (!renderTxt.create(500, 500))
		return -1;
	SpriteResource vent(resManager.getTexture("res/texture/rubble/vent.png"));
	SpriteResource terminal(resManager.getTexture("res/texture/rubble/terminal_red.png"));
	terminal.get().setPosition(100, 0);
	SpriteResource stove(resManager.getTexture("res/texture/rubble/stove.png"));
	stove.get().setPosition(200, 0);
	renderTxt.clear();
	renderTxt.draw(vent.get());
	renderTxt.draw(terminal.get());
	renderTxt.draw(stove.get());
	renderTxt.display();
	sf::Sprite bufferSprite(renderTxt.getTexture());
	bufferSprite.setPosition(100, 700);



	// initial size
	windowSizeChanged(window.getSize(), settings, fpsMeter, hudView, gameWorldView, pipBuck, mainMenu);

	// TODO remove
	if (campaign.load("res/campaigns/test") && pipBuck.setupCampaignInfos())
	{
		gameState = STATE_PLAYING;
		debugCoords.setString(litSprintf("(%u, %u)", campaign.getPlayerRoomCoords().x, campaign.getPlayerRoomCoords().y));
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
			window.draw(bufferSprite);
			for (Button* btn : buttons)
			{
				window.draw(*btn);
			}
			window.draw(debugCoords);
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
