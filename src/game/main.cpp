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
		btn->setScale(scale);
	}
}

int main()
{
	GameState gameState = STATE_MAINMENU;
	SettingsManager settings;
	settings.loadConfig();

	sf::RenderWindow window;
	uint windowW, windowH;
	sf::Font fontMedium;
	sf::Font fontNormal;
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

	if (!fontNormal.loadFromFile(PATH_FONT_NORMAL))
	{
		Log::e(STR_FONT_LOAD_FAIL);
		exit(1);
	}

	if (!fontMedium.loadFromFile(PATH_FONT_MEDIUM))
	{
		Log::e(STR_FONT_LOAD_FAIL);
		exit(1);
	}

	Log::setFont(&fontNormal);
	Log::setPosition(settings.getScreenCorner(SETT_ANCHOR_LOG), windowW, windowH);
	Log::setWriteLogToFile(settings.getBool(SETT_WRITE_LOG_TO_FILE));
	Log::setPrintMsgs(settings.getBool(SETT_PRINT_MSGS));
	Log::setVerboseDebug(settings.getBool(SETT_VERBOSE_DEBUG));
	Log::setScale(initialScale);

	ResourceManager resManager;

	FpsMeter fpsMeter(initialScale, fontNormal);
	fpsMeter.setPosition(settings.getScreenCorner(SETT_ANCHOR_FPS), windowW, windowH);
	
	WindowCursor cursor;
	if (!cursor.loadCursors(settings.getBool(SETT_PREFER_CUSTOM_CURSOR)))
	{
		Log::e(STR_CURSOR_LOAD_FAIL);
		exit(1);
	}

	cursor.setCursor(&window, POINTER);

	sf::Text dummyTab;
	dummyTab.setFont(fontMedium);
	dummyTab.setPosition(200, 300);
	dummyTab.setString("Weapons");



	Button tab1Btn(GUI_NORMAL, BTN_NARROW, "Weapons", fontMedium);
	Button tab2Btn(GUI_NORMAL, BTN_NARROW, "Armor", fontMedium);
	Button tab3Btn(GUI_NORMAL, BTN_NARROW, "Equipment", fontMedium);
	
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

	Button debugBtn(initialScale, BTN_BIG, "test sound", fontMedium, [&sound]() {
		sound.play();
	});
	debugBtn.setPosition(300, 400);
	buttons.push_back(&debugBtn);

	Button size1(initialScale, BTN_NARROW, "small", fontMedium, [&settings, &buttons, &fpsMeter]() {
		settings.setGuiScale(SETT_GUI_SCALE, GUI_SMALL);
		setGuiScale(buttons, GUI_SMALL);
		fpsMeter.setScale(GUI_SMALL);
		Log::setScale(GUI_SMALL);
	});
	size1.setPosition(500, 400);
	buttons.push_back(&size1);

	Button size2(initialScale, BTN_NARROW, "normal", fontMedium, [&settings, &buttons, &fpsMeter]() {
		settings.setGuiScale(SETT_GUI_SCALE, GUI_NORMAL);
		setGuiScale(buttons, GUI_NORMAL);
		fpsMeter.setScale(GUI_NORMAL);
		Log::setScale(GUI_NORMAL);
	});
	size2.setPosition(500, 440);
	buttons.push_back(&size2);

	Button size3(initialScale, BTN_NARROW, "large", fontMedium, [&settings, &buttons, &fpsMeter]() {
		settings.setGuiScale(SETT_GUI_SCALE, GUI_LARGE);
		setGuiScale(buttons, GUI_LARGE);
		fpsMeter.setScale(GUI_LARGE);
		Log::setScale(GUI_LARGE);
	});
	size3.setPosition(500, 480);
	buttons.push_back(&size3);

	bool selected = false;
	Button btnSel(initialScale, BTN_NARROW, "toggle sel", fontMedium, [&debugBtn, &selected]() {
		selected = !selected;
		debugBtn.setSelected(selected);
	});
	btnSel.setPosition(500, 520);
	buttons.push_back(&btnSel);

	Button unpauseBtn(initialScale, BTN_BIG, "unpause", fontMedium, [&gameState]() {
		gameState = STATE_PLAYING;
	});
	unpauseBtn.setPosition(300, 500);
	buttons.push_back(&unpauseBtn);

	Button saveBtn(initialScale, BTN_NORMAL, "Save config", fontMedium, [&settings]() {
		settings.saveConfig();
	});
	saveBtn.setPosition(100, 500);
	buttons.push_back(&saveBtn);


	Animation *fire = new Animation(*resManager.loadTexture("res/entities/fire.png"), 50, 67, {
		{ANIM_STATIC, 17}
	});
	fire->setPosition(100, 200);
	animations.push_back(fire);


	Animation *mchavi = new Animation(*resManager.loadTexture("res/entities/mchavi.png"), 130, 130, {
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
	mchavi->setPosition(500, 200);
	animations.push_back(mchavi);

	Button mchavi1(initialScale, BTN_NORMAL, "stand", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_STAND);
	});
	mchavi1.setPosition(700, 60);
	buttons.push_back(&mchavi1);

	Button mchavi2(initialScale, BTN_NORMAL, "walk", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_WALK);
	});
	mchavi2.setPosition(700, 90);
	buttons.push_back(&mchavi2);

	Button mchavi3(initialScale, BTN_NORMAL, "trot", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_TROT);
	});
	mchavi3.setPosition(700, 120);
	buttons.push_back(&mchavi3);

	Button mchavi4(initialScale, BTN_NORMAL, "gallop", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_GALLOP);
	});
	mchavi4.setPosition(700, 150);
	buttons.push_back(&mchavi4);

	Button mchavi5(initialScale, BTN_NORMAL, "jump", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_JUMP);
	});
	mchavi5.setPosition(700, 180);
	buttons.push_back(&mchavi5);

	Button mchavi6(initialScale, BTN_NORMAL, "die ground", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_DIE_GROUND);
	});
	mchavi6.setPosition(700, 210);
	buttons.push_back(&mchavi6);

	Button mchavi7(initialScale, BTN_NORMAL, "die air", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_DIE_AIR);
	});
	mchavi7.setPosition(700, 240);
	buttons.push_back(&mchavi7);

	Button mchavi8(initialScale, BTN_NORMAL, "tk hold", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_TK_HOLD);
	});
	mchavi8.setPosition(700, 270);
	buttons.push_back(&mchavi8);

	Button mchavi9(initialScale, BTN_NORMAL, "swim", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_SWIM);
	});
	mchavi9.setPosition(700, 300);
	buttons.push_back(&mchavi9);

	Button mchavi10(initialScale, BTN_NORMAL, "climb", fontMedium, [&mchavi]() {
		mchavi->setAnimation(ANIM_CLIMB);
	});
	mchavi10.setPosition(700, 330);
	buttons.push_back(&mchavi10);

	mchavi->setAnimation(ANIM_SWIM);



	Campaign campaign;

	Button loadCamp(initialScale, BTN_NORMAL, "load test campaign", fontMedium, [&campaign, &resManager, &gameState]() {
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

	Button unloadCamp(initialScale, BTN_NORMAL, "unload campaign", fontMedium, [&campaign, &gameState]() {
		campaign.unload();
		Log::d(STR_CAMPAIGN_UNLOADED);
		gameState = STATE_MAINMENU;
	});
	unloadCamp.setPosition(700, 550);
	buttons.push_back(&unloadCamp);

	Button campLoc1(initialScale, BTN_NORMAL, "goto loc 1", fontMedium, [&campaign]() {
		campaign.changeLocation("surface");
	});
	campLoc1.setPosition(900, 500);
	buttons.push_back(&campLoc1);

	Button campLoc2(initialScale, BTN_NORMAL, "goto loc 2", fontMedium, [&campaign]() {
		campaign.changeLocation("technical_tunnels");
	});
	campLoc2.setPosition(900, 550);
	buttons.push_back(&campLoc2);


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
