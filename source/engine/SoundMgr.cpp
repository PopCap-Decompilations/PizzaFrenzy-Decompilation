#include "SoundMgr.h"

#include <algorithm>

#include "Application.h"
#include "Range.h"
#include "SoundManifestHandler.h"
#include "SoundPool.h"

namespace engine
{
	// 0x46DDC0: false when parsing the manifest throws
	bool SoundMgr::load(std::string fileName)
	{
		SoundManifestHandler handler;
		handler.setSoundManager(this);
		try
		{
			getApplication()->loadXml(fileName, &handler);
		}
		catch (...)
		{
			// 0x46DE46 (load$catch): the code after the catch block, which destroys the locals and returns false
			return false;
		}
		return true;
		// 0x46DE5C (load$epilog): the epilog both returns share
	}

	// 0x46DF70: a member picked with probability proportional to its weight
	RandomSoundEntry* SoundGroup::pickRandom() const
	{
		if (m_sounds.empty())
			return 0;
		if (m_sounds.size() == 1)
			return m_sounds[0].m_sound;
		WeightedSound key;
		key.m_sound = 0;
		key.m_weight = randomFloat(0.0f, m_totalWeight);
		return std::lower_bound(m_sounds.begin(), m_sounds.end(), key)->m_sound;
	}

	// 0x46DFE0
	SoundPool* SoundMgr::findSound(std::string name) const
	{
		std::map<std::string, SoundPool*>::const_iterator it = m_sounds.find(name);
		if (it != m_sounds.end())
			return it->second;
		return 0;
	}

	// 0x46E420: a random group plays its member with the member's volume and pitch (the arguments are ignored)
	bool SoundMgr::playSound(const std::string& name, float volume, float pitch)
	{
		SoundPool* sound = findSound(name);
		if (sound)
		{
			sound->play(volume, pitch);
			return true;
		}
		std::map<std::string, SoundGroup*>::iterator it = m_randomSounds.find(name);
		if (it != m_randomSounds.end())
		{
			RandomSoundEntry* entry = it->second->pickRandom();
			if (entry)
			{
				playSound(entry->m_name, entry->m_volume, entry->m_pitch);
				return true;
			}
		}
		return false;
	}

	// 0x46E4C0: a free handle of the sound (or of a random member of the group); 0 when every handle is busy
	SoundHandle* SoundMgr::getSound(const std::string& name)
	{
		SoundPool* sound = findSound(name);
		if (sound)
			return sound->getHandle(false);
		std::map<std::string, SoundGroup*>::iterator it = m_randomSounds.find(name);
		if (it != m_randomSounds.end())
		{
			RandomSoundEntry* entry = it->second->pickRandom();
			if (entry)
				return getSound(entry->m_name);
		}
		return 0;
	}

	// 0x46F090: the random groups and their entries are not deleted
	SoundMgr::~SoundMgr()
	{
		for (std::map<std::string, SoundPool*>::iterator it = m_sounds.begin(); it != m_sounds.end(); ++it)
			delete it->second;
		m_sounds.clear();
	}

	// 0x46F1C0: a name already registered is kept (true)
	bool SoundMgr::addSound(const std::string& name, const std::string& fileName, int instanceCount, float volume, float pitch, int category)
	{
		if (m_sounds.find(name) == m_sounds.end())
		{
			if (!getApplication()->loadSound(name, fileName))
			{
				getApplication()->log("Warning: sound file %s=%s missing or invalid\n", name.c_str(), fileName.c_str());
				return false;
			}
			m_sounds[name] = new SoundPool(getSoundSystem(), name.c_str(), instanceCount, volume, pitch, category);
		}
		return true;
	}

	// 0x46F2E0: the group is created by its first member
	bool SoundMgr::addRandomSound(const std::string& groupName, const std::string& soundName, float weight, float volume, float pitch)
	{
		SoundGroup* group;
		std::map<std::string, SoundGroup*>::iterator it = m_randomSounds.find(groupName);
		if (it == m_randomSounds.end())
		{
			group = new SoundGroup;
			m_randomSounds[groupName] = group;
		}
		else
			group = it->second;

		RandomSoundEntry* entry = new RandomSoundEntry;
		entry->m_name = soundName;
		entry->m_pitch = pitch;
		entry->m_volume = volume;
		group->m_totalWeight += weight;
		WeightedSound sound;
		sound.m_sound = entry;
		sound.m_weight = group->m_totalWeight;
		group->m_sounds.push_back(sound);
		return true;
	}

	// 0x46F3D0
	SoundMgr::SoundMgr()
	{
	}
}
