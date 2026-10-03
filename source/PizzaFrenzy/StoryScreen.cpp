// StoryScreen: the story screen (story scripts, DAILY REPORT and BONUS REPORT, game-won totals, topping upgrade).
#include <map>
#include <string>
#include <vector>

#include <windows.h>

#include "engine/Application.h"
#include "engine/Image.h"
#include "engine/ImageButton.h"
#include "engine/KeyframeCurve.h"
#include "engine/Point.h"
#include "engine/Properties.h"
#include "engine/Range.h"
#include "engine/SoundHandle.h"
#include "engine/SoundMgr.h"
#include "engine/Splat.h"
#include "engine/SplatFactory.h"
#include "engine/StringUtil.h"
#include "engine/Surface.h"
#include "engine/TextItem.h"
#include "engine/TextTyper.h"
#include "engine/User.h"
#include "engine/UserManager.h"
#include "engine/Xml.h"
#include "Constants.h"
#include "GameLogic.h"
#include "GameProgress.h"
#include "PizzaFrenzy.h"
#include "StorySequence.h"
#include "StoryScreen.h"
#include "TileManager.h"

namespace
{
	// The slide keys are built by an inlined constructor-like helper of the original (no address): the value comes
	// by value (copy-constructed from the local), then the key's members are assigned one by one (the value's
	// default constructor runs after the argument is built). engine::Keyframe itself is a plain struct.
	engine::Keyframe<engine::Vector2> makeKeyframe(float time, engine::Vector2 value, float inTime, float inWeight,
		float outTime, float outWeight)
	{
		engine::Keyframe<engine::Vector2> key;
		key.time = time;
		key.value = value;
		key.inTime = inTime;
		key.inWeight = inWeight;
		key.outTime = outTime;
		key.outWeight = outWeight;
		return key;
	}
}

// 0x443BB0 (folded)
void StoryScreen::activate()
{
	engine::Screen::activate();
}

// 0x43C470
void StoryScreen::deactivate()
{
	engine::Screen::deactivate();
}

// 0x43C480
void StoryScreen::setLevel(Level* level)
{
	m_level = level;
}

// 0x43C490
void StoryScreen::show()
{
	engine::Screen::show();
	m_step = 1;
}

// 0x43C4B0
void StoryScreen::hide()
{
	engine::Screen::hide();
	m_step = 3;
}

// 0x43C550: the pulldown screen drops in from 400 above with a 20 overshoot, the blackboard slides in from 670 to
// the right with a 20 overshoot
void StoryScreen::initSlideAnimations()
{
	m_screenTween.start(0.4f, 1.0f);
	m_blackboardTween.start(0.4f, 1.0f);
	engine::Vector2 from;
	engine::Vector2 to;
	engine::Vector2 overshoot;

	to = m_pulldownScreenGroup->getPosition();
	from.x = to.x;
	from.y = to.y - 400.0f;
	overshoot.x = to.x;
	overshoot.y = to.y + 20.0f;
	m_screenSlide.clear();
	m_screenSlide.addKey(makeKeyframe(0.0f, from, 0.0f, 1.0f, 0.0f, 0.0f));
	m_screenSlide.addKey(makeKeyframe(0.75f, overshoot, 0.0f, 1.0f, 0.0f, 0.0f));
	m_screenSlide.addKey(makeKeyframe(1.0f, to, 0.0f, 1.0f, 0.0f, 0.0f));

	to = m_blackboardGroup->getPosition();
	from.x = to.x + 670.0f;
	from.y = to.y;
	overshoot.x = to.x - 20.0f;
	overshoot.y = to.y;
	m_blackboardSlide.clear();
	m_blackboardSlide.addKey(makeKeyframe(0.0f, from, 0.0f, 1.0f, 0.0f, 0.0f));
	m_blackboardSlide.addKey(makeKeyframe(0.75f, overshoot, 0.0f, 1.0f, 0.0f, 0.0f));
	m_blackboardSlide.addKey(makeKeyframe(1.0f, to, 0.0f, 1.0f, 0.0f, 0.0f));
}

// 0x43C870
void StoryScreen::updateSlideAnimations()
{
	m_screenSlide.animate(m_screenTween.getValue(), m_pulldownScreenGroup);
	m_blackboardSlide.animate(m_blackboardTween.getValue(), m_blackboardGroup);
}

// 0x43C8D0
void StoryScreen::playStory()
{
	m_script->start();
	m_step = 2;
}

// 0x43C8F0
void StoryScreen::showGameWonTexts()
{
	m_totalSales->setVisible(true);
	m_elapsedTime->setVisible(true);
	m_finalFoodBank->setVisible(true);
}

// 0x43C920
void StoryScreen::setQuitButtonVisible(bool visible)
{
	m_quitButtonGroup->setVisible(visible);
	m_quitButton->setEnabled(visible);
}

// 0x43C9E0
void StoryScreen::prepareShow()
{
	resetStory();
	m_levelUpgradeGroup->setVisible(false);
	engine::Screen::prepareShow();
}

