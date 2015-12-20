/*
* Copyright 2013 Autodesk, Inc. All rights reserved.
* Use of this software is subject to the terms of the Autodesk license agreement and any attachments or Appendices thereto provided at the time of installation or download,
* or which otherwise accompanies this software in either electronic or hard copy form, or which is signed by you and accepted by Autodesk.
*/


#ifndef Navigation_SharedPoolList_H
#define Navigation_SharedPoolList_H

#include "gwnavruntime/base/types.h"
#include "gwnavruntime/containers/pool.h"
#include "gwnavruntime/base/memory.h"

namespace Kaim
{

/// This is the list node used in SharedPoolList implementation (SP stands for SharedPool).
template <typename T>
class SPListNode
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(SPListNode)

private:
	typedef Pool<SPListNode<T> > NodePool;
	typedef typename NodePool::Key  NodeKey;

public:
	SPListNode() : m_prev(KY_NULL), m_next(KY_NULL) {}
	explicit SPListNode(const T& data) : m_prev(KY_NULL), m_next(KY_NULL), m_data(data) {}

public: // internal
	SPListNode* m_prev;
	SPListNode* m_next;
	NodeKey m_nodeKey;
	T m_data;
};


/// Iterator on SharedPoolList nodes (SPL stands for SharedPoolList).
template <typename T>
class SPL_Iterator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef SPListNode<T> Node;


	// ---------------------------------- Public Member Functions ----------------------------------

	SPL_Iterator() : m_node(KY_NULL) {}
	explicit SPL_Iterator(Node* listNode) : m_node(listNode) {}
	SPL_Iterator(const SPL_Iterator& rhs) : m_node(rhs.m_node) {}

	KY_INLINE const SPL_Iterator& operator=(const SPL_Iterator& rhs) { m_node = rhs.m_node; return *this; }

	KY_INLINE bool operator==(const SPL_Iterator& rhs) const { return m_node == rhs.m_node; }
	KY_INLINE bool operator!=(const SPL_Iterator& rhs) const { return m_node != rhs.m_node; }

	KY_INLINE SPL_Iterator& operator++()   { m_node = m_node->m_next; return *this; }
	KY_INLINE SPL_Iterator& operator--()   { m_node = m_node->m_prev; return *this; }

	KY_INLINE SPL_Iterator operator++(int) { SPL_Iterator old = *this; m_node = m_node->m_next; return old;   }
	KY_INLINE SPL_Iterator operator--(int) { SPL_Iterator old = *this; m_node = m_node->m_prev; return old;   }

	KY_INLINE T& operator*() const  { return  m_node->m_data; }
	KY_INLINE T* operator->() const { return &m_node->m_data; }

	KY_INLINE SPL_Iterator GetNext() const { return SPL_Iterator(m_node->m_next); }
	KY_INLINE SPL_Iterator GetPrev() const { return SPL_Iterator(m_node->m_prev); }

	KY_INLINE bool IsNull()    const { return m_node == KY_NULL; }
	KY_INLINE bool IsNotNull() const { return m_node != KY_NULL; }

public: // internal
	Node* m_node;
};


/// Const iterator on SharedPoolList nodes (SPL stands for SharedPoolList).
template <typename T>
class SPL_ConstIterator
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)

public:
	// ---------------------------------- Public typedef ----------------------------------

	typedef SPListNode<T> Node;


	// ---------------------------------- Public Member Functions ----------------------------------

	SPL_ConstIterator() : m_node(KY_NULL) {}
	SPL_ConstIterator(const SPL_Iterator<T>& it) : m_node(it.m_node) {}
	SPL_ConstIterator(const SPL_ConstIterator& it) : m_node(it.m_node) {}
	explicit SPL_ConstIterator(const Node* listNode) : m_node(listNode) {}

	KY_INLINE const SPL_ConstIterator& operator=(const SPL_ConstIterator& rhs) { m_node = rhs.m_node; return *this; }

	KY_INLINE bool operator==(const SPL_ConstIterator& rhs) const { return m_node == rhs.m_node; }
	KY_INLINE bool operator!=(const SPL_ConstIterator& rhs) const { return m_node != rhs.m_node; }

	KY_INLINE SPL_ConstIterator& operator++()   { m_node = m_node->m_next; return *this; }
	KY_INLINE SPL_ConstIterator& operator--()   { m_node = m_node->m_prev; return *this; }

	KY_INLINE SPL_ConstIterator operator++(int) { SPL_ConstIterator old = *this; m_node = m_node->m_next; return old;   }
	KY_INLINE SPL_ConstIterator operator--(int) { SPL_ConstIterator old = *this; m_node = m_node->m_prev; return old;   }

	KY_INLINE const T& operator*() const  { return  m_node->m_data; }
	KY_INLINE const T* operator->() const { return &m_node->m_data; }

	KY_INLINE SPL_ConstIterator GetNext() const { return SPL_ConstIterator(m_node->m_next); }
	KY_INLINE SPL_ConstIterator GetPrev() const { return SPL_ConstIterator(m_node->m_prev); }

	KY_INLINE bool IsNull()    const { return m_node == KY_NULL; }
	KY_INLINE bool IsNotNull() const { return m_node != KY_NULL; }

