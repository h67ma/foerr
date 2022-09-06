#include <iostream>
//#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
//#include <unistd.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include "window/cursor_manager.hpp"
#include "util/i18n.hpp"
#include "consts.hpp"
#include "hud/log.hpp"
#include "settings/settings_manager.hpp"
#include "resource_manager.hpp"
#include "hud/fps_meter.hpp"
#include "hud/button.hpp"
#include "entities/animation.hpp"
#include "campaigns/campaign.hpp"
#include "window/util.hpp"
#include "hud/hover_manager.hpp"
#include "hud/pipbuck/pipbuck.hpp"
#include "hud/main_menu.hpp"
#include "hud/loading_screen.hpp"

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
	uint windowW, windowH;
	std::vector<Button*> buttons; // TODO all buttons should reside inside PipBuck/main menu/etc
	std::vector<Animation*> animations;
	sf::View gameWorldView({ GAME_AREA_MID_X, GAME_AREA_MID_Y }, { GAME_AREA_WIDTH, GAME_AREA_HEIGHT });
	sf::View hudView;

	// TODO find a platform-independent way to display stack trace on crash
	//signal(SIGSEGV, stackTraceHandler);

	recreateWindow(window, settings);

	windowW = window.getSize().x;
	windowH = window.getSize().y;

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

	LoadingScreen loadingScreen(initialScale, hudColor, resManager, window.getSize().x, window.getSize().y);
	window.clear();
	window.draw(loadingScreen);
	window.display();

	if (!resManager.loadCore())
	{
		Log::e(STR_LOAD_CORE_FAIL);
		window.close();
		exit(1);
	}

	Log::setFont(resManager.getFont(FONT_NORMAL));
	Log::setPosition(settings.getScreenCorner(SETT_ANCHOR_LOG), windowW, windowH);
	Log::setWriteLogToFile(settings.getBool(SETT_WRITE_LOG_TO_FILE));
	Log::setPrintMsgs(settings.getBool(SETT_PRINT_MSGS));
	Log::setVerboseDebug(settings.getBool(SETT_VERBOSE_DEBUG));
	Log::setGuiScale(initialScale);

	FpsMeter fpsMeter(initialScale, *resManager.getFont(FONT_NORMAL));
	fpsMeter.setPosition(settings.getScreenCorner(SETT_ANCHOR_FPS), windowW, windowH);
	
	CursorManager cursorMgr;
	if (!cursorMgr.loadCursors(settings.getBool(SETT_PREFER_CUSTOM_CURSOR)))
	{
		Log::e(STR_CURSOR_LOAD_FAIL);
		window.close();
		exit(1);
	}

	cursorMgr.setCursor(window, POINTER);

	Campaign campaign;
	HoverManager hoverMgr;
	MainMenu mainMenu(initialScale, hudColor, initialFxVol, resManager, window, campaign, gameState);
	PipBuck pipBuck(initialScale, hudColor, initialFxVol, resManager, campaign, gameState);



	// TODO remove all comments below

	// howto set gui scale
	//settings.setGuiScale(SETT_GUI_SCALE, GUI_SMALL);
	//setGuiScale() for all buttons
	//fpsMeter.setGuiScale(GUI_SMALL);
	//Log::setGuiScale(GUI_SMALL);

	// howto save settings
	//settings.saveConfig();



	Animation *fire = new Animation(*resManager.getTexture("res/entities/fire.png"), 50, 67, {
		{ANIM_STATIC, 17}
	});
	fire->setPosition(100, 300);
	animations.push_back(fire);

	Animation *fire2 = new Animation(*resManager.getTexture("res/entities/fire.png"), 50, 67, {
		{ANIM_STATIC, 17}
	});
	fire2->setPosition(150, 300);
	animations.push_back(fire2);


	Animation *mchavi = new Animation(*resManager.getTexture("res/entities/mchavi.png"), 130, 130, {
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
	mchavi->setPosition(1200, 100);
	animations.push_back(mchavi);

	Button mchavi1(initialScale, BTN_NORMAL, hudColor, resManager, 100, 60, "stand", [&mchavi]() {
		mchavi->setAnimation(ANIM_STAND);
	});
	buttons.push_back(&mchavi1);
	hoverMgr.addHoverable(&mchavi1);

	Button mchavi2(initialScale, BTN_NORMAL, hudColor, resManager, 100, 90, "walk", [&mchavi]() {
		mchavi->setAnimation(ANIM_WALK);
	});
	buttons.push_back(&mchavi2);
	hoverMgr.addHoverable(&mchavi2);

	Button mchavi3(initialScale, BTN_NORMAL, hudColor, resManager, 100, 120, "trot", [&mchavi]() {
		mchavi->setAnimation(ANIM_TROT);
	});
	buttons.push_back(&mchavi3);
	hoverMgr.addHoverable(&mchavi3);

	Button mchavi4(initialScale, BTN_NORMAL, hudColor, resManager, 100, 150, "gallop", [&mchavi]() {
		mchavi->setAnimation(ANIM_GALLOP);
	});
	buttons.push_back(&mchavi4);
	hoverMgr.addHoverable(&mchavi4);

	Button mchavi5(initialScale, BTN_NORMAL, hudColor, resManager, 100, 180, "jump", [&mchavi]() {
		mchavi->setAnimation(ANIM_JUMP);
	});
	buttons.push_back(&mchavi5);
	hoverMgr.addHoverable(&mchavi5);

	Button mchavi6(initialScale, BTN_NORMAL, hudColor, resManager, 100, 210, "die ground", [&mchavi]() {
		mchavi->setAnimation(ANIM_DIE_GROUND);
	});
	buttons.push_back(&mchavi6);
	hoverMgr.addHoverable(&mchavi6);

	Button mchavi7(initialScale, BTN_NORMAL, hudColor, resManager, 100, 240, "die air", [&mchavi]() {
		mchavi->setAnimation(ANIM_DIE_AIR);
	});
	buttons.push_back(&mchavi7);
	hoverMgr.addHoverable(&mchavi7);

	Button mchavi8(initialScale, BTN_NORMAL, hudColor, resManager, 100, 270, "tk hold", [&mchavi]() {
		mchavi->setAnimation(ANIM_TK_HOLD);
	});
	buttons.push_back(&mchavi8);
	hoverMgr.addHoverable(&mchavi8);

	Button mchavi9(initialScale, BTN_NORMAL, hudColor, resManager, 100, 300, "swim", [&mchavi]() {
		mchavi->setAnimation(ANIM_SWIM);
	});
	buttons.push_back(&mchavi9);
	hoverMgr.addHoverable(&mchavi9);

	Button mchavi10(initialScale, BTN_NORMAL, hudColor, resManager, 100, 330, "climb", [&mchavi]() {
		mchavi->setAnimation(ANIM_CLIMB);
	});
	buttons.push_back(&mchavi10);
	hoverMgr.addHoverable(&mchavi10);

	mchavi->setAnimation(ANIM_SWIM);



	Button cursor1(initialScale, BTN_NARROW, hudColor, resManager, 1100, 100, "pointer", [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, POINTER);
	});
	buttons.push_back(&cursor1);
	hoverMgr.addHoverable(&cursor1);

	Button cursor2(initialScale, BTN_NARROW, hudColor, resManager, 1100, 150, "x white", [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_WHITE);
	});
	buttons.push_back(&cursor2);
	hoverMgr.addHoverable(&cursor2);

	Button cursor3(initialScale, BTN_NARROW, hudColor, resManager, 1100, 200, "x yellow", [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_YELLOW);
	});
	buttons.push_back(&cursor3);
	hoverMgr.addHoverable(&cursor3);

	Button cursor4(initialScale, BTN_NARROW, hudColor, resManager, 1100, 250, "x red", [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_RED);
	});
	buttons.push_back(&cursor4);
	hoverMgr.addHoverable(&cursor4);

	// initial size
	windowSizeChanged(window, settings, fpsMeter, hudView, gameWorldView, pipBuck);

	campaign.load("res/campaigns/test", resManager); // TODO remove
	gameState = STATE_PLAYING; // TODO remove

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
					hoverMgr.handleMouseMove(event.mouseMove.x, event.mouseMove.y);
				}
				else if (event.type == sf::Event::KeyPressed)
				{
					// TODO get appropriate action from control map

					if (event.key.code == sf::Keyboard::Tab || event.key.code == sf::Keyboard::Escape)
					{
						pipBuck.open();
					}
					else if (event.key.code == sf::Keyboard::F11)
					{
						toggleFullscreen(window, settings, fpsMeter, hudView, gameWorldView, pipBuck);
					}
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					//sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						for (Button* btn : buttons)
						{
							if (btn->handleLeftClick(event.mouseButton.x, event.mouseButton.y) != CLICK_NOT_CONSUMED)
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
					pipBuck.handleMouseMove(event.mouseMove.x, event.mouseMove.y);
				}
				else if (event.type == sf::Event::KeyPressed)
				{
					// TODO get appropriate action from control map

					if (event.key.code == sf::Keyboard::F11)
					{
						toggleFullscreen(window, settings, fpsMeter, hudView, gameWorldView, pipBuck);
					}
					else if (event.key.code == sf::Keyboard::Tab || event.key.code == sf::Keyboard::Escape)
					{
						pipBuck.close();
					}
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					//sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						pipBuck.handleLeftClick(event.mouseButton.x, event.mouseButton.y);
					}
				}
			}
			else if (gameState == STATE_MAINMENU)
			{
				if (event.type == sf::Event::MouseMoved)
				{
					mainMenu.handleMouseMove(event.mouseMove.x, event.mouseMove.y);
				}
				else if (event.type == sf::Event::MouseButtonPressed)
				{
					//sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
					if (event.mouseButton.button == sf::Mouse::Left)
					{
						mainMenu.handleLeftClick(event.mouseButton.x, event.mouseButton.y);
					}
				}
			}

			// below events apply to all game states
			if (event.type == sf::Event::Closed)
			{
				// TODO autosave if campaign loaded
				Log::d(STR_SHUTTING_DOWN);
				window.close();
			}
			else if (event.type == sf::Event::Resized)
			{
				windowSizeChanged(window, settings, fpsMeter, hudView, gameWorldView, pipBuck);
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

		if (gameState == STATE_PLAYING || gameState == STATE_PIPBUCK)
		{
			for(Animation* animation : animations)
			{
				if (gameState == STATE_PLAYING)
					animation->maybeNextFrame();
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
			for (Button* btn : buttons)
			{
				window.draw(*btn);
			}
		}
		else if (gameState == STATE_PIPBUCK)
			window.draw(pipBuck);
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

	for (Animation* animation : animations)
	{
		delete animation;
	}

	return 0;
}