// 0x43CA00: the picture of a story <image img=...> element (this is not used)
engine::Bitmap* StoryScreen::getStoryImage(const std::string& name)
{
	if (name == "currentCity")
		return getGame()->getCitySelectEntry(getGame()->m_cityIndex);
	if (name.empty())
		return 0;
	engine::Bitmap* image = engine::getApplication()->getImage(name.c_str());
	image->setPivotType(1);
	return image;
}

// 0x43CA70
void StoryScreen::stopStory()
{
	m_pulldownScreen->removeAllChildren();
	m_blackboard->removeAllChildren();
	if (m_rollupSound && m_rollupSound->isPlaying())
		m_rollupSound->stop();
	if (m_script)
		m_script->clear();
	resetStory();
	m_levelUpgradeGroup->setVisible(false);
}

// 0x43CAE0: hides every bubble and typer, then shows the speaker's; 0 for an unknown speaker
engine::TextTyper* StoryScreen::showSpeechBubble(const std::string& speaker)
{
	m_lorenzoBubble->setVisible(false);
	m_paulaBubble->setVisible(false);
	m_lisaBubble->setVisible(false);
	m_niccoloBubble->setVisible(false);
	m_lorenzoText->setVisible(false);
	m_paulaText->setVisible(false);
	m_lisaText->setVisible(false);
	m_niccoloText->setVisible(false);
	if (speaker == "Lorenzo")
	{
		m_lorenzoBubble->setVisible(true);
		m_lorenzoText->setVisible(true);
		return m_lorenzoText;
	}
	else if (speaker == "Lisa")
	{
		m_lisaBubble->setVisible(true);
		m_lisaText->setVisible(true);
		return m_lisaText;
	}
	else if (speaker == "Paula")
	{
		m_paulaBubble->setVisible(true);
		m_paulaText->setVisible(true);
		return m_paulaText;
	}
	else if (speaker == "Niccolo")
	{
		m_niccoloBubble->setVisible(true);
		m_niccoloText->setVisible(true);
		return m_niccoloText;
	}
	return 0;
}

// 0x43CD10
void StoryScreen::setStory(const std::string& file)
{
	m_storyFile = file;
	loadStory();
}

// 0x43CD40: binds the layout's components; each speaker's text item is replaced by a typer with its position, font
// and colour, and emptied
void StoryScreen::initLayout()
{
	m_pulldownScreenGroup = static_cast<engine::Container*>(getComponent("pulldownScreenGroup"));
	m_blackboardGroup = static_cast<engine::Container*>(getComponent("blackboardGroup"));
	m_pulldownScreen = static_cast<engine::Container*>(getComponent("pulldownScreen"));
	m_blackboard = static_cast<engine::Container*>(getComponent("blackboard"));
	m_quitButtonGroup = static_cast<engine::Container*>(getComponent("quitButtonGroup"));
	m_quitButton = static_cast<engine::ImageButton*>(getComponent("quitButton"));
	m_continueButtonGroup = static_cast<engine::Container*>(getComponent("continueButtonGroup"));
	m_continueButton = static_cast<engine::ImageButton*>(getComponent("continueButton"));

	m_paula = static_cast<engine::Container*>(getComponent("Paula"));
	m_paulaBubble = static_cast<engine::Image*>(getComponent("PaulaBubble"));
	engine::TextItem* paulaText = static_cast<engine::TextItem*>(getComponent("paulaText"));
	m_paulaText = new engine::TextTyper();
	m_paulaText->setPosition(paulaText->getPosition());
	m_paulaText->setFont(paulaText->getFont());
	m_paulaText->setup(g_typerWidth, g_typerSpeed, g_typerDelay);
	m_paulaText->setColor(paulaText->getColor());
	m_paulaText->setColorMode(paulaText->getColorMode());
	m_paulaText->setIndent(0.0f, 0.0f);
	paulaText->setText("");
	m_paula->addChild(m_paulaText);

	m_lisa = static_cast<engine::Container*>(getComponent("Lisa"));
	m_lisaBubble = static_cast<engine::Image*>(getComponent("LisaBubble"));
	engine::TextItem* lisaText = static_cast<engine::TextItem*>(getComponent("lisaText"));
	m_lisaText = new engine::TextTyper();
	m_lisaText->setPosition(lisaText->getPosition());
	m_lisaText->setFont(lisaText->getFont());
	m_lisaText->setup(g_typerWidth, g_typerSpeed, g_typerDelay);
	m_lisaText->setColor(lisaText->getColor());
	m_lisaText->setColorMode(lisaText->getColorMode());
	m_lisaText->setIndent(0.0f, 0.0f);
	lisaText->setText("");
	m_lisa->addChild(m_lisaText);

	m_lorenzo = static_cast<engine::Container*>(getComponent("Lorenzo"));
	m_lorenzoBubble = static_cast<engine::Image*>(getComponent("LorenzoBubble"));
	engine::TextItem* lorenzoText = static_cast<engine::TextItem*>(getComponent("lorenzoText"));
	m_lorenzoText = new engine::TextTyper();
	m_lorenzoText->setPosition(lorenzoText->getPosition());
	m_lorenzoText->setFont(lorenzoText->getFont());
	m_lorenzoText->setup(395.0f, g_typerSpeed, g_typerDelay);
	m_lorenzoText->setColor(lorenzoText->getColor());
	m_lorenzoText->setColorMode(lorenzoText->getColorMode());
	m_lorenzoText->setIndent(0.0f, 0.0f);
	lorenzoText->setText("");
	m_lorenzo->addChild(m_lorenzoText);

	m_niccolo = static_cast<engine::Container*>(getComponent("Niccolo"));
	m_niccoloBubble = static_cast<engine::Image*>(getComponent("NiccoloBubble"));
	engine::TextItem* niccoloText = static_cast<engine::TextItem*>(getComponent("niccoloText"));
	m_niccoloText = new engine::TextTyper();
	m_niccoloText->setPosition(niccoloText->getPosition());
	m_niccoloText->setFont(niccoloText->getFont());
	m_niccoloText->setup(g_typerWidth, g_typerSpeed, g_typerDelay);
	m_niccoloText->setColor(niccoloText->getColor());
	m_niccoloText->setColorMode(niccoloText->getColorMode());
	m_niccoloText->setIndent(0.0f, 0.0f);
	niccoloText->setText("");
	m_niccolo->addChild(m_niccoloText);

	m_skipString = static_cast<engine::TextItem*>(getComponent("skipString"));
	initSlideAnimations();
	m_step = 0;

	m_levelUpgradeGroup = static_cast<engine::FadeContainer*>(getComponent("levelUpgradeGroup"));
	m_toppingImage = static_cast<engine::Image*>(getComponent("toppingImage"));
	m_toppingName = static_cast<engine::TextItem*>(getComponent("toppingName"));
	m_levelText = static_cast<engine::TextItem*>(getComponent("levelText"));
	m_toppingClass = static_cast<engine::TextItem*>(getComponent("toppingClass"));
	m_nextLevelText = static_cast<engine::TextItem*>(getComponent("nextLevelText"));
	m_levelFormat = m_levelText->getText();
	m_nextLevelFormat = m_nextLevelText->getText();
	engine::TextItem* description = static_cast<engine::TextItem*>(getComponent("toppingDescription"));
	m_toppingDescription = new engine::TextTyper();
	m_toppingDescription->setPosition(description->getPosition());
	m_toppingDescription->setFont(description->getFont());
	m_toppingDescription->setup(290.0f, g_typerSpeed, g_typerDelay);
	m_toppingDescription->setColor(description->getColor());
	m_toppingDescription->setColorMode(description->getColorMode());
	m_toppingDescription->setIndent(0.0f, 0.0f);
	description->setText("");
	m_levelUpgradeGroup->addChild(m_toppingDescription);

	m_rollupSound = PizzaFrenzy::getSounds()->getSound("menu_rollup");
	if (m_rollupSound)
		m_rollupSound->setLooping(true);
	setFlags(2);
}

