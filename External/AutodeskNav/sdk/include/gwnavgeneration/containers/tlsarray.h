/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_ArrayTLS_H
#define Navigation_ArrayTLS_H

#include "gwnavruntime/containers/kyarray.h"
#include "gwnavruntime/containers/bitfield.h"
#include "gwnavgeneration/common/generatormemory.h"
#include "gwnavruntime/containers/binaryheap.h"
#include "gwnavruntime/containers/circulararray.h"


namespace Kaim
{

// ArrayTLS is a ArrayDH that always uses GeneratorMemory::TlsHeap()
// This avoid to pass GeneratorMemory::TlsHeap() to ArrayDH constructors
template<class T>
class KyArrayTLS : public KyArrayDH<T, MemStat_NavDataGen>
{
public:
	typedef T                                ValueType;
	typedef KyArrayTLS                       SelfType;
	typedef KyArrayDH<T, MemStat_NavDataGen> BaseType;

	KyArrayTLS()                                 : BaseType(GeneratorMemory::TlsHeap())       {}
	explicit KyArrayTLS(int size)                : BaseType(GeneratorMemory::TlsHeap(), size) {}
	KyArrayTLS(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const { return ArrayAlg::DoesContain<SelfType,ValueType>(*this, value); }
	void RemoveConsecutiveDuplicates() { return ArrayAlg::RemoveConsecutiveDuplicates<SelfType>(*this); }
};


// ArrayTLS_POD is a ArrayDH_POD that always uses GeneratorMemory::TlsHeap()
// This avoid to pass GeneratorMemory::TlsHeap() to ArrayDH_POD constructors
template<class T>
class KyArrayTLS_POD : public ArrayDH_POD<T, MemStat_NavDataGen>
{
public:
	typedef T                                  ValueType;
	typedef KyArrayTLS_POD                     SelfType;
	typedef ArrayDH_POD<T, MemStat_NavDataGen> BaseType;

	KyArrayTLS_POD()                        : BaseType(GeneratorMemory::TlsHeap())       {}
	explicit KyArrayTLS_POD(int size)       : BaseType(GeneratorMemory::TlsHeap(), size) {}
	KyArrayTLS_POD(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const { return ArrayAlg::DoesContain<SelfType,ValueType>(*this, value); }
	void RemoveConsecutiveDuplicates() { return ArrayAlg::RemoveConsecutiveDuplicates<SelfType>(*this); }
};

// ArrayTLS_POD is a ArrayDH_POD that always uses GeneratorMemory::TlsHeap()
// This avoid to pass GeneratorMemory::TlsHeap() to ArrayDH_POD constructors
template<class T>
class KyArrayTLS_CPP : public ArrayDH_CPP<T, MemStat_NavDataGen>
{
public:
	typedef T                                 ValueType;
	typedef KyArrayTLS_CPP                     SelfType;
	typedef ArrayDH_CPP<T, MemStat_NavDataGen> BaseType;

	KyArrayTLS_CPP()                        : BaseType(GeneratorMemory::TlsHeap())       {}
	explicit KyArrayTLS_CPP(int size)       : BaseType(GeneratorMemory::TlsHeap(), size) {}
	KyArrayTLS_CPP(const SelfType& a) : BaseType(a) {}
	const SelfType& operator=(const SelfType& a) { BaseType::operator=(a); return *this; }

	KyUInt32 GetCount() const { return (KyUInt32)BaseType::GetSize(); }
	bool DoesContain(const ValueType& value) const { return ArrayAlg::DoesContain<SelfType,ValueType>(*this, value); }
	void RemoveConsecutiveDuplicates() { return ArrayAlg::RemoveConsecutiveDuplicates<SelfType>(*this); }
};

class BitFieldTLS : public BitFieldBase
{
	KY_DEFINE_NEW_DELETE_OPERATORS(MemStat_NavDataGen)
public:
	explicit BitFieldTLS() : BitFieldBase() {}
	explicit BitFieldTLS(KyUInt32 bitsCount) : BitFieldBase() { Resize(bitsCount); }
	void Resize(KyUInt32 newBitsCount)  { Resize_(newBitsCount, MemStat_NavDataGen, GeneratorMemory::TlsHeap());  }
	void Reserve(KyUInt32 newBitsCount) { Reserve_(newBitsCount, MemStat_NavDataGen, GeneratorMemory::TlsHeap()); }
};

template <class T, typename Comparator = DefaultLess<T>, typename BinaryHeapIndexTracker = BinaryHeapIndexTracker_None<T> >
class BinaryHeapTls : public BinaryHeap<T, KyArrayTLS<T>, Comparator, BinaryHeapIndexTracker>
{
};

template <class T, typename Comparator = DefaultLess<T>, typename BinaryHeapIndexTracker = BinaryHeapIndexTracker_None<T> >
class BinaryHeapTLS_CPP : public BinaryHeap<T, KyArrayTLS_CPP<T>, Comparator, BinaryHeapIndexTracker>
{
};


} // namespace Kaim


#endif // Navigation_ArrayTLS_H
