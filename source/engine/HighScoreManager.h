// engine::HighScoreManager: the singleton that keeps the local scoreboards in the registry and talks to the
// GameHouse global high-score server.
#pragma once

#include <string>

#include "RefPtr.h"

namespace engine
{
	class HighScoreEntry;
	class HighScoreTable;
	class RegistryKey;

	// No vtable, 0x20 bytes, never destroyed. The constructor is implicit (inlined in getInstance: m_key = 0, empty
	// m_gameName). Each table is a subkey (the table's name) of <company>\<game>\scoreboards holding "count" and the
	// entries' attributes as "%03d<attribute>" values.
	class HighScoreManager
	{
	public:
		// EditBox character filter for player names: letters, digits, space, '-' and '.'; anything else becomes 0
		static char filterNameChar(char c);
		bool close();															// m_key->close(), then releases it
		bool fetchGlobalScores(HighScoreTable* table);							// globalhs.jsp?game=&mode=<table name>
		static HighScoreManager* getInstance();									// created on first use
		bool open(const std::string& companyKey, const std::string& gameKey);	// <company>\<game>\scoreboards
		void setGameName(const std::string& name);								// the GameHouse game= id
		// anti-cheat code of addglobalhs.jsp's score=: random 3-digit salt + a byte hash of both names, "%sA%dA%d".
		// encodeScore, loadEntry and saveEntry don't use this but are members (__thiscall: ret 0x14 / ret 0xC).
		std::string encodeScore(const std::string& gameName, const std::string& playerName, int level, int points);
		bool loadEntry(int index, HighScoreEntry* entry, RegistryKey* key);
		void submitGlobalScore(HighScoreTable* table, HighScoreEntry* entry);	// addglobalhs.jsp?game&mode&name&score&s
		bool saveEntry(int index, HighScoreEntry* entry, RegistryKey* key);
		bool loadScores(HighScoreTable* table);
		bool saveScores(HighScoreTable* table);

		RefPtr<RegistryKey> m_key;							// +0x00 <company>\<game>\scoreboards
		std::string m_gameName;								// +0x04 GameHouse game id

		static HighScoreManager* s_instance;				// 0x532F68
	};
}