// 0x43E090: called once after initLayout by the game
void StoryScreen::initReport(GameProgress* progress)
{
	m_progress = progress;
	m_stats = static_cast<engine::Container*>(getComponent("stats"));
	m_actualRevenue = static_cast<engine::TextItem*>(getComponent("actualRevenue"));
	m_salesTarget = static_cast<engine::TextItem*>(getComponent("salesTarget"));
	m_bonusSales = static_cast<engine::TextItem*>(getComponent("bonusSales"));
	m_totalScore = static_cast<engine::TextItem*>(getComponent("totalScore"));
	m_ordersDelivered = static_cast<engine::TextItem*>(getComponent("ordersDelivered"));
	m_foodBank = static_cast<engine::TextItem*>(getComponent("foodBank"));
	m_serviceRating = static_cast<engine::Container*>(getComponent("serviceRating"));
	m_comboGroup = static_cast<engine::Container*>(getComponent("comboGroup"));
	m_longestCombo = static_cast<engine::TextItem*>(getComponent("longestCombo"));
	m_bonusStats = static_cast<engine::Container*>(getComponent("bonusStats"));
	m_perfectToppings = static_cast<engine::TextItem*>(getComponent("perfectToppings"));
	m_toppingLineFormat = m_perfectToppings->getText();
	m_goodToppings = static_cast<engine::TextItem*>(getComponent("goodToppings"));
	m_okayToppings = static_cast<engine::TextItem*>(getComponent("okayToppings"));
	m_missedToppings = static_cast<engine::TextItem*>(getComponent("missedToppings"));
	m_toppingBonus = static_cast<engine::TextItem*>(getComponent("toppingBonus"));
	m_timeLeft = static_cast<engine::TextItem*>(getComponent("timeLeft"));
	m_timeLeftBonus = static_cast<engine::TextItem*>(getComponent("timeLeftBonus"));
	m_bonusTotal = static_cast<engine::TextItem*>(getComponent("bonusTotal"));
	m_bonusTotalScore = static_cast<engine::TextItem*>(getComponent("bonusTotalScore"));
	m_gameWon = static_cast<engine::Container*>(getComponent("gameWon"));
	m_totalSales = static_cast<engine::TextItem*>(getComponent("totalSales"));
	m_elapsedTime = static_cast<engine::TextItem*>(getComponent("elapsedTime"));
	m_finalFoodBank = static_cast<engine::TextItem*>(getComponent("finalFoodBank"));
}

