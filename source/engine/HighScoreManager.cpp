#include "HighScoreManager.h"

#include <cstdlib>
#include <map>
#include <string>

#include "Application.h"
#include "HighScoreEntry.h"
#include "HighScoreTable.h"
#include "Properties.h"
#include "StringUtil.h"
#include "Win32RegistryKey.h"

namespace engine
{
	static const char* s_entryKeyFormat = "%03d%s";		// 0x521208 registry value name of an entry attribute
	static std::string s_countKey("count");				// 0x52120C

	HighScoreManager* HighScoreManager::s_instance;

	// 0x469090
	char HighScoreManager::filterNameChar(char c)
	{
		// the last range never matches: c is a signed char
		if ((c >= 'a' && c <= 'z') || (c >= 'A' && c <= 'Z') || (c >= '0' && c <= '9') || c == ' ' || c == '-' ||
			c == '.' || (c >= 0xC0 && c <= 0xFF))
			return c;
		return 0;
	}

	// 0x4690D0
	bool HighScoreManager::close()
	{
		if (!m_key)
			return false;
		bool result = m_key->close();
		m_key = 0;
		return result;
	}

	// 0x469110
	bool HighScoreManager::fetchGlobalScores(HighScoreTable* table)
	{
		Application* app = getApplication();
		const std::string& mode = table->getName();
		std::string url;
		format(url, "http://www.gamehouse.com/globalhs.jsp?game=%s&mode=%s", m_gameName.c_str(), mode.c_str());
		table->clear();
		table->sendRequest(app->openUrl(url));
		return true;
	}

	// 0x4691E0
	HighScoreManager* HighScoreManager::getInstance()
	{
		if (!s_instance)
			s_instance = new HighScoreManager();
		return s_instance;
	}

	// 0x469250
	bool HighScoreManager::open(const std::string& companyKey, const std::string& gameKey)
	{
		std::string path = companyKey;
		path += '\\';
		path += gameKey;
		path += "\\scoreboards";
		m_key = getApplication()->openRootRegistryKey(1, path);
		if (m_key)
			return true;
		return false;
	}

	// 0x469370
	void HighScoreManager::setGameName(const std::string& name)
	{
		m_gameName = name;
	}

	// 0x469390
	std::string HighScoreManager::encodeScore(const std::string& gameName, const std::string& playerName, int level,
		int points)
	{
		int hash = 0;
		int salt = (int)(rand() * (1000.0 / RAND_MAX));
		std::string saltText;
		std::string code;
		if (salt == 0 || salt == 1000)
		{
			salt = 999;
			saltText = "999";
		}
		else
			format(saltText, "%03d", salt);
		// the hash is rotated left a byte at a time (its top byte is read from memory) and each character OR'ed in
		for (unsigned int i = 0; i < playerName.size(); i++)
			hash = (hash << 8) | ((unsigned char*)&hash)[3] | playerName.at(i);
		for (unsigned int i = 0; i < gameName.size(); i++)
			hash = (hash << 8) | ((unsigned char*)&hash)[3] | gameName.at(i);
		format(code, "%sA%dA%d", saltText.c_str(), (hash % 1000) ^ salt ^ level, (salt * salt + points) ^ hash);
		return code;
	}

	// 0x469580
	bool HighScoreManager::loadEntry(int index, HighScoreEntry* entry, RegistryKey* key)
	{
		Properties attributes;
		entry->saveEmpty(attributes);
		std::string name;
		for (std::map<std::string, std::string>::iterator it = attributes.begin(); it != attributes.end(); ++it)
		{
			format(name, s_entryKeyFormat, index, it->first.c_str());
			it->second = key->getString(name, it->second);
		}
		entry->load(attributes);
		return true;
	}

	// 0x469720
	void HighScoreManager::submitGlobalScore(HighScoreTable* table, HighScoreEntry* entry)
	{
		Application* app = getApplication();
		Properties attributes;
		entry->save(attributes);
		std::string player = attributes.getString("player", "");
		int level = attributes.getInt("level", 0);
		int points = attributes.getInt("points", 0);
		const std::string& mode = table->getName();
		std::string code = encodeScore(m_gameName, player, level, points);
		std::string url;
		format(url, "http://www.gamehouse.com/addglobalhs.jsp?game=%s&mode=%s&name=%s&score=%s&s=%d", m_gameName.c_str(),
			mode.c_str(), player.c_str(), code.c_str(), points);
		table->clear();
		table->sendRequest(app->openUrl(url));
	}

	// 0x469AC0
	bool HighScoreManager::saveEntry(int index, HighScoreEntry* entry, RegistryKey* key)
	{
		Properties attributes;
		entry->save(attributes);
		std::string name;
		for (std::map<std::string, std::string>::const_iterator it = attributes.begin(); it != attributes.end(); ++it)
		{
			format(name, s_entryKeyFormat, index, it->first.c_str());
			key->setString(name, it->second);
		}
		return true;
	}

	// 0x469BF0
	bool HighScoreManager::loadScores(HighScoreTable* table)
	{
		Application* app = getApplication();
		RefPtr<RegistryKey> key = app->openRegistryKey(m_key, table->getName());
		int count = key->getInt(s_countKey, 0);
		for (int i = 0; i < count; i++)
		{
			HighScoreEntry* entry = table->createEntry();
			loadEntry(i, entry, key);
			table->addEntry(entry);
		}
		table->clearNewEntryIndex();
		table->setStatus(2);
		return true;
	}

	// 0x469CD0
	bool HighScoreManager::saveScores(HighScoreTable* table)
	{
		Application* app = getApplication();
		RefPtr<RegistryKey> key = app->openRegistryKey(m_key, table->getName());
		key->setInt(s_countKey, table->getCount());
		int index = 0;
		for (HighScoreTable::iterator it = table->begin(); it != table->end(); ++it, ++index)
			saveEntry(index, *it, key);
		return true;
	}
}
