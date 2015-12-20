// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
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
# pragma once
#endif
#ifndef M_ANIM_DEBUG_INTERFACE_H
#define M_ANIM_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMColour.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMBuffer.h"
#include "mCoreDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mAnimDebugInterface.h
/// \brief 
/// \ingroup AnimModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// ensure all these structs are the same layout for all compilers
#ifndef NM_HOST_CELL_SPU
  #pragma pack(push, 4)
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
struct AnimRigDefPersistentData
{
  AnimSetIndex  m_animSetIndex;
  uint32_t      m_numJoints;

  /// \brief Endian swaps in place on buffer.
  static inline AnimRigDefPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static inline AnimRigDefPersistentData *deserialiseTx(void *buffer);

  NM_INLINE AnimSetIndex getAnimSetIndex() const;
  NM_INLINE uint32_t getNumJoints() const;
  NM_INLINE uint32_t getParent(uint32_t index) const;
  NM_INLINE uint32_t* getParents();

  NM_INLINE const NMP::PosQuat *getBindPose() const;
  NM_INLINE NMP::PosQuat *getBindPose();
};

// restore the packing alignment set before this file
#ifndef NM_HOST_CELL_SPU
  #pragma pack(pop)
#endif // NM_HOST_CELL_SPU

} // namespace MR

#include "sharedDefines/mAnimDebugInterface.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // M_PHYSICS_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
