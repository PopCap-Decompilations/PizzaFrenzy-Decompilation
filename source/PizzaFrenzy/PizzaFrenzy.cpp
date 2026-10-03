#include "PizzaFrenzy.h"

#include <stdio.h>
#include <stdlib.h>
#include <time.h>
#include <algorithm>
#include <string>
#include <vector>

#include <windows.h>
#include <shellapi.h>

#include "engine/AppConfig.h"
#include "engine/Application.h"
#include "engine/Component.h"
#include "engine/Container.h"
#include "engine/EditBox.h"
#include "engine/Exception.h"
#include "engine/HighScoreManager.h"
#include "engine/Image.h"
#include "engine/InputStreamReader.h"
#include "engine/PropertyTrack.h"
#include "engine/Rect.h"
#include "engine/Scene.h"
#include "engine/Screen.h"
#include "engine/ScreenMgr.h"
#include "engine/SimpleSound.h"
#include "engine/SlideTransition.h"
#include "engine/SoundMgr.h"
#include "engine/SplatFactory.h"
#include "engine/StringUtil.h"
#include "engine/TextItem.h"
#include "engine/Transition.h"
#include "engine/User.h"
#include "engine/UserManager.h"
#include "engine/UserSelectScreen.h"
#include "engine/Xml.h"

#include "CityEditor.h"
#include "CityHandler.h"
#include "CityMap.h"
#include "CitySelectScreen.h"
#include "ConcentrationGameLogic.h"
#include "Constants.h"
#include "DecoratePizzaGame.h"
#include "GameLogic.h"
#include "GameOptionsScreen.h"
#include "GameProgress.h"
#include "GameScreen.h"
#include "GameSettings.h"
#include "HighScoreScreen.h"
#include "HudScreen.h"
#include "Level.h"
#include "MainMenuScreen.h"
#include "MemoryGame.h"
#include "MusicPlayer.h"
#include "NewToppingScreen.h"
#include "PizzaDesign.h"
#include "PizzaEditor.h"
#include "PizzaManifestHandler.h"
#include "StoryScreen.h"
#include "TileManager.h"
#include "ToppingBookScreen.h"
#include "ToppingSelectionScreen.h"
#include "UserProgress.h"

PizzaFrenzy* g_game;								// the game object (set first thing in start)
engine::RefPtr<engine::ScreenMgr> g_popupScreenMgr;	// PopupState's screen manager, created by the first PopupState
bool g_unlockAll;									// developer unlock flag, never set (cleared by MainMenuState::enter)

// 0x401000
PizzaFrenzy* getGame()
{
	return g_game;
}

// 0x401010
void PizzaFrenzy::quit()
{
	engine::getApplication()->quit(0);
}

// 0x401030
void PizzaFrenzy::onScreenEvent(const std::string& event)
{
	m_stateMachine.handleCommand(event);
}

// 0x401040
int PizzaFrenzy::getNextPizzaId(bool userPizza) const
{
	if (userPizza)
		return m_nextUserPizzaId;
	return m_nextPizzaId;
}

// 0x401060
void PizzaFrenzy::setNextPizzaId(int id, bool userPizza)
{
	if (userPizza)
		m_nextUserPizzaId = id;
	else
		m_nextPizzaId = id;
}

// 0x401090
void PizzaFrenzy::setFullscreen(bool fullscreen)
{
	engine::getApplication()->setFullScreen(fullscreen);
	onDisplayChanged();
}

// 0x4010B0
void PizzaFrenzy::updateSound(int elapsedMs)
{
	engine::getApplication();		// called and its result ignored, as in the original
	engine::SimpleSound* sound = engine::getSoundSystem();
	if (sound)
		sound->update(elapsedMs);
}

// 0x401150
void PizzaDesignerState::exit()
{
	m_owner->m_inEditor = false;
}

// 0x401D90
void PizzaFrenzy::onUpdate(engine::UpdateContext& context)
{
	m_stateMachine.update(context);
	if (engine::getSoundSystem())
		engine::getSoundSystem()->update((int)(context.elapsed * 1000.0f));
	// through a member pointer: the original calls MusicPlayer's vcall thunk for slot 2
	if (m_music)
		(m_music.get()->*&MusicPlayer::update)(context);
}

// 0x401DF0
void PizzaFrenzy::shutdown()
{
	if (m_settings)
		m_settings->save();
	m_screenMgr = 0;
	if (m_scene)
		m_scene->removeAllChildren();
	m_scene = 0;
	m_mainMenu = 0;
	m_settings = 0;
	m_splashScreen = 0;
	m_loadBar = 0;
	m_optionsScreen = 0;
	m_userSelectScreen = 0;
	m_newUserScreen = 0;
	m_highScoreScreen = 0;
	m_highScoreResetConfirm = 0;
	m_restoreGameConfirm = 0;
	m_pauseMenu = 0;
	m_storyScreen = 0;
	m_appQuitConfirm = 0;
	m_deleteUserConfirm = 0;
	m_gameExitConfirm = 0;
	m_restoreGameConfirm = 0;
	m_toppingBook = 0;
	m_toppingSelectScreen = 0;
	m_newToppingScreen = 0;
	m_decoratePizzaScreen = 0;
	m_citySelectScreen = 0;
	m_highScorePostScreen = 0;
	m_gameLogic = 0;
	m_tileManifest = 0;
	m_inEditor = false;
	m_sounds = 0;
	m_gameScreen = 0;
	m_levelEditor = 0;
	m_levelEditorScreen = 0;
	m_levelEditorTiles = 0;
	m_profile = 0;
	m_music = 0;
	g_popupScreenMgr = 0;
	engine::UserManager* users = engine::UserManager::getInstance();
	if (users)
		users->close();
	engine::HighScoreManager* scores = engine::HighScoreManager::getInstance();
	if (scores)
		scores->close();
	if (engine::getSoundSystem())
		engine::getSoundSystem()->freeUnusedSounds();
}

// 0x402240
TileManager* PizzaFrenzy::getTileManifest()
{
	return g_game->m_tileManifest;
}

// 0x402250
GameLogic* PizzaFrenzy::getGameLogic()
{
	return g_game->m_gameLogic;
}

// 0x402260
engine::SoundMgr* PizzaFrenzy::getSounds()
{
	return g_game->m_sounds;
}

// 0x402270
MusicPlayer* PizzaFrenzy::getMusicPlayer()
{
	return g_game->m_music;
}

// 0x402280
CityMap* PizzaFrenzy::getCityMap()
{
	return g_game->m_cityMap;
}

// 0x402290
GameScreen* PizzaFrenzy::getGameScreen()
{
	return g_game->m_gameScreen;
}

// 0x4022A0
UserProgress* PizzaFrenzy::getProfile()
{
	return g_game->m_profile;
}

// 0x4022B0
GameProgress* PizzaFrenzy::getGameStats()
{
	return g_game->m_gameStats;
}

// 0x4022C0
engine::SplatFactory* PizzaFrenzy::getSplatFactory()
{
	return g_game->m_splats;
}

// 0x4022D0
HudScreen* PizzaFrenzy::getHud()
{
	return g_game->m_hud;
}

// 0x4022E0
engine::Bitmap* PizzaFrenzy::getCitySelectEntry(int index)
{
	return m_citySelectScreen->getPostcard(index);
}

// 0x4022F0
void PizzaFrenzy::endLevel()
{
	if (m_gameLogic)
		m_gameLogic->unloadLevel();
	m_gameScreen->clearLayers();
	m_cityMap->clear();
	m_gameLogic = 0;
}

// 0x402340
void PizzaFrenzy::setMusicEnabled(bool enabled)
{
	engine::GameBase::setMusicEnabled(enabled);
	if (getMusicPlayer())
		getMusicPlayer()->setEnabled(enabled);
}

// 0x402370
void PizzaFrenzy::saveProgress(GameProgress* stats)
{
	m_profile->setLastCity(m_mode, m_cityIndex);
	m_profile->setLastLevel(m_mode, m_levelIndex);
	m_profile->setLastDay(m_mode, m_levelNumber);
	stats->save(m_profile, m_mode);
	m_profile->save();
}

