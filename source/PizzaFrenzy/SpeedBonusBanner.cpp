#include "SpeedBonusBanner.h"

#include <string>

#include "engine/Application.h"
#include "engine/KeyframeCurve.h"
#include "engine/Rect.h"
#include "engine/RectangleItem.h"
#include "engine/TextItem.h"
#include "AnimatedContainer.h"

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

// 0x44FB50
void SpeedBonusBanner::hide()
{
	setVisible(false);
	m_showing = false;
}

// 0x44FB70
SpeedBonusBanner::SpeedBonusBanner()
{
}

// 0x44FCD0
void SpeedBonusBanner::update(const engine::UpdateContext& context)
{
	if (m_showing && m_animation->isFinished())
	{
		m_showing = false;
		fade(false, 0.3f, 1.0f, false);
	}
}

// 0x44FD10
void SpeedBonusBanner::init()
{
	removeAllChildren();

	engine::RectangleItem* strip = new engine::RectangleItem(engine::Rect(0.0f, 500.0f, 800.0f, 600.0f));
	strip->setAlpha(0.25f);
	strip->setColor(0.0f, 0.0f, 0.0f);
	addChild(strip);

	m_bonusText = new engine::TextItem();
	m_bonusText->setFont(engine::getApplication()->getFont("res\\fonts\\titleFont.xml"));
	m_bonusText->setXAlign(1);
	m_bonusText->setYAlign(1);
	m_bonusText->setPosition(400.0f, 550.0f);
	addChild(m_bonusText);

	m_animation = new AnimatedContainer();
	m_animation->addAlphaInKey(makeKeyframe(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addAlphaOutKey(makeKeyframe(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addPositionInKey(makeKeyframe(0.0f, engine::Vector2(1000.0f, 550.0f), 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addPositionInKey(makeKeyframe(0.5f, engine::Vector2(500.0f, 550.0f), 0.5f, 0.05f, 0.0f, 0.05f));
	m_animation->addPositionInKey(makeKeyframe(1.5f, engine::Vector2(300.0f, 550.0f), 0.0f, 0.05f, 0.5f, 1.0f));
	m_animation->addPositionInKey(makeKeyframe(2.0f, engine::Vector2(-200.0f, 550.0f), 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addPositionOutKey(makeKeyframe(0.0f, engine::Vector2(-200.0f, 550.0f), 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addScaleInKey(makeKeyframe(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addScaleOutKey(makeKeyframe(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addSoundInCue(SoundCue(0.1f, "bonus_speedySwish"));
	m_animation->addSoundInCue(SoundCue(1.5f, "bonus_speedySwish"));
	addChild(m_animation);

	m_titleText = new engine::TextItem();
	m_titleText->setFont(engine::getApplication()->getFont("res\\fonts\\titleFont.xml"));
	m_titleText->setXAlign(1);
	m_titleText->setYAlign(1);
	m_titleText->setText("SPEED BONUS!");
	m_animation->addChild(m_titleText);
	m_animation->init(false);

	setVisible(false);
	m_showing = false;
}

// 0x451610 (folded)
bool SpeedBonusBanner::isFadeFinished() const
{
	return true;
}
