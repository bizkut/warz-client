/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/

namespace Kaim
{

// Class for Collection to ignore index tracking
template <typename T>
class CollectionIndexTracker_None
{
public:
	static void SetIndex(T& /*value*/, KyUInt32 /*indexInCollection*/) {}
};

// class to support index tracking
// - when PTR=C* or PTR=Ptr<C>
// - C has a method void SetIndexInCollection(KyUInt32 indexInCollection);
template <typename PTR>
class CollectionIndexTrackerPtr
{
public:
	// Called after the element has been added.
	static void SetIndex(PTR& ptr, KyUInt32 indexInCollection) { ptr->SetIndexInCollection(indexInCollection); }
};


template <class T, typename CollectionIndexTracker = CollectionIndexTracker_None<T>, int SID = Stat_Default_Mem>
class CollectionImpl
{
	KY_DEFINE_NEW_DELETE_OPERATORS(SID)

public:
	KY_INLINE T& PushBack(const T& value)
	{
		const KyUInt32 newIndex = GetCount();
		m_values.PushBack(value);
		T& valueInArray = m_values.Back();
		CollectionIndexTracker::SetIndex(valueInArray, newIndex);
		return valueInArray;
	}

	KY_INLINE T& PushBack()
	{
		const KyUInt32 newIndex = GetCount();
		m_values.PushBack();
		T& valueInArray = m_values.Back();
		CollectionIndexTracker::SetIndex(valueInArray, newIndex);
		return valueInArray;
	}

	KY_INLINE T& PushBackUnique(const T& value)
	{
		for (KyUInt32 index = 0; index < m_values.GetCount(); ++index)
		{
			if (m_values[index] == value)
				return m_values[index];
		}

		return PushBack(value);
	}

	KY_INLINE void Clear()
	{
		const KyUInt32 count = GetCount();
		for (KyUInt32 index = 0; index < count; ++index)
			CollectionIndexTracker::SetIndex(m_values[index], CollectionInvalidIndex);

		m_values.Clear();
	}

	KY_INLINE void ClearAndReleaseMemory() { m_values.ClearAndRelease(); }
	KY_INLINE void Reserve(KyUInt32 size) { return m_values.Reserve(size); }

	KY_INLINE KyUInt32 GetCount()    const { return m_values.GetCount(); }
	KY_INLINE KyUInt32 GetCapacity() const { return (KyUInt32)m_values.GetCapacity(); }
	KY_INLINE bool IsEmpty()         const { return GetCount() == 0; }

	KY_INLINE const T& operator[](KyUInt32 index) const { return m_values[index]; }
	KY_INLINE       T& operator[](KyUInt32 index)       { return m_values[index]; }

	KY_INLINE const T* GetDataPtr() const { return m_values.GetDataPtr(); }
	KY_INLINE       T* GetDataPtr()       { return m_values.GetDataPtr(); }

	/* Removes the element at specified index in the container.
		\returns index of the following element in the container.*/
	KyUInt32 RemoveAt(KyUInt32 index)
	{
		const KyUInt32 lastIndex = GetCount() - 1;

		CollectionIndexTracker::SetIndex(m_values[index], CollectionInvalidIndex);
		if (index != lastIndex)
		{
			m_values[index] = m_values[lastIndex];
			CollectionIndexTracker::SetIndex(m_values[index], index);
		}
		m_values.Resize(lastIndex);
		return index;
	}

	KyUInt32 FindFirstOccurrence(const T& value, KyUInt32 startIndex = 0)
	{
		for (KyUInt32 index = startIndex; index < m_values.GetCount(); ++index)
		{
			if (m_values[index] == value)
				return (KyUInt32)index;
		}
		return CollectionInvalidIndex;
	}

	KyUInt32 RemoveFirstOccurrence(const T& value, KyUInt32 startIndex = 0)
	{
		KY_DEBUG_ASSERTN(m_values.GetCount() < KyUInt32MAXVAL, ("More than KyUInt32MAXVAL values"));
		for (KyUInt32 index = startIndex; index < m_values.GetCount(); ++index)
		{
			if (m_values[index] == value)
				return RemoveAt(index);
		}
		return CollectionInvalidIndex;
	}