// 0x4023D0
void PizzaFrenzy::showCitySelect()
{
	m_citySelectScreen->updateLocks();
	m_screenMgr->showScreen(m_citySelectScreen, 0.0f);
}

// 0x402400
void LoadingState::enter()
{
	m_delay = 1.0f;
	m_owner->m_screenMgr->showScreen(m_owner->m_splashScreen, 0.0f);
}

// 0x402420: m_startGame, m_city and m_fromPauseMenu are left uninitialised (set by enter())
PopupState::PopupState(PizzaFrenzy* game, engine::Screen* screen, engine::StateBase* returnState)
	: engine::State<PizzaFrenzy>(game)
{
	if (!g_popupScreenMgr)
		g_popupScreenMgr = new engine::ScreenMgr();
	m_screen = screen;
	m_returnState = returnState;
	m_closing = false;
}

// 0x402670
void PopupState::exit()
{
	m_owner->m_screenMgr->setColor(0.0f, 0.0f, 0.0f);
	m_owner->m_screenMgr->setColorMode(0);
}

// 0x4026A0
void PopupState::removePopupLayer()
{
	g_popupScreenMgr->clear();
	m_owner->m_scene->removeChild(g_popupScreenMgr);
	m_owner->m_screenMgr->activate();
}

// 0x4026E0
void BonusRoundState::enter()
{
	m_owner->m_screenMgr->showScreen(m_owner->m_decoratePizzaScreen, 0.0f);
	m_owner->saveProgress(m_owner->m_gameStats);
	m_owner->m_gameLogic->setPaused(false);
}

// 0x402720 (folded)
void PlayingState::update(engine::UpdateContext& context)
{
	m_owner->m_gameLogic->update(context);
}

// 0x402720 (folded)
void BonusRoundState::update(engine::UpdateContext& context)
{
	m_owner->m_gameLogic->update(context);
}

// 0x402730
void LevelEditorState::enter()
{
	m_owner->m_inEditor = true;
	m_owner->m_screenMgr->showScreen(m_owner->m_levelEditorScreen, 0.0f);
	m_owner->m_music->stop();
	if (!m_owner->m_levelEditor->hasCity())
		m_owner->m_levelEditor->newCity();
}

// 0x402790
void LevelEditorState::exit()
{
	m_owner->m_inEditor = false;
	m_owner->m_levelEditorScreen->setSnapToGrid(false);
	m_owner->m_levelEditorScreen->setToolbarHidden(false);
}

// 0x4027C0
void LevelEditorState::update(engine::UpdateContext& context)
{
	m_owner->m_levelEditor->update(context);
}

// 0x4027D0
void PizzaDesignerState::enter()
{
	m_owner->m_inEditor = true;
	m_owner->m_screenMgr->showScreen(m_owner->m_userPizzaEditorScreen, 0.0f);
	m_owner->m_music->stop();
}

// 0x402860: m_delay is set by enter()
LoadingState::LoadingState(PizzaFrenzy* game)
	: engine::State<PizzaFrenzy>(game)
{
}

// 0x402920: m_needNewUser and m_endLevelPending are set by enter()
MainMenuState::MainMenuState(PizzaFrenzy* game)
	: engine::State<PizzaFrenzy>(game)
{
}

// 0x4029B0
PlayingState::PlayingState(PizzaFrenzy* game)
	: engine::State<PizzaFrenzy>(game)
{
}

// 0x402A40
BonusRoundState::BonusRoundState(PizzaFrenzy* game)
	: engine::State<PizzaFrenzy>(game)
{
}

// 0x402AD0: m_storyDone and m_showingLevelUp are set by enter()
LevelSummaryState::LevelSummaryState(PizzaFrenzy* game, Level* level)
	: engine::State<PizzaFrenzy>(game), m_level(level)
{
	m_started = false;
}

// 0x402BA0: the flags are set by enter()
StoryState::StoryState(PizzaFrenzy* game, Level* level)
	: engine::State<PizzaFrenzy>(game), m_level(level)
{
}

// 0x402C40
ToppingSelectionState::ToppingSelectionState(PizzaFrenzy* game, Level* level)
	: engine::State<PizzaFrenzy>(game), m_level(level)
{
}

// 0x402CE0
NewToppingState::NewToppingState(PizzaFrenzy* game)
	: engine::State<PizzaFrenzy>(game)
{
}

// 0x402D70
LevelEditorState::LevelEditorState(PizzaFrenzy* game)
	: engine::State<PizzaFrenzy>(game)
{
}

// 0x402E00
PizzaDesignerState::PizzaDesignerState(PizzaFrenzy* game, bool fromGame)
	: engine::State<PizzaFrenzy>(game), m_fromGame(fromGame)
{
}

// 0x403640: m_needNewUser stays set (enter() clears it)
void MainMenuState::update(engine::UpdateContext& context)
{
	if (m_needNewUser && !m_owner->m_screenMgr->isTransitioning())
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_newUserScreen, this));
	if (m_endLevelPending && !m_owner->m_screenMgr->isTransitioning())
	{
		m_owner->endLevel();
		m_endLevelPending = false;
	}
}

// 0x4036F0
void LevelEditorState::onAction(const std::string& action)
{
	if (action == "editorExit")
	{
		if (m_owner->m_levelEditor->confirmDiscardChanges())
			m_owner->m_stateMachine.setState(new MainMenuState(m_owner));
	}
	else if (action == "editorTiles")
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_levelEditorTiles, this));
}

// 0x4037E0
void LevelSummaryState::onAction(const std::string& action)
{
	if (action == "storyDone")
		m_storyDone = true;
	else if (action == "confirmQuit")
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_gameExitConfirm, this));
}

// 0x4038A0
void StoryState::onAction(const std::string& action)
{
	if (action == "storyDone")
		m_storyDone = true;
	else if (action == "endGame")
	{
		m_owner->m_storyScreen->stopStory();
		m_endGame = true;
	}
}

// 0x403B60
void PizzaFrenzy::selectCity(int city)
{
	m_levelIndex = 0;
	m_cityIndex = city;
	m_levelNumber = 1;
	for (int i = 0; i < city; i++)
		m_levelNumber += m_cities[i]->m_levels.size();
}

// 0x403BB0
City* PizzaFrenzy::getCurrentCity() const
{
	return m_cities[m_cityIndex];
}

// 0x403BC0
void PizzaFrenzy::advanceLevel()
{
	m_levelIndex++;
	if (m_levelIndex >= (int)m_cities[m_cityIndex]->m_levels.size())
	{
		m_levelIndex = 0;
		m_cityIndex++;
		m_profile->updateMaxCity(m_cityIndex);
		m_profile->save();
		if (m_cityIndex >= (int)m_cities.size())
			return;
	}
	m_levelNumber++;
}

// 0x4050D0
void PizzaFrenzy::getSettings(engine::AppConfig& settings)
{
	engine::Application* app = engine::getApplication();
	std::string company = app->loadString(201);
	std::string product = app->loadString(202);
	m_settings = new GameSettings(company, product);
	m_settings->load();
	settings.title = app->loadString(203);
	settings.height = 600;
	settings.width = 800;
	settings.fullscreen = m_settings->isFullScreen();
	settings.frameRate = 30;
	settings.iconId = 103;
	srand((unsigned int)time(0));
}

// 0x405270
bool PizzaFrenzy::loadCity(const std::string& file, GameScreen* gameScreen)
{
	bool loaded = true;
	gameScreen->clearLayers();
	engine::RefPtr<engine::XmlHandlerStack> parser = new engine::XmlHandlerStack();
	engine::RefPtr<CityHandler> handler = new CityHandler(m_cityMap, "city", parser);
	try
	{
		parser->pushHandler(handler);
		engine::getApplication()->loadXml(file, parser);
		gameScreen->buildMap();
	}
	catch (...)
	{
		MessageBoxA(0, "Failed to load city", "Error", MB_OK);
		loaded = false;
	}
	engine::getApplication()->purgeImages();
	if (engine::getSoundSystem())
		engine::getSoundSystem()->freeUnusedSounds();
	parser = 0;		// released here, before the handler (0x40541D)
	return loaded;
}

