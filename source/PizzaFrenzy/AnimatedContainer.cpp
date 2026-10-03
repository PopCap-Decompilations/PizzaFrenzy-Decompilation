// AnimatedContainer: a container animated by keyframe tracks and sound cues; SoundCue and SoundCueTrack.
#include "AnimatedContainer.h"

#include <algorithm>

#include "engine/SoundMgr.h"
#include "PizzaFrenzy.h"

// 0x42C130
void AnimatedContainer::addScaleInKey(const engine::Keyframe<float>& key)
{
	m_scaleIn.addKey(key);
}

// 0x42C140
void AnimatedContainer::addAlphaInKey(const engine::Keyframe<float>& key)
{
	m_alphaIn.addKey(key);
}

// 0x42C150
void AnimatedContainer::addScaleOutKey(const engine::Keyframe<float>& key)
{
	m_scaleOut.addKey(key);
}

// 0x42C160
void AnimatedContainer::addAlphaOutKey(const engine::Keyframe<float>& key)
{
	m_alphaOut.addKey(key);
}

// 0x42C170
void AnimatedContainer::addPositionInKey(const engine::Keyframe<engine::Vector2>& key)
{
	m_positionIn.addKey(key);
}

// 0x42C180
void AnimatedContainer::addPositionOutKey(const engine::Keyframe<engine::Vector2>& key)
{
	m_positionOut.addKey(key);
}

// 0x42C190
void AnimatedContainer::addSoundInCue(const SoundCue& cue)
{
	m_soundsIn.addCue(cue);
}

// 0x42C1A0
void AnimatedContainer::applyIn()
{
	float time = m_timer.getValue();
	m_scaleIn.animate(time, this);
	m_alphaIn.animate(time, this);
	m_positionIn.animate(time, this);
	m_soundsIn.update(time);
}

// 0x42C200
void AnimatedContainer::applyOut()
{
	float time = m_timer.getValue();
	m_scaleOut.animate(time, this);
	m_alphaOut.animate(time, this);
	m_positionOut.animate(time, this);
	m_soundsOut.update(time);
}

// 0x42C260
bool AnimatedContainer::isFinished()
{
	return m_state == 5;
}

// 0x42C270
AnimatedContainer::~AnimatedContainer()
{
}

// 0x42C3A0
void AnimatedContainer::startIn()
{
	float duration = (std::max)(m_positionIn.getEndTime(), m_scaleIn.getEndTime());
	duration = (std::max)(duration, m_alphaIn.getEndTime());
	m_timer.start(duration, duration);
	m_timer.restart();
}

// 0x42C430
void AnimatedContainer::startOut()
{
	float duration = (std::max)(m_positionOut.getEndTime(), m_scaleOut.getEndTime());
	duration = (std::max)(duration, m_alphaOut.getEndTime());
	m_timer.start(duration, duration);
	m_timer.restart();
}

// 0x42C4C0
void AnimatedContainer::update(engine::UpdateContext& context)
{
	switch (m_state)
	{
	case 1:
		m_timer.update(context.elapsed);
		applyIn();
		if (m_timer.isFinished())
			m_state = 2;
		break;
	case 3:
		m_timer.update(context.elapsed);
		applyIn();
		if (m_timer.isFinished())
		{
			m_state = 4;
			startOut();
		}
		break;
	case 4:
		m_timer.update(context.elapsed);
		applyOut();
		if (m_timer.isFinished())
		{
			if (m_removeWhenDone)
				setFlags(0x10);
			m_state = 5;
		}
		break;
	}
	engine::Container::update(context);
}

// 0x42C690
void AnimatedContainer::rewind()
{
	startIn();
	applyIn();
	m_state = 0;
}

// 0x42C6B0
void AnimatedContainer::playIn()
{
	setFlags(4);
	switch (m_state)
	{
	case 0:
	case 1:
	case 3:
		m_state = 1;
		break;
	case 2:
	case 4:
	case 5:
		m_state = 1;
		startIn();
		break;
	}
}

// 0x42C950
void AnimatedContainer::init(bool removeWhenDone)
{
	m_removeWhenDone = removeWhenDone;
	startIn();
	applyIn();
	m_state = 0;
}

// 0x42C980
AnimatedContainer::AnimatedContainer()
{
}

// 0x42CAD0
SoundCue::SoundCue(float time, const std::string& sound)
	: std::pair<const float, std::string>(time, sound)
{
}

// 0x42CB30
void SoundCueTrack::update(float time)
{
	std::map<float, std::string>::iterator next = m_next;
	if (next != m_cues.end())
		++next;
	while (m_next != m_cues.end() && time > m_next->first)
	{
		PizzaFrenzy::getSounds()->playSound(m_next->second, 1.0f, 1.0f);
		m_next = next;
		if (next != m_cues.end())
			++next;
	}

	std::map<float, std::string>::iterator previous = m_next;
	if (previous != m_cues.begin())
		--previous;
	while (m_next != m_cues.begin() && time < previous->first)
	{
		m_next = previous;
		if (previous != m_cues.begin())
			--previous;
	}
}

// 0x42D320
void SoundCueTrack::addCue(const SoundCue& cue)
{
	m_next = m_cues.insert(cue).first;
	m_next = m_cues.begin();
}

// 0x42D380
SoundCueTrack::~SoundCueTrack()
{
	m_cues.clear();
	m_next = m_cues.end();
}

// 0x42D440
SoundCueTrack::SoundCueTrack()
{
	m_cues.clear();
	m_next = m_cues.end();
}
