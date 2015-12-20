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
#if (!defined(NMTL_FIXED_FREE_LIST_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_FIXED_FREE_LIST_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include "nmtl/defs.h"
#include "nmtl/allocator.h"
#include "nmtl/type_utils.h"

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
// Forward declarations
template <typename _Type>
class NMTL_TEMPLATE_EXPORT fixed_free_list;

template<typename type, bool>
struct __destroy_method2
{
  static NMTL_INLINE void destroy(type* ptr)
  {
    (ptr)->~type();
  }
};

template<typename type>
struct __destroy_method2<type, false>
{
  static NMTL_INLINE void destroy(type*) {}
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Templated free-list "memory allocator"; if you have a fixed number of items you want to manage in a highly
///        dynamic fashion, (eg. lots of create/destroy in unordered fashion) this free-list can provide alloc/free in
///        constant (near instant) time.
///        e.g. fixed_free_list<ContactPoint>  cpList(2048);
///           ContactPoint* cp = cpList.alloc();
///            ...
///           cpList.free(cp);
//----------------------------------------------------------------------------------------------------------------------
template <class _Type>
class fixed_free_list
{
public:
  NMTL_INLINE fixed_free_list(int numObjects, _Type* preAllocObjects = 0, _Type** preAllocFreeList = 0, nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR);
  NMTL_INLINE ~fixed_free_list();

  NMTL_INLINE void freeAll();                                           ///< return the list to its default state, with all objects ready to alloc() again

  NMTL_INLINE _Type* alloc();
  NMTL_INLINE void   free(_Type* obj);

  NMTL_INLINE int  capacity() const { return m_numObjects; }            ///< maximum number of objects managed by this list
  NMTL_INLINE int  numFree() const { return m_top; }                    ///< number of items still available to alloc()
  NMTL_INLINE int  numUsed() const { return m_numObjects - m_top; }     ///< number of items alloc()ed and in use

private:
  fixed_free_list();

  nmtl::allocator*  m_allocator;

  _Type*            m_objects;
  _Type**           m_freeObjects;

  int               m_numObjects;
  int               m_top;

  bool              m_autoDeleteObjects;

  // manually calls the destructor on ptr
  NMTL_INLINE void destroy(_Type* ptr)
  {
    __destroy_method2< _Type, nmtl::is_class<_Type>::result >::destroy(ptr);
  }
};

//----------------------------------------------------------------------------------------------------------------------
template <class _Type>
NMTL_INLINE fixed_free_list<_Type>::fixed_free_list(
  int numObjects, _Type* preAllocObjects, _Type** preAllocFreeList, nmtl::allocator* allocator_) :
  m_numObjects(numObjects),
  m_top(-1),
  m_autoDeleteObjects(false)
{
  m_allocator = allocator_;

  NMTL_ASSERT(m_numObjects > 0);

  if (preAllocObjects)
  {
    m_objects = preAllocObjects;
    m_freeObjects = preAllocFreeList;
  }
  else
  {
    m_objects = reinterpret_cast<_Type*>(m_allocator->calloc(sizeof(_Type) * m_numObjects));
    m_freeObjects = reinterpret_cast<_Type**>(m_allocator->calloc(sizeof(_Type*) * m_numObjects));

    // call constructor on type, if appropriate; if POD, is ignored
    for (int i = 0; i < m_numObjects; i++)
      new (&m_objects[i]) _Type;

    m_autoDeleteObjects = true;
  }

  NMTL_ASSERT(m_objects);
  NMTL_ASSERT(m_freeObjects);

  // create the list of free objects
  freeAll();
}

//----------------------------------------------------------------------------------------------------------------------
template <class _Type>
NMTL_INLINE fixed_free_list<_Type>::~fixed_free_list()
{
  if (m_autoDeleteObjects)
  {
    // call destructors on types that require it
    for (int i = 0; i < m_numObjects; i++)
      destroy(&m_objects[i]);

    m_allocator->free(m_freeObjects);
    m_allocator->free(m_objects);
  }

  m_objects = 0;
  m_freeObjects = 0;
  m_numObjects = m_top = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template <class _Type>
NMTL_INLINE void fixed_free_list<_Type>::freeAll()
{
  int index = (m_numObjects - 1);

  for (m_top = 0; m_top < m_numObjects; m_top++)
  {
    m_freeObjects[m_top] = &m_objects[index --];
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <class _Type>
NMTL_INLINE _Type* fixed_free_list<_Type>::alloc()
{
  // return a free object, or 0 if we're out
  if (m_top > 0)
  {
    return m_freeObjects[--m_top];
  }
  else
  {
    return 0;
  }
}

//----------------------------------------------------------------------------------------------------------------------
template <class _Type>
NMTL_INLINE void fixed_free_list<_Type>::free(_Type* obj)
{
  // cause panic if someone is free'ing an object that is not controlled by this list
  NMTL_ASSERT((obj >= &(m_objects[0])) && (obj <= &(m_objects[m_numObjects-1])));
  NMTL_ASSERT((m_top < m_numObjects));

  m_freeObjects[m_top++] = obj;
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_FIXED_FREE_LIST_H
