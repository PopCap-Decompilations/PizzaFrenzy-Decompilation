// engine::Properties: a name-to-value string map (XML attributes, user profiles) with typed accessors.
#pragma once

#include <map>
#include <string>

#include "Color.h"
#include "Object.h"
#include "Point.h"
#include "Range.h"

namespace engine
{
	// Object +0x00, m_values +0x0C, then the vtordisp (+0x18) and the Interface subobject (+0x1C): 0x20 bytes.
	// The attribute list the XML parser hands to XmlHandler::startElement, the property bag of engine::User and
	// the parameters of the global-score request. The destructor and the copy constructor (0x47C080) are
	// implicit: the destructor 0x40D1E0 stores no vtables.
	class Properties : public Object
	{
	public:
		// 0x40D130 (copy emitted in another object: the constructor is inline)
		Properties()
		{
		}

		std::map<std::string, std::string>::iterator begin();	// mutable: HighScoreManager::loadEntry writes through it
		std::map<std::string, std::string>::iterator end();
		std::string getString(const std::string& name, const std::string& defaultValue) const;
		int getInt(const std::string& name, int defaultValue) const;
		float getFloat(const std::string& name, float defaultValue) const;
		Range getRange(const std::string& name, const Range& defaultValue) const;			// "( %f , %f )" or one "%f"
		IntRange getIntRange(const std::string& name, const IntRange& defaultValue) const;	// "( %d , %d )" or one "%d"
		float getTime(const std::string& name, float defaultValue) const;					// "%d:%f" (minutes:seconds) or "%f"
		bool getBool(const std::string& name, bool defaultValue) const;					// "true" / "false"
		Vector2 getPoint(const std::string& name, const Vector2& defaultValue) const;		// " %f , %f "
		Point getIntPoint(const std::string& name, const Point& defaultValue) const;		// " %d , %d "
		Color getColor(const std::string& name, const Color& defaultValue) const;			// " %d , %d , %d , %d " (RGB or RGBA, 0..255)
		void clear();
		void setString(const std::string& name, const std::string& value);
		void setInt(const std::string& name, int value);
		void setFloat(const std::string& name, float value);
		void setBool(const std::string& name, bool value);

		std::map<std::string, std::string> m_values;	// +0x0C attribute name to value text
	};
}
