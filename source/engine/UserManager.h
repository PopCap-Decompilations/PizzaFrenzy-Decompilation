// engine::UserManager: the singleton that keeps the player profiles (engine::User) in the registry, sorted by name,
// with the current one; and the name predicates its functions give the STL algorithms.
#pragma once

#include <string>
#include <vector>

#include "RefPtr.h"
#include "sigslot.h"

namespace engine
{
	class RegistryKey;
	class User;

	// No vtable, 0x28 bytes, never destroyed (no destructor is emitted). The profiles are the subkeys of
	// <company>\<game>\users; the current user's name is its "activeUser" value.
	class UserManager
	{
	public:
		UserManager();

		int getCurrentUserIndex() const;										// 0x469DC0 (folded)
		int getUserCount() const;
		bool close();															// m_key->close(), then releases it
		void setCurrentUser(int index);											// stores "activeUser", emits
		User* getUser(int index);												// 0 if out of range
		User* getCurrentUser();
		bool saveUser(User* user);												// every attribute as a string value
		bool removeUser(int index);												// deletes the subkey, fixes the index
		bool addUser(User* user);												// sorted insert of a new name, saved
		static UserManager* getInstance();										// created on first use
		bool loadUser(const std::string& name);									// a new User from the name's subkey
		bool loadUsers();														// every subkey, then "activeUser"
		bool open(const std::string& companyKey, const std::string& gameKey);	// <company>\<game>\users

		sigslot::signal0<> m_changedSignal;					// +0x00 the users or the current user changed
		RefPtr<RegistryKey> m_key;							// +0x10 <company>\<game>\users
		std::vector<User*> m_users;							// +0x14 sorted by name (compareNoCase); referenced
		int m_currentIndex;									// +0x24

		static UserManager* s_instance;						// 0x532F6C
	};

	// std::lower_bound/upper_bound order of m_users (0x46A0D0, 0x46A130): compareNoCase of the names < 0
	struct UserNameLess
	{
		bool operator()(User* a, User* b) const;
	};

	// std::find_if predicate of loadUsers (0x46A280, which gets it by value): the user whose name equals the
	// "activeUser" value
	struct UserNameEquals
	{
		UserNameEquals(const std::string& name)
			: m_name(name)
		{
		}

		bool operator()(User* user) const;

		std::string m_name;									// +0x00
	};
}
