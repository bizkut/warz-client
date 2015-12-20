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
#if (!defined(NMTL_DEQUE_H) && !defined(NMTL_NO_SENTRY)) || defined(NMTL_NO_SENTRY)
#ifndef NMTL_NO_SENTRY
  #define NMTL_DEQUE_H
#endif

//----------------------------------------------------------------------------------------------------------------------
#include "nmtl/vector.h"

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
/// \class   NMTL::deque
/// \brief   FIFO queue implemented with a container
/// \ingroup NaturalMotionTemplateLibrary
//----------------------------------------------------------------------------------------------------------------------
template<typename T>
class NMTL_TEMPLATE_EXPORT deque :
  public vector<T>
{
public:
  typedef typename vector<T>::pointer         pointer;
  typedef typename vector<T>::iterator        iterator;
  typedef typename vector<T>::reference       reference;
  typedef typename vector<T>::const_pointer   const_pointer;
  typedef typename vector<T>::const_iterator  const_iterator;
  typedef typename vector<T>::const_reference const_reference;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace NMTL_NAMESPACE

#undef NMTL_TEMPLATE_EXPORT
#ifdef NMTL_KILL_TEMPLATE_EXPORT
  #undef NMTL_TEMPLATE_EXPORT
#endif

#endif // NMTL_DEQUE_H
