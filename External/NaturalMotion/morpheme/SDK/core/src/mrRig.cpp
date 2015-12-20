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
#include "NMPlatform/NMHierarchy.h"
#include "morpheme/mrRig.h"
#include "morpheme/mrManager.h"
#include "sharedDefines/mAnimDebugInterface.h"

#ifdef NM_HOST_CELL_SPU
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
bool AnimRigDef::locate()
{
  NMP::endianSwap(m_trajectoryBoneIndex);
  NMP::endianSwap(m_characterRootBoneIndex);
  NMP::endianSwap(m_blendFrameOrientation);
  NMP::endianSwap(m_blendFrameTranslation);

  REFIX_SWAP_PTR(NMP::Hierarchy, m_hierarchy);
  m_hierarchy->locate();

  REFIX_SWAP_PTR(NMP::OrderedStringTable, m_boneNameMap);
  m_boneNameMap->locate();

  REFIX_SWAP_PTR(AttribDataTransformBuffer, m_bindPose);
  AttribDataTransformBuffer::locate(m_bindPose);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimRigDef::dislocate()
{
  AttribDataTransformBuffer::dislocate(m_bindPose);
  UNFIX_SWAP_PTR(AttribDataTransformBuffer, m_bindPose);

  NMP::endianSwap(m_trajectoryBoneIndex);
  NMP::endianSwap(m_characterRootBoneIndex);
  NMP::endianSwap(m_blendFrameOrientation);
  NMP::endianSwap(m_blendFrameTranslation);

  // Dislocate the member hierarchy.
  m_hierarchy->dislocate();
  UNFIX_SWAP_PTR(NMP::Hierarchy, m_hierarchy);

  m_boneNameMap->dislocate();
  UNFIX_SWAP_PTR(NMP::OrderedStringTable, m_boneNameMap);

  return true;
}
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void AnimRigDef::relocate()
{
  // Rig class itself.
  void* ptr = this;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AnimRigDef));

  // Hierarchy.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_hierarchy = (NMP::Hierarchy*) ptr;
  m_hierarchy->relocate();
  NMP::Memory::Format format = NMP::Hierarchy::getMemoryRequirements(m_hierarchy->getNumEntries());
  ptr = (void*)(((size_t)ptr) + format.size);

  // Bone name map.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  m_boneNameMap = (NMP::OrderedStringTable*) ptr;
  m_boneNameMap->relocate();
  format = m_boneNameMap->getInstanceMemoryRequirements();
  ptr = (void*)(((size_t)ptr) + format.size);
  ptr = (void*) NMP::Memory::align(ptr, MR_TRANSFORM_ATTRIB_ALIGNMENT);

  // Bind pose.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  m_bindPose = (AttribDataTransformBuffer*) ptr;
  AttribDataTransformBuffer::relocate(m_bindPose);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimRigDef::getInstanceMemoryRequirements() const
{
  NMP::Memory::Format result(sizeof(AnimRigDef), MR_TRANSFORM_ATTRIB_ALIGNMENT);

  // m_hierarchy.
  result += NMP::Hierarchy::getMemoryRequirements(m_hierarchy->getNumEntries());

  // m_boneNameMap, bone name string table.
  result += m_boneNameMap->getInstanceMemoryRequirements();

  // m_bindPose.
  result += m_bindPose->getInstanceMemoryRequirements();

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimRigDef::serializeTx(
  AnimSetIndex animSetIndex,
  void* outputBuffer,
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(AnimRigDefPersistentData);
  dataSize += (sizeof(NMP::PosQuat) * sizeof(uint32_t)) * m_hierarchy->getNumEntries();

  NMP::DataBuffer* bindPoseBuffer = m_bindPose->m_transformBuffer;
  if(!bindPoseBuffer)
  {
    return 0;
  }

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AnimRigDefPersistentData* outputData = (AnimRigDefPersistentData*)outputBuffer;
    outputData->m_animSetIndex = animSetIndex;
    outputData->m_numJoints = m_hierarchy->getNumEntries();

    uint32_t* outputParents = outputData->getParents();
    NMP::PosQuat* outputBindPose = outputData->getBindPose();
    for (uint32_t i =0; i < outputData->m_numJoints ; ++i)
    {
      outputParents[i] = m_hierarchy->getParentIndex(i);
      NMP::netEndianSwap(outputParents[i]);

      outputBindPose[i].m_pos = *bindPoseBuffer->getChannelPos(i);
      outputBindPose[i].m_quat = *bindPoseBuffer->getChannelQuat(i);
      NMP::netEndianSwap(outputBindPose[i]);
    }

    NMP::netEndianSwap(outputData->m_animSetIndex);
    NMP::netEndianSwap(outputData->m_numJoints);
  }
  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t AnimRigDef::getBoneIndexFromName(const char* boneName) const
{
  uint32_t boneID;

  NMP_ASSERT(m_boneNameMap);
  boneID = m_boneNameMap->getIDForString(boneName);

  if (boneID == NMP_STRING_NOT_FOUND)
    return BONE_NAME_NOT_FOUND;

  return boneID;
}

//----------------------------------------------------------------------------------------------------------------------
const char* AnimRigDef::getBoneNameFromIndex(uint32_t boneID) const
{
  NMP_ASSERT(m_boneNameMap);
  return m_boneNameMap->getStringForID(boneID);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataRig functions
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataRig::locate(AttribData* target)
{
  AttribDataRig* result = (AttribDataRig*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_rigMemReqs);
  NMP::endianSwap(result->m_rig);
  result->m_rig = (MR::AnimRigDef*)Manager::getInstance().getObjectPtrFromObjectID((ObjectID)result->m_rig);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRig::dislocate(AttribData* target)
{
  AttribDataRig* result = (AttribDataRig*)target;

  AttribData::dislocate(target);

  NMP::endianSwap(result->m_rigMemReqs);

  result->m_rig = (MR::AnimRigDef*)Manager::getInstance().getObjectIDFromObjectPtr(result->m_rig);
  NMP_ASSERT((ObjectID)result->m_rig != INVALID_OBJECT_ID);
  NMP::endianSwap(result->m_rig);
}
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_HOST_CELL_SPU
void AttribDataRig::prepForSPU(AttribData* target, NMP::MemoryAllocator* allocator)
{
  AttribDataRig* result = (AttribDataRig*)target;

  // Allocate local memory to store the referenced rig in.
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result->m_rigMemReqs);
  NMP_SPU_ASSERT(resource.ptr);

  // DMA rig from system mem to locally allocated result->m_rig.
  NMP::SPUDMAController::dmaGet(resource.ptr, (uint32_t)result->m_rig, result->m_rigMemReqs.size);
  NMP::SPUDMAController::dmaWaitAll();

  // Set the new rig pointer.
  result->m_rig = (AnimRigDef*)resource.ptr;

  // Sort out all internal pointers in the rig.
  result->m_rig->relocate();
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRig::getMemoryRequirements()
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(AttribDataRig), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRig* AttribDataRig::init(NMP::Memory::Resource& resource, AnimRigDef* rig, uint16_t refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);

  // Placement new the resource to ensure the base constructor is called.
  NMP::Memory::Format memReq = getMemoryRequirements();
  AttribDataRig* result = (AttribDataRig*)resource.alignAndIncrement(memReq);

  result->setType(ATTRIB_TYPE_RIG);
  result->m_rig = rig;
  result->setRefCount(refCount);

  // Store the memory requirements of the referenced rig asset (Needed when relocating this asset in a new memory space).
  result->m_rigMemReqs = rig->getInstanceMemoryRequirements();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
bool locateAnimRigDef(uint32_t NMP_USED_FOR_ASSERTS(assetType), void* assetMemory)
{
  NMP_ASSERT(assetType == MR::Manager::kAsset_Rig);
  MR::AnimRigDef* rig = (MR::AnimRigDef*)assetMemory;
  return rig->locate();
}
#endif

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
