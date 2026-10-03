// engine::RefPtr<T>: holds a counted reference to an engine::Object (addRef/release through the Interface
// subobject). Its members are inlined almost everywhere; the out-of-line copies in the original are the
// constructors (0x411B70, 0x494830), the destructor (0x401160), reset (0x401180) and the two assignments
// (0x4017C0, 0x4014A0). The constructors that take a pointer start empty and assign, which is why the original
// tests the (null) old pointer before releasing it.
#pragma once

namespace engine
{
	template <class T>
	class RefPtr
	{
	public:
		// 0x411B70 (the copy the eh vector constructor iterator calls)
		RefPtr()
			: m_ptr(0)
		{
		}

		// 0x494830 (folded)
		RefPtr(T* ptr)
			: m_ptr(0)
		{
			*this = ptr;
		}

		RefPtr(const RefPtr& other)
			: m_ptr(0)
		{
			*this = other.m_ptr;
		}

		// 0x401160
		~RefPtr()
		{
			if (m_ptr)
				m_ptr->release();
		}

		// 0x401180: the new object is referenced before the old one is released
		void reset(T* ptr)
		{
			if (ptr)
				ptr->addRef();
			if (m_ptr)
				m_ptr->release();
			m_ptr = ptr;
		}

		// 0x4017C0
		T* operator=(T* ptr)
		{
			if (ptr)
				ptr->addRef();
			if (m_ptr)
				m_ptr->release();
			m_ptr = ptr;
			return ptr;
		}

		// 0x4014A0
		RefPtr& operator=(const RefPtr& other)
		{
			T* ptr = other.m_ptr;
			if (ptr)
				ptr->addRef();
			if (m_ptr)
				m_ptr->release();
			m_ptr = ptr;
			return *this;
		}

		T* operator->() const
		{
			return m_ptr;
		}

		T& operator*() const
		{
			return *m_ptr;
		}

		operator T*() const
		{
			return m_ptr;
		}

		T* get() const
		{
			return m_ptr;
		}

	private:
		T* m_ptr;
	};
}