// 0x43ED20
void StoryScreen::resetStory()
{
	m_screenTween.restart();
	m_screenTween.pause();
	m_blackboardTween.restart();
	m_blackboardTween.pause();
	m_paulaText->reset();
	m_paulaText->pause();
	m_lisaText->reset();
	m_lisaText->pause();
	m_lorenzoText->reset();
	m_lorenzoText->pause();
	m_niccoloText->reset();
	m_niccoloText->pause();
	m_paula->setVisible(false);
	m_lisa->setVisible(false);
	m_lorenzo->setVisible(false);
	m_niccolo->setVisible(false);
	m_pulldownScreen->removeAllChildren();
	m_blackboard->removeAllChildren();
	m_skipString->setVisible(true);
	m_quitButtonGroup->setVisible(false);
	m_quitButton->setEnabled(false);
	m_cancelAction = "storyDone";
	updateSlideAnimations();
	m_step = 0;
}

// 0x43EE60: parses m_storyFile into a new script; a parse error only shows a message box
void StoryScreen::loadStory()
{
	if (m_script)
	{
		m_script->stop();
		m_script = 0;
	}
	engine::RefPtr<engine::XmlHandlerStack> handler = new engine::XmlHandlerStack();
	engine::RefPtr<StoryHandler> story = new StoryHandler("story", handler);
	try
	{
		handler->pushHandler(story);
		engine::getApplication()->loadXml(m_storyFile, handler);
	}
	catch (...)
	{
		MessageBoxA(NULL, "Failed to load story", "Error", MB_OK);
		handler = 0;
		story = 0;
		return;
	}
	m_script = story->m_sequence;
	handler = 0;
	story = 0;
	m_script->setScreen(this);
}

// 0x43F0F0
void StoryScreen::showFunFact()
{
	std::string fact = PizzaFrenzy::getTileManifest()->getRandomFunFact();
	engine::TextTyper* typer = showSpeechBubble("Lorenzo");
	typer->setText(fact);
	typer->reset();
	typer->start();
	PizzaFrenzy::getSounds()->playSound("tip_couponConvert", 1.0f, 1.0f);
}

// 0x43F240
void StoryScreen::showGameWon()
{
	m_bonusStats->setVisible(false);
	m_stats->setVisible(false);
	m_gameWon->setVisible(true);
	m_totalSales->setVisible(false);
	m_elapsedTime->setVisible(false);
	m_finalFoodBank->setVisible(false);
	m_totalSales->setNumber(m_progress->m_score, "$");
	m_elapsedTime->setTime((int)m_progress->m_playTime, true);
	engine::User* user = engine::UserManager::getInstance()->getCurrentUser();
	if (user)
		m_finalFoodBank->setNumber(user->getAttributes().getInt("foodBank", 0), "");
}

// 0x43F3F0: fills in the BONUS REPORT lines (hidden, revealed by updateStory) and adds the bonus to the score
void StoryScreen::initBonusReport()
{
	m_gameWon->setVisible(false);
	m_stats->setVisible(false);
	m_bonusStats->setVisible(true);
	m_perfectToppings->setVisible(false);
	m_goodToppings->setVisible(false);
	m_okayToppings->setVisible(false);
	m_missedToppings->setVisible(false);
	m_toppingBonus->setVisible(false);
	m_timeLeft->setVisible(false);
	m_timeLeftBonus->setVisible(false);
	m_bonusTotal->setVisible(false);
	m_bonusTotalScore->setVisible(true);
	std::string text;
	engine::format(text, m_toppingLineFormat.c_str(), m_progress->m_perfectToppings, 100, 100 * m_progress->m_perfectToppings);
	m_perfectToppings->setText(text);
	engine::format(text, m_toppingLineFormat.c_str(), m_progress->m_goodToppings, 20, 20 * m_progress->m_goodToppings);
	m_goodToppings->setText(text);
	engine::format(text, m_toppingLineFormat.c_str(), m_progress->m_okayToppings, 5, 5 * m_progress->m_okayToppings);
	m_okayToppings->setText(text);
	engine::format(text, m_toppingLineFormat.c_str(), -m_progress->m_missedToppings, 50, 50 * m_progress->m_missedToppings);
	m_missedToppings->setText(text);
	m_bonus = 100 * m_progress->m_perfectToppings + 20 * m_progress->m_goodToppings + 5 * m_progress->m_okayToppings
		- 50 * m_progress->m_missedToppings;
	m_toppingBonus->setNumber(m_bonus, "$");
	m_timeBonus = 100 * m_progress->m_secondsLeft;
	m_timeLeft->setTime(m_progress->m_secondsLeft, true);
	m_timeLeftBonus->setNumber(m_timeBonus, "$");
	m_bonus += m_timeBonus;
	m_bonus = m_bonus < 0 ? 0 : m_bonus;
	m_shownTotal = m_progress->m_score;
	m_bonusTotal->setNumber(0, "$");
	m_bonusTotalScore->setNumber(m_progress->m_score, "$");
	if (m_bonus > 0)
		m_progress->m_score += m_bonus;
	m_bonusToCount = m_bonus;
}

