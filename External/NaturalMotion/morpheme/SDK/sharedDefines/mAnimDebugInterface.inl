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
#ifndef M_ANIM_DEBUG_INTERFACE_INL
#define M_ANIM_DEBUG_INTERFACE_INL
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mAnimDebugInterface.inl
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
/// AnimRigDefPersistentData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AnimRigDefPersistentData *AnimRigDefPersistentData::endianSwap(void *buffer)
{
  AnimRigDefPersistentData *data = (AnimRigDefPersistentData *) buffer;

  NMP::netEndianSwap(data->m_animSetIndex);
  NMP::netEndianSwap(data->m_numJoints);

  uint32_t* parents = data->getParents();
  NMP::PosQuat* bindPose = data->getBindPose();
  for (uint32_t i = 0; i < data->m_numJoints; ++i)
  {
    NMP::netEndianSwap(parents[i]);
    NMP::netEndianSwap(bindPose[i]);
  }

  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AnimRigDefPersistentData *AnimRigDefPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AnimSetIndex AnimRigDefPersistentData::getAnimSetIndex() const
{
  return m_animSetIndex;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnimRigDefPersistentData::getNumJoints() const
{
  return m_numJoints;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnimRigDefPersistentData::getParent(uint32_t index) const
{
  NMP_ASSERT(index < m_numJoints);
  const uint32_t *firstParent = reinterpret_cast<const uint32_t *>(this + 1);
  const uint32_t *parent = firstParent + index;
  return *parent;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t* AnimRigDefPersistentData::getParents()
{
  uint32_t *firstParent = reinterpret_cast<uint32_t *>(this + 1);
  return firstParent;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::PosQuat *AnimRigDefPersistentData::getBindPose() const
{
  const uint32_t *firstParent = reinterpret_cast<const uint32_t *>(this + 1);

  const NMP::PosQuat *bindPoseStart = reinterpret_cast<const NMP::PosQuat*>(firstParent + m_numJoints);
  return bindPoseStart;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::PosQuat *AnimRigDefPersistentData::getBindPose()
{
  uint32_t *firstParent = reinterpret_cast<uint32_t *>(this + 1);

  NMP::PosQuat *bindPoseStart = reinterpret_cast<NMP::PosQuat*>(firstParent + m_numJoints);
  return bindPoseStart;
}

// restore the packing alignment set before this file
#ifndef NM_HOST_CELL_SPU
  #pragma pack(pop)
#endif // NM_HOST_CELL_SPU

}

#endif
