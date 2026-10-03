// engine::SoundManifestHandler: the SAX handler of res\manifests\soundManifest.xml.
#pragma once

#include <string>

#include "XmlHandlerBase.h"

namespace engine
{
	class Properties;
	class SoundMgr;

	// <sound name file count vol pitch category/> -> SoundMgr::addSound; <randomSound name> <sound name weight vol
	// pitch/>... </randomSound> -> SoundMgr::addRandomSound. Built on the stack by SoundMgr::load. Layout:
	// XmlHandlerBase +0x00, members +0x14, then the vtordisp and Interface (0x3C bytes).
	class SoundManifestHandler : public XmlHandlerBase
	{
	public:
		// no sound manager, empty m_randomSoundName
		SoundManifestHandler();
		virtual ~SoundManifestHandler();

		// slot 1: m_soundManager = manager (body folded, 0x490FC0)
		virtual void setSoundManager(SoundMgr* manager);

		// XmlHandler
		// <randomSound name> sets m_randomSoundName; <sound>: addRandomSound inside a <randomSound>, else addSound
		virtual void startElement(const std::string& name, const Properties& attributes);
		// </randomSound>: clears m_randomSoundName
		virtual void endElement(const std::string& name);

		SoundMgr* m_soundManager;				// +0x14 set by setSoundManager
		std::string m_randomSoundName;			// +0x18 name of the open <randomSound>, empty outside one
	};
}
