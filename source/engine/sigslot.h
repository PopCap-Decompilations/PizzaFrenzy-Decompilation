// sigslot: Sarah Thompson's signal/slot library (sigslot.h 1.0.0, public domain) as this game has it: the
// single_threaded policy only (no virtual functions: the signal vtables hold only slot_disconnect and
// slot_duplicate, has_slots' only its destructor), the arities 0 to 2, and three changes to the stock library:
// - has_slots keeps its senders in a std::multiset (connect inserts without a uniqueness test, 0x406750), and
//   signal_disconnect erases one entry (find + erase(iterator), inlined in 0x462540 and 0x4715F0);
// - slot_disconnect deletes each connection before erasing it (stock 1.0.0 leaves "delete *it" commented out);
// - emit saves the next iterator before each call (a slot may disconnect itself).
// The binary's functions are this template's instantiations: the address lists above the members name them
// (identical instantiations were folded by /OPT:ICF; the folded vtables 0x5012B0/0x5012B8 serve several signals).
#pragma once

#include <list>
#include <set>

namespace sigslot
{
	class single_threaded
	{
	public:
		void lock()
		{
		}

		void unlock()
		{
		}
	};

	template<class mt_policy>
	class lock_block
	{
	public:
		mt_policy* m_mutex;

		lock_block(mt_policy* mtx)
			: m_mutex(mtx)
		{
			m_mutex->lock();
		}

		~lock_block()
		{
			m_mutex->unlock();
		}
	};

	template<class mt_policy>
	class has_slots;

	// Connection interfaces: vtable {getdest, emit, clone, duplicate}, no virtual destructor (a connection is
	// deleted with a plain operator delete).
	template<class mt_policy>
	class _connection_base0
	{
	public:
		virtual has_slots<mt_policy>* getdest() const = 0;
		virtual void emit() = 0;
		virtual _connection_base0* clone() = 0;
		virtual _connection_base0* duplicate(has_slots<mt_policy>* pnewdest) = 0;
	};

	template<class arg1_type, class mt_policy>
	class _connection_base1
	{
	public:
		virtual has_slots<mt_policy>* getdest() const = 0;
		virtual void emit(arg1_type) = 0;
		virtual _connection_base1<arg1_type, mt_policy>* clone() = 0;
		virtual _connection_base1<arg1_type, mt_policy>* duplicate(has_slots<mt_policy>* pnewdest) = 0;
	};

	template<class arg1_type, class arg2_type, class mt_policy>
	class _connection_base2
	{
	public:
		virtual has_slots<mt_policy>* getdest() const = 0;
		virtual void emit(arg1_type, arg2_type) = 0;
		virtual _connection_base2<arg1_type, arg2_type, mt_policy>* clone() = 0;
		virtual _connection_base2<arg1_type, arg2_type, mt_policy>* duplicate(has_slots<mt_policy>* pnewdest) = 0;
	};

	template<class mt_policy>
	class _signal_base : public mt_policy
	{
	public:
		virtual void slot_disconnect(has_slots<mt_policy>* pslot) = 0;
		virtual void slot_duplicate(const has_slots<mt_policy>* poldslot, has_slots<mt_policy>* pnewslot) = 0;
	};

	// Base of every signal receiver (vtable 0x4F9A14 {deleting destructor}): the signals it is connected to.
	// 0x408230 has_slots<single_threaded> scalar deleting destructor (compiler-generated)
	template<class mt_policy = single_threaded>
	class has_slots : public mt_policy
	{
	private:
		typedef std::multiset<_signal_base<mt_policy>*> sender_set;
		typedef typename sender_set::const_iterator const_iterator;

	public:
		has_slots()
		{
			;
		}

		has_slots(const has_slots& hs)
			: mt_policy(hs)
		{
			lock_block<mt_policy> lock(this);
			const_iterator it = hs.m_senders.begin();
			const_iterator itEnd = hs.m_senders.end();

			while (it != itEnd)
			{
				(*it)->slot_duplicate(&hs, this);
				m_senders.insert(*it);
				++it;
			}
		}