// 0x405470: `this` is not used
bool PizzaFrenzy::loadLevelManifest(const std::string& file)
{
	engine::RefPtr<engine::XmlHandlerStack> parser = new engine::XmlHandlerStack();
	engine::RefPtr<LevelManifestHandler> handler = new LevelManifestHandler("LevelManifest", parser);
	try
	{
		parser->pushHandler(handler);
		engine::getApplication()->loadXml(file, parser);
	}
	catch (...)
	{
		engine::getApplication()->log("error parsing level manifest\n");
		throw engine::Exception("Error parsing level manifest\n");
	}
	return true;
}

// 0x4057B0: `this` is not used; errors in the user pizzas are ignored
bool PizzaFrenzy::loadPizzaManifest(engine::Reader* reader, bool userPizzas)
{
	engine::RefPtr<engine::XmlHandlerStack> parser = new engine::XmlHandlerStack();
	engine::RefPtr<PizzaManifestHandler> handler = new PizzaManifestHandler("pm", parser, userPizzas);
	try
	{
		parser->pushHandler(handler);
		engine::XmlParseScope scope;
		engine::parseXml(parser, reader);
	}
	catch (...)
	{
		if (!userPizzas)
			throw engine::Exception("Error parsing pizza manifest\n");
	}
	return true;
}

// 0x4059D0
void PizzaFrenzy::onDisplayChanged()
{
	if (m_settings && m_optionsScreen)
	{
		m_settings->setFullScreen(engine::getApplication()->isFullScreen());
		m_optionsScreen->setup(this, m_settings);
	}
	m_stateMachine.handleCommand("blur");
}

// 0x405A90
void PizzaFrenzy::onActivate(bool active)
{
	if (!active)
	{
		m_active = false;
		m_stateMachine.handleCommand("blur");
	}
	else
		m_active = true;
}

// 0x405B30: opens the game link (http:// or https://) with the browser of the registry's http open command
void PizzaFrenzy::openGameLink()
{
	std::string url = m_settings->getVisitUrl();
	if (url.size() < 7)
		return;
	std::string scheme = url.substr(0, 7);
	if (scheme != "http://")
	{
		scheme = url.substr(0, 8);
		if (scheme != "https://")
		{
			MessageBoxA(engine::getApplication()->getWindowHandle(), "Bad URL", "Bad URL", MB_OK);
			return;
		}
	}
	HKEY key = 0;
	std::string command;
	if (RegOpenKeyExA(HKEY_CLASSES_ROOT, "http\\shell\\open\\command", 0, KEY_READ, &key) == ERROR_SUCCESS)
	{
		DWORD size = 0;
		if (RegQueryValueExA(key, 0, 0, 0, 0, &size) == ERROR_SUCCESS && size > 0)
		{
			char* buffer = new char[size];
			if (buffer)
			{
				if (RegQueryValueExA(key, 0, 0, 0, (LPBYTE)buffer, &size) == ERROR_SUCCESS)
					command = buffer;
				delete[] buffer;
			}
		}
		RegCloseKey(key);
	}
	if (command.size() != 0)
	{
		std::string::size_type first = command.find('"');
		std::string::size_type last = command.rfind('"');
		if (first != std::string::npos && last != std::string::npos)
		{
			engine::toUpper(command);
			if (last - first > 3)
				command = command.substr(first + 1, last - first - 1);
			else
				command = command.substr(0, command.find(".EXE") + 4);
		}
		else
		{
			std::string::size_type slash = command.rfind('\\');
			if (slash != std::string::npos)
			{
				std::string::size_type space = command.rfind(' ', slash);
				if (space != std::string::npos)
					command = command.substr(0, space);
			}
		}
	}
	ShellExecuteA(engine::getApplication()->getWindowHandle(), 0, command.c_str(), url.c_str(), 0, SW_SHOWNORMAL);
}

// 0x405F90
void MainMenuState::enter()
{
	engine::getApplication();		// called and its result ignored, as in the original
	if (engine::UserManager::getInstance()->getCurrentUser())
	{
		g_unlockAll = false;
		m_needNewUser = false;
	}
	else
		m_needNewUser = true;
	if (m_owner->m_showHighScoresOnMenu)
	{
		m_owner->m_highScoreScreen->showCurrentTable();
		m_owner->m_screenMgr->showScreen(m_owner->m_highScoreScreen, 0.0f);
		m_owner->m_showHighScoresOnMenu = false;
	}
	else if (m_owner->m_showCitySelectOnMenu)
	{
		m_owner->m_citySelectScreen->updateLocks();
		m_owner->m_screenMgr->showScreen(m_owner->m_citySelectScreen, 0.0f);
		m_owner->m_showCitySelectOnMenu = false;
	}
	else
		m_owner->m_screenMgr->showScreen(m_owner->m_mainMenu, 0.2f);
	if (m_owner->m_settings->getShowLink())
		m_owner->m_mainMenu->setGameLink(m_owner->m_settings->getILink());
	else
		m_owner->m_mainMenu->setGameLink("");
	m_owner->m_music->play(m_owner->m_tileManifest->getMusicClip("music_mainMenu"));
	m_owner->m_mainMenu->refresh();
	m_endLevelPending = true;
}

// 0x406170
void PopupState::enter()
{
	m_owner->m_screenMgr->setColor(-0.2f, -0.2f, -0.2f);
	m_owner->m_screenMgr->setColorMode(1);
	m_owner->m_screenMgr->deactivate();
	m_owner->m_scene->addChild(g_popupScreenMgr);
	showPopupScreen(m_screen);
	m_screen = 0;
	PizzaFrenzy::getSounds()->playSound("menu_in", 1.0f, 1.0f);
	m_startGame = false;
	m_city = 0;
	m_fromPauseMenu = false;
}

// 0x406270: `this` is not used
void PopupState::showPopupScreen(engine::Screen* screen)
{
	g_popupScreenMgr->showScreen(screen, 0.0f);
	PizzaFrenzy::getSounds()->playSound("menu_in", 1.0f, 1.0f);
}

// 0x406310
void PopupState::closePopup()
{
	g_popupScreenMgr->showScreen(0, 0.0f);
	m_closing = true;
	PizzaFrenzy::getSounds()->playSound("menu_out", 1.0f, 1.0f);
}

// 0x4063B0
void PlayingState::enter()
{
	m_owner->m_screenMgr->showScreen(m_owner->m_gameScreen, 0.0f);
	m_owner->saveProgress(m_owner->m_gameStats);
	m_owner->m_gameLogic->setPaused(false);
	if (!m_owner->m_active)
		onAction("blur");
}

// 0x4064A0
void LevelSummaryState::enter()
{
	m_owner->m_screenMgr->showScreen(m_owner->m_storyScreen, 0.0f);
	m_owner->m_storyScreen->activate();
	m_owner->m_music->play(m_owner->m_tileManifest->getMusicClip("music_story"));
	m_showingLevelUp = false;
	m_storyDone = false;
}

// 0x406560
void StoryState::enter()
{
	m_owner->m_music->play(m_owner->m_tileManifest->getMusicClip("music_story"));
	m_owner->m_screenMgr->showScreen(m_owner->m_storyScreen, 0.0f);
	m_owner->m_storyScreen->activate();
	m_started = false;
	m_endGame = false;
	m_storyDone = false;
}

// 0x406630
void NewToppingState::enter()
{
	PizzaFrenzy::getSounds()->playSound("bonus_newTopping", 1.0f, 1.0f);
	m_owner->m_music->play(m_owner->m_tileManifest->getMusicClip("music_story"));
	m_owner->m_screenMgr->showScreen(m_owner->m_newToppingScreen, 0.0f);
}

