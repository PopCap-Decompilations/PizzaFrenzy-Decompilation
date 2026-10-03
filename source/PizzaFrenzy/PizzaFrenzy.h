// PizzaFrenzy (the game object the engine application drives), the game states it runs, and the free functions and
// globals of PizzaFrenzy.cpp.
#pragma once

#include <map>
#include <string>
#include <vector>

#include <windows.h>

#include "engine/GameBase.h"
#include "engine/RefPtr.h"
#include "engine/State.h"
#include "engine/StateMachine.h"
#include "engine/sigslot.h"

namespace engine
{
	class Application;
	class Bitmap;
	class Image;
	class Reader;
	class Scene;
	class Screen;
	class ScreenMgr;
	class SoundMgr;
	class SplatFactory;
	class StateBase;
	class TextItem;
	class UserSelectScreen;
}

class City;
class CityEditor;
class CityEditorScreen;
class CityMap;
class CitySelectScreen;
class GameLogic;
class GameProgress;
class GameScreen;
class GameSettings;
class HighScoreScreen;
class HudScreen;
class Level;
class MainMenuScreen;
class MusicPlayer;
class NewToppingScreen;
class PizzaDesign;
class PizzaEditor;
class PizzaOptionsScreen;
class StoryScreen;
class TileManager;
class Topping;
class ToppingBookScreen;
class ToppingSelectionScreen;
class UserProgress;

// The game: created by createGame() for the engine application, which drives it through engine::Game
// (getSettings, start, shutdown). It owns the scene, the screen manager, the state machine of the game states
// below, every screen, the manifests, the user profile, and the current mode, city and level. It receives the
// application's signals and every screen's action signal (sigslot). g_game points to it.
class PizzaFrenzy : public engine::GameBase, public sigslot::has_slots<>
{
public:
	PizzaFrenzy();
	virtual ~PizzaFrenzy();

	// engine::Game
	virtual void getSettings(engine::AppConfig& settings);				// slot 1
	virtual void start(engine::Application* app);						// slot 2
	// 0x492310 (folded)
	virtual void startScreenSaver(engine::Application* app)				// slot 3: empty
	{
	}
	// 0x4D1520 (folded)
	virtual void configureScreenSaver(HINSTANCE instance, HWND parent)	// slot 4: empty
	{
	}
	virtual void shutdown();											// slot 5

	// engine::GameBase
	virtual void setMusicEnabled(bool enabled);							// slot 7

	virtual void onUpdate(engine::UpdateContext& context);				// slot 8: application update signal (+0x04)
	virtual void onScreenEvent(const std::string& event);				// slot 9: every screen's action signal
	virtual void onDisplayChanged();									// slot 10: application signals +0xD4, +0xE4
	virtual void onActivate(bool active);								// slot 11: application signal +0x24
	virtual void setFullscreen(bool fullscreen);						// slot 12
	virtual void updateSound(int elapsedMs);							// slot 13: application signal +0x104
	virtual void addCity(City* city);									// slot 14
	virtual void addPizza(PizzaDesign* pizza, bool userPizza);			// slot 15
	virtual void saveProgress(GameProgress* stats);						// slot 16
	virtual void quit();												// slot 17
	virtual void onKeyPress(int key);									// slot 18
	virtual void openGameLink();										// slot 19

	int getNextPizzaId(bool userPizza) const;
	void setNextPizzaId(int id, bool userPizza);
	engine::Bitmap* getCitySelectEntry(int index);
	void endLevel();
	void showCitySelect();
	void selectCity(int city);
	City* getCurrentCity() const;
	void advanceLevel();
	bool loadCity(const std::string& file, GameScreen* gameScreen);
	bool loadLevelManifest(const std::string& file);
	bool loadPizzaManifest(engine::Reader* reader, bool userPizzas);
	void startLevel();
	void showPauseMenu(engine::StateBase* returnState);
	void finishLevel();
	void showToppingSelection();
	void prepareLevel();
	PizzaDesign* getPizza(int id);
	void playLevel(bool restored);
	void startGame(int mode, int city);
	Topping* getPizzaTopping(int pizzaId);

	// the game's parts, for the other classes (through g_game)
	static TileManager* getTileManifest();
	static GameLogic* getGameLogic();
	static engine::SoundMgr* getSounds();
	static MusicPlayer* getMusicPlayer();
	static CityMap* getCityMap();
	static GameScreen* getGameScreen();
	static UserProgress* getProfile();
	static GameProgress* getGameStats();
	static engine::SplatFactory* getSplatFactory();
	static HudScreen* getHud();

