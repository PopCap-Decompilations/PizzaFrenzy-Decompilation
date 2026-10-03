#include "Win32RegistryKey.h"

#include "Application.h"

namespace engine
{
	static const char* s_registryRoot = "Software\\";

	// 0x4C00B0
	Win32RegistryKey::Win32RegistryKey()
		: m_key(0)
	{
	}

	// 0x4C0130
	bool Win32RegistryKey::deleteSubKey(const std::string& name)
	{
		return RegDeleteKeyA(m_key, name.c_str()) == ERROR_SUCCESS;
	}

	// 0x4C0170
	bool Win32RegistryKey::setInt(const std::string& name, int value)
	{
		if (m_key != 0 && RegSetValueExA(m_key, name.c_str(), 0, REG_DWORD, (const BYTE*)&value, sizeof(value)) == ERROR_SUCCESS)
		{
			return true;
		}
		return false;
	}

	// 0x4C01B0
	int Win32RegistryKey::getInt(const std::string& name, int defaultValue)
	{
		if (m_key != 0)
		{
			DWORD type;
			DWORD size;
			if (RegQueryValueExA(m_key, name.c_str(), 0, &type, 0, &size) == ERROR_SUCCESS && type == REG_DWORD
				&& size == sizeof(int))
			{
				int value;
				if (RegQueryValueExA(m_key, name.c_str(), 0, 0, (LPBYTE)&value, &size) == ERROR_SUCCESS)
				{
					return value;
				}
			}
		}
		return defaultValue;
	}

	// 0x4C0240
	bool Win32RegistryKey::openKey(HKEY parent, const std::string& path)
	{
		if (m_key != 0)
		{
			return false;
		}
		DWORD disposition;
		LONG result = RegCreateKeyExA(parent, path.c_str(), 0, 0, REG_OPTION_NON_VOLATILE, KEY_READ | KEY_WRITE, 0, &m_key,
			&disposition);
		if (result == ERROR_SUCCESS)
		{
			return true;
		}
		Application* application = getApplication();
		std::string message = application->getErrorMessage(result);
		application->logSystem("Failed to open registry key %s.  %s\n", path.c_str(), message.c_str());
		m_key = 0;
		return false;
	}

	// 0x4C0350
	bool Win32RegistryKey::close()
	{
		if (m_key != 0)
		{
			LONG result = RegCloseKey(m_key);
			m_key = 0;
			if (result == ERROR_SUCCESS)
			{
				return true;
			}
			// The original passes the std::string itself (a bitwise copy) through the ellipsis, not its c_str().
			Application* application = getApplication();
			application->logSystem("Failed to close registry key.  %s\n", application->getErrorMessage(result));
		}
		return false;
	}

	// 0x4C0410
	bool Win32RegistryKey::setString(const std::string& name, const std::string& value)
	{
		if (m_key != 0)
		{
			LONG result = RegSetValueExA(m_key, name.c_str(), 0, REG_SZ, (const BYTE*)value.c_str(), value.size() + 1);
			if (result == ERROR_SUCCESS)
			{
				return true;
			}
			Application* application = getApplication();
			application->logSystem("Failed to set registry value %s.  %s\n", name.c_str(),
				application->getErrorMessage(result).c_str());
		}
		return false;
	}

	// 0x4C0510
	Win32RegistryKey::~Win32RegistryKey()
	{
		close();
	}

	// 0x4C0590
	bool Win32RegistryKey::open(RegistryKey* parent, const std::string& path)
	{
		HKEY parentKey = static_cast<Win32RegistryKey*>(parent)->m_key;
		if (parentKey != 0)
		{
			return openKey(parentKey, path);
		}
		return false;
	}

	// 0x4C05E0
	bool Win32RegistryKey::open(int root, const std::string& path)
	{
		HKEY rootKey;
		switch (root)
		{
		case 0:
			rootKey = HKEY_LOCAL_MACHINE;
			break;
		case 1:
			rootKey = HKEY_CURRENT_USER;
			break;
		default:
			return false;
		}
		std::string fullPath(s_registryRoot);
		fullPath += path;
		return openKey(rootKey, fullPath);
	}

	// 0x4C06B0
	std::string Win32RegistryKey::getString(const std::string& name, const std::string& defaultValue)
	{
		std::string value = defaultValue;
		if (m_key != 0)
		{
			DWORD type;
			DWORD size;
			if (RegQueryValueExA(m_key, name.c_str(), 0, &type, 0, &size) == ERROR_SUCCESS && type == REG_SZ)
			{
				DWORD length = size + 1;
				char* buffer = new char[length];
				if (RegQueryValueExA(m_key, name.c_str(), 0, 0, (LPBYTE)buffer, &size) == ERROR_SUCCESS)
				{
					buffer[length - 1] = 0;
					value = buffer;
				}
				delete[] buffer;
			}
		}
		return value;
	}

	// 0x4C07F0
	bool Win32RegistryKey::getSubKeyNames(std::vector<std::string>& names)
	{
		DWORD index = 0;
		LONG result;
		do
		{
			char name[MAX_PATH];
			DWORD nameLength = MAX_PATH;
			FILETIME lastWriteTime;
			result = RegEnumKeyExA(m_key, index, name, &nameLength, 0, 0, 0, &lastWriteTime);
			if (result == ERROR_SUCCESS)
			{
				names.push_back(name);
			}
			++index;
		} while (result == ERROR_SUCCESS);
		return result == ERROR_NO_MORE_ITEMS;
	}

	// 0x4C0910
	bool Win32RegistryKey::getValueNames(std::vector<std::string>& names)
	{
		DWORD index = 0;
		LONG result;
		do
		{
			char name[MAX_PATH];
			DWORD nameLength = MAX_PATH;
			result = RegEnumValueA(m_key, index, name, &nameLength, 0, 0, 0, 0);
			if (result == ERROR_SUCCESS)
			{
				names.push_back(name);
			}
			++index;
		} while (result == ERROR_SUCCESS);
		return result == ERROR_NO_MORE_ITEMS;
	}
}
