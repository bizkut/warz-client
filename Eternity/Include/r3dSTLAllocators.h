//=========================================================================
//	Module: r3dSTLAllocators.h
//	Copyright (C) Online Warmongers Group Inc. 2013.
//=========================================================================

#pragma once

#include <limits>

//////////////////////////////////////////////////////////////////////////

template <typename T, int AT>
class r3dSTLCustomAllocator
{
public:
	//type definitions
	typedef size_t    size_type;
	typedef ptrdiff_t difference_type;
	typedef T*        pointer;
	typedef const T*  const_pointer;
	typedef T&        reference;
	typedef const T&  const_reference;
	typedef T         value_type;

	//rebind allocator to type U
	template <class U>
	struct rebind
	{
		typedef r3dSTLCustomAllocator<U, AT> other;
	};


	//return address of values
	pointer address (reference value) const
	{
		return &value;
	}
	const_pointer address (const_reference value) const
	{
		return &value;
	}
	/*constructors and destructor
	*-nothing to do because the allocator has no state
	*/
	r3dSTLCustomAllocator() throw() {}
	r3dSTLCustomAllocator(const r3dSTLCustomAllocator&) throw() {}
	template <class U>
	r3dSTLCustomAllocator (const r3dSTLCustomAllocator<U, AT>&) throw() {}
	~r3dSTLCustomAllocator() throw() {}

	//return maximum number of elements that can be allocated
	size_type max_size () const throw()
	{
		//for numeric_limits see Section 4.3, page 59
		return std::numeric_limits<size_t>::max() / sizeof(T);
	}

	//allocate but don't initialize num elements of type T
	pointer allocate (size_type num, typename r3dSTLCustomAllocator<void, AT>::const_pointer hint = 0)
	{
		//allocate memory with global new
		return (pointer) r3dAllocateMemory(num * sizeof(T), 1, static_cast<r3dAllocationTypes>(AT));
	}


	//initialize elements of allocated storage p with value value
	void construct (pointer p, const T& value)
	{
		//initialize memory with placement new
		new((void*)p)T(value);
	}


	//destroy elements of initialized storage p
	void destroy (pointer p)
	{
		// destroy objects by calling their destructor
		p->~T();
	}


	//deallocate storage p of deleted elements
	void deallocate (pointer p, size_type num)
	{
		//deallocate memory with global delete
		r3dDeallocateMemory(p, 1, static_cast<r3dAllocationTypes>(AT));
	}
};

//return that all specializations of this allocator are interchangeable
template <class T1, class T2, int AT>
bool operator== (const r3dSTLCustomAllocator<T1, AT>&,
				 const r3dSTLCustomAllocator<T2, AT>&) throw()
{
	return true;
}
template <class T1, class T2, int AT>
bool operator!= (const r3dSTLCustomAllocator<T1, AT>&,
				 const r3dSTLCustomAllocator<T2, AT>&) throw()
{
	return false;
}

//////////////////////////////////////////////////////////////////////////

template<int AT>
class r3dSTLCustomAllocator<void, AT>
{	// allocator for type void
public:
	typedef void _Ty;
	typedef _Ty _FARQ *pointer;
	typedef const _Ty _FARQ *const_pointer;
	typedef _Ty value_type;

	template<class _Other>
	struct rebind
	{	// convert an allocator<void> to an allocator <_Other>
		typedef r3dSTLCustomAllocator<_Other, AT> other;
	};

	r3dSTLCustomAllocator() _THROW0()
	{	// construct default allocator (do nothing)
	}

	r3dSTLCustomAllocator(const r3dSTLCustomAllocator<_Ty, AT>&) _THROW0()
	{	// construct by copying (do nothing)
	}

	template<class _Other>
	r3dSTLCustomAllocator(const r3dSTLCustomAllocator<_Other, AT>&) _THROW0()
	{	// construct from related allocator (do nothing)
	}

	template<class _Other>
	r3dSTLCustomAllocator<_Ty, AT>& operator=(const r3dSTLCustomAllocator<_Other, AT>&)
	{	// assign from a related allocator (do nothing)
		return (*this);
	}
};