// 0x406DC0
void PizzaFrenzy::startLevel()
{
	Level* level = m_cities[m_cityIndex]->m_levels.at(m_levelIndex);
	m_isBonusLevel = level->m_bonus;
	getProfile()->updateHighestDay(m_mode, m_levelNumber);
	getProfile()->save();
	if (m_isBonusLevel)
	{
		DecoratePizzaGame* bonusGame = new DecoratePizzaGame();
		bonusGame->init(m_decoratePizzaScreen, m_gameStats);
		bonusGame->startLevel(level);
		m_gameLogic = bonusGame;
		m_stateMachine.setState(new BonusRoundState(this));
	}
	else
	{
		loadCity(level->getCityFile(), m_gameScreen);
		// case order as the original's exception states give it
		switch (m_mode)
		{
		case 2:
			m_gameLogic = new ConcentrationGameLogic();
			break;
		case 0:
			m_gameLogic = new GameLogic();
			break;
		case 1:
			m_gameLogic = new MemoryGame();
			break;
		}
		m_gameLogic->init(m_gameScreen, m_cityMap, m_hud, m_gameStats);
		m_gameLogic->startLevel(level);
		m_stateMachine.setState(new PlayingState(this));
	}
}

// 0x407080
void PizzaFrenzy::showPauseMenu(engine::StateBase* returnState)
{
	engine::TextItem* caption1 = static_cast<engine::TextItem*>(m_pauseMenu->getComponent("pauseCaption1"));
	engine::TextItem* caption2 = static_cast<engine::TextItem*>(m_pauseMenu->getComponent("pauseCaption2"));
	Level* level = m_cities[m_cityIndex]->m_levels.at(m_levelIndex);
	std::string caption;
	if (level->m_bonus)
		caption = engine::getApplication()->formatString(228, m_levelNumber);
	else
		caption = engine::getApplication()->formatString(227, m_levelNumber, m_cities[m_cityIndex]->getName().c_str());
	caption1->setText(caption);
	caption2->setNumber(m_gameStats->m_cash + m_gameStats->m_score, engine::getApplication()->loadString(229));
	m_stateMachine.setState(new PopupState(this, m_pauseMenu, returnState));
}

// 0x407370
void PizzaFrenzy::finishLevel()
{
	Level* level = m_cities[m_cityIndex]->m_levels.at(m_levelIndex);
	m_storyScreen->setLevel(level);
	endLevel();
	if (m_isBonusLevel)
	{
		m_storyScreen->setStory("res\\story\\bonusSummary.xml");
		m_storyScreen->initBonusReport();
	}
	else
	{
		m_storyScreen->setStory("res\\story\\levelSummary.xml");
		m_storyScreen->initDailyReport();
	}
	advanceLevel();
	if (m_cityIndex >= (int)m_cities.size())
	{
		m_profile->clearGameInProgress(m_mode);
		m_storyScreen->setStory("res\\story\\gameWon.xml");
		m_stateMachine.setState(new StoryState(this, 0));
	}
	else
		m_stateMachine.setState(new LevelSummaryState(this, level));
}

// 0x4075E0
void PlayingState::onAction(const std::string& action)
{
	if (action == "pauseGame" || action == "blur" || action == "menu")
	{
		m_owner->m_gameLogic->setPaused(true);
		m_owner->showPauseMenu(this);
	}
	else if (action == "levelEnd")
		m_owner->finishLevel();
	else if (action == "gameOver")
	{
		m_owner->m_profile->clearGameInProgress(m_owner->m_mode);
		if (m_owner->m_highScoreScreen->submitScore(false))
			m_owner->m_showHighScoresOnMenu = true;
		if (m_owner->m_gameStats->m_score > 0)
			m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_highScorePostScreen, this));
		else
			m_owner->m_stateMachine.setState(new MainMenuState(m_owner));
	}
}

// 0x407790
void BonusRoundState::onAction(const std::string& action)
{
	if (action == "pauseGame" || action == "blur" || action == "menu")
	{
		m_owner->m_gameLogic->setPaused(true);
		m_owner->showPauseMenu(this);
	}
	else if (action == "levelEnd")
	{
		UserProgress* profile = PizzaFrenzy::getProfile();
		profile->setUnlockTopping(true);
		profile->save();
		m_owner->finishLevel();
	}
	else
		m_owner->m_gameLogic->handleEvent(action);
}

// 0x407850: skipped with 2 or fewer toppings unlocked
void ToppingSelectionState::enter()
{
	if (PizzaFrenzy::getGameStats()->m_toppingLevels.size() <= 2)
	{
		m_owner->startLevel();
		return;
	}
	m_owner->m_music->play(m_owner->m_tileManifest->getMusicClip("music_story"));
	m_owner->m_screenMgr->showScreen(m_owner->m_toppingSelectScreen, 0.0f);
	m_owner->m_toppingSelectScreen->activate();
}

// 0x407940
void ToppingSelectionState::onAction(const std::string& action)
{
	if (action == "selectionDone")
		m_owner->startLevel();
	else if (action == "menu")
		m_owner->showPauseMenu(this);
}

// 0x407FC0
void PizzaFrenzy::showToppingSelection()
{
	Level* level = m_cities[m_cityIndex]->m_levels.at(m_levelIndex);
	if (level->m_bonus)
	{
		startLevel();
		return;
	}
	m_toppingSelectScreen->setLevel(level);
	m_stateMachine.setState(new ToppingSelectionState(this, level));
}

// 0x408090
void PizzaFrenzy::prepareLevel()
{
	Level* level = m_cities[m_cityIndex]->m_levels.at(m_levelIndex);
	std::vector<engine::RefPtr<Topping> >& toppings = level->getToppings();
	UserProgress* profile = getProfile();
	for (std::vector<engine::RefPtr<Topping> >::iterator it = toppings.begin(); it != toppings.end(); ++it)
		profile->addNewTopping(*it);
	profile->save();
	if (profile->getUnlockTopping() && profile->getNewToppings().size() != 0)
	{
		m_newToppingScreen->populateToppings();
		m_stateMachine.setState(new NewToppingState(this));
	}
	else
		showToppingSelection();
}

// 0x408250
void PizzaDesignerState::onAction(const std::string& action)
{
	if (action == "editorExit")
	{
		if (m_fromGame)
		{
			m_owner->m_gameStats->unlockPizzaToppings();
			m_owner->showToppingSelection();
		}
		else
			m_owner->m_stateMachine.setState(new MainMenuState(m_owner));
	}
	else if (action == "namePizza")
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_pizzaNameScreen, this));
}

// 0x408360
void StoryState::update(engine::UpdateContext& context)
{
	m_owner->m_storyScreen->updateStory(context);
	if (!m_started && !m_owner->m_screenMgr->isTransitioning())
	{
		m_owner->m_storyScreen->playStory();
		m_started = true;
	}
	if (m_storyDone)
	{
		m_owner->m_storyScreen->stopStory();
		m_owner->prepareLevel();
	}
	else if (m_endGame)
	{
		if (m_owner->m_highScoreScreen->submitScore(false))
			m_owner->m_showHighScoresOnMenu = true;
		if (m_owner->m_gameStats->m_score > 0)
			m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_highScorePostScreen, this));
		else
			m_owner->m_stateMachine.setState(new MainMenuState(m_owner));
	}
}

// 0x4084B0
void NewToppingState::onAction(const std::string& action)
{
	if (action == "toppingSelected")
	{
		m_owner->saveProgress(m_owner->m_gameStats);
		if ((int)m_owner->m_gameStats->m_toppingLevels.size() == g_pizzaEditorUnlock)
			m_owner->m_screenMgr->showScreen(m_owner->m_pizzaDesignerScreen, 0.0f);
		else
			m_owner->showToppingSelection();
	}
	else if (action == "keepPlaying")
		m_owner->showToppingSelection();
	else if (action == "designPizza")
	{
		m_owner->m_pizzaEditor->open();
		m_owner->m_stateMachine.setState(new PizzaDesignerState(m_owner, true));
	}
}

// 0x408B10
PizzaFrenzy::~PizzaFrenzy()
{
}

