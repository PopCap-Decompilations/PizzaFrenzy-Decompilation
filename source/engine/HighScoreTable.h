// engine::HighScoreTable: a named, ranked, size-limited list of high-score entries, downloadable on a thread.
#pragma once

#include <string>
#include <vector>

#include <windows.h>

#include "HighScoreEntry.h"
#include "Object.h"
#include "RefPtr.h"
#include "Runnable.h"

namespace engine
{
	class URLConnection;

	// Object +0x00, Runnable +0x0C (run() downloads the global table on a thread made by the application), members
	// from +0x14, then the vtordisp (+0x84) and the Interface subobject (+0x88): 0x8C bytes. The game's subclass
	// (0x4FD768) creates its entries and finds their position by score. m_lock guards m_status and m_request against
	// the download thread (the destructor does not delete it).
	class HighScoreTable : public Object, public Runnable
	{
	public:
		typedef std::vector<HighScoreEntry*>::iterator iterator;

		HighScoreTable(const std::string& name, int maxEntries);
		virtual ~HighScoreTable();

		virtual HighScoreEntry* createEntry() = 0;								// slot 1
		virtual iterator findPosition(HighScoreEntry* entry) = 0;				// slot 2: insertion point by score

		virtual int run();														// slot 0 (engine::Runnable)

		int getNewEntryIndex() const;
		void clearNewEntryIndex();
		void setStatus(int status);
		int getStatus() const;
		int getCount() const;
		bool isEmpty() const;
		void sendRequest(URLConnection* request);
		iterator begin();
		iterator end();
		HighScoreEntry* getEntry(unsigned int index) const;
		int getRankFor(HighScoreEntry* entry);
		void selectRank(int rank);
		void clear();
		void setTitle(const std::string& title);	// folded with Settings::setVisitUrl (0x4833C0); porting.csv: setUrl
		const std::string& getTitle() const;	// folded with Settings::getVisitUrl (0x479840), called by HighScoreScreen
		const std::string& getName() const;		// folded with Settings::getILink (0x479850), called by HighScoreManager
		bool insert(HighScoreEntry* entry);
		bool addEntry(HighScoreEntry* entry);

		std::string m_name;						// +0x14 mode name (e.g. "memoryMode")
		std::string m_title;					// +0x30 display title (HighScoreScreen: resource strings 205-207)
		int m_status;							// +0x4C 0 idle, 1 downloading, 2 done, 3 failed
		std::vector<HighScoreEntry*> m_entries;	// +0x50 ordered by rank; raw pointers referenced by insert (addRef) and
												// released by clear, insert's trimming and the destructor
		int m_maxEntries;						// +0x60 e.g. 10
		int m_newEntryIndex;					// +0x64 index of the entry just added, -1 (not set by the constructor)
		mutable CRITICAL_SECTION m_lock;		// +0x68 (mutable: getStatus is const)
		RefPtr<URLConnection> m_request;		// +0x80 request being downloaded (Application::openUrl)
	};

	// std::find_if predicate of HighScoreTable::selectRank (inlined in the instantiation 0x482D30).
	struct RankEquals
	{
		RankEquals(int value)
			: rank(value)
		{
		}

		bool operator()(HighScoreEntry* entry) const
		{
			return entry->getRank() == rank;
		}

		int rank;								// +0x00
	};

	// std::lower_bound comparison of HighScoreTable::insert (inlined in the instantiation 0x4831C0).
	struct ByRank
	{
		bool operator()(HighScoreEntry* a, HighScoreEntry* b) const
		{
			return a->getRank() < b->getRank();
		}
	};
}