public: // internal
	const Node* m_node;
};


/// SharedPoolList represents an ordered list of elements of a single type, drawn as needed from
/// a pre-allocated pool that you provide.
/// \tparam T  The type of object to be stored in this list.
/// \section construction Constructing the list
/// In order to use this class, you must create an instance of SharedPoolList::Pool.
/// You must then set the SharedPoolList to use this pool of objects, either in its constructor or by
/// calling SharedPoolList::SetNodePool().
/// For example:
/// \code
/// typedef SharedPoolList<MyValue> MyList
/// MyList::NodePool myPool;
/// MyList myList(&myPool);
/// // or myList.SetPool(&myPool);
/// \endcode
/// \section iterating Iterating through the list
/// To iterate through the elements of the list in ascending order:
/// \code
/// for (MyList::ConstIterator it = myList.Begin(); it != myList.End(); ++it)
/// {
/// 	// You can use the * operator to retrieve the instance of your data type currently pointed to by the Iterator:
/// 	const MyValue& value = *it;

/// 	// ...
/// } \endcode
template <typename T>
class SharedPoolList
{
	KY_DEFINE_NEW_DELETE_OPERATORS(Stat_Default_Mem)
	KY_CLASS_WITHOUT_COPY(SharedPoolList)

public:
	// ---------------------------------- Public typedefs ----------------------------------

	typedef SPListNode<T>           Node;
	typedef SPL_Iterator<T>         Iterator;
	typedef SPL_ConstIterator<T>    ConstIterator;
	typedef Pool<SPListNode<T> >    NodePool;
	typedef typename NodePool::Key  NodeKey;

public:
	// ---------------------------------- Main API Functions ----------------------------------

	explicit SharedPoolList(NodePool* nodePool = KY_NULL) : m_nodePool(nodePool), m_count(0)
	{ m_root.m_next = m_root.m_prev = RootAsNode(); }

	void SetNodePool(NodePool* nodePool) { m_nodePool = nodePool; }

	~SharedPoolList() { Clear(); }

	void Clear();

	KyUInt32 GetCount() const { return m_count; }


	// ---------------------------------- Bounds ----------------------------------

	ConstIterator GetFirst() const { return ConstIterator(m_root.m_next); }
	ConstIterator GetLast()  const { return ConstIterator(m_root.m_prev); }
	Iterator      GetFirst()       { return      Iterator(m_root.m_next); }
	Iterator      GetLast ()       { return      Iterator(m_root.m_prev); }

	ConstIterator Begin()    const { return ConstIterator(m_root.m_next); }
	ConstIterator End()      const { return ConstIterator(RootAsNode());  }
	Iterator      Begin()          { return      Iterator(m_root.m_next); }
	Iterator      End()            { return      Iterator(RootAsNode());  }



	// ---------------------------------- Utility Functions ----------------------------------

	// Determine if list is empty (i.e.) points to itself.
	// Go through void* access to avoid issues with strict-aliasing optimizing out the
	// access after RemoveNode(), etc.
	bool IsEmpty() const { return m_root.m_next == RootAsNode(); }

	bool IsFirst(ConstIterator it) const { return it.m_node == m_root.m_next; }
	bool IsFirst(Iterator it) const      { return it.m_node == m_root.m_next; }

	bool IsLast (ConstIterator it) const { return it.m_node == m_root.m_prev; }
	bool IsLast (Iterator it) const      { return it.m_node == m_root.m_prev; }

	bool IsNull (ConstIterator it) const { return it.m_node == RootAsNode(); }
	bool IsNull (Iterator it) const      { return it.m_node == RootAsNode(); }

	bool IsValid(ConstIterator it) const { return it.m_node != RootAsNode(); }
	bool IsValid(Iterator it) const      { return it.m_node != RootAsNode(); }

	ConstIterator Find(const T& data) const;
	Iterator      Find(const T& data);


	// ---------------------------------- Node Insertion ----------------------------------

	Iterator InsertBefore(const T& data, Iterator nextNodeIt) { Node* newNode = NewNode(data); InsertNodeBefore(newNode, nextNodeIt.m_node); return Iterator(newNode); }
	Iterator InsertAfter(Iterator prevNodeIt, const T& data)  { Node* newNode = NewNode(data); InsertNodeAfter(prevNodeIt.m_node, newNode);  return Iterator(newNode); }

	Iterator InsertBefore(Iterator nextNodeIt) { Node* newNode = NewNode(); InsertNodeBefore(newNode, nextNodeIt.m_node); return Iterator(newNode); }
	Iterator InsertAfter(Iterator prevNodeIt)  { Node* newNode = NewNode(); InsertNodeAfter(prevNodeIt.m_node, newNode);  return Iterator(newNode); }