// 0x409220: createDisplay's result (undefined in Win32Application) is not used
void PizzaFrenzy::start(engine::Application* app)
{
	g_game = this;
	app->createDisplay(m_settings->isFullScreen());
	app->setReadFromDisk(0);
	app->addArchive("PizzaFrenzy.saf");
	m_scene = new engine::Scene();
	m_scene->activate();
	initSound();
	setSoundEnabled(m_settings->isSoundEnabled());
	m_sounds = new engine::SoundMgr();
	m_sounds->load("res\\manifests\\soundManifest.xml");
	m_screenMgr = new engine::ScreenMgr();
	m_scene->addChild(m_screenMgr);

	engine::UserManager* users = engine::UserManager::getInstance();
	users->open(app->loadString(201), app->loadString(202));
	engine::HighScoreManager* scores = engine::HighScoreManager::getInstance();
	scores->open(app->loadString(201), app->loadString(202));
	scores->setGameName("PZFZ");

	engine::FadeTransition* fade = new engine::FadeTransition(0.5f);
	m_splashScreen = new engine::Screen();
	m_splashScreen->load("res/screenLayouts/splashScreen.xml");
	m_splashScreen->setTransitions(fade, fade);
	m_splashScreen->m_actionSignal.connect(this, &PizzaFrenzy::onScreenEvent);
	engine::Bitmap* background = engine::getApplication()->getDiskImage("splash/splashBg.jpg");
	engine::Image* backgroundImage = new engine::Image(background);
	engine::Container* backgroundHolder = (engine::Container*)m_splashScreen->getComponent("bgImage");
	backgroundHolder->addChild(backgroundImage);
	m_loadBar = (engine::Image*)m_splashScreen->getComponent("loadBar");
	m_loadBar->updateBounds();
	m_loadBarHeight = m_loadBar->getImageHeight();
	m_loadBarWidth = m_loadBar->getImageWidth();
	engine::IntRect source;
	source.set(0, 0, 0, m_loadBarHeight);
	m_loadBar->setUseSourceRect(true);
	m_loadBar->setSourceRect(source);

	app->m_maximizeSignal.connect(this, &PizzaFrenzy::onDisplayChanged);
	app->m_minimizeSignal.connect(this, &PizzaFrenzy::onDisplayChanged);
	app->m_activateSignal.connect(this, &PizzaFrenzy::onActivate);
	app->m_updateSignal.connect(this, &PizzaFrenzy::onUpdate);
	app->m_modalTimerSignal.connect(this, &PizzaFrenzy::updateSound);
	m_stateMachine.setState(new LoadingState(this));
}

// 0x409990
PizzaDesign* PizzaFrenzy::getPizza(int id)
{
	return m_pizzasById[id];
}

// 0x4099B0: a user pizza becomes (or updates) the topping "pizza%d" with four upgrades
void PizzaFrenzy::addPizza(PizzaDesign* pizza, bool userPizza)
{
	if (userPizza)
	{
		if (std::find(m_userPizzas.begin(), m_userPizzas.end(), pizza) == m_userPizzas.end())
			m_userPizzas.push_back(pizza);
		engine::Bitmap* thumbnail = pizza->createThumbnail();
		engine::Bitmap* coupon = engine::getApplication()->getImage("res\\pizza\\couponBlue.jpg");
		std::string name = pizza->getName();
		engine::toUpper(name);
		Topping* topping;
		if (m_userPizzaCombos.find(pizza->getId()) == m_userPizzaCombos.end())
		{
			std::string comboName;
			engine::format(comboName, "pizza%d", pizza->m_id);
			topping = new Topping(comboName, name, thumbnail, coupon, true);
			m_userPizzaCombos[pizza->getId()] = topping;
		}
		else
		{
			topping = m_userPizzaCombos[pizza->getId()];
			topping->setImage(thumbnail);
			topping->m_display = name;
		}
		topping->m_upgrades.clear();
		for (int level = 0; level < 4; level++)
		{
			ToppingUpgrade* upgrade = new ToppingUpgrade(pizza->getName(),
				engine::getApplication()->loadString(219), level + 1, (level + (std::max)(level - 2, 0)) * 5);
			topping->m_upgrades.push_back(upgrade);
		}
	}
	else
	{
		if (std::find(m_pizzas.begin(), m_pizzas.end(), pizza) == m_pizzas.end())
			m_pizzas.push_back(pizza);
		m_pizzasById[pizza->getId()] = pizza;
	}
}

// 0x409EB0
void PizzaFrenzy::addCity(City* city)
{
	m_cities.push_back(city);
}

// 0x409F30: `restored` is not used
void PizzaFrenzy::playLevel(bool restored)
{
	Level* level = m_cities[m_cityIndex]->m_levels.at(m_levelIndex);
	if (!level->getStoryFile(m_mode).empty()
		&& (m_settings->getReplayStory() || getProfile()->getHighestDay(m_mode) < m_levelNumber))
	{
		m_storyScreen->setStory(level->getStoryFile(m_mode));
		m_stateMachine.setState(new StoryState(this, level));
	}
	else if (level->m_bonus)
		startLevel();
	else
		prepareLevel();
}

// 0x40A100: letter keys in either case; all but P and F are developer cheats that need g_unlockAll
void PizzaFrenzy::onKeyPress(int key)
{
	switch (key)
	{
	case 'P':
	case 'p':
		onScreenEvent("pauseGame");
		break;
	case 'F':
	case 'f':
		onScreenEvent("pauseGame");
		setFullscreen(!engine::getApplication()->isFullScreen());
		break;
	case 'N':
	case 'n':
		if (g_unlockAll)
		{
			getMusicPlayer()->stop();
			m_gameLogic->endLevel();
			endLevel();
			advanceLevel();
			playLevel(false);
		}
		break;
	case 'D':
	case 'd':
		if (g_unlockAll)
			m_scene->dump(0);
		break;
	case 'T':
	case 't':
		if (g_unlockAll)
		{
			UserProgress* profile = getProfile();
			for (std::vector<engine::RefPtr<Topping> >::iterator it = m_tileManifest->m_toppingList.begin();
				it != m_tileManifest->m_toppingList.end(); ++it)
				profile->setToppingLevel(*it, 3);
		}
		break;
	case 'S':
	case 's':
		if (g_unlockAll)
			m_gameLogic->cheatWinLevel();
		break;
	}
}