// 0x43F7F0: skips the bonus count-up ("continue" during steps 13-21)
void StoryScreen::finishBonusReport()
{
	m_perfectToppings->setVisible(true);
	m_goodToppings->setVisible(true);
	m_okayToppings->setVisible(true);
	m_missedToppings->setVisible(true);
	m_toppingBonus->setVisible(true);
	m_timeLeft->setVisible(true);
	m_timeLeftBonus->setVisible(true);
	m_bonusTotal->setVisible(true);
	m_bonusTotalScore->setNumber(m_progress->m_score, "$");
	if (m_bonus > 0)
		m_bonusTotal->setNumber(m_bonus, "$");
	if (m_rollupSound && m_rollupSound->isPlaying())
		m_rollupSound->stop();
	m_step = 2;
}

// 0x43F980: the TOPPING UPGRADE popup, with a script of its own (Lisa explains the first upgrade of the game)
void StoryScreen::showToppingUpgrade(Topping* topping, ToppingUpgrade* upgrade, int level)
{
	stopStory();
	m_script = new StorySequence();
	m_script->setScreen(this);
	m_toppingDescription->setText(upgrade->m_description);
	m_toppingName->setText(upgrade->m_name);
	m_toppingImage->setImage(topping->m_image);
	std::string text;
	engine::format(text, m_levelFormat.c_str(), level + 1);
	m_levelText->setText(text);
	m_toppingClass->setText(topping->m_display);
	if ((int)topping->m_upgrades.size() > level + 1)
	{
		int combo = topping->m_upgrades[level + 1]->m_combo;
		std::string next;
		std::string frenzy = getGame()->getGameLogic()->getFrenzyText(combo);
		engine::format(next, m_nextLevelFormat.c_str(), frenzy.c_str());
		m_nextLevelText->setText(next);
		m_nextLevelText->setVisible(true);
	}
	else
	{
		m_nextLevelText->setVisible(false);
	}
	m_levelUpgradeGroup->fade(true, 0.3f, 1.0f, false);
	m_toppingDescription->start();
	PizzaFrenzy::getSounds()->playSound("bonus_upgrade", 1.0f, 1.0f);

	bool firstUpgrade = true;
	for (std::map<Topping*, int>::iterator it = PizzaFrenzy::getGameStats()->m_toppingLevels.begin();
		it != PizzaFrenzy::getGameStats()->m_toppingLevels.end(); ++it)
	{
		if (it->second > 0)
		{
			firstUpgrade = false;
			break;
		}
	}
	if (firstUpgrade)
	{
		m_script->addAction(new PauseAction(1.0f));
		m_script->addAction(new EventAction("showSpeaker", "Lisa"));
		std::string message;
		if (getGame()->m_mode == 2)
			message = engine::getApplication()->formatString(235, topping->m_display.c_str());
		else
			message = engine::getApplication()->formatString(234, topping->m_display.c_str());
		m_script->addAction(new DialogAction(message, "Lisa"));
	}
	m_script->addAction(new WaitEventAction("", "fastForwardStep", "showClickToContinue", 1.0f));
	if (firstUpgrade)
		m_script->addAction(new EventAction("hideSpeaker", "Lisa"));
}

// 0x440240: Lorenzo announces the chef title reached, as a script of its own
void StoryScreen::showChefTitle()
{
	m_script = new StorySequence();
	m_script->setScreen(this);
	m_script->addAction(new EventAction("showSpeaker", "Lorenzo"));
	int level = m_progress->getTotalUpgradeLevel();
	std::string text = engine::getApplication()->formatString(231,
		PizzaFrenzy::getTileManifest()->getTitle(level)->m_text.c_str());
	m_script->addAction(new DialogAction(text, "Lorenzo"));
	m_script->addAction(new WaitEventAction("", "fastForwardStep", "showClickToContinue", 1.0f));
}

// 0x4406C0
void StoryScreen::updateOrderTexts()
{
	std::string text;
	engine::format(text, "%d/%d", m_deliveredCount - m_deliveredToCount, m_progress->m_levelOrders);
	m_ordersDelivered->setText(text);
	m_foodBank->setNumber(m_shownFoodBank, "");
}

// 0x4408A0
StoryScreen::~StoryScreen()
{
}