		void signal_connect(_signal_base<mt_policy>* sender)
		{
			lock_block<mt_policy> lock(this);
			m_senders.insert(sender);
		}

		void signal_disconnect(_signal_base<mt_policy>* sender)
		{
			lock_block<mt_policy> lock(this);
			typename sender_set::iterator it = m_senders.find(sender);

			if (it != m_senders.end())
				m_senders.erase(it);
		}

		// 0x4081B0 has_slots<single_threaded>
		virtual ~has_slots()
		{
			disconnect_all();
		}

		// 0x405F10 has_slots<single_threaded>
		void disconnect_all()
		{
			lock_block<mt_policy> lock(this);
			const_iterator it = m_senders.begin();
			const_iterator itEnd = m_senders.end();

			while (it != itEnd)
			{
				(*it)->slot_disconnect(this);
				++it;
			}

			m_senders.erase(m_senders.begin(), m_senders.end());
		}

	private:
		sender_set m_senders;
	};

	// ---- arity 0 -------------------------------------------------------------------------------------------------

	template<class mt_policy>
	class _signal_base0 : public _signal_base<mt_policy>
	{
	public:
		typedef std::list<_connection_base0<mt_policy>*> connections_list;

		_signal_base0()
		{
			;
		}

		_signal_base0(const _signal_base0& s)
			: _signal_base<mt_policy>(s)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator it = s.m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = s.m_connected_slots.end();

			while (it != itEnd)
			{
				(*it)->getdest()->signal_connect(this);
				m_connected_slots.push_back((*it)->clone());

				++it;
			}
		}

		// 0x4625D0 _signal_base0<single_threaded>
		~_signal_base0()
		{
			disconnect_all();
		}

		// the one folded body of every signal's disconnect_all is listed under _signal_base1
		void disconnect_all()
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				(*it)->getdest()->signal_disconnect(this);
				delete *it;

