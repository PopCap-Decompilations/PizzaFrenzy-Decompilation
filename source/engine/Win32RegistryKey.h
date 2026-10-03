// engine::RegistryKey, the registry key interface the application's openRegistryKey/openRootRegistryKey return,
// and engine::Win32RegistryKey, its implementation over the Win32 registry (keys under "Software\").
#pragma once

#include <string>
#include <vector>

#include <windows.h>

#include "Interface.h"
#include "Object.h"

namespace engine
{
	// Abstract (ten pure virtuals, no destructor); its implicit constructor is inlined in Win32RegistryKey's.
	// {vfptr, vbptr, Interface}: 0xC bytes on its own, at +0xC in Win32RegistryKey.
	class RegistryKey : public virtual Interface
	{
	public:
		// MSVC groups overloaded virtuals and reverses their order: declared open(root) then open(parent), they
		// take slots 1 and 0.
		virtual bool open(int root, const std::string& path) = 0;										// slot 1
		virtual bool open(RegistryKey* parent, const std::string& path) = 0;							// slot 0
		virtual bool close() = 0;																		// slot 2
		virtual bool getSubKeyNames(std::vector<std::string>& names) = 0;								// slot 3
		virtual bool deleteSubKey(const std::string& name) = 0;										// slot 4
		virtual bool getValueNames(std::vector<std::string>& names) = 0;								// slot 5
		virtual bool setString(const std::string& name, const std::string& value) = 0;				// slot 6
		virtual std::string getString(const std::string& name, const std::string& defaultValue) = 0;	// slot 7
		virtual bool setInt(const std::string& name, int value) = 0;									// slot 8
		virtual int getInt(const std::string& name, int defaultValue) = 0;								// slot 9
	};

	class Win32RegistryKey : public Object, public RegistryKey
	{
	public:
		Win32RegistryKey();
		virtual ~Win32RegistryKey();

		// RegistryKey
		virtual bool open(RegistryKey* parent, const std::string& path);
		virtual bool open(int root, const std::string& path);
		virtual bool close();
		virtual bool getSubKeyNames(std::vector<std::string>& names);
		virtual bool deleteSubKey(const std::string& name);
		virtual bool getValueNames(std::vector<std::string>& names);
		virtual bool setString(const std::string& name, const std::string& value);
		virtual std::string getString(const std::string& name, const std::string& defaultValue);
		virtual bool setInt(const std::string& name, int value);
		virtual int getInt(const std::string& name, int defaultValue);

		bool openKey(HKEY parent, const std::string& path);

		HKEY m_key;								// +0x14 0 when closed
	};
}