// 0x441170: the story <event action param> elements (through StorySequence::doAction)
void StoryScreen::handleStoryEvent(const std::string& action, const std::string& param)
{
	if (action == "showSpeaker")
	{
		engine::FadeContainer* speaker = static_cast<engine::FadeContainer*>(getComponent(param));
		if (speaker)
			speaker->fade(true, 0.5f, 1.0f, false);
		std::string name;
		engine::format(name, "%sBubble", param.c_str());
		engine::Component* bubble = getComponent(name);
		if (bubble)
			bubble->setVisible(false);
		engine::format(name, "%sText", param.c_str());
		engine::Component* text = getComponent(name);
		if (text)
			text->setVisible(false);
	}
	else if (action == "hideSpeaker")
	{
		engine::FadeContainer* speaker = static_cast<engine::FadeContainer*>(getComponent(param));
		if (speaker)
			speaker->fade(false, 0.5f, 1.0f, false);
	}
	else if (action == "showScreen")
	{
		m_screenTween.setForward(true);
		m_screenTween.resume();
	}
	else if (action == "hideScreen")
	{
		m_pulldownScreen->removeAllChildren();
		m_screenTween.setForward(false);
	}
	else if (action == "showBlackboard")
	{
		m_blackboardTween.setForward(true);
		m_blackboardTween.resume();
	}
	else if (action == "hideBlackboard")
	{
		m_blackboardTween.setForward(false);
	}
	else if (action == "showScores")
	{
		m_step = 4;
	}
	else if (action == "showFunFact")
	{
		showFunFact();
	}
	else if (action == "showBonus")
	{
		m_step = 13;
	}
	else if (action == "hideUpgrades")
	{
		m_levelUpgradeGroup->setVisible(false);
	}
	else if (action == "showClickToContinue")
	{
	}
	else if (action == "hideClickToContinue")
	{
	}
	else if (action == "showQuitButton")
	{
		setQuitButtonVisible(true);
	}
	else if (action == "hideQuitButton")
	{
		setQuitButtonVisible(false);
	}
	else if (action == "gameWon")
	{
		showGameWon();
		showGameWonTexts();
	}
	else if (action == "endGame")
	{
		onAction("endGame");
	}
	else if (action == "setCancelAction")
	{
		m_cancelAction = param;
	}
}

// 0x4414F0: replaces the base Enter/Esc/hotkey handling while a script runs
void StoryScreen::onKeyDown(int keyCode)
{
	if (m_script)
	{
		if ((keyCode == 'F' || keyCode == 'f') && g_unlockAll)
		{
			showFunFact();
		}
		else if (keyCode == VK_ESCAPE && m_skipString->isVisible())
		{
			onAction(m_cancelAction);
		}
		else
		{
			m_script->onEvent("fastForwardStep");
			m_script->onEvent("continue");
		}
	}
}

// 0x441650: skips the daily count-up ("continue" during steps 4-11)
void StoryScreen::finishDailyReport()
{
	m_salesTarget->setVisible(true);
	m_actualRevenue->setVisible(true);
	m_bonusSales->setVisible(true);
	m_ordersDelivered->setVisible(true);
	m_totalScore->setNumber(m_progress->m_score, "$");
	if (m_bonus > 0)
		m_bonusSales->setNumber(m_bonus, "$");
	engine::User* user = engine::UserManager::getInstance()->getCurrentUser();
	m_shownFoodBank = user->getAttributes().getInt("foodBank", 0);
	m_deliveredToCount = 0;
	updateOrderTexts();
	for (int i = 0; i < (int)m_stars.size(); i++)
		m_stars[i]->setVisible(true);
	m_longestCombo->setVisible(true);
	m_longestCombo->setNumber(m_comboCount, "");
	for (unsigned int j = 0; j < m_comboIcons.size(); j++)
		m_comboIcons[j]->setVisible(true);
	if (m_rollupSound && m_rollupSound->isPlaying())
		m_rollupSound->stop();
	m_step = 2;
}

