#include "Properties.h"

#include <stdio.h>
#include <stdlib.h>

#include "StringUtil.h"

namespace engine
{
	// 0x475650
	std::map<std::string, std::string>::iterator Properties::begin()
	{
		return m_values.begin();
	}

	// 0x475660
	std::map<std::string, std::string>::iterator Properties::end()
	{
		return m_values.end();
	}

	// 0x475890
	std::string Properties::getString(const std::string& name, const std::string& defaultValue) const
	{
		std::string value;
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		value = it == m_values.end() ? defaultValue : it->second;
		return value;
	}

	// 0x475960
	int Properties::getInt(const std::string& name, int defaultValue) const
	{
		std::string value;
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it == m_values.end())
			return defaultValue;
		value = it->second;
		return atoi(value.c_str());
	}

	// 0x475A40
	float Properties::getFloat(const std::string& name, float defaultValue) const
	{
		std::string value;
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it == m_values.end())
			return defaultValue;
		value = it->second;
		return (float)atof(value.c_str());
	}

	// 0x475B20: "( min , max )", or one number for both
	Range Properties::getRange(const std::string& name, const Range& defaultValue) const
	{
		std::string value;
		Range range = defaultValue;
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it != m_values.end())
		{
			value = it->second;
			if (value.c_str()[0] == '(')
			{
				sscanf(value.c_str(), "( %f , %f )", &range.min, &range.max);
			}
			else
			{
				sscanf(value.c_str(), "%f", &range.min);
				range.max = range.min;
			}
		}
		return range;
	}

	// 0x475C50: "( min , max )", or one number for both
	IntRange Properties::getIntRange(const std::string& name, const IntRange& defaultValue) const
	{
		std::string value;
		IntRange range = defaultValue;
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it != m_values.end())
		{
			value = it->second;
			if (value.c_str()[0] == '(')
			{
				sscanf(value.c_str(), "( %d , %d )", &range.min, &range.max);
			}
			else
			{
				sscanf(value.c_str(), "%d", &range.min);
				range.max = range.min;
			}
		}
		return range;
	}

	// 0x475D80: "minutes:seconds" or seconds
	float Properties::getTime(const std::string& name, float defaultValue) const
	{
		std::string value;
		float result = defaultValue;
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it != m_values.end())
		{
			value = it->second;
			int minutes;
			float seconds;
			if (sscanf(value.c_str(), "%d:%f", &minutes, &seconds) == 2)
			{
				result = minutes * 60 + seconds;
			}
			else
			{
				sscanf(value.c_str(), "%f", &seconds);
				result = seconds;
			}
		}
		return result;
	}

	// 0x475EA0: the outer value is never used (the found branch declares its own)
	bool Properties::getBool(const std::string& name, bool defaultValue) const
	{
		std::string value;
		bool result = defaultValue;
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it != m_values.end())
		{
			std::string value = it->second;
			if (value == "true")
				result = true;
			else if (value == "false")
				result = false;
		}
		return result;
	}

	// 0x475FA0
	Vector2 Properties::getPoint(const std::string& name, const Vector2& defaultValue) const
	{
		std::string value;
		Vector2 point(defaultValue);
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it != m_values.end())
		{
			float x = defaultValue.x;
			float y = defaultValue.y;
			value = it->second;
			sscanf(value.c_str(), " %f , %f ", &x, &y);
			point.x = x;
			point.y = y;
		}
		return point;
	}

	// 0x4760A0
	Point Properties::getIntPoint(const std::string& name, const Point& defaultValue) const
	{
		std::string value;
		Point point(defaultValue);
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it != m_values.end())
		{
			value = it->second;
			int x;
			int y;
			sscanf(value.c_str(), " %d , %d ", &x, &y);
			point.x = x;
			point.y = y;
		}
		return point;
	}

	// 0x4761A0: " r , g , b , a " (0..255); three values keep the alpha
	Color Properties::getColor(const std::string& name, const Color& defaultValue) const
	{
		std::string value;
		Color color(defaultValue);
		std::map<std::string, std::string>::const_iterator it = m_values.find(name);
		if (it != m_values.end())
		{
			value = it->second;
			int r;
			int g;
			int b;
			int a;
			int count = sscanf(value.c_str(), " %d , %d , %d , %d ", &r, &g, &b, &a);
			if (count >= 3)
			{
				if (count == 4)
					color.set(r, g, b, a);
				else
					color.set(r, g, b);
			}
		}
		return color;
	}

	// 0x476540
	void Properties::clear()
	{
		m_values.clear();
	}

	// 0x4769B0
	void Properties::setString(const std::string& name, const std::string& value)
	{
		m_values[name] = value;
	}

	// 0x4769D0
	void Properties::setInt(const std::string& name, int value)
	{
		std::string text;
		format(text, "%d", value);
		m_values[name] = text;
	}

	// 0x476A80
	void Properties::setFloat(const std::string& name, float value)
	{
		std::string text;
		format(text, "%f", value);
		m_values[name] = text;
	}

	// 0x476B30
	void Properties::setBool(const std::string& name, bool value)
	{
		std::string text;
		format(text, "%s", value ? "true" : "false");
		m_values[name] = text;
	}
}
