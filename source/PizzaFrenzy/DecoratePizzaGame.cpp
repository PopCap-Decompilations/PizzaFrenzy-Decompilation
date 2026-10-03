// DecoratePizzaGame (the bonus level, decoratePizza.xml) and its states: intro, play, score and end of each pizza.
#include "DecoratePizzaGame.h"

#include "engine/Application.h"
#include "engine/Component.h"
#include "engine/Container.h"
#include "engine/Image.h"
#include "engine/KeyframeCurve.h"
#include "engine/PropertyTrack.h"
#include "engine/ScreenLayout.h"
#include "engine/SoundHandle.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/StringUtil.h"
#include "engine/TextItem.h"
#include "engine/TextTyper.h"
#include "AnimatedText.h"
#include "Constants.h"
#include "GameProgress.h"
#include "GameScreen.h"
#include "Level.h"
#include "MusicPlayer.h"
#include "MusicTrack.h"
#include "OrderButton.h"
#include "PizzaDesign.h"
#include "PizzaFrenzy.h"
#include "TileManager.h"
#include "ToppingButton.h"

namespace
{
	// The keys are built by an inlined constructor-like helper of the original (no address): the value comes by
	// value, then the key's members are assigned one by one (the value's default constructor runs after the
	// argument is built). engine::Keyframe itself is a plain struct.
	template <class T>
	engine::Keyframe<T> makeKeyframe(float time, T value, float inTime, float inWeight, float outTime, float outWeight)
	{
		engine::Keyframe<T> key;
		key.time = time;
		key.value = value;
		key.inTime = inTime;
		key.inWeight = inWeight;
		key.outTime = outTime;
		key.outWeight = outWeight;
		return key;
	}
}

// 0x45BCC0
void DecoratePizzaGame::handleEvent(const std::string& name)
{
	m_action.handleCommand(name);
}

// 0x45BD20
void DecoratePizzaGame::update(engine::UpdateContext& ctx)
{
	m_timerGroupTimer.update(ctx.elapsed);
	m_timerGroupPath.animate(m_timerGroupTimer.getValue(), m_timerGroup);
	m_player->m_playTime += ctx.elapsed;
	m_action.update(ctx);
}

// 0x45BD90
void DecoratePizzaGame::unloadLevel()
{
	m_userToppingLayer->removeAllChildren();
	m_targetToppingLayer->removeAllChildren();
}

// 0x45BDB0
void DecoratePizzaGame::onMouseMove(const engine::Point& pos)
{
	engine::Point center(m_userToppingLayer->getScreenPosition());
	engine::Vector2 offset(pos - center);
	if (offset.length() < 140.0f)
		m_cursorImage->setAlpha(1.0f);
	else
		m_cursorImage->setAlpha(0.5f);
	m_cursorImage->setPosition(engine::Vector2(pos));
}