// 0x441BF0: m_step 2 runs the script; 4-12 count up the DAILY REPORT, 13-21 the BONUS REPORT
void StoryScreen::updateStory(engine::UpdateContext& ctx)
{
	switch (m_step)
	{
	case 2:
		m_screenTween.update(ctx.elapsed);
		m_blackboardTween.update(ctx.elapsed);
		updateSlideAnimations();
		if (m_script)
		{
			m_script->update(ctx);
			if (m_script->isDone())
				onAction(m_cancelAction);
		}
		break;
	case 4:
		m_actualRevenue->setVisible(true);
		PizzaFrenzy::getSounds()->playSound("tip_cash", 1.0f, 1.0f);
		m_stepTimer = 0.3f;
		m_step = 5;
		break;
	case 5:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_salesTarget->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("tip_cash", 1.0f, 1.0f);
			m_stepTimer = 0.3f;
			m_step = 6;
		}
		break;
	case 6:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_bonusSales->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("tip_cash", 1.0f, 1.0f);
			m_stepTimer = 0.3f;
			if (m_bonusToCount > 0)
			{
				m_stepTimer = 0.3f;
				m_step = 7;
			}
			else
			{
				m_stepTimer = 0.3f;
				m_step = 8;
			}
		}
		break;
	case 7:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			if (m_bonusToCount > 0)
			{
				int amount = (int)(ctx.elapsed * 2000.0f);
				m_bonusToCount -= amount;
				m_shownTotal += amount;
				if (m_bonusToCount < 0)
				{
					m_shownTotal += m_bonusToCount;
					m_bonusToCount = 0;
				}
				m_bonusSales->setNumber(m_bonus - m_bonusToCount, "$");
				m_totalScore->setNumber(m_shownTotal, "$");
				if (m_rollupSound && !m_rollupSound->isPlaying())
					m_rollupSound->play();
			}
			else
			{
				if (m_rollupSound)
					m_rollupSound->stop();
				PizzaFrenzy::getSounds()->playSound("tip_cash", 1.0f, 1.0f);
				m_stepTimer = 0.3f;
				m_step = 8;
			}
		}
		break;
	case 8:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_ordersDelivered->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			m_stepTimer = 0.3f;
			m_step = 9;
		}
		break;
	case 9:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			if (m_deliveredToCount > 0)
			{
				int amount = (int)(ctx.elapsed * 100.0f);
				m_deliveredToCount -= amount;
				m_shownFoodBank += amount;
				if (m_deliveredToCount < 0)
				{
					m_shownFoodBank += m_deliveredToCount;
					m_deliveredToCount = 0;
				}
				updateOrderTexts();
				if (m_rollupSound && !m_rollupSound->isPlaying())
					m_rollupSound->play();
			}
			else
			{
				if (m_rollupSound)
					m_rollupSound->stop();
				m_stepTimer = 0.3f;
				m_step = 10;
			}
		}
		break;
	case 10:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			if (m_starIndex < (int)m_stars.size())
			{
				m_stars.at(m_starIndex++)->setVisible(true);
				PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
				m_stepTimer = 0.3f;
				if (m_starIndex == (int)m_stars.size() && m_progress->m_satisfaction % 20 >= 10)
					m_shownTotal += 250;
				else
					m_shownTotal += 500;
				m_totalScore->setNumber(m_shownTotal, "$");
			}
			else
			{
				m_stepTimer = 0.5f;
				m_step = 11;
				PizzaFrenzy::getSounds()->playSound("popup_flipover", 1.0f, 1.0f);
			}
		}
		break;
	case 11:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			if (m_comboIndex < (int)m_comboIcons.size())
			{
				m_longestCombo->setVisible(true);
				m_comboIcons.at(m_comboIndex)->setVisible(true);
				float pitch = m_comboIndex * 0.05f + 1.0f;
				PizzaFrenzy::getSounds()->playSound("bonus_toppingCombo", 1.0f, pitch);
				int frenzy = (m_comboIndex + 1) / g_frenzyComboSize;
				if (frenzy > 0 && (m_comboIndex + 1) % g_frenzyComboSize == 0)
				{
					std::string sound;
					int soundLevel = frenzy - 1;
					if (soundLevel > 5)
						soundLevel = 5;
					engine::format(sound, "frenzy%d", soundLevel);
					PizzaFrenzy::getSounds()->playSound(sound, 1.0f, 1.0f);
					int cash = g_frenzyComboBonus * frenzy;
					m_shownTotal += cash;
					m_totalScore->setNumber(m_shownTotal, "$");
					engine::Vector2 position = m_comboIcons.at(m_comboIndex)->getScreenPosition();
					engine::Splat* splat = PizzaFrenzy::getSplatFactory()->createSplat("cashTipSplat", position.x,
						position.y, 0, 0);
					std::string text;
					engine::format(text, "$%d", cash);
					splat->setText(text);
				}
				m_stepTimer = 0.22f;
				m_longestCombo->setNumber(m_comboIndex + 1, "");
				m_comboIndex++;
			}
			else
			{
				m_step = 12;
				m_stepTimer = 0.5f;
			}
		}
		break;
	case 12:
		m_step = 2;
		break;
	case 13:
		m_perfectToppings->setVisible(true);
		PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
		m_stepTimer = 0.3f;
		m_step = 14;
		break;
	case 14:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_goodToppings->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			m_stepTimer = 0.3f;
			m_step = 15;
		}
		break;
	case 15:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_okayToppings->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			m_stepTimer = 0.3f;
			m_step = 16;
		}
		break;
	case 16:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_missedToppings->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			m_step = 17;
			m_stepTimer = 0.5f;
		}
		break;
	case 17:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_toppingBonus->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			m_step = 18;
			m_stepTimer = 0.5f;
		}
		break;
	case 18:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_timeLeft->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			m_stepTimer = 0.3f;
			m_step = 19;
		}
		break;
	case 19:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_timeLeftBonus->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			m_step = 20;
			m_stepTimer = 0.5f;
		}
		break;
	case 20:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			m_bonusTotal->setVisible(true);
			PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			m_stepTimer = 0.3f;
			m_step = 21;
		}
		break;
	case 21:
		m_stepTimer -= ctx.elapsed;
		if (m_stepTimer <= 0.0f)
		{
			if (m_bonusToCount > 0)
			{
				int amount = (int)(ctx.elapsed * 1000.0f);
				m_bonusToCount -= amount;
				m_shownTotal += amount;
				if (m_bonusToCount < 0)
				{
					m_shownTotal += m_bonusToCount;
					m_bonusToCount = 0;
				}
				m_bonusTotal->setNumber(m_bonus - m_bonusToCount, "$");
				m_bonusTotalScore->setNumber(m_shownTotal, "$");
				if (m_rollupSound && !m_rollupSound->isPlaying())
					m_rollupSound->play();
			}
			else
			{
				if (m_rollupSound)
					m_rollupSound->stop();
				m_step = 2;
				m_stepTimer = 0.5f;
			}
		}
		break;
	}
	engine::Screen::update(ctx);
}

