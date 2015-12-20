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
#ifdef _MSC_VER
#pragma once
#endif
#ifndef NMTL_CONTAINER_DESTROY_H
#define NMTL_CONTAINER_DESTROY_H

//----------------------------------------------------------------------------------------------------------------------
namespace nmtl
{

//----------------------------------------------------------------------------------------------------------------------
template<typename type, typename iter, bool>
struct __destroy_method
{
  static NMTL_INLINE void destroy(iter ptr)
  {
    (ptr)->~type();
  }
};

template<typename type, typename iter>
struct __destroy_method<type, iter, false>
{
  static NMTL_INLINE void destroy(iter) {}
};

  //----------------------------------------------------------------------------------------------------------------------
} // namespace nmtl

#endif // NMTL_CONTAINER_DESTROY_H
