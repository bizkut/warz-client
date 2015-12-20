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
#if (!defined(NMTL_STACK_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_STACK_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#ifndef NMTL_NO_SENTRY
  #include "nmtl/pod_vector.h"
#endif

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
/// \class   NMTL::stack
/// \brief   FILO stack implemented with a container
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class NMTL_TEMPLATE_EXPORT pod_stack
{
public:
  typedef pod_stack<T> my_type;
  typedef pod_vector<T> container_type;
  typedef typename container_type::value_type value_type;
  typedef typename container_type::size_type size_type;
  typedef typename container_type::reference reference;
  typedef typename container_type::const_reference const_reference;

  /// \brief  ctor
  NMTL_INLINE pod_stack(nmtl::allocator* allocator_ = NMTL_DEFAULT_ALLOCATOR) :
    m_container(allocator_)
  {
  }

  /// \brief  copy ctor
  /// \param  s - the stack to copy
  NMTL_INLINE pod_stack(const pod_stack<T>& s) :
    m_container(s.m_container)
  {
  }

  /// \brief  construct by copying specified container
  /// \param  _Cont - pod_stack container to copy
  NMTL_INLINE explicit pod_stack(const container_type& container) :
    m_container(container)
  {
  }

  /// \brief  test if stack is empty
  /// \return true if empty
  NMTL_INLINE bool empty() const
  {
    return (m_container.empty());
  }

  /// \brief  test length of stack
  /// \return the size of the stack
  NMTL_INLINE size_type size() const
  {
    return (m_container.size());
  }

  /// \brief  return last element of mutable stack
  /// \return the top of the stack
  NMTL_INLINE reference top()
  {
    return (m_container.back());
  }

  /// \brief  return last element of non-mutable stack
  /// \return the top of the stack
  NMTL_INLINE const_reference top() const
  {
    return (m_container.back());
  }

  /// \brief  push element onto the stack
  /// \param  _Val - the value to push on the stack
  NMTL_INLINE void push(const value_type& _Val)
  {
    m_container.push_back(_Val);
  }

  /// \brief  erase last element
  NMTL_INLINE void pop()
  {
    m_container.pop_back();
  }

protected:
  /// the underlying container
  container_type m_container;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_NAMESPACE
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_STACK_H
