// MusicTrack: a music entry of the tile manifest, played by MusicPlayer.
#pragma once

#include <string>

#include "engine/Object.h"

// A <music name file vol fadeInTime fadeOutTime loop category> of the tile manifest, created by TileManifestHandler
// and registered by name in TileManager::m_music. ICF merged its vtable into Title's (0x4FFFCC); implicit destructor.
// The members end at +0x3C; the vtordisp (+0x3C) and the Interface subobject (+0x40) follow them (0x44 bytes).
class MusicTrack : public engine::Object
{
public:
	MusicTrack(std::string file, float fadeInTime, float fadeOutTime, float volume, bool loop, int category);

	std::string getFile() const;
	float getFadeInTime() const;
	float getFadeOutTime() const;
	float getVolume() const;
	bool getLoop() const;
	int getCategory() const;

	std::string m_file;															// +0x0C
	float m_fadeInTime;															// +0x28
	float m_fadeOutTime;														// +0x2C
	float m_volume;																// +0x30
	bool m_loop;																// +0x34
	int m_category;																// +0x38
};