// 0x40A330: after the delay, one batch of screens and manifests per frame while the load bar advances
void LoadingState::update(engine::UpdateContext& context)
{
	m_delay -= context.elapsed;
	if (!m_owner->m_screenMgr->isTransitioning() && m_delay < 0.0f)
	{
		float progress;
		if (!m_owner->m_tileManifest)
		{
			m_owner->m_tileManifest = new TileManager();
			m_owner->m_tileManifest->loadManifest("res\\manifests\\tileManifest.xml");
			m_owner->m_profile = new UserProgress();
			m_owner->m_profile->init();
			progress = 0.05f;
		}
		else if (!m_owner->m_mainMenu)
		{
			m_owner->m_mainMenu = new MainMenuScreen();
			m_owner->m_mainMenu->load("res/screenLayouts/mainMenu.xml");
			m_owner->m_mainMenu->init();
			m_owner->m_mainMenu->setTransitions(new engine::FadeTransition(0.5f), new engine::FadeTransition(0.5f));
			m_owner->m_mainMenu->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			progress = 0.1f;
		}
		else if (!m_owner->m_optionsScreen)
		{
			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(0.0f, -600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_optionsScreen = new PizzaOptionsScreen();
			m_owner->m_optionsScreen->load("res/screenLayouts/optionMenu.xml");
			m_owner->m_optionsScreen->setup(m_owner, m_owner->m_settings);
			m_owner->m_optionsScreen->setTransitions(slide, slide);
			m_owner->m_optionsScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			progress = 0.15f;
		}
		else if (!m_owner->m_pauseMenu)
		{
			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(0.0f, -600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_pauseMenu = new engine::Screen();
			m_owner->m_pauseMenu->load("res/screenLayouts/pauseMenu.xml");
			m_owner->m_pauseMenu->setTransitions(slide, slide);
			m_owner->m_pauseMenu->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			progress = 0.2f;
		}
		else if (!m_owner->m_appQuitConfirm)
		{
			engine::SlideTransition* quitSlide = new engine::SlideTransition(0.2f);
			quitSlide->setMotion(engine::Vector2(0.0f, 600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_appQuitConfirm = new engine::Screen();
			m_owner->m_appQuitConfirm->load("res/screenLayouts/appQuitConfirm.xml");
			m_owner->m_appQuitConfirm->setTransitions(quitSlide, quitSlide);
			m_owner->m_appQuitConfirm->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);

			engine::SlideTransition* exitSlide = new engine::SlideTransition(0.2f);
			exitSlide->setMotion(engine::Vector2(0.0f, 600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_gameExitConfirm = new engine::Screen();
			m_owner->m_gameExitConfirm->load("res/screenLayouts/gameExitConfirm.xml");
			m_owner->m_gameExitConfirm->setTransitions(exitSlide, exitSlide);
			m_owner->m_gameExitConfirm->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);

			engine::SlideTransition* deleteSlide = new engine::SlideTransition(0.2f);
			deleteSlide->setMotion(engine::Vector2(0.0f, 600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_deleteUserConfirm = new engine::Screen();
			m_owner->m_deleteUserConfirm->load("res/screenLayouts/deleteUserConfirm.xml");
			m_owner->m_deleteUserConfirm->setTransitions(deleteSlide, deleteSlide);
			m_owner->m_deleteUserConfirm->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			m_owner->m_deleteUserText = (engine::TextItem*)m_owner->m_deleteUserConfirm->getComponent("confirmString");
			m_owner->m_deleteUserFormat = m_owner->m_deleteUserText->getText();
			progress = 0.25f;
		}
		else if (!m_owner->m_restoreGameConfirm)
		{
			engine::FadeTransition* fade = new engine::FadeTransition(0.1f);
			m_owner->m_restoreGameConfirm = new engine::Screen();
			m_owner->m_restoreGameConfirm->load("res/screenLayouts/restoreGameConfirm.xml");
			m_owner->m_restoreGameConfirm->setTransitions(fade, fade);
			m_owner->m_restoreGameConfirm->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			progress = 0.3f;
		}
		else if (!m_owner->m_gameScreen)
		{
			engine::FadeTransition* fade = new engine::FadeTransition(0.5f);
			m_owner->m_gameScreen = new GameScreen();
			m_owner->m_gameScreen->setTransitions(fade, fade);
			m_owner->m_gameScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			m_owner->m_gameStats = new GameProgress();
			m_owner->m_cityMap = new CityMap();
			m_owner->m_hud = new HudScreen();
			m_owner->m_hud->init(m_owner->m_gameStats);
			m_owner->m_hud->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			m_owner->m_gameScreen->init(m_owner->m_hud, m_owner->m_cityMap);
			m_owner->m_splats = new engine::SplatFactory();
			m_owner->m_splats->load("res\\manifests\\splats.xml");
			m_owner->m_scene->addChild(m_owner->m_splats);
			progress = 0.4f;
		}
		else if (!m_owner->m_decoratePizzaScreen)
		{
			engine::FadeTransition* decorateFade = new engine::FadeTransition(0.5f);
			m_owner->m_decoratePizzaScreen = new GameScreen();
			m_owner->m_decoratePizzaScreen->setTransitions(decorateFade, decorateFade);
			m_owner->m_decoratePizzaScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			m_owner->m_decoratePizzaScreen->load("res\\screenLayouts\\decoratePizza.xml");

			engine::FadeTransition* designerFade = new engine::FadeTransition(0.5f);
			m_owner->m_pizzaDesignerScreen = new GameScreen();
			m_owner->m_pizzaDesignerScreen->setTransitions(designerFade, designerFade);
			m_owner->m_pizzaDesignerScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			m_owner->m_pizzaDesignerScreen->load("res\\screenLayouts\\pizzaDesignerScreen.xml");
			progress = 0.43f;
		}
		else if (m_owner->m_pizzas.empty())
		{
			engine::RefPtr<engine::Reader> reader = engine::getApplication()->getReader("res\\manifests\\pizzas.xml");
			m_owner->loadPizzaManifest(reader, false);
			try
			{
				reader = engine::getApplication()->getDiskReader("userPizzas.xml");
				m_owner->loadPizzaManifest(reader, true);
			}
			catch (...)
			{
			}
			progress = 0.44f;
		}
		else if (!m_owner->m_userPizzaEditorScreen)
		{
			engine::FadeTransition* fade = new engine::FadeTransition(0.3f);
			m_owner->m_userPizzaEditorScreen = new engine::Screen();
			m_owner->m_userPizzaEditorScreen->setTransitions(fade, fade);
			m_owner->m_userPizzaEditorScreen->load("res\\screenLayouts\\userPizzaEditor.xml");

			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(0.0f, -600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_pizzaNameScreen = new engine::Screen();
			m_owner->m_pizzaNameScreen->load("res/screenLayouts/pizzaNameScreen.xml");
			m_owner->m_pizzaNameScreen->setTransitions(slide, slide);

			m_owner->m_pizzaEditor = new PizzaEditor(m_owner->m_userPizzaEditorScreen, m_owner->m_pizzaNameScreen,
				&m_owner->m_userPizzas, true);
			m_owner->m_pizzaEditor->init();
			progress = 0.45f;
		}
		else if (!m_owner->m_storyScreen)
		{
			engine::FadeTransition* fade = new engine::FadeTransition(0.4f);
			m_owner->m_storyScreen = new StoryScreen();
			m_owner->m_storyScreen->load("res/screenLayouts/storyScreen.xml");
			m_owner->m_storyScreen->initLayout();
			m_owner->m_storyScreen->initReport(m_owner->m_gameStats);
			m_owner->m_storyScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			m_owner->m_storyScreen->setTransitions(fade, fade);
			progress = 0.55f;
		}
		else if (!m_owner->m_unlockDesignerNotify)
		{
			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(800.0f, 0.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_unlockDesignerNotify = new engine::Screen();
			m_owner->m_unlockDesignerNotify->load("res/screenLayouts/unlockPizzaDesignerNotify.xml");
			m_owner->m_unlockDesignerNotify->setTransitions(slide, slide);
			m_owner->m_unlockDesignerNotify->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			engine::TextItem* notifyText = (engine::TextItem*)m_owner->m_unlockDesignerNotify->getComponent("notifyText");
			if (notifyText)
				m_owner->m_unlockDesignerFormat = notifyText->getText();
			progress = 0.6f;
		}
		else if (!m_owner->m_userSelectScreen)
		{
			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(0.0f, -600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_userSelectScreen = new engine::UserSelectScreen();
			m_owner->m_userSelectScreen->load("res/screenLayouts/userSelect.xml");
			m_owner->m_userSelectScreen->init(m_owner);
			m_owner->m_userSelectScreen->setTransitions(slide, slide);
			m_owner->m_userSelectScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			progress = 0.65f;
		}
		else if (!m_owner->m_newUserScreen)
		{
			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(0.0f, -600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_newUserScreen = new engine::Screen();
			m_owner->m_newUserScreen->load("res/screenLayouts/newUserEntry.xml");
			m_owner->m_newUserScreen->setTransitions(slide, slide);
			m_owner->m_newUserScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			engine::EditBox* nameEntry = (engine::EditBox*)m_owner->m_newUserScreen->getComponent("nameEntry");
			nameEntry->setCharFilter(&engine::HighScoreManager::filterNameChar);
			progress = 0.7f;
		}
		else if (!m_owner->m_highScoreScreen)
		{
			engine::FadeTransition* fade = new engine::FadeTransition(0.3f);
			m_owner->m_highScoreScreen = new HighScoreScreen();
			m_owner->m_highScoreScreen->load("res/screenLayouts/highScores.xml");
			m_owner->m_highScoreScreen->setTransitions(fade, fade);
			m_owner->m_highScoreScreen->init();

			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(0.0f, -600.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_highScoreResetConfirm = new engine::Screen();
			m_owner->m_highScoreResetConfirm->load("res/screenLayouts/highScoreResetConfirm.xml");
			m_owner->m_highScoreResetConfirm->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			m_owner->m_highScoreResetConfirm->setTransitions(slide, slide);
			progress = 0.75f;
		}
		else if (!m_owner->m_highScorePostScreen)
		{
			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(800.0f, 0.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_highScorePostScreen = new engine::Screen();
			m_owner->m_highScorePostScreen->load("res/screenLayouts/highScoreGlobalPost.xml");
			m_owner->m_highScorePostScreen->setTransitions(slide, slide);
			m_owner->m_highScorePostScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			progress = 0.8f;
		}
		else if (!m_owner->m_toppingBook)
		{
			engine::SlideTransition* slide = new engine::SlideTransition(0.2f);
			slide->setMotion(engine::Vector2(-800.0f, 0.0f), engine::Vector2(0.0f, 0.0f));
			m_owner->m_toppingBook = new ToppingBookScreen();
			m_owner->m_toppingBook->load("res/screenLayouts/toppingBook.xml");
			m_owner->m_toppingBook->setTransitions(slide, slide);
			m_owner->m_toppingBook->init();

			engine::FadeTransition* selectionFade = new engine::FadeTransition(0.5f);
			m_owner->m_toppingSelectScreen = new ToppingSelectionScreen();
			m_owner->m_toppingSelectScreen->load("res/screenLayouts/toppingSelectionScreen.xml");
			m_owner->m_toppingSelectScreen->setTransitions(selectionFade, selectionFade);
			m_owner->m_toppingSelectScreen->init();

			engine::FadeTransition* newToppingFade = new engine::FadeTransition(0.5f);
			m_owner->m_newToppingScreen = new NewToppingScreen();
			m_owner->m_newToppingScreen->load("res/screenLayouts/newToppingScreen.xml");
			m_owner->m_newToppingScreen->setTransitions(newToppingFade, newToppingFade);
			m_owner->m_newToppingScreen->init();
			progress = 0.85f;
		}
		else if (!m_owner->m_music)
		{
			m_owner->m_music = new MusicPlayer();
			m_owner->setMusicEnabled(m_owner->m_settings->isMusicEnabled());
			progress = 0.9f;
		}
		else if (m_owner->m_cities.empty())
		{
			m_owner->loadLevelManifest("res\\manifests\\levelManifestSpeed.xml");
			progress = 0.95f;
		}
		else if (!m_owner->m_citySelectScreen)
		{
			engine::FadeTransition* fade = new engine::FadeTransition(0.3f);
			m_owner->m_citySelectScreen = new CitySelectScreen();
			m_owner->m_citySelectScreen->load("res/screenLayouts/citySelectScreen.xml");
			m_owner->m_citySelectScreen->setTransitions(fade, fade);
			m_owner->m_citySelectScreen->init(m_owner->m_cities);
			m_owner->m_citySelectScreen->m_actionSignal.connect(m_owner, &PizzaFrenzy::onScreenEvent);
			progress = 1.0f;
		}
		else
		{
			m_owner->m_stateMachine.setState(new MainMenuState(m_owner));
			return;
		}
		engine::IntRect source;
		source.set(0, 0, (int)(m_owner->m_loadBarWidth * progress), m_owner->m_loadBarHeight);
		m_owner->m_loadBar->setSourceRect(source);
	}
}

// 0x40BF80: shows the first served topping whose count reached a higher upgrade level. `level` is not initialised:
// with no matching upgrade it keeps the previous topping's value.
bool LevelSummaryState::showToppingLevelUp()
{
	for (std::map<Topping*, int>::iterator it = m_owner->m_gameStats->m_bestToppingCombos.begin();
		it != m_owner->m_gameStats->m_bestToppingCombos.end(); ++it)
	{
		Topping* topping = it->first;
		if (topping)
		{
			int current = m_owner->m_gameStats->m_toppingLevels[topping];
			int level;
			for (unsigned int i = 0; i < topping->m_upgrades.size(); i++)
			{
				if (it->second >= topping->m_upgrades[i]->m_combo)
					level = i;
			}
			if (level > current)
			{
				m_owner->m_storyScreen->showToppingUpgrade(topping, topping->m_upgrades[level], level);
				m_owner->m_gameStats->m_toppingLevels[topping] = level;
				getGame()->saveProgress(m_owner->m_gameStats);
				return true;
			}
		}
	}
	return false;
}

// 0x40C170: the level indices, the load bar size and m_isBonusLevel are left uninitialised
PizzaFrenzy::PizzaFrenzy()
{
	m_mainMenu = 0;
	m_splashScreen = 0;
	m_optionsScreen = 0;
	m_userSelectScreen = 0;
	m_newUserScreen = 0;
	m_pizzaNameScreen = 0;
	m_highScoreScreen = 0;
	m_pauseMenu = 0;
	m_storyScreen = 0;
	m_appQuitConfirm = 0;
	m_deleteUserConfirm = 0;
	m_gameExitConfirm = 0;
	m_restoreGameConfirm = 0;
	m_toppingBook = 0;
	m_toppingSelectScreen = 0;
	m_newToppingScreen = 0;
	m_citySelectScreen = 0;
	m_highScorePostScreen = 0;
	m_levelEditor = 0;
	m_pizzaEditor = 0;
	m_gameLogic = 0;
	m_gameScreen = 0;
	m_tileManifest = 0;
	m_inEditor = false;
	m_sounds = 0;
	m_splats = 0;
	m_showHighScoresOnMenu = false;
	m_showCitySelectOnMenu = false;
	m_nextPizzaId = 0;
	m_nextUserPizzaId = 0;
	m_active = true;
}

// 0x40C700: resumes the mode's saved game, or starts the city
void PizzaFrenzy::startGame(int mode, int city)
{
	m_mode = mode;
	m_isBonusLevel = false;
	m_gameStats->reset();
	if (m_profile->hasGameInProgress(m_mode))
	{
		m_cityIndex = m_profile->getLastCity(m_mode);
		m_levelIndex = m_profile->getLastLevel(m_mode);
		m_levelNumber = m_profile->getLastDay(m_mode);
		m_gameStats->load(m_profile, m_mode);
		playLevel(true);
	}
	else
	{
		selectCity(city);
		playLevel(false);
	}
}

// 0x40C7A0
void MainMenuState::onAction(const std::string& action)
{
	if (action == "newSpeedGame")
	{
		m_owner->m_mode = 0;
		if (m_owner->m_profile->hasGameInProgress(m_owner->m_mode))
			m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_restoreGameConfirm, this));
		else if (m_owner->m_profile->getMaxCity() > 0)
			m_owner->showCitySelect();
		else
			m_owner->startGame(m_owner->m_mode, 0);
	}
	else if (action == "newMemoryGame")
	{
		m_owner->m_mode = 1;
		if (m_owner->m_profile->hasGameInProgress(m_owner->m_mode))
			m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_restoreGameConfirm, this));
		else if (m_owner->m_profile->getMaxCity() > 0)
			m_owner->showCitySelect();
		else
			m_owner->startGame(m_owner->m_mode, 0);
	}
	else if (action == "newConcentrationGame")
	{
		m_owner->m_mode = 2;
		if (m_owner->m_profile->hasGameInProgress(m_owner->m_mode))
			m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_restoreGameConfirm, this));
		else if (m_owner->m_profile->getMaxCity() > 0)
			m_owner->showCitySelect();
		else
			m_owner->startGame(m_owner->m_mode, 0);
	}
	else if (action == "viewToppingBook")
	{
		m_owner->m_toppingBook->showPages();
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_toppingBook, this));
	}
	else if (action == "nextLevel")
	{
		m_owner->advanceLevel();
		m_owner->playLevel(false);
	}
	else if (action == "options")
	{
		m_owner->m_optionsScreen->setup(m_owner, m_owner->m_settings);
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_optionsScreen, this));
	}
	else if (action == "notme")
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_userSelectScreen, this));
	else if (action == "showHighScores")
	{
		m_owner->m_highScoreScreen->showCurrentTable();
		m_owner->m_screenMgr->showScreen(m_owner->m_highScoreScreen, 0.0f);
	}
	else if (action == "highScoreDone")
		m_owner->m_screenMgr->showScreen(m_owner->m_mainMenu, 0.0f);
	else if (action == "citySelectCancel")
		m_owner->m_screenMgr->showScreen(m_owner->m_mainMenu, 0.0f);
	else if (action == "bookCancel")
		m_owner->m_screenMgr->showScreen(m_owner->m_mainMenu, 0.0f);
	else if (action == "highScoreReset")
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_highScoreResetConfirm, this));
	else if (action.substr(0, 10) == "citySelect")
	{
		int city;
		sscanf(action.c_str(), "citySelect%d", &city);
		if (city >= 0 && city < (int)m_owner->m_cities.size())
			m_owner->startGame(m_owner->m_mode, city);
	}
	else if (action == "edit")
	{
		m_owner->m_levelEditor->newCity();
		m_owner->m_stateMachine.setState(new LevelEditorState(m_owner));
	}
	else if (action == "editPizza")
	{
		if ((int)PizzaFrenzy::getProfile()->m_toppingLevels.size() >= g_pizzaEditorUnlock || g_unlockAll)
		{
			m_owner->m_pizzaEditor->open();
			m_owner->m_stateMachine.setState(new PizzaDesignerState(m_owner, false));
		}
		else
		{
			engine::TextItem* notifyText = (engine::TextItem*)m_owner->m_unlockDesignerNotify->getComponent("notifyText");
			if (notifyText)
			{
				std::string text;
				engine::format(text, m_owner->m_unlockDesignerFormat.c_str(),
					PizzaFrenzy::getProfile()->m_toppingLevels.size());
				notifyText->setText(text);
			}
			m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_unlockDesignerNotify, this));
		}
	}
	else if (action == "quit")
		m_owner->m_stateMachine.setState(new PopupState(m_owner, m_owner->m_appQuitConfirm, this));
	else if (action == "gameLink")
		m_owner->openGameLink();
	else if (action == "loseFullScreen")
	{
		if (engine::getApplication()->isFullScreen())
		{
			onAction("pauseGame");
			m_owner->setFullscreen(!engine::getApplication()->isFullScreen());
		}
	}
}

