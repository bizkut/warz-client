/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_KyArray_H
#define Navigation_KyArray_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/kernel/SF_Array.h"


namespace Kaim
{

// Gameware Navigation array generic interface
//
// **************************
//		- UPInt   GetSize()       const;
//		- bool    IsEmpty()       const;
//		- UPInt   GetCapacity()   const;
//		- UPInt   GetNumBytes()   const;
//
//		void    ClearAndRelease();
//		void    Clear();
//		void    Resize(UPInt newSize);
//
//		void    Reserve(UPInt newCapacity);
//
//		// Basic access.
//		ValueType& At(UPInt index);
//		const ValueType& At(UPInt index); const
//		ValueType ValueAt(UPInt index) const;
//		ValueType& operator [] (UPInt index);
//		const ValueType& operator [] (UPInt index) const;
//
//		// Raw pointer to the data. Use with caution!
//		const ValueType* GetDataPtr() const;
//		ValueType* GetDataPtr();
//
//		// Insert the given element at the end of the array.
//		void    PushBack(const ValueType& val);
//		template<class S>
//		void PushBackAlt(const S& val);
//
//		// Remove the last element.
//		void    PopBack(UPInt count = 1);
//		ValueType& PushDefault();
//
//		ValueType Pop();
//
//		// Access the first element.
//		ValueType&    Front();
//		const ValueType&    Front() const;
//	
//		// Access the last element.
//		ValueType&    Back();
//		const ValueType&    Back() const;
//	
//		// Array copy.  Copies the contents of a into this array.
//		const SelfType& operator = (const SelfType& a);
//
//		// Removing multiple elements from the array.
//		void    RemoveMultipleAt(UPInt index, UPInt num);
//		
//		// Removing an element from the array is an expensive operation!
//		// It compacts only after removing the last element.
//		void    RemoveAt(UPInt index);
//		// Insert the given object at the given index shifting all the elements up.
//		void    InsertAt(UPInt index, const ValueType& val = ValueType());
//		// Insert the given object at the given index shifting all the elements up.
//		void    InsertMultipleAt(UPInt index, UPInt num, const ValueType& val = ValueType());
//		// Append the given data to the array.
//		void    Append(const SelfType& other);
//		// Append the given data to the array.
//		void    Append(const ValueType other[], UPInt count);
//	
//		bool DoesContain(const ValueType& value) const;
//		void RemoveConsecutiveDuplicates();
// **************************
// // Gameware Navigation array different type
// - KyArray :
//		General purpose array for movable objects that require explicit 
//		construction/destruction. Global heap is in use.
// - KyArrayPOD
//		General purpose array for movable objects that DOES NOT require  
//		construction/destruction. Constructors and destructors are not called! 
//		Global heap is in use.
// - KyArrayLH
//		General purpose array for movable objects that require explicit 
//		construction/destruction. Local heap is in use.
// - KyArrayLH_POD
//		General purpose array for movable objects that DOES NOT require  
//		construction/destruction. Constructors and destructors are not called! 
//		Local heap is in use.
// - KyArrayDH
//		General purpose array for movable objects that require explicit 
//		construction/destruction. Dynamic heap is in use.
// - KyArrayDH_POD
//		General purpose array for movable objects that DOES NOT require  
//		construction/destruction. Constructors and destructors are not called! 
//		Dynamic heap is in use.



// ---- KyArrayDefaultPolicy ----
/// Default resize behavior for Gameware Navigation array. No minimal capacity, Granularity=4, 
/// Never Shrinking. 
typedef ArrayConstPolicy<0, 4, true> KyArrayDefaultPolicy;


// ---- KyArray ----
/// General purpose array for movable objects that require explicit 
/// construction/destruction. Global heap is in use.
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArray : public Array<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                               ValueType;
	typedef KyArray<T, SID, SizePolicy>     SelfType;
	typedef Array<T, SID, SizePolicy>       BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	KyArray() : BaseType() {}
	explicit KyArray(int size) : BaseType(size) {}
	KyArray(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }

	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};


// ---- KyArrayPOD ----
/// General purpose array for movable objects that DOES NOT require  
/// construction/destruction. Constructors and destructors are not called! 
/// Global heap is in use.
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArrayPOD : public ArrayPOD<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                               ValueType;
	typedef KyArrayPOD<T, SID, SizePolicy>  SelfType;
	typedef ArrayPOD<T, SID, SizePolicy>    BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	KyArrayPOD() : BaseType() {}
	explicit KyArrayPOD(int size) : BaseType(size) {}
	KyArrayPOD(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};