	engine::RefPtr<engine::Scene> m_scene;								// +0x20 root scene: screen manager, splat factory and popup screen manager are its children
	engine::StateMachine m_stateMachine;								// +0x24 runs the game states below
	engine::RefPtr<engine::SoundMgr> m_sounds;							// +0x3C sound effects (res\manifests\soundManifest.xml)
	engine::RefPtr<engine::ScreenMgr> m_screenMgr;						// +0x40 main screen manager
	engine::RefPtr<GameSettings> m_settings;							// +0x44 fullscreen, music, sound, story option, current user, game link
	int m_levelIndex;													// +0x48 level in the current city
	int m_cityIndex;													// +0x4C current city (index into m_cities)
	int m_unused50;														// +0x50 never read or written, not even initialised
	int m_levelNumber;													// +0x54 overall 1-based level number (pause caption, profile)
	int m_mode;															// +0x58 0 speed, 1 memory, 2 concentration
	engine::RefPtr<engine::Screen> m_splashScreen;						// +0x5C res/screenLayouts/splashScreen.xml
	engine::RefPtr<engine::Image> m_loadBar;							// +0x60 the splash's "loadBar", clipped to width * progress while loading
	int m_loadBarWidth;													// +0x64 full width of m_loadBar
	int m_loadBarHeight;												// +0x68 height of m_loadBar
	engine::RefPtr<engine::UserSelectScreen> m_userSelectScreen;		// +0x6C res/screenLayouts/userSelect.xml
	engine::RefPtr<engine::Screen> m_newUserScreen;						// +0x70 res/screenLayouts/newUserEntry.xml
	engine::RefPtr<engine::Screen> m_deleteUserConfirm;					// +0x74 res/screenLayouts/deleteUserConfirm.xml
	engine::RefPtr<engine::TextItem> m_deleteUserText;					// +0x78 its "confirmString"
	std::string m_deleteUserFormat;										// +0x7C original text of "confirmString" (takes the user name)
	engine::RefPtr<MainMenuScreen> m_mainMenu;							// +0x98 res/screenLayouts/mainMenu.xml
	engine::RefPtr<PizzaOptionsScreen> m_optionsScreen;					// +0x9C res/screenLayouts/optionMenu.xml
	engine::RefPtr<HighScoreScreen> m_highScoreScreen;					// +0xA0 res/screenLayouts/highScores.xml
	engine::RefPtr<engine::Screen> m_highScoreResetConfirm;				// +0xA4 res/screenLayouts/highScoreResetConfirm.xml
	engine::RefPtr<engine::Screen> m_restoreGameConfirm;				// +0xA8 res/screenLayouts/restoreGameConfirm.xml
	engine::RefPtr<engine::Screen> m_appQuitConfirm;					// +0xAC res/screenLayouts/appQuitConfirm.xml
	engine::RefPtr<engine::Screen> m_gameExitConfirm;					// +0xB0 res/screenLayouts/gameExitConfirm.xml
	engine::RefPtr<engine::Screen> m_unlockDesignerNotify;				// +0xB4 res/screenLayouts/unlockPizzaDesignerNotify.xml
	std::string m_unlockDesignerFormat;									// +0xB8 original "notifyText" of that screen (takes the levels completed)
	engine::RefPtr<engine::Screen> m_pauseMenu;							// +0xD4 res/screenLayouts/pauseMenu.xml
	engine::RefPtr<engine::Screen> m_highScorePostScreen;				// +0xD8 res/screenLayouts/highScoreGlobalPost.xml
	engine::RefPtr<StoryScreen> m_storyScreen;							// +0xDC res/screenLayouts/storyScreen.xml
	engine::RefPtr<ToppingBookScreen> m_toppingBook;					// +0xE0 res/screenLayouts/toppingBook.xml
	engine::RefPtr<ToppingSelectionScreen> m_toppingSelectScreen;		// +0xE4 res/screenLayouts/toppingSelectionScreen.xml
	engine::RefPtr<GameScreen> m_pizzaDesignerScreen;					// +0xE8 res\screenLayouts\pizzaDesignerScreen.xml
	engine::RefPtr<NewToppingScreen> m_newToppingScreen;				// +0xEC res/screenLayouts/newToppingScreen.xml
	engine::RefPtr<CitySelectScreen> m_citySelectScreen;				// +0xF0 res/screenLayouts/citySelectScreen.xml
	engine::RefPtr<GameScreen> m_decoratePizzaScreen;					// +0xF4 res\screenLayouts\decoratePizza.xml (bonus round)
	engine::RefPtr<CityEditor> m_levelEditor;							// +0xF8 developer city editor: released, never assigned
	engine::RefPtr<PizzaEditor> m_pizzaEditor;							// +0xFC user pizza editor
	engine::RefPtr<engine::Screen> m_userPizzaEditorScreen;				// +0x100 res\screenLayouts\userPizzaEditor.xml
	engine::RefPtr<engine::Screen> m_pizzaNameScreen;					// +0x104 res/screenLayouts/pizzaNameScreen.xml
	engine::RefPtr<CityEditorScreen> m_levelEditorScreen;				// +0x108 the city editor's screen: released, never assigned
	engine::RefPtr<engine::Screen> m_levelEditorTiles;					// +0x10C the city editor's tile popup: released, never assigned
	engine::RefPtr<GameLogic> m_gameLogic;								// +0x110 the running level's gameplay controller
	engine::RefPtr<GameScreen> m_gameScreen;							// +0x114 screen the city map is played on
	engine::RefPtr<HudScreen> m_hud;									// +0x118
	engine::RefPtr<CityMap> m_cityMap;									// +0x11C filled by loadCity
	engine::RefPtr<GameProgress> m_gameStats;							// +0x120 the running game's score and topping levels
	engine::RefPtr<TileManager> m_tileManifest;							// +0x124 res\manifests\tileManifest.xml (tiles, toppings, music)
	engine::RefPtr<UserProgress> m_profile;								// +0x128 the current user's progress
	engine::RefPtr<engine::SplatFactory> m_splats;						// +0x12C res\manifests\splats.xml, child of m_scene
	engine::RefPtr<MusicPlayer> m_music;								// +0x130
	bool m_showHighScoresOnMenu;										// +0x134 MainMenuState shows the high scores instead of the menu
	bool m_showCitySelectOnMenu;										// +0x135 MainMenuState shows the city select instead of the menu
	bool m_isBonusLevel;												// +0x136 the level's bonus flag (startLevel)
	bool m_inEditor;													// +0x137 LevelEditorState or PizzaDesignerState is active
	std::vector<engine::RefPtr<City> > m_cities;						// +0x138 res\manifests\levelManifestSpeed.xml (addCity)
	std::vector<engine::RefPtr<PizzaDesign> > m_pizzas;					// +0x148 standard pizzas (res\manifests\pizzas.xml)
	std::map<int, engine::RefPtr<PizzaDesign> > m_pizzasById;			// +0x158 standard pizzas by id
	std::vector<engine::RefPtr<PizzaDesign> > m_userPizzas;				// +0x164 user pizzas (userPizzas.xml), shared with m_pizzaEditor
	std::map<int, engine::RefPtr<Topping> > m_userPizzaCombos;			// +0x174 the "pizza%d" topping of each user pizza, by pizza id
	int m_nextPizzaId;													// +0x180 next id for a standard pizza
	int m_nextUserPizzaId;												// +0x184 next id for a user pizza
	bool m_active;														// +0x188 the application is active (onActivate)
};