// 0x40CF30: once the closing popup's transition is over, removes the popup layer and returns (or starts the game)
void PopupState::update(engine::UpdateContext& context)
{
	if (m_closing && !g_popupScreenMgr->isTransitioning())
	{
		m_owner->m_scene->removeChild(g_popupScreenMgr);
		m_owner->m_screenMgr->activate();
		m_closing = false;
		if (m_startGame)
		{
			if (!m_owner->m_profile->hasGameInProgress(m_owner->m_mode) && m_owner->m_profile->getMaxCity() != 0)
			{
				m_owner->m_showCitySelectOnMenu = true;
				m_owner->m_stateMachine.setState(m_returnState);
			}
			else
				m_owner->startGame(m_owner->m_mode, m_city);
		}
		else
			m_owner->m_stateMachine.setState(m_returnState);
	}
}

// 0x40CFF0
void LevelSummaryState::update(engine::UpdateContext& context)
{
	m_owner->m_storyScreen->updateStory(context);
	if (!m_started && !m_owner->m_screenMgr->isTransitioning())
	{
		m_owner->m_storyScreen->playStory();
		m_started = true;
	}
	else if (m_storyDone)
	{
		m_storyDone = false;
		if (showToppingLevelUp())
		{
			m_showingLevelUp = true;
			m_owner->m_storyScreen->playStory();
		}
		else if (m_showingLevelUp)
		{
			m_showingLevelUp = false;
			m_owner->m_storyScreen->showChefTitle();
			m_owner->m_storyScreen->playStory();
		}
		else
		{
			m_owner->m_storyScreen->stopStory();
			m_owner->playLevel(false);
		}
	}
}

