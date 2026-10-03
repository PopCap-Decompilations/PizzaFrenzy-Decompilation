#include "MusicPlayer.h"

#include <string>

#include "engine/Application.h"
#include "engine/SoundHandle.h"
#include "MusicTrack.h"

// 0x4505E0
void MusicPlayer::stop()
{
	static_cast<MusicState*>(m_states.getState())->stop();
}

// 0x4505F0
void MusicPlayer::setVolume(float volume)
{
	if (m_sound)
		m_sound->setVolume(volume);
}

// 0x450610
void MusicStoppedState::enter()
{
	if (m_owner->m_sound)
		m_owner->m_sound->pause();
}

// 0x450630
void MusicStoppedState::stop()
{
	if (m_owner->m_sound)
		m_owner->m_sound->stop();
}

// 0x450650 (folded)
void MusicPlayingState::enter()
{
	if (m_owner->m_sound)
		m_owner->m_sound->play();
}

// 0x450650 (folded)
void MusicPlayingState::enable()
{
	if (m_owner->m_sound)
		m_owner->m_sound->play();
}

// 0x450670
void MusicFadeOutState::enter()
{
	if (m_owner->m_sound)
	{
		m_owner->m_sound->stop();
		m_owner->m_fadingOut = true;
	}
}

// 0x450690
void MusicFadeOutState::update(engine::UpdateContext& context)
{
	if (m_owner->m_sound)
	{
		if (!m_owner->m_sound->isPlaying())
		{
			m_owner->m_fadingOut = false;
			m_owner->m_sound->destroy();
			m_owner->m_sound = 0;
			startNextTrack();
		}
	}
	else
	{
		startNextTrack();
	}
}

// 0x4506E0
bool MusicFadeOutState::isPlaying()
{
	return m_owner->m_sound && m_owner->m_sound->isPlaying();
}

// 0x450710
void MusicPlayer::setState(MusicState* state)
{
	m_states.switchState(state);
}

// 0x450720
void MusicPlayer::update(engine::UpdateContext& info)
{
	m_states.update(info);
}

// 0x450750
MusicPlayer::~MusicPlayer()
{
	if (m_sound)
	{
		m_sound->setFadeOutTime(0.0f);
		m_sound->destroy();
		m_sound = 0;
	}
}

// 0x450820
void MusicPlayer::setEnabled(bool enabled)
{
	m_enabled = enabled;
	if (enabled)
		static_cast<MusicState*>(m_states.getState())->enable();
	else
		static_cast<MusicState*>(m_states.getState())->disable();
}

// 0x450850
MusicState::MusicState(MusicPlayer* player)
	: engine::State<MusicPlayer>(player)
{
}

// 0x4508E0
MusicStoppedState::MusicStoppedState(MusicPlayer* player)
	: MusicState(player)
{
}

// 0x450930
MusicPlayer::MusicPlayer()
{
	m_sound = 0;
	m_states.switchState(new MusicStoppedState(this));
	m_fadingOut = false;
	m_nextTrack = 0;
	m_enabled = true;
}

// 0x450AA0
void MusicPlayer::play(MusicTrack* track)
{
	if (track && (track != m_track || !static_cast<MusicState*>(m_states.getState())->isPlaying() || m_fadingOut))
		static_cast<MusicState*>(m_states.getState())->play(track);
}

// 0x450AF0
void MusicStoppedState::enable()
{
	m_owner->setState(new MusicPlayingState(m_owner));
}

// 0x450B90
void MusicPlayingState::update(engine::UpdateContext& context)
{
	if (m_owner->m_nextTrack)
		m_owner->setState(new MusicFadeOutState(m_owner));
	else if (m_owner->m_sound && !m_owner->m_sound->isPlaying())
		m_owner->setState(new MusicStoppedState(m_owner));
}

// 0x450CA0
void MusicPlayingState::disable()
{
	m_owner->setState(new MusicStoppedState(m_owner));
}

// 0x450D40
void MusicPlayingState::stop()
{
	if (m_owner->m_nextTrack)
		m_owner->m_nextTrack = 0;
	m_owner->setState(new MusicFadeOutState(m_owner));
}

// 0x450E00
void MusicFadeOutState::disable()
{
	m_owner->setState(new MusicFadeOutState(m_owner));
}

// 0x450EA0
void MusicFadeOutState::stop()
{
	m_owner->m_nextTrack = 0;
}

// 0x450ED0
void MusicStoppedState::play(MusicTrack* track)
{
	m_owner->m_nextTrack = track;
	if (m_owner->m_enabled)
		m_owner->setState(new MusicPlayingState(m_owner));
}

// 0x450FB0 (folded)
void MusicPlayingState::play(MusicTrack* track)
{
	m_owner->m_nextTrack = track;
}

// 0x450FB0 (folded)
void MusicFadeOutState::play(MusicTrack* track)
{
	m_owner->m_nextTrack = track;
}

// 0x450FF0
void MusicFadeOutState::startNextTrack()
{
	if (!m_owner->m_nextTrack)
	{
		m_owner->setState(new MusicStoppedState(m_owner));
	}
	else
	{
		m_owner->m_track = m_owner->m_nextTrack;
		m_owner->m_nextTrack = 0;
		engine::getApplication()->loadSoundStream(m_owner->m_track->getFile(), m_owner->m_track->getFile());
		m_owner->m_sound = engine::getApplication()->createSound(m_owner->m_track->getFile(), false, 1);
		if (m_owner->m_sound)
		{
			m_owner->m_sound->setFadeInTime(m_owner->m_track->getFadeInTime());
			m_owner->m_sound->setFadeOutTime(m_owner->m_track->getFadeOutTime());
			m_owner->m_sound->setLooping(m_owner->m_track->getLoop());
			m_owner->m_sound->setVolume(m_owner->m_track->getVolume());
			m_owner->m_sound->setCategory(m_owner->m_track->getCategory());
			m_owner->m_sound->play();
			m_owner->setState(new MusicPlayingState(m_owner));
		}
		else
		{
			m_owner->m_track = 0;
			m_owner->setState(new MusicStoppedState(m_owner));
		}
	}
}

// 0x492310 (folded)
void MusicStoppedState::update(engine::UpdateContext& context)
{
}

// 0x4529D0 (folded)
bool MusicStoppedState::isPlaying()
{
	return false;
}

// 0x4D0470 (folded)
void MusicStoppedState::disable()
{
}

// 0x451610 (folded)
bool MusicPlayingState::isPlaying()
{
	return true;
}

// 0x4D0470 (folded)
void MusicFadeOutState::enable()
{
}