				++it;
			}

			m_connected_slots.erase(m_connected_slots.begin(), m_connected_slots.end());
		}

		void disconnect(has_slots<mt_policy>* pclass)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				if ((*it)->getdest() == pclass)
				{
					delete *it;
					m_connected_slots.erase(it);
					pclass->signal_disconnect(this);
					return;
				}

				++it;
			}
		}

		void slot_disconnect(has_slots<mt_policy>* pslot)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				typename connections_list::iterator itNext = it;
				++itNext;

				if ((*it)->getdest() == pslot)
				{
					delete *it;
					m_connected_slots.erase(it);
				}

				it = itNext;
			}
		}

		void slot_duplicate(const has_slots<mt_policy>* oldtarget, has_slots<mt_policy>* newtarget)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				if ((*it)->getdest() == oldtarget)
				{
					m_connected_slots.push_back((*it)->duplicate(newtarget));
				}

				++it;
			}
		}

	protected:
		connections_list m_connected_slots;
	};

	template<class dest_type, class mt_policy>
	class _connection0 : public _connection_base0<mt_policy>
	{
	public:
		_connection0()
		{
			m_pobject = 0;
			m_pmemfun = 0;
		}

		_connection0(dest_type* pobject, void (dest_type::*pmemfun)())
		{
			m_pobject = pobject;
			m_pmemfun = pmemfun;
		}

		// 0x4032D0 _connection0<PizzaFrenzy>
		// 0x4279F0 _connection0<UserProgress>
		// 0x48F240 _connection0<Table>
		// 0x491990 _connection0<ListBox>
		// 0x4BD180 _connection0<Win32Application>
		// 0x4CD150 _connection0<DDrawDisplay>
		virtual _connection_base0<mt_policy>* clone()
		{
			return new _connection0<dest_type, mt_policy>(*this);
		}

		// 0x401C20 _connection0<PizzaFrenzy>
		// 0x4279A0 _connection0<UserProgress>
		// 0x48F170 _connection0<Table>
		// 0x4917D0 _connection0<ListBox>
		// 0x4BD0D0 _connection0<Win32Application>
		// 0x4CD070 _connection0<DDrawDisplay>
		virtual _connection_base0<mt_policy>* duplicate(has_slots<mt_policy>* pnewdest)
		{
			return new _connection0<dest_type, mt_policy>((dest_type*)pnewdest, m_pmemfun);
		}

		// 0x48F150 _connection0<T> (folded)
		// 0x4BD0B0 _connection0<Win32Application>
		virtual void emit()
		{
			(m_pobject->*m_pmemfun)();
		}

		// 0x4917C0 _connection0<ListBox> (folded)
		// 0x4BD0C0 _connection0<Win32Application>
		// 0x4CD0C0 _connection0<DDrawDisplay> (folded)
		virtual has_slots<mt_policy>* getdest() const
		{
			return m_pobject;
		}

	private:
		dest_type* m_pobject;
		void (dest_type::*m_pmemfun)();
	};

	// 0x4628A0 signal0<> implicit destructor (jmp ~_signal_base0)
	template<class mt_policy = single_threaded>
	class signal0 : public _signal_base0<mt_policy>
	{
	public:
		typedef _signal_base0<mt_policy> base;
		typedef typename base::connections_list connections_list;
		using base::m_connected_slots;

		signal0()
		{
			;
		}

		signal0(const signal0<mt_policy>& s)
			: _signal_base0<mt_policy>(s)
		{
			;
		}

		// 0x408830 signal0<>::connect<PizzaFrenzy>
		// 0x428720 signal0<>::connect<UserProgress>
		// 0x48FCF0 signal0<>::connect<Table>
		// 0x491E00 signal0<>::connect<ListBox>
		// 0x4BE4E0 signal0<>::connect<Win32Application>
		// 0x4CDAC0 signal0<>::connect<DDrawDisplay>
		template<class desttype>
		void connect(desttype* pclass, void (desttype::*pmemfun)())
		{
			lock_block<mt_policy> lock(this);
			_connection0<desttype, mt_policy>* conn = new _connection0<desttype, mt_policy>(pclass, pmemfun);
			m_connected_slots.push_back(conn);
			pclass->signal_connect(this);
		}

		// 0x469E80 signal0<>
		void emit()
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator itNext, it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				itNext = it;
				++itNext;

				(*it)->emit();

				it = itNext;
			}
		}

		void operator()()
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator itNext, it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				itNext = it;
				++itNext;

				(*it)->emit();

				it = itNext;
			}
		}
	};

	// ---- arity 1 -------------------------------------------------------------------------------------------------

	template<class arg1_type, class mt_policy>
	class _signal_base1 : public _signal_base<mt_policy>
	{
	public:
		typedef std::list<_connection_base1<arg1_type, mt_policy>*> connections_list;

		_signal_base1()
		{
			;
		}

		_signal_base1(const _signal_base1<arg1_type, mt_policy>& s)
			: _signal_base<mt_policy>(s)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator it = s.m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = s.m_connected_slots.end();

			while (it != itEnd)
			{
				(*it)->getdest()->signal_connect(this);
				m_connected_slots.push_back((*it)->clone());

				++it;
			}
		}

		// 0x4644F0 _signal_base1<A> (folded: vtable 0x5012B0, also signal0<>)
		// 0x463100 _signal_base1<A> (folded: vtable 0x5012B8, also signal2<int, int>)
		// 0x463150 _signal_base1<const Point&>
		// 0x468810 _signal_base1<const MouseEvent&>
		void slot_duplicate(const has_slots<mt_policy>* oldtarget, has_slots<mt_policy>* newtarget)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				if ((*it)->getdest() == oldtarget)
				{
					m_connected_slots.push_back((*it)->duplicate(newtarget));
				}

				++it;
			}
		}

		// 0x462930 _signal_base1<UpdateContext&>
		// 0x462990 _signal_base1<Graphics&>
		// 0x4629F0 _signal_base1<bool>
		// 0x462A50 _signal_base1<int>
		// 0x462B10 _signal_base1<char>
		// 0x462B70 _signal_base1<const Point&>
		// 0x462BD0 _signal_base1<?> (Application's unused +0x114 signal)
		// 0x4643A0 _signal_base1<const std::string&>
		// 0x4687B0 _signal_base1<const MouseEvent&>
		~_signal_base1()
		{
			disconnect_all();
		}

		// 0x462540 _signal_base1<A> (folded: one body for every signal)
		void disconnect_all()
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				(*it)->getdest()->signal_disconnect(this);
				delete *it;

				++it;
			}

			m_connected_slots.erase(m_connected_slots.begin(), m_connected_slots.end());
		}

		// 0x4715F0 signal1::disconnect (folded: one body for every signal)
		void disconnect(has_slots<mt_policy>* pclass)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				if ((*it)->getdest() == pclass)
				{
					delete *it;
					m_connected_slots.erase(it);
					pclass->signal_disconnect(this);
					return;
				}

				++it;
			}
		}

		// 0x461E70 _signal_base1<A> (folded: one body for every signal)
		void slot_disconnect(has_slots<mt_policy>* pslot)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				typename connections_list::iterator itNext = it;
				++itNext;

				if ((*it)->getdest() == pslot)
				{
					delete *it;
					m_connected_slots.erase(it);
				}

				it = itNext;
			}
		}

	protected:
		connections_list m_connected_slots;
	};

	template<class dest_type, class arg1_type, class mt_policy>
	class _connection1 : public _connection_base1<arg1_type, mt_policy>
	{
	public:
		_connection1()
		{
			m_pobject = 0;
			m_pmemfun = 0;
		}

		_connection1(dest_type* pobject, void (dest_type::*pmemfun)(arg1_type))
		{
			m_pobject = pobject;
			m_pmemfun = pmemfun;
		}

		// 0x403290 _connection1<PizzaFrenzy, const std::string&>
		// 0x403310 _connection1<PizzaFrenzy, bool>
		// 0x403350 _connection1<PizzaFrenzy, UpdateContext&>
		// 0x403390 _connection1<PizzaFrenzy, int>
		// 0x40E200 _connection1<PizzaEditor, const std::string&>
		// 0x40E240 _connection1<PizzaEditor, const Point&>
		// 0x42D750 _connection1<Tip>
		// 0x437010 _connection1<HighScoreScreen, const std::string&>
		// 0x43B4C0 _connection1<NewToppingScreen, const std::string&>
		// 0x443D40 _connection1<ToppingBookScreen, const std::string&>
		// 0x445F90 _connection1<ToppingSelectionScreen, const std::string&>
		// 0x445FD0 _connection1<ToppingSelectionScreen, const Point&>
		// 0x4494F0 _connection1<MovieStarEventPopup, void*>
		// 0x44A9C0 _connection1<SpecialEventPopup, int>
		// 0x456870 _connection1<GameLogic>
		// 0x4568B0 _connection1<GameLogic, int>
		// 0x4568F0 _connection1<LevelIntroAction>
		// 0x45B4B0 _connection1<ConcentrationGameLogic>
		// 0x45C250 _connection1<DecoratePizzaGame>
		// 0x464800 _connection1<ScreenLayout, int>
		// 0x464840 _connection1<ScreenLayout, const std::string&>
		// 0x466650 _connection1<OptionsScreen, const std::string&>
		// 0x471400 _connection1<Scene, UpdateContext&>
		// 0x471440 _connection1<Scene, Graphics&>
		// 0x471480 _connection1<Scene, const Point&>
		// 0x472050 _connection1<EditBox, int>
		// 0x4720D0 _connection1<EditBox, char>
		// 0x4733E0 _connection1<UserSelectScreen, const std::string&>
		// 0x473420 _connection1<UserSelectScreen, int>
		// 0x4CD190 _connection1<DDrawDisplay, bool>
		virtual _connection_base1<arg1_type, mt_policy>* clone()
		{
			return new _connection1<dest_type, arg1_type, mt_policy>(*this);
		}

		// 0x401BD0 _connection1<PizzaFrenzy, const std::string&>
		// 0x401C70 _connection1<PizzaFrenzy, bool>
		// 0x401CC0 _connection1<PizzaFrenzy, UpdateContext&>
		// 0x401D10 _connection1<PizzaFrenzy, int>
		// 0x40E0D0 _connection1<PizzaEditor, const std::string&>
		// 0x40E120 _connection1<PizzaEditor, const Point&>
		// 0x42D550 _connection1<Tip>
		// 0x436D60 _connection1<HighScoreScreen, const std::string&>
		// 0x43B470 _connection1<NewToppingScreen, const std::string&>
		// 0x443C60 _connection1<ToppingBookScreen, const std::string&>
		// 0x445E00 _connection1<ToppingSelectionScreen, const std::string&>
		// 0x445E80 _connection1<ToppingSelectionScreen, const Point&>
		// 0x4494A0 _connection1<MovieStarEventPopup, void*>
		// 0x44A8E0 _connection1<SpecialEventPopup, int>
		// 0x455ED0 _connection1<GameLogic>
		// 0x455F30 _connection1<GameLogic, int>
		// 0x455F90 _connection1<LevelIntroAction>
		// 0x45B460 _connection1<ConcentrationGameLogic>
		// 0x45BCD0 _connection1<DecoratePizzaGame>
		// 0x464760 _connection1<ScreenLayout, int>
		// 0x4647B0 _connection1<ScreenLayout, const std::string&>
		// 0x466600 _connection1<OptionsScreen, const std::string&>
		// 0x471140 _connection1<Scene, UpdateContext&>
		// 0x471190 _connection1<Scene, Graphics&>
		// 0x4711E0 _connection1<Scene, const Point&>
		// 0x471C90 _connection1<EditBox, int>
		// 0x471D60 _connection1<EditBox, char>
		// 0x473340 _connection1<UserSelectScreen, const std::string&>
		// 0x473390 _connection1<UserSelectScreen, int>
		// 0x4CD0D0 _connection1<DDrawDisplay, bool>
		virtual _connection_base1<arg1_type, mt_policy>* duplicate(has_slots<mt_policy>* pnewdest)
		{
			return new _connection1<dest_type, arg1_type, mt_policy>((dest_type*)pnewdest, m_pmemfun);
		}

		// 0x445E50 _connection1<T, A> (folded)
		virtual void emit(arg1_type a1)
		{
			(m_pobject->*m_pmemfun)(a1);
		}

		// 0x42D540 _connection1<Tip> (folded)
		// 0x473330 _connection1<T, A> (folded)
		virtual has_slots<mt_policy>* getdest() const
		{
			return m_pobject;
		}

	private:
		dest_type* m_pobject;
		void (dest_type::*m_pmemfun)(arg1_type);
	};

	// implicit destructors (jmp ~_signal_base1):
	// 0x4631A0 signal1<UpdateContext&>
	// 0x4631B0 signal1<Graphics&>
	// 0x4631C0 signal1<bool>
	// 0x4631D0 signal1<int>
	// 0x4631F0 signal1<char>
	// 0x463200 signal1<const Point&>
	// 0x463210 signal1<?> (Application's unused +0x114 signal)
	// 0x464400 signal1<const std::string&>
	// 0x468940 signal1<const MouseEvent&>
	template<class arg1_type, class mt_policy = single_threaded>
	class signal1 : public _signal_base1<arg1_type, mt_policy>
	{
	public:
		typedef _signal_base1<arg1_type, mt_policy> base;
		typedef typename base::connections_list connections_list;
		using base::m_connected_slots;

		signal1()
		{
			;
		}

		signal1(const signal1<arg1_type, mt_policy>& s)
			: _signal_base1<arg1_type, mt_policy>(s)
		{
			;
		}

		// 0x408790 signal1<const std::string&>::connect<PizzaFrenzy>
		// 0x4088D0 signal1<bool>::connect<PizzaFrenzy>
		// 0x408970 signal1<UpdateContext&>::connect<PizzaFrenzy>
		// 0x408A10 signal1<int>::connect<PizzaFrenzy>
		// 0x40F1C0 signal1<const std::string&>::connect<PizzaEditor>
		// 0x40F260 signal1<const Point&>::connect<PizzaEditor>
		// 0x42D790 signal1::connect<Tip>
		// 0x437970 signal1<const std::string&>::connect<HighScoreScreen>
		// 0x43BD90 signal1<const std::string&>::connect<NewToppingScreen>
		// 0x444A00 signal1<const std::string&>::connect<ToppingBookScreen>
		// 0x446DF0 signal1<const std::string&>::connect<ToppingSelectionScreen>
		// 0x446E90 signal1<const Point&>::connect<ToppingSelectionScreen>
		// 0x4495B0 signal1<void*>::connect<MovieStarEventPopup>
		// 0x44B060 signal1<int>::connect<SpecialEventPopup>
		// 0x459330 signal1::connect<GameLogic>
		// 0x4593D0 signal1::connect<GameLogic>
		// 0x459470 signal1::connect<LevelIntroAction>
		// 0x45BB30 signal1::connect<ConcentrationGameLogic>
		// 0x45DF90 signal1<const Point&>::connect<DecoratePizzaGame>
		// 0x465BD0 signal1<int>::connect<ScreenLayout>
		// 0x465C70 signal1<const std::string&>::connect<ScreenLayout>
		// 0x466930 signal1<const std::string&>::connect<OptionsScreen>
		// 0x471860 signal1<UpdateContext&>::connect<Scene>
		// 0x471900 signal1<Graphics&>::connect<Scene>
		// 0x4719A0 signal1<const Point&>::connect<Scene>
		// 0x472CC0 signal1<int>::connect<EditBox>
		// 0x472E00 signal1<char>::connect<EditBox>
		// 0x473760 signal1<const std::string&>::connect<UserSelectScreen>
		// 0x473800 signal1<int>::connect<UserSelectScreen>
		// 0x4CDB60 signal1<bool>::connect<DDrawDisplay>
		template<class desttype>
		void connect(desttype* pclass, void (desttype::*pmemfun)(arg1_type))
		{
			lock_block<mt_policy> lock(this);
			_connection1<desttype, arg1_type, mt_policy>* conn =
				new _connection1<desttype, arg1_type, mt_policy>(pclass, pmemfun);
			m_connected_slots.push_back(conn);
			pclass->signal_connect(this);
		}

		// 0x4BD1E0 signal1<A> (folded: one body for the 4-byte arguments of the window procedure's signals)
		void emit(arg1_type a1)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator itNext, it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				itNext = it;
				++itNext;

				(*it)->emit(a1);

				it = itNext;
			}
		}

		void operator()(arg1_type a1)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator itNext, it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				itNext = it;
				++itNext;

				(*it)->emit(a1);

				it = itNext;
			}
		}
	};

	// ---- arity 2 -------------------------------------------------------------------------------------------------

	template<class arg1_type, class arg2_type, class mt_policy>
	class _signal_base2 : public _signal_base<mt_policy>
	{
	public:
		typedef std::list<_connection_base2<arg1_type, arg2_type, mt_policy>*> connections_list;

		_signal_base2()
		{
			;
		}

		_signal_base2(const _signal_base2<arg1_type, arg2_type, mt_policy>& s)
			: _signal_base<mt_policy>(s)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator it = s.m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = s.m_connected_slots.end();

			while (it != itEnd)
			{
				(*it)->getdest()->signal_connect(this);
				m_connected_slots.push_back((*it)->clone());

				++it;
			}
		}

		void slot_duplicate(const has_slots<mt_policy>* oldtarget, has_slots<mt_policy>* newtarget)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				if ((*it)->getdest() == oldtarget)
				{
					m_connected_slots.push_back((*it)->duplicate(newtarget));
				}

				++it;
			}
		}

		// 0x462AB0 _signal_base2<int, int>
		~_signal_base2()
		{
			disconnect_all();
		}

		void disconnect_all()
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				(*it)->getdest()->signal_disconnect(this);
				delete *it;

				++it;
			}

			m_connected_slots.erase(m_connected_slots.begin(), m_connected_slots.end());
		}

		void disconnect(has_slots<mt_policy>* pclass)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				if ((*it)->getdest() == pclass)
				{
					delete *it;
					m_connected_slots.erase(it);
					pclass->signal_disconnect(this);
					return;
				}

				++it;
			}
		}

		void slot_disconnect(has_slots<mt_policy>* pslot)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::iterator it = m_connected_slots.begin();
			typename connections_list::iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				typename connections_list::iterator itNext = it;
				++itNext;

				if ((*it)->getdest() == pslot)
				{
					delete *it;
					m_connected_slots.erase(it);
				}

				it = itNext;
			}
		}

	protected:
		connections_list m_connected_slots;
	};

	template<class dest_type, class arg1_type, class arg2_type, class mt_policy>
	class _connection2 : public _connection_base2<arg1_type, arg2_type, mt_policy>
	{
	public:
		_connection2()
		{
			m_pobject = 0;
			m_pmemfun = 0;
		}

		_connection2(dest_type* pobject, void (dest_type::*pmemfun)(arg1_type, arg2_type))
		{
			m_pobject = pobject;
			m_pmemfun = pmemfun;
		}

		// 0x472090 _connection2<EditBox, int, int>
		virtual _connection_base2<arg1_type, arg2_type, mt_policy>* clone()
		{
			return new _connection2<dest_type, arg1_type, arg2_type, mt_policy>(*this);
		}

		// 0x471D10 _connection2<EditBox, int, int>
		virtual _connection_base2<arg1_type, arg2_type, mt_policy>* duplicate(has_slots<mt_policy>* pnewdest)
		{
			return new _connection2<dest_type, arg1_type, arg2_type, mt_policy>((dest_type*)pnewdest, m_pmemfun);
		}

		// 0x471CE0 _connection2<EditBox, int, int>
		virtual void emit(arg1_type a1, arg2_type a2)
		{
			(m_pobject->*m_pmemfun)(a1, a2);
		}

		virtual has_slots<mt_policy>* getdest() const
		{
			return m_pobject;
		}

	private:
		dest_type* m_pobject;
		void (dest_type::*m_pmemfun)(arg1_type, arg2_type);
	};

	// 0x4631E0 signal2<int, int> implicit destructor (jmp ~_signal_base2)
	template<class arg1_type, class arg2_type, class mt_policy = single_threaded>
	class signal2 : public _signal_base2<arg1_type, arg2_type, mt_policy>
	{
	public:
		typedef _signal_base2<arg1_type, arg2_type, mt_policy> base;
		typedef typename base::connections_list connections_list;
		using base::m_connected_slots;

		signal2()
		{
			;
		}

		signal2(const signal2<arg1_type, arg2_type, mt_policy>& s)
			: _signal_base2<arg1_type, arg2_type, mt_policy>(s)
		{
			;
		}

		// 0x472D60 signal2<int, int>::connect<EditBox>
		template<class desttype>
		void connect(desttype* pclass, void (desttype::*pmemfun)(arg1_type, arg2_type))
		{
			lock_block<mt_policy> lock(this);
			_connection2<desttype, arg1_type, arg2_type, mt_policy>* conn =
				new _connection2<desttype, arg1_type, arg2_type, mt_policy>(pclass, pmemfun);
			m_connected_slots.push_back(conn);
			pclass->signal_connect(this);
		}

		// 0x4BD1B0 signal2<int, int>
		void emit(arg1_type a1, arg2_type a2)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator itNext, it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				itNext = it;
				++itNext;

				(*it)->emit(a1, a2);

				it = itNext;
			}
		}

		void operator()(arg1_type a1, arg2_type a2)
		{
			lock_block<mt_policy> lock(this);
			typename connections_list::const_iterator itNext, it = m_connected_slots.begin();
			typename connections_list::const_iterator itEnd = m_connected_slots.end();

			while (it != itEnd)
			{
				itNext = it;
				++itNext;

				(*it)->emit(a1, a2);

				it = itNext;
			}
		}
	};
}
