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
#ifndef MR_ANIM_RIG_DEF_H
#define MR_ANIM_RIG_DEF_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMHierarchy.h"
#include "NMPlatform/NMQuat.h"
#include "NMPlatform/NMStringTable.h"
#include "morpheme/mrAttribData.h"

//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AnimRigDef
/// \brief Hierarchical relationship of shape bones/channels and the root bone/channel index.
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------
class AnimRigDef
{
public:
  static const int32_t NO_PARENT_BONE = -1;
  static const uint32_t INVALID_BONE_INDEX = 0xFFFFFFFF;
  static const uint32_t BONE_NAME_NOT_FOUND = 0xFFFFFFFF;

public:
  NM_INLINE uint32_t getNumBones() const;
  NM_INLINE int32_t getParentBoneIndex(int32_t boneIndex) const;
  uint32_t getTrajectoryBoneIndex() const { return m_trajectoryBoneIndex; }
  uint32_t getCharacterRootBoneIndex() const { return m_characterRootBoneIndex; }
  const NMP::Quat* getBlendFrameOrientation() const { return &m_blendFrameOrientation; }
  const NMP::Vector3* getBlendFrameTranslation() const { return &m_blendFrameTranslation; }
  void setTrajectoryBoneIndex(uint32_t trajectoryBoneIndex) { m_trajectoryBoneIndex = trajectoryBoneIndex; }
  void setCharacterRootBoneIndex(uint32_t characterRootBoneIndex) { m_characterRootBoneIndex = characterRootBoneIndex; }
  const NMP::Hierarchy* getHierarchy() const { return m_hierarchy; }
  NM_INLINE const char* getBoneName(uint32_t boneIndex) const;
  NM_INLINE const NMP::Vector3* getBindPoseBonePos(uint32_t boneIndex) const;
  NM_INLINE const NMP::Quat* getBindPoseBoneQuat(uint32_t boneIndex) const;
  AttribDataTransformBuffer* getBindPose() { return m_bindPose; }
  const AttribDataTransformBuffer* getBindPose() const { return m_bindPose; }

  uint32_t getBoneIndexFromName(const char* boneName) const;
  const char* getBoneNameFromIndex(uint32_t boneID) const;

  /// \brief Prepare a dislocated Rig for use.
  bool locate();

  /// \brief Dislocate a Rig ready to move to a new memory location or storing as a binary asset.
  bool dislocate();

  /// \brief Re-seat this asset after copying its memory block to a new location.
  void relocate();

  /// \brief Get the memory requirements of this instance.
  NMP::Memory::Format getInstanceMemoryRequirements() const;

  uint32_t serializeTx(AnimSetIndex animSetIndex, void* outputBuffer, uint32_t outputBufferSize) const;

protected:
  AnimRigDef();
  ~AnimRigDef();

  NMP::Quat       m_blendFrameOrientation;  ///< Blending root transform.
  NMP::Vector3    m_blendFrameTranslation;  ///< All animation blending takes place within the frame of this transform.
                                            ///<  Also forms the basis for mapping between physics 
                                            ///<  and animation rig positions.

  NMP::Hierarchy* m_hierarchy;              ///< Each entry contains the parent index of each bone.
  uint32_t        m_trajectoryBoneIndex;    ///< Root index of bone hierarchy.
  uint32_t        m_characterRootBoneIndex; ///< Bones that we want to maintain their relative offset from the
                                            ///<  trajectory, when resetting to blend frame. e.g. Hip bone. 
                                            //    MORPH-21343: There could validly be several of these, need to
                                            //    update this to an array.
  NMP::OrderedStringTable*   m_boneNameMap; ///< Map of bone index to string names for each joint.
  AttribDataTransformBuffer* m_bindPose;    ///< Bind-pose orientations/positions.
};

//----------------------------------------------------------------------------------------------------------------------
// Describes hierarchy of bones etc.
class AttribDataRig : public AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataRig* init(NMP::Memory::Resource& resource, AnimRigDef* rig, uint16_t refCount = 0);

  NM_INLINE AttribDataRig() { setType(ATTRIB_TYPE_RIG); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_RIG; }

  static void locate(AttribData* target);
  static void dislocate(AttribData* target);
  static void prepForSPU(AttribData* target, NMP::MemoryAllocator* allocator);

  AnimRigDef*          m_rig;         ///< Pointer into shared array of resources.
  NMP::Memory::Format  m_rigMemReqs;  ///< Requirements of referenced rig.
};

//----------------------------------------------------------------------------------------------------------------------
// Rig functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AnimRigDef::getNumBones() const
{
  if (m_hierarchy)
    return m_hierarchy->getNumEntries();
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t AnimRigDef::getParentBoneIndex(int32_t index) const
{
  NMP_ASSERT(m_hierarchy);
  return m_hierarchy->getParentIndex(index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const char* AnimRigDef::getBoneName(uint32_t index) const
{
  NMP_ASSERT(m_boneNameMap);
  return m_boneNameMap->getStringForID(index);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Vector3* AnimRigDef::getBindPoseBonePos(uint32_t boneIndex) const
{
  NMP_ASSERT(boneIndex < m_hierarchy->getNumEntries());
  NMP_ASSERT(boneIndex < m_bindPose->m_transformBuffer->getLength());
  return m_bindPose->m_transformBuffer->getPosQuatChannelPos(boneIndex);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE const NMP::Quat* AnimRigDef::getBindPoseBoneQuat(uint32_t boneIndex) const
{
  NMP_ASSERT(boneIndex < m_hierarchy->getNumEntries());
  NMP_ASSERT(boneIndex < m_bindPose->m_transformBuffer->getLength());
  return m_bindPose->m_transformBuffer->getPosQuatChannelQuat(boneIndex);
}

//----------------------------------------------------------------------------------------------------------------------
bool locateAnimRigDef(uint32_t assetType, void* assetMemory);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_ANIM_RIG_DEF_H
//----------------------------------------------------------------------------------------------------------------------