// Modal state: shows a screen on the popup screen manager (g_popupScreenMgr) over the dimmed main one. When the
// popup has closed it returns to m_returnState, or starts (or restores) a game.
class PopupState : public engine::State<PizzaFrenzy>
{
public:
	PopupState(PizzaFrenzy* game, engine::Screen* screen, engine::StateBase* returnState);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void exit();												// slot 2
	virtual void update(engine::UpdateContext& context);				// slot 3
	virtual void onAction(const std::string& action);					// slot 4

	virtual void closePopup();											// slot 5
	virtual void removePopupLayer();									// slot 6
	virtual void showPopupScreen(engine::Screen* screen);				// slot 7

	engine::RefPtr<engine::Screen> m_screen;							// +0x10 shown by enter(), then released
	engine::RefPtr<engine::StateBase> m_returnState;					// +0x14 state set again when the popup has closed
	bool m_closing;														// +0x18 closePopup() started; update() waits for the popup transition
	bool m_startGame;													// +0x19 on close, start the game instead of returning
	int m_city;															// +0x1C city for startGame when m_startGame is set
	bool m_fromPauseMenu;												// +0x20 a sub-popup of the pause menu: "done"/"bookCancel" go back to it
};

// First state after the splash screen: its update creates one batch of screens and manifests per frame while the
// load bar advances, then switches to MainMenuState.
class LoadingState : public engine::State<PizzaFrenzy>
{
public:
	LoadingState(PizzaFrenzy* game);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void update(engine::UpdateContext& context);				// slot 3

	float m_delay;														// +0x10 1.0 in enter(), minus the elapsed time; loading starts below 0
};