// ---- KyArrayLH ----
/// General purpose array for movable objects that require explicit 
/// construction/destruction. Local heap is in use.
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArrayLH : public ArrayLH<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                              ValueType;
	typedef KyArrayLH<T, SID, SizePolicy>  SelfType;
	typedef ArrayLH<T, SID, SizePolicy>    BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	KyArrayLH() : BaseType() {}
	explicit KyArrayLH(int size) : BaseType(size) {}
	KyArrayLH(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};

// ---- KyArrayCPP ----
// General purpose fully C++ compliant array. Can be used with non-movable data.
// Global heap is in use.
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArrayCPP : public ArrayCPP<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                               ValueType;
	typedef KyArrayCPP<T, SID, SizePolicy>  SelfType;
	typedef ArrayCPP<T, SID, SizePolicy>    BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	KyArrayCPP() : BaseType() {}
	explicit KyArrayCPP(int size) : BaseType(size) {}
	KyArrayCPP(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};

// ---- KyArrayLH_POD ----
/// General purpose array for movable objects that DOES NOT require  
/// construction/destruction. Constructors and destructors are not called! 
/// Local heap is in use.
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArrayLH_POD : public ArrayLH_POD<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                                  ValueType;
	typedef KyArrayLH_POD<T, SID, SizePolicy>  SelfType;
	typedef ArrayLH_POD<T, SID, SizePolicy>    BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	KyArrayLH_POD() : BaseType() {}
	explicit KyArrayLH_POD(int size) : BaseType(size) {}
	KyArrayLH_POD(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};

// ---- KyArrayLH_CPP ----
// General purpose fully C++ compliant array. Can be used with non-movable data.
// Local heap is in use.
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArrayLH_CPP : public ArrayLH_CPP<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                                  ValueType;
	typedef KyArrayLH_CPP<T, SID, SizePolicy>  SelfType;
	typedef ArrayLH_CPP<T, SID, SizePolicy>    BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	KyArrayLH_CPP() : BaseType() {}
	explicit KyArrayLH_CPP(int size) : BaseType(size) {}
	KyArrayLH_CPP(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};

// ---- KyArrayDH ----
/// General purpose array for movable objects that require explicit 
/// construction/destruction. Dynamic heap is in use.
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArrayDH : public ArrayDH<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                              ValueType;
	typedef KyArrayDH<T, SID, SizePolicy>  SelfType;
	typedef ArrayDH<T, SID, SizePolicy>    BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	explicit KyArrayDH(MemoryHeap* heap) : BaseType(heap) {}
	KyArrayDH(MemoryHeap* heap, int size) : BaseType(heap, size) {}
	KyArrayDH(MemoryHeap* heap, const SelfType& a) : BaseType(heap, a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};


// ---- KyArrayDH_POD ----
/// General purpose array for movable objects that DOES NOT require  
/// construction/destruction. Constructors and destructors are not called! 
/// Dynamic heap is in use.
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArrayDH_POD : public ArrayDH_POD<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                                  ValueType;
	typedef KyArrayDH_POD<T, SID, SizePolicy>  SelfType;
	typedef ArrayDH_POD<T, SID, SizePolicy>    BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	explicit KyArrayDH_POD(MemoryHeap* heap) : BaseType(heap) {}
	KyArrayDH_POD(MemoryHeap* heap, int size) : BaseType(heap, size) {}
	KyArrayDH_POD(MemoryHeap* heap, const SelfType& a) : BaseType(heap, a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};


// ---- KyArrayDH_CPP ----
// General purpose fully C++ compliant array. Can be used with non-movable data.
// Dynamic heap is in use
template<class T, int SID=Stat_Default_Mem, class SizePolicy=KyArrayDefaultPolicy>
class KyArrayDH_CPP : public ArrayDH_CPP<T, SID, SizePolicy>
{
public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef T                                ValueType;
	typedef KyArrayDH_CPP<T, SID, SizePolicy> SelfType;
	typedef ArrayDH_CPP<T, SID, SizePolicy>   BaseType;


	// ---------------------------------- Public Member Functions ----------------------------------

	explicit KyArrayDH_CPP(MemoryHeap* heap) : BaseType(heap) {}
	KyArrayDH_CPP(MemoryHeap* heap, int size) : BaseType(heap, size) {}
	KyArrayDH_CPP(MemoryHeap* heap, const SelfType& a) : BaseType(heap, a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const;
	void RemoveConsecutiveDuplicates();
};
} // namespace Kaim

#include "gwnavruntime/containers/kyarray.inl"

#endif // Navigation_KyArray_H
