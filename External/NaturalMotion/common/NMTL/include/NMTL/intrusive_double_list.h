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
#if (!defined(NMTL_INTRUSIVE_DOUBLE_LIST_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_INTRUSIVE_DOUBLE_LIST_H
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
/// \namespace  NMTL::IntrusiveDoubleList
/// \brief      Template functions for implementing double linked lists in pre-existing classes.
/// \ingroup    NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
/// \details
/// These functions implement double linked list operations for classes that manage their own data and allocation.
/// Each element in the list is a class type ElementType which must contain two members of type ElementType* to store
/// previous and next siblings in the chain; these must set to 0 when the element is instantiated.  The list of elements
/// is maintained by two pointers of ElementType* to represent the first an last elements in the list, these must also
/// be set to 0 before any operations are performed on the list.
/// These variables of the first and last in the list and the previous and next in the element need to be passed into
/// these functions, the latter two are class member pointers so will require the syntax &ElementType::variableName.
/// e.g.
/// struct TreeNode
/// {
///   TreeNode() : m_first(0), m_last(0), m_previous(0), m_next(0) { }
///   void addChild(TreeNode* child)
///   {
///     nmtl::IntrusiveDoubleList::append(m_first, m_last, &TreeNode::m_previous, &TreeNode::m_next, child);
///   }
///
///   TreeNode* m_first;
///   TreeNode* m_last;
///   TreeNode* m_previous;
///   TreeNode* m_next;
/// };
//----------------------------------------------------------------------------------------------------------------------
namespace IntrusiveDoubleList
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Append an element to the end of a linked list.
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  listLast - a reference to the last element pointer in the list.
/// \param  elementPrevious - a pointer to the class member in ElementType which stores it's previous sibling.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
/// \param  element - the element to append to the end of the list.
//----------------------------------------------------------------------------------------------------------------------
template <class ElementType>
void append(
  ElementType*& listFirst,
  ElementType*& listLast,
  ElementType* ElementType::*elementPrevious,
  ElementType* ElementType::*elementNext,
  ElementType* element)
{
  NMTL_ASSERT(element);
  NMTL_ASSERT(!(element->*elementNext));
  NMTL_ASSERT(!(element->*elementPrevious));

  // If the list is empty then set up it's first element, otherwise hook up the last element to the new instance.
  if (!listFirst)
  {
    NMTL_ASSERT(!listLast);
    listFirst = element;
  }
  else
  {
    NMTL_ASSERT(listLast);
    listLast->*elementNext = element;
  }

  // The last element is about to be replaced with the instance, hook up it's previous ptr accordingly.
  element->*elementPrevious = listLast;

  // Ensure that the last element in the list is the new instance.
  listLast = element;
  element->*elementNext = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Insert an element before the given entry in the list.
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  listLast - a reference to the last element pointer in the list.
/// \param  elementPrevious - a pointer to the class member in ElementType which stores it's previous sibling.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
/// \param  element - the element to append to the end of the list.
/// \param  insertPoint - the element to insert before.
//----------------------------------------------------------------------------------------------------------------------
template <class ElementType>
void insertBefore(
  ElementType*& listFirst,
  ElementType*& listLast,
  ElementType* ElementType::*elementPrevious,
  ElementType* ElementType::*elementNext,
  ElementType* element,
  ElementType* insertPoint)
{
  if (!insertPoint)
  {
    // Default behaviour is to append to the end of the list.
    append(
      listFirst,
      listLast,
      elementPrevious,
      elementNext,
      element);
  }
  else
  {
    NMTL_ASSERT(element);
    NMTL_ASSERT(listFirst);
    NMTL_ASSERT(listLast);
    NMTL_ASSERT(!(element->*elementNext));
    NMTL_ASSERT(!(element->*elementPrevious));

    element->*elementNext = insertPoint;
    if (insertPoint->*elementPrevious)
    {
      // Insert this element between insertPoint and it's previous sibling.
      insertPoint->*elementPrevious->*elementNext = element;
      element->*elementPrevious = insertPoint->*elementPrevious;
    }
    else
    {
      // Insert this element as the new first entry in the list.
      element->*elementPrevious = 0;
      listFirst = element;
    }
    insertPoint->*elementPrevious = element;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Remove an element from a list, the element must be in the list and the list must not already be empty.
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  listLast - a reference to the last element pointer in the list.
/// \param  elementPrevious - a pointer to the class member in ElementType which stores it's previous sibling.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
/// \param  element - the element to remove from the list.
//----------------------------------------------------------------------------------------------------------------------
template <typename ElementType>
void remove(
  ElementType*& listFirst,
  ElementType*& listLast,
  ElementType* ElementType::*elementPrevious,
  ElementType* ElementType::*elementNext,
  ElementType* element)
{
  NMTL_ASSERT(element);
  NMTL_ASSERT(listFirst);
  NMTL_ASSERT(listLast);

  if (element->*elementPrevious)
  {
    element->*elementPrevious->*elementNext = element->*elementNext;
    if (element->*elementNext)
    {
      // The element has a next sibling, ensure it's previous is hooked up.
      element->*elementNext->*elementPrevious = element->*elementPrevious;
    }
    else
    {
      // The element is the last in the list, make the previous one the new last entry.
      listLast = element->*elementPrevious;
    }
  }
  else
  {
    listFirst = element->*elementNext;
    if (element->*elementNext)
    {
      // The element is the first in the list, make the next one the first.
      element->*elementNext->*elementPrevious = 0;
    }
    else
    {
      // The element was the last element in the list, last in list is now null.
      listLast = 0;
    }
  }

  element->*elementPrevious = 0;
  element->*elementNext = 0;
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief  Return the number of elements in the list which starts at listFirst
/// \param  listFirst - a reference to the first element pointer in the list.
/// \param  elementNext - a pointer to the class member in ElementType which stores it's next sibling.
//----------------------------------------------------------------------------------------------------------------------
template <typename ElementType>
size_t size(
  ElementType* const& listFirst,
  ElementType* ElementType::*elementNext)
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
} // namespace IntrusiveDoubleList

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_INTRUSIVE_DOUBLE_LIST_H