// The main menu: new speed/memory/concentration game, city select, topping book, options, high scores, user
// select, the editors, quit and the game link.
class MainMenuState : public engine::State<PizzaFrenzy>
{
public:
	MainMenuState(PizzaFrenzy* game);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void update(engine::UpdateContext& context);				// slot 3
	virtual void onAction(const std::string& action);					// slot 4

	bool m_needNewUser;													// +0x10 no current user: update() opens the new-user popup
	bool m_endLevelPending;												// +0x11 update() calls PizzaFrenzy::endLevel() once the menu is shown
};

// A normal level is running: the gameplay controller is updated every frame; pause, blur, menu, levelEnd and
// gameOver are handled here.
class PlayingState : public engine::State<PizzaFrenzy>
{
public:
	PlayingState(PizzaFrenzy* game);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void update(engine::UpdateContext& context);				// slot 3 (folded with BonusRoundState's)
	virtual void onAction(const std::string& action);					// slot 4
};

// The bonus pizza-decorating round on the decoratePizza screen; other actions go to its gameplay controller.
class BonusRoundState : public engine::State<PizzaFrenzy>
{
public:
	BonusRoundState(PizzaFrenzy* game);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void update(engine::UpdateContext& context);				// slot 3 (folded with PlayingState's)
	virtual void onAction(const std::string& action);					// slot 4
};

// The level summary story after a level (res\story\levelSummary.xml or bonusSummary.xml): shows the topping
// level-ups, then plays the next level.
class LevelSummaryState : public engine::State<PizzaFrenzy>
{
public:
	LevelSummaryState(PizzaFrenzy* game, Level* level);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void update(engine::UpdateContext& context);				// slot 3
	virtual void onAction(const std::string& action);					// slot 4

	bool showToppingLevelUp();

	bool m_started;														// +0x10 the story was started after the screen transition
	bool m_storyDone;													// +0x11 "storyDone" received
	bool m_showingLevelUp;												// +0x12 a topping level-up page is shown
	Level* m_level;														// +0x14 the level just completed (not referenced)
};

// The story before a level (the level's story file for the mode), or the game-won ending (no level,
// res\story\gameWon.xml).
class StoryState : public engine::State<PizzaFrenzy>
{
public:
	StoryState(PizzaFrenzy* game, Level* level);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void update(engine::UpdateContext& context);				// slot 3
	virtual void onAction(const std::string& action);					// slot 4

	bool m_started;														// +0x10 the story was started after the screen transition
	bool m_storyDone;													// +0x11 "storyDone": go on to prepareLevel()
	bool m_endGame;														// +0x12 "endGame" (ending): submit the score, back to the menu
	Level* m_level;														// +0x14 the level whose story is shown, 0 for the ending (not referenced)
};

// The topping selection before a level ("selectionDone" starts it); skipped with 2 or fewer toppings unlocked.
class ToppingSelectionState : public engine::State<PizzaFrenzy>
{
public:
	ToppingSelectionState(PizzaFrenzy* game, Level* level);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void onAction(const std::string& action);					// slot 4

	Level* m_level;														// +0x10 the level the toppings are chosen for (not referenced)
};

// The "new topping" screen after a level that unlocks toppings ("toppingSelected", "keepPlaying", "designPizza").
class NewToppingState : public engine::State<PizzaFrenzy>
{
public:
	NewToppingState(PizzaFrenzy* game);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void onAction(const std::string& action);					// slot 4
};

// The developer city editor ("editorExit", "editorTiles"). Its editor and screens (PizzaFrenzy +0xF8, +0x108,
// +0x10C) are never created in the release build.
class LevelEditorState : public engine::State<PizzaFrenzy>
{
public:
	LevelEditorState(PizzaFrenzy* game);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void exit();												// slot 2
	virtual void update(engine::UpdateContext& context);				// slot 3
	virtual void onAction(const std::string& action);					// slot 4
};

// The user pizza editor (userPizzaEditor.xml; "namePizza", "editorExit"), entered from the main menu or from
// NewToppingState.
class PizzaDesignerState : public engine::State<PizzaFrenzy>
{
public:
	PizzaDesignerState(PizzaFrenzy* game, bool fromGame);

	// engine::StateBase
	virtual void enter();												// slot 1
	virtual void exit();												// slot 2
	virtual void onAction(const std::string& action);					// slot 4

	bool m_fromGame;													// +0x10 entered from NewToppingState: "editorExit" goes back to the topping selection
};

PizzaFrenzy* getGame();
engine::Game* createGame();

extern PizzaFrenzy* g_game;
extern engine::RefPtr<engine::ScreenMgr> g_popupScreenMgr;
extern bool g_unlockAll;
