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
#include "physics/mrPhysicsAttribData.h"
#include "physics/mrPhysicsRigDef.h"
#include "morpheme/mrManager.h"
#include <string.h>
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsRig functions
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsRig::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(AttribDataPhysicsRig), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPhysicsRig::create(
  NMP::MemoryAllocator* allocator,
  PhysicsRig*           physicsRig,
  uint16_t              refCount)
{
  AttribDataHandle result;
  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);

  AttribDataPhysicsRig* attrib = (AttribDataPhysicsRig*)resource.alignAndIncrement(result.m_format);
  attrib->setType(ATTRIB_TYPE_PHYSICS_RIG);
  attrib->setRefCount(refCount);
  attrib->m_physicsRig = physicsRig;
  attrib->m_allocator = allocator; // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData = attrib;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsRigDef functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsRigDef::locate(AttribData* target)
{
  AttribDataPhysicsRigDef* result = (AttribDataPhysicsRigDef*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_physicsRigDef);

  if (result->m_physicsRigDef)
  {
    result->m_physicsRigDef = (MR::PhysicsRigDef*)Manager::getInstance().getObjectPtrFromObjectID((ObjectID)result->m_physicsRigDef);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsRigDef::dislocate(AttribData* target)
{
  AttribDataPhysicsRigDef* result = (AttribDataPhysicsRigDef*)target;

  AttribData::dislocate(target);

  // Physics rig is optional.
  if (result->m_physicsRigDef)
  {
    result->m_physicsRigDef = (MR::PhysicsRigDef*)Manager::getInstance().getObjectIDFromObjectPtr(result->m_physicsRigDef);
    NMP_ASSERT((ObjectID)result->m_physicsRigDef != INVALID_OBJECT_ID);
  }

  NMP::endianSwap(result->m_physicsRigDef);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsRigDef::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(AttribDataPhysicsRigDef), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsRigDef* AttribDataPhysicsRigDef::init(
  NMP::Memory::Resource& resource,
  PhysicsRigDef*         physicsRigDef)
{
  NMP::Memory::Format format = getMemoryRequirements();
  AttribDataPhysicsRigDef* result = (AttribDataPhysicsRigDef*)resource.alignAndIncrement(format);

  result->setRefCount(MR::IS_DEF_ATTRIB_DATA);
  result->setType(ATTRIB_TYPE_PHYSICS_RIG_DEF);
  result->m_physicsRigDef = physicsRigDef;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AnimToPhysicsMap functions
//----------------------------------------------------------------------------------------------------------------------
bool AnimToPhysicsMap::locate()
{
  NMP::endianSwap(m_mapAnimToPhysics);
  REFIX_PTR_RELATIVE(int32_t, m_mapAnimToPhysics, this);

  NMP::endianSwap(m_mapPhysicsToAnim);
  REFIX_PTR_RELATIVE(int32_t, m_mapPhysicsToAnim, this);

  NMP::endianSwap(m_numAnimChannels);
  NMP::endianSwap(m_numPhysicsChannels);

  // Fixup each of the actual int values.
  for (int32_t i = 0; i < m_numAnimChannels; ++i)
  {
    NMP::endianSwap(m_mapAnimToPhysics[i]);
  }
  for (int32_t i = 0; i < m_numPhysicsChannels; ++i)
  {
    NMP::endianSwap(m_mapPhysicsToAnim[i]);
  }

  NMP::endianSwap(m_bonesFilter);
  REFIX_PTR_RELATIVE(AttribDataFloatArray, m_bonesFilter, this);
  AttribDataFloatArray::locate(m_bonesFilter);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool AnimToPhysicsMap::dislocate()
{
  // Fixup each of the actual int values.
  for (int32_t i = 0; i < m_numAnimChannels; ++i)
  {
    NMP::endianSwap(m_mapAnimToPhysics[i]);
  }

  for (int32_t i = 0; i < m_numPhysicsChannels; ++i)
  {
    NMP::endianSwap(m_mapPhysicsToAnim[i]);
  }

  UNFIX_PTR_RELATIVE(int32_t, m_mapAnimToPhysics, this);
  NMP::endianSwap(m_mapAnimToPhysics);

  UNFIX_PTR_RELATIVE(int32_t, m_mapPhysicsToAnim, this);
  NMP::endianSwap(m_mapPhysicsToAnim);

  NMP::endianSwap(m_numAnimChannels);
  NMP::endianSwap(m_numPhysicsChannels);

  AttribDataFloatArray::dislocate(m_bonesFilter);
  UNFIX_PTR_RELATIVE(AttribDataFloatArray, m_bonesFilter, this);
  NMP::endianSwap(m_bonesFilter);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AnimToPhysicsMap::getMemoryRequirements(uint32_t numChannels, uint32_t numPhysicsChannels)
{
  NMP::Memory::Format result(sizeof(AnimToPhysicsMap), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the anim to physics map.
  result += NMP::Memory::Format(sizeof(int32_t) * numChannels, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for the physics to anim map.
  result += NMP::Memory::Format(sizeof(int32_t) * numPhysicsChannels, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for the bones filter.
  result += AttribDataFloatArray::getMemoryRequirements(numChannels);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AnimToPhysicsMap* AnimToPhysicsMap::init(
  NMP::Memory::Resource& resource,
  uint32_t               numChannels,
  uint32_t               numPhysicsChannels)
{
  NMP::Memory::Format format(sizeof(AnimToPhysicsMap), MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(format);
  AnimToPhysicsMap* result = (AnimToPhysicsMap*)resource.ptr;
  resource.increment(format);

  result->m_numAnimChannels = numChannels;
  result->m_numPhysicsChannels = numPhysicsChannels;

  // Array of int for the anim to physics map.
  format = NMP::Memory::Format(sizeof(int32_t) * result->m_numAnimChannels, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_mapAnimToPhysics = (int32_t*)resource.ptr;
  resource.increment(format);

  // Array of int for the physics to anim map.
  result->m_mapPhysicsToAnim = 0;
  if (result->m_numPhysicsChannels > 0)
  {
    format = NMP::Memory::Format(sizeof(int32_t) * result->m_numPhysicsChannels, NMP_NATURAL_TYPE_ALIGNMENT);
    resource.align(format);
    result->m_mapPhysicsToAnim = (int32_t*)resource.ptr;
    resource.increment(format);
  }

  for (uint32_t i = 0; i < (uint32_t)result->m_numAnimChannels; ++i)
  {
    result->m_mapAnimToPhysics[i] = -1;
  }

  for (uint32_t i = 0; i < (uint32_t)result->m_numPhysicsChannels; ++i)
  {
    result->m_mapPhysicsToAnim[i] = -1;
  }

  result->m_bonesFilter = MR::AttribDataFloatArray::init(resource, (uint32_t)result->m_numAnimChannels);

  for (uint32_t i = 0; i < (uint32_t)result->m_numAnimChannels; ++i)
  {
    result->m_bonesFilter->m_values[i] = 0.0f;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t AnimToPhysicsMap::getAnimIndexFromPhysicsIndex(int32_t physicsIndex) const
{
  if (physicsIndex >= 0 && physicsIndex < m_numPhysicsChannels)
  {
    return m_mapPhysicsToAnim[physicsIndex];
  }

  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t AnimToPhysicsMap::getPhysicsIndexFromAnimIndex(int32_t animIndex) const
{
  if (animIndex >= 0 && animIndex < m_numAnimChannels)
  {
    return m_mapAnimToPhysics[animIndex];
  }

  return -1;
}

//----------------------------------------------------------------------------------------------------------------------
void AnimToPhysicsMap::initializeBonesFilter()
{
  for (int32_t j = 0 ; j < m_numAnimChannels; ++j)
  {
    int32_t i = getPhysicsIndexFromAnimIndex(j);
    m_bonesFilter->m_values[j] = i != -1 ? 0.0f : 1.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AnimToPhysicsMap::makeMappingBetweenPhysicsAndAnim(int32_t physics, int32_t anim)
{
  m_mapPhysicsToAnim[physics] = anim;
  m_mapAnimToPhysics[anim] = physics;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataAnimToPhysicsMap functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataAnimToPhysicsMap::locate(AttribData* target)
{
  AttribDataAnimToPhysicsMap* result = (AttribDataAnimToPhysicsMap*)target;
  AttribData::locate(target);

  NMP::endianSwap(result->m_animToPhysicsMap);
  REFIX_PTR_RELATIVE(AnimToPhysicsMap, result->m_animToPhysicsMap, result);
  result->m_animToPhysicsMap->locate();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataAnimToPhysicsMap::dislocate(AttribData* target)
{
  AttribDataAnimToPhysicsMap* result = (AttribDataAnimToPhysicsMap*)target;
  AttribData::dislocate(target);

  result->m_animToPhysicsMap->dislocate();

  UNFIX_PTR_RELATIVE(AnimToPhysicsMap, result->m_animToPhysicsMap, result);
  NMP::endianSwap(result->m_animToPhysicsMap);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataAnimToPhysicsMap::getMemoryRequirements(uint32_t numChannels, uint32_t numPhysicsChannels)
{
  NMP::Memory::Format result(sizeof(AttribDataAnimToPhysicsMap), MR_ATTRIB_DATA_ALIGNMENT);

  result += AnimToPhysicsMap::getMemoryRequirements(numChannels, numPhysicsChannels);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataAnimToPhysicsMap* AttribDataAnimToPhysicsMap::init(
  NMP::Memory::Resource& resource,
  uint32_t               numChannels,
  uint32_t               numPhysicsChannels,
  uint16_t               refCount)
{
  NMP::Memory::Format format(sizeof(AttribDataAnimToPhysicsMap), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataAnimToPhysicsMap* result = (AttribDataAnimToPhysicsMap*)resource.alignAndIncrement(format);
  result->setRefCount(refCount);

  format = AnimToPhysicsMap::getMemoryRequirements(numChannels, numPhysicsChannels);
  resource.align(format);
  result->m_animToPhysicsMap = AnimToPhysicsMap::init(resource, numChannels, numPhysicsChannels);
  result->setType(ATTRIB_TYPE_ANIM_TO_PHYSICS_MAP);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTrajectoryOverrideDefAnimSet functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTrajectoryOverrideDefAnimSet::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataTrajectoryOverrideDefAnimSet), MR_ATTRIB_DATA_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTrajectoryOverrideDefAnimSet* AttribDataTrajectoryOverrideDefAnimSet::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataTrajectoryOverrideDefAnimSet* const result = 
    static_cast<AttribDataTrajectoryOverrideDefAnimSet*>(resource.ptr);
  resource.increment(sizeof(AttribDataTrajectoryOverrideDefAnimSet));

  result->setType(getDefaultType());
  result->setRefCount(refCount);

  result->m_directionInCharacterSpace.setToZero();
  result->m_projectOntoGroundDistance = 0.0f;
  result->m_physicsPartIndex = 0;
  result->m_projectOntoGround = false;

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTrajectoryOverrideDefAnimSet::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);

  AttribDataHandle result;
  result.m_attribData = init(resource, refCount);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// For Manager registration.
void AttribDataTrajectoryOverrideDefAnimSet::locate(AttribData* target)
{
  AttribData::locate(target);

  static_cast<AttribDataTrajectoryOverrideDefAnimSet*>(target)->endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTrajectoryOverrideDefAnimSet::dislocate(AttribData* target)
{
  static_cast<AttribDataTrajectoryOverrideDefAnimSet*>(target)->endianSwap();

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTrajectoryOverrideDefAnimSet::endianSwap()
{
  NMP::endianSwap(m_directionInCharacterSpace);
  NMP::endianSwap(m_projectOntoGroundDistance);
  NMP::endianSwap(m_physicsPartIndex);
  NMP::endianSwap(m_projectOntoGround);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCollisionFilterMask functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataCollisionFilterMask::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataCollisionFilterMask), MR_ATTRIB_DATA_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataCollisionFilterMask* AttribDataCollisionFilterMask::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataCollisionFilterMask* const result = static_cast<AttribDataCollisionFilterMask*>(resource.ptr);
  resource.increment(sizeof(AttribDataCollisionFilterMask));

  result->setType(getDefaultType());
  result->setRefCount(refCount);

  // Set to default ctor value.
  result->m_collisionFilterMask = MR::CollisionFilterMask();

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataCollisionFilterMask::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);

  AttribDataHandle result;
  result.m_attribData = init(resource, refCount);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
/// For Manager registration.
void AttribDataCollisionFilterMask::locate(AttribData* target)
{
  AttribData::locate(target);

  static_cast<AttribDataCollisionFilterMask*>(target)->endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCollisionFilterMask::dislocate(AttribData* target)
{
  static_cast<AttribDataCollisionFilterMask*>(target)->endianSwap();

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCollisionFilterMask::endianSwap()
{
  NMP::endianSwap(m_collisionFilterMask.m_word0);
  NMP::endianSwap(m_collisionFilterMask.m_word1);
  NMP::endianSwap(m_collisionFilterMask.m_word2);
  NMP::endianSwap(m_collisionFilterMask.m_word3);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
