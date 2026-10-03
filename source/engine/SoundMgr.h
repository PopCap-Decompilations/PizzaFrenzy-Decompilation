// engine::SoundMgr (the named sound registry filled from the sound manifest) and its random sound groups.
#pragma once

#include <map>
#include <string>
#include <vector>

#include "Object.h"

namespace engine
{
	class SoundHandle;
	class SoundPool;

	// a member sound of a <randomSound> group (0x24 bytes, created by SoundMgr::addRandomSound, never deleted)
	struct RandomSoundEntry
	{
		std::string m_name;						// +0x00 sound played (a <sound> of the manifest)
		float m_pitch;							// +0x1C
		float m_volume;							// +0x20
	};

	// one element of SoundGroup::m_sounds; std::lower_bound in pickRandom compares m_weight only
	struct WeightedSound
	{
		bool operator<(const WeightedSound& other) const
		{
			return m_weight < other.m_weight;
		}

		RandomSoundEntry* m_sound;				// +0x00
		float m_weight;							// +0x04 cumulative weight up to and including this entry
	};

	// a <randomSound>: weighted members (0x14 bytes; the constructor is always inlined)
	struct SoundGroup
	{
		SoundGroup()
			: m_totalWeight(0.0f)
		{
		}

		RandomSoundEntry* pickRandom() const;

		std::vector<WeightedSound> m_sounds;	// +0x00
		float m_totalWeight;					// +0x10
	};

	// Object +0x00, members from +0x0C, then the vtordisp (+0x24) and the Interface subobject (+0x28): 0x2C bytes.
	// Plays sounds by name; a randomSound group name plays a weighted random member.
	class SoundMgr : public Object
	{
	public:
		SoundMgr();
		virtual ~SoundMgr();

		bool load(std::string fileName);
		SoundPool* findSound(std::string name) const;
		bool playSound(const std::string& name, float volume, float pitch);
		SoundHandle* getSound(const std::string& name);
		bool addSound(const std::string& name, const std::string& fileName, int instanceCount, float volume, float pitch, int category);
		bool addRandomSound(const std::string& groupName, const std::string& soundName, float weight, float volume, float pitch);

		std::map<std::string, SoundPool*> m_sounds;			// +0x0C sounds by name, deleted by the destructor
		std::map<std::string, SoundGroup*> m_randomSounds;	// +0x18 random groups by name (never deleted: leaked)
	};
}
