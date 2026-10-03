#include "MusicTrack.h"

// 0x4041E0 (folded)
std::string MusicTrack::getFile() const
{
	return m_file;
}

// 0x450470 (folded)
float MusicTrack::getFadeInTime() const
{
	return m_fadeInTime;
}

// 0x450480 (folded)
float MusicTrack::getFadeOutTime() const
{
	return m_fadeOutTime;
}

// 0x450490 (folded)
float MusicTrack::getVolume() const
{
	return m_volume;
}

// 0x4504A0 (folded)
bool MusicTrack::getLoop() const
{
	return m_loop;
}

// 0x4504B0 (folded)
int MusicTrack::getCategory() const
{
	return m_category;
}

// 0x4504C0
MusicTrack::MusicTrack(std::string file, float fadeInTime, float fadeOutTime, float volume, bool loop, int category)
{
	m_file = file;
	m_fadeInTime = fadeInTime;
	m_fadeOutTime = fadeOutTime;
	m_volume = volume;
	m_loop = loop;
	m_category = category;
}
