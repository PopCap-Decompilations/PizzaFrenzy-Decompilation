#include "UserManager.h"

#include <algorithm>
#include <map>
#include <string>
#include <vector>

#include "Application.h"
#include "Properties.h"
#include "StringUtil.h"
#include "User.h"
#include "Win32RegistryKey.h"

namespace engine
{
	static std::string s_activeUserKey("activeUser");	// 0x521228

	UserManager* UserManager::s_instance;

	// Inlined in the instantiations of std::lower_bound (0x46A0D0) and std::upper_bound (0x46A130).
	bool UserNameLess::operator()(User* a, User* b) const
	{
		return compareNoCase(a->getName(), b->getName()) < 0;
	}

	// Inlined in the instantiation of std::find_if (0x46A280).
	bool UserNameEquals::operator()(User* user) const
	{
		return m_name == user->getName();
	}

	// 0x469DC0 (folded)
	int UserManager::getCurrentUserIndex() const
	{
		return m_currentIndex;
	}

	// 0x469DF0
	int UserManager::getUserCount() const
	{
		return m_users.size();
	}

	// 0x469E40
	bool UserManager::close()
	{
		if (!m_key)
			return false;
		bool result = m_key->close();
		m_key = 0;
		return result;
	}

	// 0x469EB0
	void UserManager::setCurrentUser(int index)
	{
		if (index != m_currentIndex && index >= 0 && index < (int)m_users.size())
		{
			m_currentIndex = index;
			m_key->setString(s_activeUserKey, m_users[index]->getName());
			m_changedSignal.emit();
		}
	}

	// 0x469F00
	User* UserManager::getUser(int index)
	{
		if (index >= 0 && index < (int)m_users.size())
			return m_users[index];
		return 0;
	}

	// 0x469F40
	User* UserManager::getCurrentUser()
	{
		if (m_currentIndex >= 0 && m_currentIndex < (int)m_users.size())
			return m_users[m_currentIndex];
		return 0;
	}

	// 0x469F70
	bool UserManager::saveUser(User* user)
	{
		Application* app = getApplication();
		RefPtr<RegistryKey> key;
		key = app->openRegistryKey(m_key, user->getName());
		if (!key)
			return false;
		Properties& attributes = user->getAttributes();
		bool result = true;
		for (std::map<std::string, std::string>::const_iterator it = attributes.begin(); result && it != attributes.end();
			++it)
			result &= key->setString(it->first, it->second);
		return result;
	}

	// 0x46A190
	bool UserManager::removeUser(int index)
	{
		if (index < 0 || index >= (int)m_users.size())
			return false;
		User* user = m_users[index];
		if (!m_key->deleteSubKey(user->getName()))
			return false;
		m_users.erase(m_users.begin() + index);
		user->release();
		if (m_currentIndex > index)
			setCurrentUser(m_currentIndex - 1);
		else if (m_currentIndex == (int)m_users.size())
			setCurrentUser(std::max<int>(0, (int)m_users.size() - 1));
		m_changedSignal.emit();
		return true;
	}

	// 0x46A5A0
	UserManager::UserManager()
		: m_currentIndex(0)
	{
	}

	// 0x46A600
	bool UserManager::addUser(User* user)
	{
		std::vector<User*>::iterator lower = std::lower_bound(m_users.begin(), m_users.end(), user, UserNameLess());
		std::vector<User*>::iterator upper = std::upper_bound(lower, m_users.end(), user, UserNameLess());
		if (lower != upper)
			return false;
		std::vector<User*>::iterator it = m_users.insert(lower, user);
		if (saveUser(user))
		{
			user->addRef();
			m_currentIndex = it - m_users.begin();
			m_changedSignal.emit();
			return true;
		}
		m_users.erase(it);
		return false;
	}

	// 0x46A700
	UserManager* UserManager::getInstance()
	{
		if (!s_instance)
			s_instance = new UserManager();
		return s_instance;
	}

	// 0x46A770
	bool UserManager::loadUser(const std::string& name)
	{
		Application* app = getApplication();
		RefPtr<RegistryKey> key;
		key = app->openRegistryKey(m_key, name);
		if (!key)
			return false;
		RefPtr<User> user = new User();
		user->setName(name);
		Properties& attributes = user->getAttributes();
		std::vector<std::string> names;
		if (key->getValueNames(names))
		{
			for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it)
			{
				std::string value = key->getString(*it, "");
				attributes.setString(*it, value);
			}
		}
		std::vector<User*>::iterator lower = std::lower_bound(m_users.begin(), m_users.end(), user, UserNameLess());
		std::vector<User*>::iterator upper = std::upper_bound(lower, m_users.end(), user, UserNameLess());
		if (lower != upper)
			return false;
		std::vector<User*>::iterator it = m_users.insert(lower, user);
		user->addRef();
		m_currentIndex = it - m_users.begin();
		return true;
	}

	// 0x46AAE0
	bool UserManager::loadUsers()
	{
		Application* app = getApplication();	// unused
		std::vector<std::string> names;
		if (!m_key->getSubKeyNames(names))
			return false;
		for (std::vector<std::string>::iterator it = names.begin(); it != names.end(); ++it)
			loadUser(*it);
		std::string activeUser = m_key->getString(s_activeUserKey, "");
		std::vector<User*>::iterator found = std::find_if(m_users.begin(), m_users.end(), UserNameEquals(activeUser));
		if (found != m_users.end())
		{
			int index = found - m_users.begin();
			if (index >= 0 && index < (int)m_users.size())
				m_currentIndex = index;
		}
		return true;
	}

	// 0x46ACB0
	bool UserManager::open(const std::string& companyKey, const std::string& gameKey)
	{
		std::string path = companyKey;
		path += '\\';
		path += gameKey;
		path += "\\users";
		m_key = getApplication()->openRootRegistryKey(1, path);
		if (m_key)
			return loadUsers();
		return false;
	}
}
