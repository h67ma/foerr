#include <iostream>
//#include <execinfo.h>
#include <signal.h>
#include <stdlib.h>
//#include <unistd.h>

#include <SFML/Graphics.hpp>
#include <SFML/System/Clock.hpp>
#include <SFML/Audio.hpp>
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

void setGuiScale(std::list<Button*> &buttons, GuiScale scale)
{
	for (Button* btn : buttons)
	{
		btn->setGuiScale(scale);
	}
}

int main()
{
	GameState gameState = STATE_MAINMENU;
	SettingsManager settings;
	settings.loadConfig();

	sf::RenderWindow window;
	uint windowW, windowH;
	std::list<Button*> buttons;
	std::vector<Animation*> animations;
	sf::View gameWorldView({ GAME_AREA_MID_X, GAME_AREA_MID_Y }, { GAME_AREA_WIDTH, GAME_AREA_HEIGHT });
	sf::View hudView;

	// TODO find a platform-independent way to display stack trace on crash
	//signal(SIGSEGV, stackTraceHandler);

	recreateWindow(window, settings);

	windowW = window.getSize().x;
	windowH = window.getSize().y;

	GuiScale initialScale = settings.getGuiScale(SETT_GUI_SCALE);
	sf::Color hudColor = settings.getColor(SETT_HUD_COLOR);

	ResourceManager resManager;

	if (!resManager.loadCore())
	{
		Log::e(STR_LOAD_CORE_FAIL);
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
		exit(1);
	}

	cursorMgr.setCursor(window, POINTER);

	sf::Text dummyTab;
	dummyTab.setFont(*resManager.getFont(FONT_MEDIUM));
	dummyTab.setPosition(200, 300);
	dummyTab.setString("Weapons");



	Button tab1Btn(GUI_NORMAL, BTN_NARROW, hudColor, "Weapons", *resManager.getFont(FONT_MEDIUM));
	Button tab2Btn(GUI_NORMAL, BTN_NARROW, hudColor, "Armor", *resManager.getFont(FONT_MEDIUM));
	Button tab3Btn(GUI_NORMAL, BTN_NARROW, hudColor, "Equipment", *resManager.getFont(FONT_MEDIUM));
	
	tab1Btn.setCallback([&dummyTab, &tab1Btn, &tab2Btn, &tab3Btn]() {
		tab1Btn.setSelected(true);
		tab2Btn.setSelected(false);
		tab3Btn.setSelected(false);
		dummyTab.setString("Weapons");
	});
	tab1Btn.setPosition(100, 100);

	tab2Btn.setCallback([&dummyTab, &tab1Btn, &tab2Btn, &tab3Btn]() {
		tab1Btn.setSelected(false);
		tab2Btn.setSelected(true);
		tab3Btn.setSelected(false);
		dummyTab.setString("Armor");
	});
	tab2Btn.setPosition(240, 100);

	tab3Btn.setCallback([&dummyTab, &tab1Btn, &tab2Btn, &tab3Btn]() {
		tab1Btn.setSelected(false);
		tab2Btn.setSelected(false);
		tab3Btn.setSelected(true);
		dummyTab.setString("Equipment");
	});
	tab3Btn.setPosition(380, 100);

	tab1Btn.setSelected(true);

	buttons.push_back(&tab1Btn);
	buttons.push_back(&tab2Btn);
	buttons.push_back(&tab3Btn);

	sf::SoundBuffer buffer;
	if (!buffer.loadFromFile("res/audio/13_skill.wav"))
		return -1;

	sf::Sound sound;
	sound.setBuffer(buffer);

	Button debugBtn(initialScale, BTN_BIG, hudColor, "test sound", *resManager.getFont(FONT_MEDIUM), [&sound]() {
		sound.play();
	});
	debugBtn.setPosition(300, 400);
	buttons.push_back(&debugBtn);

	Button size1(initialScale, BTN_NARROW, hudColor, "small", *resManager.getFont(FONT_MEDIUM), [&settings, &buttons, &fpsMeter]() {
		settings.setGuiScale(SETT_GUI_SCALE, GUI_SMALL);
		setGuiScale(buttons, GUI_SMALL);
		fpsMeter.setGuiScale(GUI_SMALL);
		Log::setGuiScale(GUI_SMALL);
	});
	size1.setPosition(500, 400);
	buttons.push_back(&size1);

	Button size2(initialScale, BTN_NARROW, hudColor, "normal", *resManager.getFont(FONT_MEDIUM), [&settings, &buttons, &fpsMeter]() {
		settings.setGuiScale(SETT_GUI_SCALE, GUI_NORMAL);
		setGuiScale(buttons, GUI_NORMAL);
		fpsMeter.setGuiScale(GUI_NORMAL);
		Log::setGuiScale(GUI_NORMAL);
	});
	size2.setPosition(500, 440);
	buttons.push_back(&size2);

	Button size3(initialScale, BTN_NARROW, hudColor, "large", *resManager.getFont(FONT_MEDIUM), [&settings, &buttons, &fpsMeter]() {
		settings.setGuiScale(SETT_GUI_SCALE, GUI_LARGE);
		setGuiScale(buttons, GUI_LARGE);
		fpsMeter.setGuiScale(GUI_LARGE);
		Log::setGuiScale(GUI_LARGE);
	});
	size3.setPosition(500, 480);
	buttons.push_back(&size3);

	Button unpauseBtn(initialScale, BTN_BIG, hudColor, "unpause", *resManager.getFont(FONT_MEDIUM), [&gameState]() {
		gameState = STATE_PLAYING;
	});
	unpauseBtn.setPosition(300, 500);
	buttons.push_back(&unpauseBtn);

	Button saveBtn(initialScale, BTN_NORMAL, hudColor, "Save config", *resManager.getFont(FONT_MEDIUM), [&settings]() {
		settings.saveConfig();
	});
	saveBtn.setPosition(100, 500);
	buttons.push_back(&saveBtn);


	Animation *fire = new Animation(*resManager.getTexture("res/entities/fire.png"), 50, 67, {
		{ANIM_STATIC, 17}
	});
	fire->setPosition(100, 200);
	animations.push_back(fire);

	Animation *fire2 = new Animation(*resManager.getTexture("res/entities/fire.png"), 50, 67, {
		{ANIM_STATIC, 17}
	});
	fire2->setPosition(150, 200);
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
	mchavi->setPosition(800, 200);
	animations.push_back(mchavi);

	Button mchavi1(initialScale, BTN_NORMAL, hudColor, "stand", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_STAND);
	});
	mchavi1.setPosition(700, 60);
	buttons.push_back(&mchavi1);

	Button mchavi2(initialScale, BTN_NORMAL, hudColor, "walk", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_WALK);
	});
	mchavi2.setPosition(700, 90);
	buttons.push_back(&mchavi2);

	Button mchavi3(initialScale, BTN_NORMAL, hudColor, "trot", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_TROT);
	});
	mchavi3.setPosition(700, 120);
	buttons.push_back(&mchavi3);

	Button mchavi4(initialScale, BTN_NORMAL, hudColor, "gallop", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_GALLOP);
	});
	mchavi4.setPosition(700, 150);
	buttons.push_back(&mchavi4);

	Button mchavi5(initialScale, BTN_NORMAL, hudColor, "jump", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_JUMP);
	});
	mchavi5.setPosition(700, 180);
	buttons.push_back(&mchavi5);

	Button mchavi6(initialScale, BTN_NORMAL, hudColor, "die ground", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_DIE_GROUND);
	});
	mchavi6.setPosition(700, 210);
	buttons.push_back(&mchavi6);

	Button mchavi7(initialScale, BTN_NORMAL, hudColor, "die air", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_DIE_AIR);
	});
	mchavi7.setPosition(700, 240);
	buttons.push_back(&mchavi7);

	Button mchavi8(initialScale, BTN_NORMAL, hudColor, "tk hold", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_TK_HOLD);
	});
	mchavi8.setPosition(700, 270);
	buttons.push_back(&mchavi8);

	Button mchavi9(initialScale, BTN_NORMAL, hudColor, "swim", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_SWIM);
	});
	mchavi9.setPosition(700, 300);
	buttons.push_back(&mchavi9);

	Button mchavi10(initialScale, BTN_NORMAL, hudColor, "climb", *resManager.getFont(FONT_MEDIUM), [&mchavi]() {
		mchavi->setAnimation(ANIM_CLIMB);
	});
	mchavi10.setPosition(700, 330);
	buttons.push_back(&mchavi10);

	mchavi->setAnimation(ANIM_SWIM);



	Campaign campaign;

	Button loadCamp(initialScale, BTN_NORMAL, hudColor, "load test campaign", *resManager.getFont(FONT_MEDIUM), [&campaign, &resManager, &gameState]() {
		if (campaign.load("res/campaigns/test", resManager))
		{
			Log::d("Loaded campaign %s (%s)", campaign.getTitle().c_str(), campaign.getDescription().c_str());
			gameState = STATE_PLAYING;
		}
		else
			Log::e(STR_CAMPAIGN_LOAD_ERR, "res/campaigns/test");
	});
	loadCamp.setPosition(700, 500);
	buttons.push_back(&loadCamp);

	Button unloadCamp(initialScale, BTN_NORMAL, hudColor, "unload campaign", *resManager.getFont(FONT_MEDIUM), [&campaign, &gameState, &resManager]() {
		campaign.unload(resManager);
		Log::d(STR_CAMPAIGN_UNLOADED);
		gameState = STATE_MAINMENU;
	});
	unloadCamp.setPosition(700, 550);
	buttons.push_back(&unloadCamp);

	Button campLoc1(initialScale, BTN_NORMAL, hudColor, "goto loc 1", *resManager.getFont(FONT_MEDIUM), [&campaign]() {
		campaign.changeLocation("surface");
	});
	campLoc1.setPosition(900, 500);
	buttons.push_back(&campLoc1);

	Button campLoc2(initialScale, BTN_NORMAL, hudColor, "goto loc 2", *resManager.getFont(FONT_MEDIUM), [&campaign]() {
		campaign.changeLocation("technical_tunnels");
	});
	campLoc2.setPosition(900, 550);
	buttons.push_back(&campLoc2);


	Button cursor1(initialScale, BTN_NARROW, hudColor, "pointer", *resManager.getFont(FONT_MEDIUM), [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, POINTER);
	});
	cursor1.setPosition(1100, 100);
	buttons.push_back(&cursor1);

	Button cursor2(initialScale, BTN_NARROW, hudColor, "x white", *resManager.getFont(FONT_MEDIUM), [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_WHITE);
	});
	cursor2.setPosition(1100, 150);
	buttons.push_back(&cursor2);

	Button cursor3(initialScale, BTN_NARROW, hudColor, "x yellow", *resManager.getFont(FONT_MEDIUM), [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_YELLOW);
	});
	cursor3.setPosition(1100, 200);
	buttons.push_back(&cursor3);

	Button cursor4(initialScale, BTN_NARROW, hudColor, "x red", *resManager.getFont(FONT_MEDIUM), [&cursorMgr, &window]() {
		cursorMgr.setCursor(window, CROSSHAIR_RED);
	});
	cursor4.setPosition(1100, 250);
	buttons.push_back(&cursor4);


	windowSizeChanged(window, settings, fpsMeter, hudView, gameWorldView);

	while (window.isOpen())
	{
		sf::Event event;
		while (window.pollEvent(event))
		{
			if (event.type == sf::Event::Closed)
			{
				window.close();
			}
			else if (event.type == sf::Event::Resized)
			{
				windowSizeChanged(window, settings, fpsMeter, hudView, gameWorldView);
			}
			else if (event.type == sf::Event::LostFocus)
			{
				if (gameState == STATE_PLAYING)
					Log::d(STR_GAME_PAUSED);

				gameState = STATE_PAUSEMENU;
			}
			else if (event.type == sf::Event::TextEntered)
			{
				if (event.text.unicode < 128)
					Log::d("ASCII character typed: %c", static_cast<char>(event.text.unicode));
			}
			else if (event.type == sf::Event::KeyPressed)
			{
				if (event.key.code == sf::Keyboard::F11)
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
					windowSizeChanged(window, settings, fpsMeter, hudView, gameWorldView);
				}
			}
			else if (event.type == sf::Event::MouseButtonPressed)
			{
				//sf::Vector2f worldPos = window.mapPixelToCoords(sf::Vector2i(event.mouseButton.x, event.mouseButton.y));
				if (event.mouseButton.button == sf::Mouse::Left)
				{
					for (Button* btn : buttons)
					{
						if (btn->maybeHandleClick(event.mouseButton.x, event.mouseButton.y))
							break; // click consumed, no need to check other buttons
					}
				}
			}
		}

		window.clear();

		// game world entities
		window.setView(gameWorldView);

		if ((gameState == STATE_PLAYING || gameState == STATE_PAUSEMENU) && campaign.isLoaded())
			window.draw(campaign);

		for(Animation* animation : animations)
		{
			if (gameState == STATE_PLAYING)
				animation->maybeNextFrame();
			window.draw(*animation);
		}

		// TODO something like this probably, campaign will call its kids which will call individual animations at the end
		//if (gameState == STATE_PLAYING)
		//	campaign.maybeNextFrame();

		// hud
		window.setView(hudView);

		for (Button* btn : buttons)
		{
			window.draw(*btn);
		}

		window.draw(dummyTab);

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

	for(Animation* animation : animations)
	{
		delete animation;
	}

	return 0;
}
