/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


/*	Contains the definition of a system used internally to allocate memory for STL components */


// primary contact: GUAL - secondary contact: NOBODY
#ifndef GwNavGen_StlAllocator_H
#define GwNavGen_StlAllocator_H

#include <stddef.h> // size_t, ptrdiff_t
#include "gwnavgeneration/common/generatormemory.h"
#include "gwnavruntime/base/memory.h"


namespace Kaim
{


template<class T>
struct GlobalStlAllocator
{
public:
	// stl boilerplate
	typedef T           value_type;
	typedef T*          pointer;
	typedef T&          reference;
	typedef const T*    const_pointer;
	typedef const T&    const_reference;
	typedef size_t      size_type;
	typedef ptrdiff_t   difference_type;

	GlobalStlAllocator() {}
	GlobalStlAllocator(const GlobalStlAllocator<T>&) {}

	pointer       address(reference       value) const { return &value; }
	const_pointer address(const_reference value) const { return &value; }

	template<class Other> struct rebind { typedef GlobalStlAllocator<Other> other; };
	template<class Other> GlobalStlAllocator(const GlobalStlAllocator<Other>&) {}
	template<class Other> GlobalStlAllocator<T>& operator=(const GlobalStlAllocator<Other>&) { return (*this); }

	void construct(pointer ptr, const T& value) { ::new (ptr) T(value); }

	void destroy(pointer ptr) { (void)ptr; ptr->~T(); } // to avoid warning

	size_type max_size() const  { return (size_type)(KyUInt32MAXVAL) / sizeof(T); } // estimate maximum array size

public:
	// real stuff
	pointer allocate(size_type count)              { return (T*)KY_ALLOC(sizeof(T) * count, Stat_Default_Mem); }
	pointer allocate(size_type count, const void*) { return (T*)KY_ALLOC(sizeof(T) * count, Stat_Default_Mem); }
	void deallocate(pointer ptr, size_type)        { KY_FREE(ptr); }
};
// more stl boilerplate
template<class T, class Other> 
inline bool operator==(const GlobalStlAllocator<T>&, const GlobalStlAllocator<Other>&)  { return true; }
template<class T, class Other> 
inline bool operator!=(const GlobalStlAllocator<T>&, const GlobalStlAllocator<Other>&) { return false; }


template<class T>
struct TlsStlAllocator
{
public:
	// stl boilerplate
	typedef T           value_type;
	typedef T*          pointer;
	typedef T&          reference;
	typedef const T*    const_pointer;
	typedef const T&    const_reference;
	typedef size_t      size_type;
	typedef ptrdiff_t   difference_type;

	TlsStlAllocator() {}
	TlsStlAllocator(const TlsStlAllocator<T>&) {}

	pointer       address(reference       value) const { return &value; }
	const_pointer address(const_reference value) const { return &value; }

	template<class Other> struct rebind { typedef TlsStlAllocator<Other> other; };
	template<class Other> TlsStlAllocator(const TlsStlAllocator<Other>&) {}
	template<class Other> TlsStlAllocator<T>& operator=(const TlsStlAllocator<Other>&) { return (*this); }

	void construct(pointer ptr, const T& value) { ::new (ptr) T(value); }

	void destroy(pointer ptr) { (void)ptr; ptr->~T(); }

	size_type max_size() const  { return (size_type)(KyUInt32MAXVAL) / sizeof(T); } // estimate maximum array size

public:
	// real stuff
	pointer allocate(size_type count)              { return (T*)KY_HEAP_ALLOC(Kaim::GeneratorMemory::TlsHeap(), sizeof(T) * count, Stat_Default_Mem); }
	pointer allocate(size_type count, const void*) { return (T*)KY_HEAP_ALLOC(Kaim::GeneratorMemory::TlsHeap(), sizeof(T) * count, Stat_Default_Mem); }
	void deallocate(pointer ptr, size_type)        { KY_FREE(ptr); }
};
// more stl boilerplate
template<class T, class Other> 
inline bool operator==(const TlsStlAllocator<T>&, const TlsStlAllocator<Other>&)  { return true; }
template<class T, class Other> 
inline bool operator!=(const TlsStlAllocator<T>&, const TlsStlAllocator<Other>&) { return false; }


} // namespace Kaim

#endif // GwNavGen_StlAllocator_H
