#include "HighScoreEntry.h"

#include <string>

#include "engine/Properties.h"
#include "engine/StringUtil.h"

// the attribute names of an entry (dynamic initialisers 0x4F8850, 0x4F8870, 0x4F8890)
static std::string s_playerKey = "player";
static std::string s_pointsKey = "points";
static std::string s_levelKey = "level";

// 0x418480
void HighScoreEntry::save(engine::Properties& attributes) const
{
	engine::HighScoreEntry::save(attributes);
	attributes.setString(s_playerKey, m_player);
	attributes.setInt(s_pointsKey, m_points);
	attributes.setInt(s_levelKey, m_level);
}

// 0x4184D0: an empty entry for the score tables' factory (points and level are left uninitialised)
HighScoreEntry::HighScoreEntry()
{
}

// 0x418880
HighScoreEntry::HighScoreEntry(const std::string& player, int points, int level)
	: m_player(player)
	, m_points(points)
	, m_level(level)
{
}

// 0x418930: the current values are the defaults
void HighScoreEntry::load(const engine::Properties& attributes)
{
	engine::HighScoreEntry::load(attributes);
	m_player = attributes.getString(s_playerKey, m_player);
	m_points = attributes.getInt(s_pointsKey, m_points);
	m_level = attributes.getInt(s_levelKey, m_level);
}

// 0x4189F0
void HighScoreEntry::saveEmpty(engine::Properties& attributes) const
{
	engine::HighScoreEntry::saveEmpty(attributes);
	attributes.setString(s_playerKey, "");
	attributes.setInt(s_pointsKey, 0);
	attributes.setInt(s_levelKey, 0);
}

// 0x418AB0: the points as "$1,234"
void HighScoreEntry::formatDisplay(engine::Properties& attributes) const
{
	std::string text;
	attributes.setString(s_playerKey, m_player);
	engine::formatNumber(text, m_points);
	text.insert(0, "$");
	attributes.setString(s_pointsKey, text);
	engine::formatNumber(text, m_level);
	attributes.setString(s_levelKey, text);
}