// 0x45BE50
void DecoratePizzaGame::setupAnimations()
{
	m_userPizzaTimer.start(0.5f, 1.0f);
	m_userPizzaTimer.restart();
	engine::Vector2 start(m_userPizzaPos);
	start.x -= 400.0f;
	m_userPizzaPath.addKey(makeKeyframe(0.0f, start, 0.0f, 1.0f, 0.0f, 1.0f));
	m_userPizzaPath.addKey(makeKeyframe(1.0f, m_userPizzaPos, 0.3f, 0.0f, 0.3f, 1.0f));
	m_userPizza->setPosition(start);
	m_buttonFade.addKey(makeKeyframe(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_buttonFade.addKey(makeKeyframe(1.0f, 1.0f, 0.3f, 0.0f, 0.3f, 1.0f));
	m_toppingButtonBar->setScale(0.0f);
	m_targetPizzaTimer.start(0.5f, 1.0f);
	m_targetPizzaTimer.restart();
	start = m_targetPizzaPos;
	start.x += 400.0f;
	m_targetPizzaPath.addKey(makeKeyframe(0.0f, start, 0.0f, 1.0f, 0.0f, 1.0f));
	m_targetPizzaPath.addKey(makeKeyframe(1.0f, m_targetPizzaPos, 0.3f, 0.0f, 0.3f, 1.0f));
	m_targetPizza->setPosition(start);
	m_timerGroupTimer.start(0.5f, 1.0f);
	m_timerGroupTimer.restart();
	m_timerGroupTimer.pause();
	start = engine::Vector2(410.0f, 90.0f);
	engine::Vector2 end(start);
	start.y -= 250.0f;
	m_timerGroupPath.addKey(makeKeyframe(0.0f, start, 0.0f, 1.0f, 0.0f, 1.0f));
	m_timerGroupPath.addKey(makeKeyframe(1.0f, end, 0.3f, 0.0f, 0.3f, 1.0f));
	m_timerGroup->setPosition(start);
}

// 0x45C240 (folded)
void DecoratePizzaEndState::exit()
{
	m_title->setFlags(16);
}

// 0x45C290
DecoratePizzaPlayState::DecoratePizzaPlayState(DecoratePizzaGame* game)
	: engine::State<DecoratePizzaGame>(game)
{
}

// 0x45C320
DecoratePizzaScoreState::DecoratePizzaScoreState(DecoratePizzaGame* game)
	: engine::State<DecoratePizzaGame>(game)
{
}

// 0x45C3C0
DecoratePizzaEndState::DecoratePizzaEndState(DecoratePizzaGame* game)
	: engine::State<DecoratePizzaGame>(game)
{
}

// 0x45C500
void DecoratePizzaGame::cheatWinLevel()
{
	m_action.setState(new DecoratePizzaEndState(this));
}

// 0x45C570
void DecoratePizzaGame::finishPizza()
{
	m_action.setState(new DecoratePizzaScoreState(this));
}

// 0x45C5E0
void DecoratePizzaPlayState::update(engine::UpdateContext& context)
{
	m_owner->m_timeLeft -= context.elapsed;
	m_owner->m_timerText->setTime((int)m_owner->m_timeLeft, true);
	if (m_owner->m_timeLeft < 5.0f && !m_hurryStarted)
	{
		if (m_hurrySound)
			m_hurrySound->play();
		m_hurryStarted = true;
	}
	if (m_owner->m_timeLeft <= 0.0f)
		m_owner->finishPizza();
}

// 0x45C6B0
bool DecoratePizzaGame::allToppingsPlaced() const
{
	int count = 0;
	for (std::map<Topping*, engine::RefPtr<OrderButton> >::const_iterator it = m_toppingButtons.begin();
		it != m_toppingButtons.end(); ++it)
	{
		count += it->second->m_count;
	}
	return count == 0;
}

// 0x45C700
void DecoratePizzaGame::startLevel(Level* level)
{
	m_timeLeft = (float)level->m_numOrders;
	m_pizzasLeft = level->getPizzas().size();
	m_timerText->setTime((int)m_timeLeft, true);
	m_pizzaCountText->setNumber(m_pizzasLeft, "");
	m_level = level;
	m_nextPizza = level->getPizzas().begin();
	m_pausedTopping = NULL;
	m_music = PizzaFrenzy::getTileManifest()->getMusicClip(level->getMusic());
	if (m_music)
	{
		m_musicVolume = m_music->getVolume();
		PizzaFrenzy::getMusicPlayer()->play(m_music);
	}
}

// 0x45C8C0
bool DecoratePizzaGame::scoreTopping(std::vector<engine::RefPtr<ToppingButton> >::iterator target)
{
	for (std::vector<engine::RefPtr<ToppingButton> >::iterator it = m_targetToppings.begin();
		it != m_targetToppings.end(); ++it)
	{
		(*it)->setSelected(false);
	}
	ToppingButton* targetButton = *target++;
	targetButton->setSelected(true);
	float bestDistance = 999.0f;
	std::vector<engine::RefPtr<ToppingButton> >::iterator best = m_userToppings.end();
	std::vector<engine::RefPtr<ToppingButton> >::iterator it = m_userToppings.begin();
	while (it != m_userToppings.end())
	{
		ToppingButton* user = *it;
		if (user->isSelected())
		{
			user->setSelected(false);
			it = m_userToppings.erase(it);
		}
		else
		{
			engine::Vector2 offset = targetButton->getPosition() - user->getPosition();
			if (offset.length() < bestDistance && targetButton->getTopping() == user->getTopping())
			{
				bestDistance = offset.length();
				best = it;
			}
			++it;
		}
	}
	// Quirk of the original: best starts as the end() taken before the erasures above, so when a selected icon was
	// erased and none matched it is stale (unequal to the new end(), and *best reads past the end). match is unused
	// then (the distance stays 999: MISS). Harmless without checked iterators; a Debug (/MTd) build asserts here.
	ToppingButton* match = NULL;
	if (best != m_userToppings.end())
		match = *best;
	if (bestDistance < 15.0f)
	{
		match->setSelected(true);
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("matchToppingSplat",
			match->getScreenPosition().x, match->getScreenPosition().y, NULL, NULL);
		splat->setText("PERFECT!");
		m_player->m_perfectToppings++;
		PizzaFrenzy::getSounds()->playSound("bonus_toppingMatch", 1.0f, 1.0f);
	}
	else if (bestDistance < 30.0f)
	{
		match->setSelected(true);
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("matchToppingSplat",
			match->getScreenPosition().x, match->getScreenPosition().y, NULL, NULL);
		splat->setText("GOOD!");
		m_player->m_goodToppings++;
		PizzaFrenzy::getSounds()->playSound("bonus_toppingMatch", 1.0f, 1.0f);
	}
	else if (bestDistance < 60.0f)
	{
		match->setSelected(true);
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("matchToppingSplat",
			match->getScreenPosition().x, match->getScreenPosition().y, NULL, NULL);
		splat->setText("OKAY!");
		m_player->m_okayToppings++;
		PizzaFrenzy::getSounds()->playSound("bonus_toppingMatch", 1.0f, 1.0f);
	}
	else
	{
		m_player->m_missedToppings++;
		engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("missToppingSplat",
			targetButton->getScreenPosition().x - 400.0f, targetButton->getScreenPosition().y, NULL, NULL);
		splat->setText("MISS!");
		PizzaFrenzy::getSounds()->playSound("bonus_toppingMiss", 1.0f, 1.0f);
	}
	return target != m_targetToppings.end();
}

// 0x45CF60
void DecoratePizzaGame::updateToppingButton(OrderButton* button)
{
	button->m_countText->setNumber(button->m_count, "");
	if (button->m_count == 0)
		button->disable();
	else
		button->enable();
}

// 0x45D030
void DecoratePizzaIntroState::enter()
{
	m_toppingIndex = 0;
	m_timer = 0.0f;
	m_phase = 0;
	m_owner->m_isPlaying = false;
	if (m_owner->m_nextPizza == m_owner->m_level->getPizzas().end())
	{
		m_owner->m_action.setState(new DecoratePizzaEndState(m_owner));
		return;
	}
	PizzaDesign* pizza = *m_owner->m_nextPizza++;
	m_owner->m_pizzasLeft--;
	m_owner->m_pizzaCountText->setNumber(m_owner->m_pizzasLeft, "");
	m_owner->setTargetPizza(pizza);
	m_toppingCount = m_owner->m_targetToppings.size();
	m_owner->m_targetPizzaTimer.setForward(true);
	m_owner->m_userPizzaTimer.setForward(true);
}

// 0x45D1B0
void DecoratePizzaEndState::enter()
{
	m_owner->m_isPlaying = false;
	engine::ParticleSystemDef* effect = PizzaFrenzy::getTileManifest()->getFx("BurstFx")->m_effect;
	m_title = new AnimatedText();
	std::string text;
	if (m_owner->m_player->m_secondsLeft == 0)
		text = engine::getApplication()->loadString(218);
	else if (m_owner->m_player->m_missedToppings > 0)
		text = engine::getApplication()->loadString(208);
	else if (m_owner->m_player->m_missedToppings == 0 && m_owner->m_player->m_okayToppings == 0
		&& m_owner->m_player->m_goodToppings == 0 && m_owner->m_player->m_perfectToppings > 0)
		text = engine::getApplication()->loadString(209);
	else
		text = engine::getApplication()->loadString(217);
	m_title->setText(text, 0.1f, 4.0f, engine::getApplication()->getFont("res\\fonts\\titleFont.xml"), effect, 0.0f,
		0.05f);
	m_owner->m_screen->addChild(m_title);
	m_title->show(engine::Vector2(400.0f, 200.0f));
	m_titleHidden = false;
	m_timer = 2.5f;
	m_owner->endLevel();
	m_owner->m_screen->deactivate();
	PizzaFrenzy::getMusicPlayer()->play(PizzaFrenzy::getTileManifest()->getMusicClip("music_levelComplete"));
}

// 0x45D4B0
void DecoratePizzaEndState::update(engine::UpdateContext& context)
{
	if (!m_titleHidden)
	{
		m_timer -= context.elapsed;
		if (m_timer < 0.0f)
		{
			m_title->hide();
			m_titleHidden = true;
			m_timer = 2.0f;
		}
	}
	else
	{
		m_timer -= context.elapsed;
		if (m_timer < 0.0f)
			getGame()->onScreenEvent("levelEnd");
	}
}

// 0x45D9E0
void DecoratePizzaIntroState::update(engine::UpdateContext& context)
{
	m_timer -= context.elapsed;
	if (m_timer < 0.0f)
	{
		switch (m_phase)
		{
		case 0:
			m_owner->m_targetPizzaTimer.resume();
			PizzaFrenzy::getSounds()->playSound("story_ScreenPulldown", 1.0f, 1.0f);
			m_phase = 1;
			m_timer = 0.0f;
			break;
		case 1:
			m_owner->m_targetPizzaTimer.update(context.elapsed);
			m_owner->m_targetPizzaPath.animate(m_owner->m_targetPizzaTimer.getValue(), m_owner->m_targetPizza);
			if (m_owner->m_targetPizzaTimer.isFinished())
			{
				m_phase = 2;
				m_timer = 0.5f;
			}
			break;
		case 2:
			if (m_toppingIndex < m_toppingCount)
			{
				m_owner->m_targetToppings.at(m_toppingIndex)->setVisible(true);
				PizzaFrenzy::getSounds()->playSound("popup", 1.0f, 1.0f);
				m_toppingIndex++;
				m_timer = 0.1f;
			}
			else
			{
				m_phase = 3;
				m_timer = 0.3f;
			}
			break;
		case 3:
			m_owner->m_userPizzaTimer.resume();
			PizzaFrenzy::getSounds()->playSound("story_ScreenPulldown", 1.0f, 1.0f);
			m_phase = 4;
			m_timer = 0.0f;
			break;
		case 4:
			m_owner->m_userPizzaTimer.update(context.elapsed);
			m_owner->m_userPizzaPath.animate(m_owner->m_userPizzaTimer.getValue(), m_owner->m_userPizza);
			m_owner->m_buttonFade.animate(m_owner->m_userPizzaTimer.getValue(), m_owner->m_toppingButtonBar);
			if (m_owner->m_userPizzaTimer.isFinished())
				m_phase = 5;
			break;
		case 5:
			m_owner->m_action.setState(new DecoratePizzaPlayState(m_owner));
			break;
		}
	}
}

// 0x45DD10
void DecoratePizzaPlayState::exit()
{
	if (m_hurrySound)
		m_hurrySound->stop();
	PizzaFrenzy::getSounds()->playSound("popup_flipover", 1.0f, 1.0f);
	engine::getApplication()->m_rightMouseDownSignal.disconnect(m_owner);
}

// 0x45E030
DecoratePizzaIntroState::DecoratePizzaIntroState(DecoratePizzaGame* game)
	: engine::State<DecoratePizzaGame>(game)
{
}

// 0x45E140
void DecoratePizzaGame::init(GameScreen* screen, GameProgress* stats)
{
	m_screen = screen;
	m_titles = screen->getComponent("titles");
	m_userPizza = screen->getComponent("userPizza");
	m_userPizzaPos = engine::Vector2(205.0f, 280.0f);
	m_userToppingLayer = static_cast<engine::Container*>(screen->getComponent("userToppings"));
	m_targetPizza = screen->getComponent("targetPizza");
	m_targetPizzaPos = engine::Vector2(605.0f, 280.0f);
	m_targetToppingLayer = static_cast<engine::Container*>(screen->getComponent("targetToppings"));
	m_timerText = static_cast<engine::TextItem*>(screen->getComponent("timer"));
	m_pizzaCountText = static_cast<engine::TextItem*>(screen->getComponent("pizzaCount"));
	m_timerGroup = screen->getComponent("timerGroup");
	m_caption = static_cast<engine::TextItem*>(screen->getComponent("caption"));
	m_toppingButtonBar = static_cast<engine::Container*>(screen->getComponent("toppingButtons"));
	m_lorenzo = static_cast<engine::Container*>(screen->getComponent("Lorenzo"));
	engine::TextItem* lorenzoText = static_cast<engine::TextItem*>(screen->getComponent("LorenzoText"));
	m_lorenzoText = new engine::TextTyper();
	m_lorenzoText->setPosition(lorenzoText->getPosition());
	m_lorenzoText->setFont(lorenzoText->getFont());
	m_lorenzoText->setup(g_typerWidth, g_typerSpeed, g_typerDelay);
	m_lorenzoText->setColor(lorenzoText->getColor());
	m_lorenzoText->setColorMode(lorenzoText->getColorMode());
	m_lorenzoText->setIndent(0.0f, 0.0f);
	lorenzoText->setText("");
	m_lorenzo->addChild(m_lorenzoText);
	m_lorenzo->setAlpha(0.0f);
	m_lorenzo->setVisible(false);
	m_player = stats;
	m_player->resetLevelStats();
	m_player->m_pizzasOrdered = 5;
	m_music = NULL;
	m_cursorImage = new engine::Image();
	setupAnimations();
	m_action.setState(new LevelIntroAction(this));
}

// 0x45EA90
void DecoratePizzaGame::startPlaying()
{
	m_action.setState(new DecoratePizzaIntroState(this));
	m_timerGroupTimer.resume();
}

// 0x45EB00
void DecoratePizzaScoreState::update(engine::UpdateContext& context)
{
	m_timer -= context.elapsed;
	if (m_timer <= 0.0f)
	{
		switch (m_phase)
		{
		case 0:
			m_current = m_owner->m_targetToppings.begin();
			m_phase = 1;
			break;
		case 1:
			if (m_owner->scoreTopping(m_current))
				++m_current;
			else
				m_phase = 2;
			m_timer = 0.1f;
			break;
		case 2:
			m_owner->m_targetPizzaTimer.setForward(false);
			m_owner->m_userPizzaTimer.setForward(false);
			m_phase = 3;
			// fall through: the pizzas start sliding out in the same frame
		case 3:
			m_owner->m_targetPizzaTimer.update(context.elapsed);
			m_owner->m_targetPizzaPath.animate(m_owner->m_targetPizzaTimer.getValue(), m_owner->m_targetPizza);
			m_owner->m_userPizzaTimer.update(context.elapsed);
			m_owner->m_userPizzaPath.animate(m_owner->m_userPizzaTimer.getValue(), m_owner->m_userPizza);
			m_owner->m_buttonFade.animate(m_owner->m_userPizzaTimer.getValue(), m_owner->m_toppingButtonBar);
			if (m_owner->m_targetPizzaTimer.isFinished() && m_owner->m_userPizzaTimer.isFinished())
			{
				if (m_owner->m_timeLeft <= 0.0f)
					m_owner->m_action.setState(new DecoratePizzaEndState(m_owner));
				else
					m_owner->m_action.setState(new DecoratePizzaIntroState(m_owner));
			}
			break;
		}
	}
}

// 0x45F010
DecoratePizzaGame::~DecoratePizzaGame()
{
}

// 0x45F3A0
void DecoratePizzaGame::createTargetToppings(PizzaDesign* pizza)
{
	m_targetToppings.clear();
	m_targetToppingLayer->removeAllChildren();
	for (std::vector<ToppingPlacement*>::iterator it = pizza->m_toppings.begin(); it != pizza->m_toppings.end(); ++it)
	{
		Topping* topping = (*it)->topping;
		ToppingButton* button = new ToppingButton();
		button->setTopping(topping);
		button->setEnabled(false);
		m_targetToppings.push_back(button);
		m_targetToppingLayer->addChild(button);
		button->setPosition(engine::Vector2((*it)->position));
		button->setVisible(false);
	}
}

// 0x45F550
void DecoratePizzaGame::placeTopping(const engine::Vector2& localPos, Topping* topping)
{
	ToppingButton* button = new ToppingButton();
	button->setTopping(topping);
	m_userToppings.push_back(button);
	m_userToppingLayer->addChild(button);
	button->setPosition(localPos);
	button->setEnabled(false);
	m_toppingButtons[topping]->m_count--;
	updateToppingButton(m_toppingButtons[topping]);
	if (allToppingsPlaced())
	{
		m_player->m_pizzasDelivered++;
		finishPizza();
	}
}

// 0x45F830
DecoratePizzaGame::DecoratePizzaGame()
{
}

// 0x45FAA0
void DecoratePizzaGame::onMouseDown(const engine::Point& pos)
{
	engine::Point center(m_userToppingLayer->getScreenPosition());
	engine::Vector2 localPos(pos - center);
	if (localPos.length() < 140.0f && m_selectedTopping && m_toppingButtons[m_selectedTopping]->m_count > 0
		&& m_isPlaying == true)
	{
		placeTopping(localPos, m_selectedTopping);
		if (m_selectedTopping && m_toppingButtons[m_selectedTopping]->m_count == 0)
			setSelectedTopping(NULL);
		PizzaFrenzy::getSounds()->playSound("order_click1", 1.0f, 1.0f);
	}
	else if (localPos.length() < 200.0f && m_selectedTopping && m_isPlaying == true)
	{
		PizzaFrenzy::getSounds()->playSound("order_invalidClick", 1.0f, 1.0f);
	}
}

// 0x45FD50
void DecoratePizzaGame::setSelectedTopping(Topping* topping)
{
	if (m_selectedTopping)
	{
		engine::getApplication()->m_mouseMoveSignal.disconnect(this);
		engine::getApplication()->m_mouseDownSignal.disconnect(this);
		m_screen->removeChild(m_cursorImage);
	}
	m_selectedTopping = topping;
	if (topping)
	{
		m_cursorImage->setImage(topping->m_image);
		engine::getApplication()->m_mouseMoveSignal.connect(this, &DecoratePizzaGame::onMouseMove);
		engine::getApplication()->m_mouseDownSignal.connect(this, &DecoratePizzaGame::onMouseDown);
		m_screen->addChild(m_cursorImage);
	}
	else if (m_cursorImage)
	{
		m_cursorImage->setImage(NULL);
	}
}

// 0x45FE70
void DecoratePizzaGame::removeToppingAt(const engine::Vector2& localPos)
{
	for (std::vector<engine::RefPtr<ToppingButton> >::iterator it = m_userToppings.begin(); it != m_userToppings.end();
		++it)
	{
		if (((*it)->getPosition() - localPos).length() < 15.0f)
		{
			ToppingButton* button = *it;
			m_toppingButtons[button->getTopping()]->m_count++;
			setSelectedTopping(button->getTopping());
			updateToppingButton(m_toppingButtons[button->getTopping()]);
			m_userToppingLayer->removeChild(button);
			m_userToppings.erase(it);
			return;
		}
	}
}

// 0x45FFA0
void DecoratePizzaGame::setPaused(bool paused)
{
	if (paused)
	{
		m_pausedTopping = m_selectedTopping;
		setSelectedTopping(NULL);
	}
	else
	{
		setSelectedTopping(m_pausedTopping);
	}
}

// 0x460010
void DecoratePizzaPlayState::onAction(const std::string& action)
{
	Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(action);
	if (topping)
		m_owner->setSelectedTopping(topping);
}

// 0x460040
void DecoratePizzaScoreState::enter()
{
	m_owner->m_player->m_secondsLeft = (int)m_owner->m_timeLeft;
	m_phase = 0;
	m_timer = 0.5f;
	m_owner->m_isPlaying = false;
	m_owner->setSelectedTopping(NULL);
}

// 0x460080
void DecoratePizzaGame::endLevel()
{
	PizzaFrenzy::getMusicPlayer()->stop();
	m_music = NULL;
	setSelectedTopping(NULL);
	m_cursorImage = NULL;
}

// 0x4600E0
void DecoratePizzaGame::createToppingButtons(PizzaDesign* pizza)
{
	m_toppingButtons.clear();
	m_toppingButtonBar->removeAllChildren();
	m_userToppings.clear();
	m_userToppingLayer->removeAllChildren();
	std::map<std::string, int> counts;
	for (std::vector<ToppingPlacement*>::iterator it = pizza->m_toppings.begin(); it != pizza->m_toppings.end(); ++it)
		counts[(*it)->topping->m_name]++;
	float x = 0.0f;
	for (std::map<std::string, int>::iterator count = counts.begin(); count != counts.end(); ++count)
	{
		Topping* topping = PizzaFrenzy::getTileManifest()->getTopping(count->first);
		OrderButton* button = new OrderButton(topping, count->second, m_screen);
		button->setScale(1.0f);
		button->setPosition(x, 0.0f);
		m_toppingButtons[topping] = button;
		m_toppingButtonBar->addChild(button);
		x += 80.0f;
	}
}

// 0x460340
void DecoratePizzaGame::onRightMouseDown(const engine::Point& pos)
{
	engine::Point center(m_userToppingLayer->getScreenPosition());
	engine::Vector2 localPos(pos - center);
	removeToppingAt(localPos);
}

// 0x460390
void DecoratePizzaPlayState::enter()
{
	m_owner->m_screen->activate();
	m_owner->m_isPlaying = true;
	PizzaFrenzy::getSounds()->playSound("popup_flipover", 1.0f, 1.0f);
	m_hurrySound = PizzaFrenzy::getSounds()->getSound("popup_wait");
	if (m_hurrySound)
		m_hurrySound->setLooping(true);
	m_hurryStarted = false;
	engine::getApplication()->m_rightMouseDownSignal.connect(m_owner, &DecoratePizzaGame::onRightMouseDown);
}

// 0x4604E0
void DecoratePizzaGame::setTargetPizza(PizzaDesign* pizza)
{
	createTargetToppings(pizza);
	createToppingButtons(pizza);
	std::string caption;
	engine::format(caption, "\"%s\"", pizza->getName().c_str());
	m_caption->setText(caption);
	setSelectedTopping(NULL);
}
