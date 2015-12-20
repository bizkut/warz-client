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
#ifndef MR_PHYSICS_SRZ_BUFFER_H
#define MR_PHYSICS_SRZ_BUFFER_H
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::PhysicsSerialisationBuffer
/// \brief Helper for serializing physics related objects into memory.
///  Physics TODO this is really primitive at the moment, and needs to be made nicer, and more tolerant
///  to errors etc. Also need to find a home for it.
//----------------------------------------------------------------------------------------------------------------------
struct PhysicsSerialisationBuffer
{
  PhysicsSerialisationBuffer() : dataStart(0), ptr(0), space(0) {}
  char* dataStart;
  char* ptr;
  size_t space;

  // adds the size, then the data
  bool addRawData(const void* data, size_t size)
  {
    if (ptr + size + sizeof(size_t) > dataStart + space)
    {
      NMP_ASSERT_FAIL();
      return false;
    }
    *((size_t*) ptr) = size;
    ptr += sizeof(size_t);
    memcpy(ptr, data, size);
    ptr += size;
    return true;
  }

  // raw data is copied into data, size is taken from the raw data
  bool getRawData(void* data)
  {
    size_t size = *((size_t*) ptr);
    if (ptr + size + sizeof(size_t) > dataStart + space)
    {
      NMP_ASSERT_FAIL();
      return false;
    }
    ptr += sizeof(size_t);
    memcpy(data, ptr, size);
    ptr += size;
    return true;
  }

  template<typename T>
  bool addValue(const T& val)
  {
    if (ptr + sizeof(T) > dataStart + space)
    {
      NMP_ASSERT_FAIL();
      return false;
    }
    *((T*) ptr) = val;
    ptr += sizeof(T);
    return true;
  }

  template<typename T>
  T getValue()
  {
    if (ptr + sizeof(T) > dataStart + space)
    {
      NMP_ASSERT_FAIL();
    }
    T val = *((T*) ptr);
    ptr += sizeof(T);
    return val;
  }

  template<typename T>
  T getValue(T& val)
  {
    if (ptr + sizeof(T) > dataStart + space)
    {
      NMP_ASSERT_FAIL();
    }
    val = *((T*) ptr);
    ptr += sizeof(T);
    return val;
  }
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_SRZ_BUFFER_H
//----------------------------------------------------------------------------------------------------------------------