	bool DoesContain(const T& value) const { return m_values.DoesContain(value); }

private:
	KyArray<T, SID> m_values;
};

template <class T, int SID>
KY_INLINE T& Collection<T, SID>::PushBack(const T& value) { return m_collectionImpl.PushBack(value); }
template <class T, int SID>
KY_INLINE T& Collection<T, SID>::PushBackUnique(const T& value) { return m_collectionImpl.PushBackUnique(value); }
template <class T, int SID>
KY_INLINE T& Collection<T, SID>::PushBack() { return m_collectionImpl.PushBack(); }

template <class T, int SID>
KY_INLINE void Collection<T, SID>::ClearAndReleaseMemory() { m_collectionImpl.ClearAndReleaseMemory(); }
template <class T, int SID>
KY_INLINE void Collection<T, SID>::Clear() { m_collectionImpl.Clear(); }
template <class T, int SID>
KY_INLINE void Collection<T, SID>::Reserve(KyUInt32 size) { m_collectionImpl.Reserve(size); }

template <class T, int SID>
KY_INLINE KyUInt32 Collection<T, SID>::GetCount()    const { return m_collectionImpl.GetCount();    }
template <class T, int SID>
KY_INLINE KyUInt32 Collection<T, SID>::GetCapacity() const { return m_collectionImpl.GetCapacity(); }
template <class T, int SID>
KY_INLINE bool     Collection<T, SID>::IsEmpty()     const { return m_collectionImpl.IsEmpty();     }

template <class T, int SID>
KY_INLINE const T& Collection<T, SID>::operator[](KyUInt32 index) const { return m_collectionImpl[index]; }
template <class T, int SID>
KY_INLINE       T& Collection<T, SID>::operator[](KyUInt32 index)       { return m_collectionImpl[index]; }

template <class T, int SID>
KY_INLINE const T* Collection<T, SID>::GetDataPtr() const { return m_collectionImpl.GetDataPtr(); }
template <class T, int SID>
KY_INLINE       T* Collection<T, SID>::GetDataPtr()       { return m_collectionImpl.GetDataPtr(); }

template <class T, int SID>
KY_INLINE KyUInt32 Collection<T, SID>::RemoveAt(KyUInt32 index) { return m_collectionImpl.RemoveAt(index); }
template <class T, int SID>
KY_INLINE KyUInt32 Collection<T, SID>::FindFirstOccurrence(const T& value, KyUInt32 startIndex) { return m_collectionImpl.FindFirstOccurrence(value, startIndex); }
template <class T, int SID>
KY_INLINE KyUInt32 Collection<T, SID>::RemoveFirstOccurrence(const T& value, KyUInt32 startIndex) { return m_collectionImpl.RemoveFirstOccurrence(value, startIndex); }
template <class T, int SID>
KY_INLINE bool Collection<T, SID>::DoesContain(const T& value) const { return m_collectionImpl.DoesContain(value); }



template <class T, int SID>
KY_INLINE T& TrackedCollection<T, SID>::PushBack(const T& value) { return m_collectionImpl.PushBack(value); }
template <class T, int SID>
KY_INLINE T& TrackedCollection<T, SID>::PushBackUnique(const T& value) { return m_collectionImpl.PushBackUnique(value); }
template <class T, int SID>
KY_INLINE T& TrackedCollection<T, SID>::PushBack() { return m_collectionImpl.PushBack(); }

template <class T, int SID>
KY_INLINE void TrackedCollection<T, SID>::Clear() { m_collectionImpl.Clear(); }
template <class T, int SID>
KY_INLINE void TrackedCollection<T, SID>::ClearAndReleaseMemory() { m_collectionImpl.ClearAndReleaseMemory(); }
template <class T, int SID>
KY_INLINE void TrackedCollection<T, SID>::Reserve(KyUInt32 size) { return m_collectionImpl.Reserve(size); }

template <class T, int SID>
KY_INLINE KyUInt32 TrackedCollection<T, SID>::GetCount()    const { return m_collectionImpl.GetCount();    }
template <class T, int SID>
KY_INLINE KyUInt32 TrackedCollection<T, SID>::GetCapacity() const { return m_collectionImpl.GetCapacity(); }
template <class T, int SID>
KY_INLINE bool     TrackedCollection<T, SID>::IsEmpty()     const { return m_collectionImpl.IsEmpty();     }

template <class T, int SID>
KY_INLINE const T& TrackedCollection<T, SID>::operator[](KyUInt32 index) const { return m_collectionImpl[index]; }
template <class T, int SID>
KY_INLINE       T& TrackedCollection<T, SID>::operator[](KyUInt32 index)       { return m_collectionImpl[index]; }

template <class T, int SID>
KY_INLINE const T* TrackedCollection<T, SID>::GetDataPtr() const { return m_collectionImpl.GetDataPtr(); }
template <class T, int SID>
KY_INLINE       T* TrackedCollection<T, SID>::GetDataPtr()       { return m_collectionImpl.GetDataPtr(); }

template <class T, int SID>
KY_INLINE KyUInt32 TrackedCollection<T, SID>::RemoveAt(KyUInt32 index) { return m_collectionImpl.RemoveAt(index); }
template <class T, int SID>
KY_INLINE KyUInt32 TrackedCollection<T, SID>::FindFirstOccurrence(const T& value, KyUInt32 startIndex) { return m_collectionImpl.FindFirstOccurrence(value, startIndex); }
template <class T, int SID>
KY_INLINE KyUInt32 TrackedCollection<T, SID>::RemoveFirstOccurrence(const T& value, KyUInt32 startIndex) { return m_collectionImpl.RemoveFirstOccurrence(value, startIndex); }
template <class T, int SID>
KY_INLINE bool TrackedCollection<T, SID>::DoesContain(const T& value) const { return m_collectionImpl.DoesContain(value); }

template <class T, int SID>
KY_INLINE KyUInt32 TrackedCollection<T, SID>::Remove(const T& value) { KY_DEBUG_ASSERT(value->GetIndexInCollection() != CollectionInvalidIndex, "You are trying to remove an element not registered in the collection. Maybe you are removing the same element twice?"); return RemoveAt(value->GetIndexInCollection()); }

} // namespace Kaim