// 0x40D0D0
engine::Game* createGame()
{
	return new PizzaFrenzy();
}

// 0x40D3E0
void PopupState::onAction(const std::string& action)
{
	if (action == "done" || action == "bookCancel")
	{
		if (m_fromPauseMenu)
		{
			m_fromPauseMenu = false;
			showPopupScreen(m_owner->m_pauseMenu);
		}
		else
			closePopup();
	}
	else if (action == "quit")
	{
		m_fromPauseMenu = true;
		showPopupScreen(m_owner->m_appQuitConfirm);
	}
	else if (action == "appQuitCancel")
		closePopup();
	else if (action == "appQuitFinal")
		m_owner->quit();
	else if (action == "saveGameContinue")
	{
		closePopup();
		m_startGame = true;
	}
	else if (action == "saveGameNoContinue")
	{
		m_owner->m_profile->clearGameInProgress(m_owner->m_mode);
		closePopup();
		m_startGame = true;
	}
	else if (action == "nameEntryDone")
	{
		engine::EditBox* nameEntry = (engine::EditBox*)m_owner->m_newUserScreen->getComponent("nameEntry");
		const std::string& name = nameEntry->getText();
		if (!name.empty())
		{
			engine::User* user = new engine::User();
			user->setName(name);
			user->getAttributes().setInt("foodBank", 0);
			engine::UserManager* users = engine::UserManager::getInstance();
			users->addUser(user);
			if (users->getUserCount() > 1)
				showPopupScreen(m_owner->m_userSelectScreen);
			else
				closePopup();
		}
	}
	else if (action == "nameEntryCancel")
		showPopupScreen(m_owner->m_userSelectScreen);
	else if (action == "newUser")
	{
		if (!m_owner->m_userSelectScreen->isFull())
			showPopupScreen(m_owner->m_newUserScreen);
	}
	else if (action == "deleteUser")
	{
		std::string text;
		engine::format(text, m_owner->m_deleteUserFormat.c_str(),
			m_owner->m_userSelectScreen->getSelectedUserName().c_str());
		m_owner->m_deleteUserText->setText(text);
		showPopupScreen(m_owner->m_deleteUserConfirm);
	}
	else if (action == "deleteUserYes")
	{
		showPopupScreen(m_owner->m_userSelectScreen);
		m_owner->m_userSelectScreen->deleteSelectedUser();
	}
	else if (action == "deleteUserNo")
		showPopupScreen(m_owner->m_userSelectScreen);
	else if (action == "pauseOptions")
	{
		m_fromPauseMenu = true;
		showPopupScreen(m_owner->m_optionsScreen);
	}
	else if (action == "pauseQuit")
	{
		m_fromPauseMenu = true;
		showPopupScreen(m_owner->m_gameExitConfirm);
	}
	else if (action == "toppingScreen")
	{
		m_fromPauseMenu = true;
		m_owner->m_toppingBook->showPages();
		showPopupScreen(m_owner->m_toppingBook);
	}
	else if (action == "gameExitSave")
	{
		m_owner->saveProgress(m_owner->m_gameStats);
		if (m_owner->m_gameLogic)
			m_owner->m_gameLogic->endLevel();
		m_returnState = new MainMenuState(m_owner);
		closePopup();
	}
	else if (action == "gameExitQuit")
	{
		if (m_owner->m_gameLogic)
			m_owner->m_gameLogic->endLevel();
		m_owner->m_profile->clearGameInProgress(m_owner->m_mode);
		if (m_owner->m_highScoreScreen->submitScore(false))
			m_owner->m_showHighScoresOnMenu = true;
		if (m_owner->m_gameStats->m_score > 0)
			showPopupScreen(m_owner->m_highScorePostScreen);
		else
		{
			m_returnState = new MainMenuState(m_owner);
			closePopup();
		}
	}
	else if (action == "gameExitCancel")
		closePopup();
	else if (action == "userSelectCancel")
		closePopup();
	else if (action == "userSelectOk")
	{
		m_owner->m_userSelectScreen->selectUser();
		closePopup();
	}
	else if (action == "highScoreResetConfirm")
	{
		m_owner->m_highScoreScreen->clearScores();
		m_owner->m_showHighScoresOnMenu = true;
		closePopup();
	}
	else if (action == "highScoreResetCancel")
	{
		m_owner->m_showHighScoresOnMenu = true;
		closePopup();
	}
	else if (action == "postGlobalScore")
	{
		m_owner->m_highScoreScreen->submitScore(true);
		m_owner->m_showHighScoresOnMenu = true;
		m_returnState = new MainMenuState(m_owner);
		closePopup();
	}
	else if (action == "cancelGlobalScore")
	{
		m_returnState = new MainMenuState(m_owner);
		closePopup();
	}
}

// 0x41A1D0: the "pizza%d" topping of a user pizza
Topping* PizzaFrenzy::getPizzaTopping(int pizzaId)
{
	return m_userPizzaCombos[pizzaId];
}
