#include "SoundManifestHandler.h"

#include "Properties.h"
#include "SoundMgr.h"

namespace engine
{
	// 0x490FC0 (folded)
	void SoundManifestHandler::setSoundManager(SoundMgr* manager)
	{
		m_soundManager = manager;
	}

	// 0x490FD0
	SoundManifestHandler::SoundManifestHandler()
	{
		m_soundManager = NULL;
	}

	// 0x491070
	SoundManifestHandler::~SoundManifestHandler()
	{
		m_soundManager = NULL;
	}

	// 0x491120
	void SoundManifestHandler::endElement(const std::string& name)
	{
		if (name == "randomSound")
			m_randomSoundName.clear();
	}

	// 0x491170
	void SoundManifestHandler::startElement(const std::string& name, const Properties& attributes)
	{
		std::string soundName = attributes.getString("name", "");
		if (name == "randomSound")
		{
			m_randomSoundName = soundName;
		}
		else if (name == "sound")
		{
			float volume = attributes.getFloat("vol", 1.0f);
			float pitch = attributes.getFloat("pitch", 1.0f);
			if (!m_randomSoundName.empty())
			{
				float weight = attributes.getFloat("weight", 1.0f);
				m_soundManager->addRandomSound(m_randomSoundName, soundName, weight, volume, pitch);
			}
			else
			{
				std::string fileName = attributes.getString("file", "");
				int count = attributes.getInt("count", 1);
				int category = attributes.getInt("category", 0);
				m_soundManager->addSound(soundName, fileName, count, volume, pitch, category);
			}
		}
	}
}
