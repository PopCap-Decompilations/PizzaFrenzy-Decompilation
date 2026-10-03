// HudMessage: the HUD's pop-up message that grows, rises and fades out within 2 s.
#include "HudMessage.h"

#include "engine/Application.h"
#include "engine/KeyframeCurve.h"
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

// 0x42BB00
void HudMessage::setText(const std::string& text)
{
	m_text->setText(text);
}

// 0x42BB10
void HudMessage::show()
{
	m_animation->rewind();
	m_animation->playIn();
}

// 0x42BB30
HudMessage::HudMessage()
{
	m_animation = new AnimatedContainer();
	m_animation->addAlphaInKey(makeKeyframe(0.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addAlphaInKey(makeKeyframe(1.5f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addAlphaInKey(makeKeyframe(2.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addPositionInKey(makeKeyframe(0.0f, engine::Vector2(0.0f, 0.0f), 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addPositionInKey(makeKeyframe(0.5f, engine::Vector2(0.0f, -30.0f), 0.5f, 0.05f, 0.0f, 0.05f));
	m_animation->addPositionInKey(makeKeyframe(1.5f, engine::Vector2(0.0f, -40.0f), 0.0f, 0.05f, 0.5f, 1.0f));
	m_animation->addPositionInKey(makeKeyframe(2.0f, engine::Vector2(0.0f, -60.0f), 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addScaleInKey(makeKeyframe(0.0f, 0.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	m_animation->addScaleInKey(makeKeyframe(0.5f, 0.5f, 0.5f, 0.05f, 0.0f, 0.05f));
	m_animation->addScaleInKey(makeKeyframe(1.5f, 0.6f, 0.0f, 0.05f, 0.5f, 1.0f));
	m_animation->addScaleInKey(makeKeyframe(2.0f, 1.0f, 0.0f, 1.0f, 0.0f, 1.0f));
	addChild(m_animation);

	m_text = new engine::TextItem();
	m_text->setBlendMode(1);
	m_text->setFont(engine::getApplication()->getFont("res\\fonts\\titleFont.xml"));
	m_text->setXAlign(1);
	m_text->setYAlign(1);
	m_animation->addChild(m_text);
	m_animation->init(false);
}
