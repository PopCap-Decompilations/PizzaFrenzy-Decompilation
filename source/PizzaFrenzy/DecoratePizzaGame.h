// DecoratePizzaGame (the bonus level, decoratePizza.xml) and its states: intro, play, score and end of each pizza.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "engine/FadeTimer.h"
#include "engine/Point.h"
#include "engine/RefPtr.h"
#include "engine/State.h"
#include "engine/sigslot.h"
#include "AnimatedContainer.h"
#include "GameLogic.h"

namespace engine
{
	class Component;
	class Container;
	class Image;
	class SoundHandle;
	class TextItem;
	class TextTyper;
}

class AnimatedText;
class GameProgress;
class GameScreen;
class Level;
class OrderButton;
class PizzaDesign;
class Topping;
class ToppingButton;

// The bonus level (a Level with the "bonus" attribute), laid out by res\screenLayouts\decoratePizza.xml: Lorenzo
// shows a target pizza; the player picks toppings from the topping buttons and drops them on their own pizza before
// the timer runs out, and each target topping is rated PERFECT/GOOD/OKAY/MISS by distance. The game creates it
// instead of the mode's GameLogic and calls init (slot 47). 0x354 bytes: GameLogic up to +0x118 (its has_slots<>
// at +0xC), the members below, then the vtordisp (+0x34C) and the Interface (+0x350).
class DecoratePizzaGame : public GameLogic
{
public:
	DecoratePizzaGame();
	virtual ~DecoratePizzaGame();

	virtual void init(GameScreen* screen, GameProgress* stats);		// slot 47: new (hides GameLogic::init, slot 1)
	virtual void setTargetPizza(PizzaDesign* pizza);				// slot 48

	virtual void update(engine::UpdateContext& ctx);				// slot 2 (GameLogic)
	virtual void handleEvent(const std::string& name);				// slot 3 (GameLogic)
	virtual void startLevel(Level* level);							// slot 4 (GameLogic)
	virtual void unloadLevel();										// slot 5 (GameLogic)
	virtual void startPlaying();									// slot 7 (GameLogic)
	virtual void endLevel();										// slot 8 (GameLogic)
	virtual void setPaused(bool paused);							// slot 11 (GameLogic)
	virtual void cheatWinLevel();									// slot 41 (GameLogic)

	void onMouseMove(const engine::Point& pos);						// Application::m_mouseMoveSignal
	void setupAnimations();
	void finishPizza();
	bool allToppingsPlaced() const;
	bool scoreTopping(std::vector<engine::RefPtr<ToppingButton> >::iterator target);
	void updateToppingButton(OrderButton* button);
	void createTargetToppings(PizzaDesign* pizza);
	void placeTopping(const engine::Vector2& localPos, Topping* topping);
	void onMouseDown(const engine::Point& pos);						// Application::m_mouseDownSignal
	void setSelectedTopping(Topping* topping);
	void removeToppingAt(const engine::Vector2& localPos);
	void createToppingButtons(PizzaDesign* pizza);
	void onRightMouseDown(const engine::Point& pos);				// Application::m_rightMouseDownSignal

	std::vector<engine::RefPtr<ToppingButton> > m_userToppings;			// +0x118 icons placed on the user pizza
	engine::RefPtr<engine::Component> m_userPizza;						// +0x128 "userPizza"
	engine::Vector2 m_userPizzaPos;										// +0x12C (205, 280); slides in from x - 400
	engine::RefPtr<engine::Container> m_userToppingLayer;				// +0x134 "userToppings": parent of the icons
	std::vector<engine::RefPtr<ToppingButton> > m_targetToppings;		// +0x138 icons of the target pizza
	engine::RefPtr<engine::Component> m_targetPizza;					// +0x148 "targetPizza"
	engine::RefPtr<engine::Container> m_targetToppingLayer;				// +0x14C "targetToppings"
	engine::Vector2 m_targetPizzaPos;									// +0x150 (605, 280); slides in from x + 400
	engine::RefPtr<engine::TextItem> m_timerText;						// +0x158 "timer": (int)m_timeLeft
	engine::RefPtr<engine::TextItem> m_pizzaCountText;					// +0x15C "pizzaCount": m_pizzasLeft
	engine::RefPtr<engine::TextTyper> m_lorenzoText;					// +0x160 styled like "LorenzoText"
	engine::RefPtr<engine::Container> m_lorenzo;						// +0x164 "Lorenzo"
	engine::RefPtr<engine::Component> m_unused168;						// +0x168 never assigned (dtor releases it)
	engine::RefPtr<engine::Container> m_toppingButtonBar;				// +0x16C "toppingButtons", 80 px apart
	engine::RefPtr<Topping> m_selectedTopping;							// +0x170 topping held by the cursor
	engine::RefPtr<Topping> m_pausedTopping;							// +0x174 m_selectedTopping kept by setPaused
	engine::RefPtr<engine::Image> m_cursorImage;						// +0x178 selected topping under the mouse
	engine::RefPtr<engine::TextItem> m_caption;							// +0x17C "caption": the pizza name in quotes
	engine::RefPtr<engine::Component> m_timerGroup;						// +0x180 "timerGroup"
	std::map<Topping*, engine::RefPtr<OrderButton> > m_toppingButtons;	// +0x184 button per topping of the pizza
	std::vector<engine::RefPtr<PizzaDesign> >::iterator m_nextPizza;	// +0x190 next pizza of Level::m_pizzas
	float m_timeLeft;													// +0x194 seconds left (Level::m_numOrders)
	int m_pizzasLeft;													// +0x198 pizzas still to show
	bool m_isPlaying;													// +0x19C clicks place toppings (play state)
	engine::PositionTrack m_targetPizzaPath;							// +0x1A0 slides the target pizza
	engine::FadeTimer m_targetPizzaTimer;								// +0x1F0 0.5 s, drives m_targetPizzaPath
	engine::PositionTrack m_userPizzaPath;								// +0x214 slides the user pizza
	engine::FadeTimer m_userPizzaTimer;									// +0x264 drives m_userPizzaPath, m_buttonFade
	engine::PositionTrack m_timerGroupPath;								// +0x288 (410, -160) to (410, 90)
	engine::FadeTimer m_timerGroupTimer;								// +0x2D8 created paused; startPlaying resumes
	engine::ScaleTrack m_buttonFade;									// +0x2FC 0 to 1 on m_toppingButtonBar
};