	Iterator PushFront(const T& data) { Node* newNode = NewNode(data); InsertNodeBefore(newNode, m_root.m_next); return Iterator(newNode); }
	Iterator PushBack(const T& data)  { Node* newNode = NewNode(data); InsertNodeAfter(m_root.m_prev, newNode);  return Iterator(newNode); }

	Iterator PushFront() { Node* newNode = NewNode(); InsertNodeBefore(newNode, m_root.m_next); return Iterator(newNode); }
	Iterator PushBack()  { Node* newNode = NewNode(); InsertNodeAfter(m_root.m_prev, newNode);  return Iterator(newNode); }


	// ---------------------------------- Node Removal ----------------------------------

	Iterator Erase(Iterator it);

	Iterator RemoveFirstOccurrence(Iterator begin, const T& data);

	KyUInt32 RemoveAllOccurrences(const T& data);

	template<class Predicate>
	KyUInt32 Remove_If(Predicate predicate)
	{
		KyUInt32 count = 0;
		for (Iterator it = Begin(); it != End(); )
		{
			if (!predicate(*it))
				++it;
			else
			{
				it = Erase(it);
				++count;
			}
		}
		return count;
	}

private:
	void InsertNodeBefore(Node* newNode, Node* nextNode) { InsertNodeBetween(nextNode->m_prev, newNode, nextNode); }
	void InsertNodeAfter(Node* prevNode, Node* newNode)  { InsertNodeBetween(prevNode, newNode, prevNode->m_next); }
	void InsertNodeBetween(Node* prevNode, Node* newNode, Node* nextNode);
	Node* NewNode(const T& data); // return NewNode with m_data=data, m_nodeKey=newone, m_prev=m_next=0
	Node* NewNode();              // return NewNode with m_data=T() , m_nodeKey=newone, m_prev=m_next=0
	void RemoveNode(Node* node) { node->m_prev->m_next = node->m_next; node->m_next->m_prev = node->m_prev; }
	Node* RootAsNode()             { return &m_root; }
	const Node* RootAsNode() const { return &m_root; }

private:
	NodePool* m_nodePool;
	Node m_root;
	KyUInt32 m_count;
};


template <typename T>
typename SharedPoolList<T>::Node* SharedPoolList<T>::NewNode(const T& data)
{
	NodeKey key;
	Node* node = KY_NULL;
	m_nodePool->New_CompactKeyAndPtr(key, node);
	
	node->m_data = data;
	node->m_nodeKey = key;

	++m_count;
	return node;
}


template <typename T>
typename SharedPoolList<T>::Node* SharedPoolList<T>::NewNode()
{
	NodeKey nodeKey = m_nodePool->New_CompactKey();
	Node* node = &m_nodePool->Get(nodeKey);
	// TODO one call to m_nodePool that returns {nodeKey, node}
	node->m_nodeKey = nodeKey;
	++m_count;
	return node;
}

template <typename T>
void SharedPoolList<T>::InsertNodeBetween(Node* prevNode, Node* newNode, Node* nextNode)
{
	prevNode->m_next = newNode;
	newNode->m_prev  = prevNode;

	newNode->m_next  = nextNode;
	nextNode->m_prev = newNode;
}


template <typename T>
typename SharedPoolList<T>::Iterator SharedPoolList<T>::Erase(Iterator it)
{
	Node* node = it.m_node;
	Node* nextNode = node->m_next;
	RemoveNode(node);
	m_nodePool->Delete(node->m_nodeKey);
	--m_count;
	return Iterator(nextNode);
}


template <typename T>
void SharedPoolList<T>::Clear()
{
	for (Iterator it = Begin(); it != End(); ++it)
		m_nodePool->Delete(it.m_node->m_nodeKey);

	m_count = 0;
	m_root.m_next = m_root.m_prev = RootAsNode();
}


template <typename T>
typename SharedPoolList<T>::ConstIterator SharedPoolList<T>::Find(const T& data) const
{
	for (ConstIterator it = Begin(); it != End(); ++it)
	{
		if (*it == data)
			return it;
	}
	return End();
}

template <typename T>
typename SharedPoolList<T>::Iterator SharedPoolList<T>::Find(const T& data)
{
	for (Iterator it = Begin(); it != End(); ++it)
	{
		if (*it == data)
			return it;
	}
	return End();
}


template <typename T>
typename SharedPoolList<T>::Iterator SharedPoolList<T>::RemoveFirstOccurrence(Iterator begin, const T& data)
{
	Iterator it = begin;
	for (; it != End(); ++it)
	{
		if (*it == data)
			return Erase(it);
	}
	return it;
}


template <typename T>
KyUInt32 SharedPoolList<T>::RemoveAllOccurrences(const T& data)
{
	KyUInt32 count = 0;
	for (Iterator it = Begin(); it != End(); )
	{
		if (*it != data)
			++it;
		else
		{
			it = Erase(it);
			++count;
		}
	}
	return count;
}

} // namespace Kaim


#endif // Navigation_SharedPoolList_H
