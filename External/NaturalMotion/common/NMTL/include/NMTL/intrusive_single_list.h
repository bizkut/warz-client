// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#if (!defined(NMTL_INTRUSIVE_SINGLE_LIST_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_INTRUSIVE_SINGLE_LIST_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include "nmtl/defs.h"

#ifndef NMTL_TEMPLATE_EXPORT
  #define NMTL_TEMPLATE_EXPORT
  #define NMTL_KILL_TEMPLATE_EXPORT
#endif
#ifndef NMTL_NAMESPACE_OVERRIDE
  #define NMTL_NAMESPACE nmtl
#else
  #define NMTL_NAMESPACE NMTL_NAMESPACE_OVERRIDE
#endif
//----------------------------------------------------------------------------------------------------------------------

namespace NMTL_NAMESPACE
{

//----------------------------------------------------------------------------------------------------------------------
/// \namespace  NMTL::IntrusiveSingleList
/// \brief      Template functions for implementing single linked lists in pre-existing classes.
/// \ingroup    NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
/// \details
/// These functions implement single linked list operations for classes that manage their own data and allocation.
/// Each element in the list is a class type ElementType which must contain a members of type ElementType* to store
/// it's next sibling in the chain, this must set to 0 when the element is instantiated.  The list of elements is
/// maintained by a pointer of type ElementType* to represent the first element in the list, this must also be set to
/// 0 before any operations are performed on the list.
/// An add operation replaces the first element in the list with a new one, so the list is effectively built in
/// reverse order.
/// These variable that stores the first element in the list and the next pointer in the element need to be passed into
/// these functions, the latter is a class member pointer so will require the syntax &ElementType::variableName.
//----------------------------------------------------------------------------------------------------------------------
/// \example
/// struct TreeNode
/// {
///   TreeNode() : m_first(0), m_previous(0) { }
///   void addChild(TreeNode* child)
///   {
///     nmtl::IntrusiveSingleList::appendToFront(m_first, &TreeNode::m_next, child);
///   }
///
///   TreeNode* m_first;
///   TreeNode* m_next;
/// };
//----------------------------------------------------------------------------------------------------------------------
namespace IntrusiveSingleList
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Append an element to the front of a linked list.
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
/// \param  element - the element to append to the end of the list.
//----------------------------------------------------------------------------------------------------------------------
template <class ElementType>
void appendToFront(
  ElementType*& listFirst,
  ElementType*  ElementType::*elementNext,
  ElementType*  element)
{
  NMTL_ASSERT(element);

  // If the list is empty then set up it's first element, otherwise hook up the last element to the new instance.
  if (!listFirst)
  {
    element->*elementNext = 0;
    listFirst = element;
  }
  else
  {
    element->*elementNext = listFirst;
    listFirst = element;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Search the list for the element before searchElement, performs an O[n] search.
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
/// \param  element - the element to append to the end of the list.
//----------------------------------------------------------------------------------------------------------------------
template <class ElementType>
bool findPrevious(
  ElementType*& listFirst,
  ElementType*  ElementType::*elementNext,
  ElementType*  searchElement,
  ElementType*& previous)
{
  NMTL_ASSERT(searchElement);

  // Element is first in list, previous is 0 but result is valid.
  if (searchElement == listFirst)
  {
    previous = 0;
    return true;
  }

  // Search for the element that comes before searchElement
  previous = listFirst;
  while (previous)
  {
    ElementType* next = previous->*elementNext;
    if (next == searchElement)
    {
      break;
    }
    previous = next;
  }

  return (previous != 0);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Insert an element before the given entry in the list, this performs an O[n] search internally.
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
/// \param  element - the element to append to the end of the list.
/// \param  insertPoint - the element to insert before.
//----------------------------------------------------------------------------------------------------------------------
template <class ElementType>
bool insertBefore(
  ElementType*& listFirst,
  ElementType*  ElementType::*elementNext,
  ElementType*  element,
  ElementType*  insertPoint)
{
  if (!insertPoint || insertPoint == listFirst)
  {
    // Default behaviour is to append to the front of the list.
    appendToFront(
      listFirst,
      elementNext,
      element);
  }
  else
  {
    NMTL_ASSERT(element);
    NMTL_ASSERT(!(element->*elementNext));
    NMTL_ASSERT(listFirst);

    // Find the element before the insert point, assertion will be caused by insertPoint not being in list.
    ElementType* preInsertPoint;
#ifdef _DEBUG
    bool found =
#endif
      findPrevious(listFirst, elementNext, insertPoint, preInsertPoint);

    NMTL_ASSERT(found);
    NMTL_ASSERT(preInsertPoint);
    if (!preInsertPoint)
    {
      return false;
    }

    preInsertPoint->*elementNext = element;
    element->*elementNext = insertPoint;
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Remove an element from a list, the element must be in the list and the list must not already be empty.
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
/// \param  element - the element to remove from the list.
//----------------------------------------------------------------------------------------------------------------------
template <typename ElementType>
bool remove(
  ElementType*& listFirst,
  ElementType*  ElementType::*elementNext,
  ElementType*  element)
{
  NMTL_ASSERT(element);
  NMTL_ASSERT(listFirst);

  if (element == listFirst)
  {
    listFirst = element->*elementNext;
  }
  else
  {
    // Find the element before the insert point, assertion will be caused by insertPoint not being in list.
    ElementType* previous;
#ifdef _DEBUG
    bool found =
#endif
      findPrevious(listFirst, elementNext, element, previous);

    NMTL_ASSERT(found);
    NMTL_ASSERT(previous);
    if (!previous)
    {
      return false;
    }

    previous->*elementNext = element->*elementNext;
  }

  element->*elementNext = 0;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Return the number of elements in the list which starts at listFirst
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
//----------------------------------------------------------------------------------------------------------------------
template <typename ElementType>
size_t size(
  ElementType* const& listFirst,
  ElementType*        ElementType::*elementNext)
{
  size_t result = 0;
  const ElementType* current = listFirst;
  while (current)
  {
    ++result;
    current = current->*elementNext;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace IntrusiveSingleList

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_INTRUSIVE_SINGLE_LIST_H