// Takes the level's next pizza (or goes to the end state), slides the target pizza in, reveals its toppings one by
// one, then slides the user pizza in and fades in the topping buttons. Acts only when m_timer is below 0. 0x38
// bytes: State +0x0, has_slots<> +0x10 (connected to nothing), the members below, vtordisp +0x30, Interface +0x34.
class DecoratePizzaIntroState : public engine::State<DecoratePizzaGame>, public sigslot::has_slots<>
{
public:
	DecoratePizzaIntroState(DecoratePizzaGame* game);

	virtual void enter();											// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);			// slot 3 (engine::StateBase)

	float m_timer;													// +0x20 delay before the next phase
	int m_phase;													// +0x24 0-5 (see update)
	int m_toppingIndex;												// +0x28 target toppings revealed so far
	int m_toppingCount;												// +0x2C m_targetToppings.size() at enter
};

// The timed play phase: counts m_timeLeft down, selects a topping by button command, picks a placed topping up on a
// right click, and loops "popup_wait" under 5 s. 0x20 bytes (vtordisp +0x18, Interface +0x1C).
class DecoratePizzaPlayState : public engine::State<DecoratePizzaGame>
{
public:
	DecoratePizzaPlayState(DecoratePizzaGame* game);

	virtual void enter();											// slot 1 (engine::StateBase)
	virtual void exit();											// slot 2 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);			// slot 3 (engine::StateBase)
	virtual void onAction(const std::string& action);				// slot 4 (engine::StateBase): a topping name

	engine::SoundHandle* m_hurrySound;								// +0x10 "popup_wait", looping (not counted)
	bool m_hurryStarted;											// +0x14 the loop has started
};

// Rates each target topping every 0.1 s, then slides both pizzas out and goes to the next pizza or to the end
// state. 0x24 bytes (vtordisp +0x1C, Interface +0x20).
class DecoratePizzaScoreState : public engine::State<DecoratePizzaGame>
{
public:
	DecoratePizzaScoreState(DecoratePizzaGame* game);

	virtual void enter();											// slot 1 (engine::StateBase)
	virtual void update(engine::UpdateContext& context);			// slot 3 (engine::StateBase)

	float m_timer;													// +0x10 step delay (0.5, then 0.1)
	int m_phase;													// +0x14 0 start, 1 rate, 2 reverse, 3 slide out
	std::vector<engine::RefPtr<ToppingButton> >::iterator m_current;	// +0x18 target topping being rated
};

// End of the level: "TIME'S UP!", "LEVEL CLEAR!", "PERFECT JOB!" or "GOOD JOB!" title and music_levelComplete,
// then the "levelEnd" event. Its exit (0x45C240) is folded with LevelEndAction's. 0x24 bytes (vtordisp +0x1C,
// Interface +0x20).
class DecoratePizzaEndState : public engine::State<DecoratePizzaGame>
{
public:
	DecoratePizzaEndState(DecoratePizzaGame* game);

	virtual void enter();											// slot 1 (engine::StateBase)
	virtual void exit();											// slot 2 (engine::StateBase), folded body 0x45C240
	virtual void update(engine::UpdateContext& context);			// slot 3 (engine::StateBase)

	engine::RefPtr<AnimatedText> m_title;							// +0x10 titleFont.xml splat at (400, 200)
	float m_timer;													// +0x14 2.5 s shown, then 2.0 s after hiding
	bool m_titleHidden;												// +0x18 second phase
};
