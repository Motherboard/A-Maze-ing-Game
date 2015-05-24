#pragma once
//This file was copied off from the irrlicht forum, it was not made by me

// C++ Standard Library:
#include <type_traits>

// Irrlicht:
#include "IReferenceCounted.h"

namespace irr
{
	namespace extra
	{
		/**
		* @brief A smart-pointer type for reference-counted Irrlicht objects.
		**/
		template<typename IrrlichtReferenceCounted>
		class irr_ptr : std::enable_if<std::is_base_of<IReferenceCounted,
			IrrlichtReferenceCounted>::value>
		{
		public:
			typedef typename std::remove_pointer<
				IrrlichtReferenceCounted>::type element_type;
			typedef element_type* pointer;
			typedef element_type const* const_pointer;

		private:
			pointer m_ptr;

			void grab()
			{
				if (m_ptr) m_ptr->grab();
			}

			void drop()
			{
				if (m_ptr) m_ptr->drop();
			}

		public:

			// Construction and destruction.
			irr_ptr()
				: m_ptr(0)
			{ }

			explicit irr_ptr(pointer ptr)
				: m_ptr(ptr)
			{ }

			~irr_ptr()
			{
				drop();
			}

			// Copying.
			irr_ptr(irr_ptr<element_type> const& other)
				: m_ptr(other.m_ptr)
			{
				grab();
			}

			irr_ptr<element_type>& operator=(irr_ptr<element_type> const& other)
			{
				reset(other.m_ptr);
			}

			// Moving.
			irr_ptr(irr_ptr<element_type>&& other)
				: m_ptr(other.m_ptr)
			{
				other.m_ptr = 0;
			}

			irr_ptr<element_type>& operator=(irr_ptr<element_type>&& other)
			{
				reset(other.m_ptr);
				other.m_ptr = 0;
			}

			// Operations.
			void reset(pointer ptr = pointer())
			{
				if (ptr != m_ptr)
				{
					drop();
					m_ptr = ptr;
					grab();
				}
			}

			void release()
			{
				drop();
				m_ptr = 0;
			}

			void swap(irr_ptr<element_type>& other)
			{
				pointer hlp = other.m_ptr;
				other.m_ptr = m_ptr;
				m_ptr = other.m_ptr;
			}

			// Operator overloads.
			pointer get()
			{
				return m_ptr;
			}

			const_pointer get() const
			{
				return m_ptr;
			}

			element_type& operator*()
			{
				return *m_ptr;
			}

			element_type const& operator*() const
			{
				return *m_ptr;
			}

			pointer operator->()
			{
				return m_ptr;
			}

			const_pointer operator->() const
			{
				return m_ptr;
			}

			explicit operator bool() const
			{
				return (m_ptr != 0);
			}
		};
	}
}

namespace std
{
	template<class IrrlichtReferenceCounted>
	void swap(::irr::extra::irr_ptr<IrrlichtReferenceCounted>& p1,
		::irr::extra::irr_ptr<IrrlichtReferenceCounted>& p2)
	{
		p1.swap(p2);
	}
}