// 0x442CA0: layout and button actions (the ActionListener at +0x1AC)
void StoryScreen::onAction(std::string action)
{
	if (action == "quitStoryScreen")
	{
		if (m_rollupSound && m_rollupSound->isPlaying())
			m_rollupSound->stop();
		getGame()->onScreenEvent("confirmQuit");
	}
	else if (action == "continue")
	{
		if (m_script)
		{
			m_script->onEvent("fastForwardStep");
			if (m_step >= 4 && m_step <= 11)
			{
				finishDailyReport();
				PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			}
			else if (m_step >= 13 && m_step <= 21)
			{
				finishBonusReport();
				PizzaFrenzy::getSounds()->playSound("menu_gradeAppears", 1.0f, 1.0f);
			}
		}
	}
	else if (action == "skip")
	{
		resetStory();
		engine::Screen::onAction("storyDone");
	}
	else if (action == "replay")
	{
		resetStory();
		m_script->start();
	}
	if (m_script)
		m_script->onEvent(action);
	engine::getApplication()->log("layout action: %s\n", action.c_str());
	engine::Screen::onAction(action);
}

// 0x442F50: the raw pointers, counters and timers are left uninitialised
StoryScreen::StoryScreen()
{
}

// 0x443290: one icon of the most served topping per order of its longest combo, 100 points each
void StoryScreen::createComboIcons()
{
	m_longestCombo->setVisible(false);
	m_longestCombo->setNumber(0, "");
	m_comboGroup->removeAllChildren();
	m_comboIcons.clear();
	m_comboIndex = 0;
	m_comboTopping = m_progress->getMostServedTopping();
	m_comboCount = m_progress->m_bestToppingCombos[m_comboTopping];
	float x = 0.0f;
	for (int i = 0; i < m_comboCount; i++)
	{
		engine::Image* icon = new engine::Image(m_comboTopping->m_pizzaImage);
		icon->setPosition(x, 0.0f);
		icon->setVisible(false);
		x -= engine::randomInt(4, 7);
		m_comboIcons.push_back(icon);
		m_comboGroup->addChild(icon);
		m_progress->m_score += 100;
	}
}

// 0x443510: a full star per 20% of satisfaction (500 points each), a half star (250) for a remainder of 10% or more
void StoryScreen::createRatingStars()
{
	int fullStars = 5 * m_progress->m_satisfaction / 100;
	m_serviceRating->removeAllChildren();
	m_starIndex = 0;
	m_stars.clear();
	engine::Bitmap* fullStar = engine::getApplication()->getImage("res\\hud\\star-full.jpg");
	fullStar->setPivotType(1);
	float x = -fullStar->getWidth() * 0.5f;
	for (int i = 0; i < fullStars; i++)
	{
		engine::Image* star = new engine::Image(fullStar);
		star->setPosition(x, 0.0f);
		star->setVisible(false);
		x -= fullStar->getWidth() + 5.0f;
		m_serviceRating->addChild(star);
		m_stars.push_back(star);
		m_progress->m_score += 500;
	}
	if (m_progress->m_satisfaction % 20 >= 10)
	{
		engine::Bitmap* halfStar = engine::getApplication()->getImage("res\\hud\\star-half.jpg");
		halfStar->setPivotType(1);
		engine::Image* star = new engine::Image(halfStar);
		star->setPosition(x, 0.0f);
		star->setVisible(false);
		m_serviceRating->addChild(star);
		m_stars.push_back(star);
		m_progress->m_score += 250;
	}
}

// 0x443830: fills in the DAILY REPORT (counted up by updateStory), adds the sales bonus and the delivered orders to
// the score and the user's food bank, and saves the progress
void StoryScreen::initDailyReport()
{
	m_gameWon->setVisible(false);
	m_bonusStats->setVisible(false);
	m_stats->setVisible(true);
	m_salesTarget->setVisible(false);
	m_actualRevenue->setVisible(false);
	m_bonusSales->setVisible(false);
	m_totalScore->setVisible(true);
	m_ordersDelivered->setVisible(false);
	m_foodBank->setVisible(true);
	m_actualRevenue->setNumber(m_progress->m_cash, "$");
	m_salesTarget->setNumber(m_progress->m_cashGoal, "$");
	m_totalScore->setNumber(m_progress->m_score, "$");
	m_shownTotal = m_progress->m_score;
	m_bonusToCount = m_progress->m_cash - m_progress->m_cashGoal;
	m_bonus = m_bonusToCount;
	m_bonusSales->setNumber(0, "$");
	m_bonusSales->setColor(1.0f, 1.0f, 1.0f);
	if (m_bonusToCount > 0)
		m_progress->m_score += m_bonusToCount;
	m_deliveredToCount = m_progress->m_ordersCompleted;
	m_deliveredCount = m_deliveredToCount;
	engine::UserManager* users = engine::UserManager::getInstance();
	engine::User* user = users->getCurrentUser();
	if (user)
	{
		m_shownFoodBank = user->getAttributes().getInt("foodBank", 0);
		user->getAttributes().setInt("foodBank", m_deliveredToCount + m_shownFoodBank);
		users->saveUser(user);
	}
	updateOrderTexts();
	createRatingStars();
	createComboIcons();
	m_stepTimer = 0.0f;
	m_revealDelay = g_dailyReportDelay;
	getGame()->saveProgress(m_progress);
}
