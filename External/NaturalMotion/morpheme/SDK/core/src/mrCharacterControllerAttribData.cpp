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
#include "morpheme/mrCharacterControllerAttribData.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCharacterController functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataCharacterProperties::locate(AttribData* target)
{
  AttribDataCharacterProperties* result = (AttribDataCharacterProperties*)target;
  AttribData::locate(target);
 
  NMP::endianSwap(result->m_worldRootTransform);
  NMP::endianSwap(result->m_prevWorldRootTransform);
  NMP::endianSwap(result->m_groundContactTime);
  NMP::endianSwap(result->m_achievedRequestedMovement);
  NMP::endianSwap(result->m_physicsAndCharacterControllerUpdate);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCharacterProperties::dislocate(AttribData* target)
{
  AttribDataCharacterProperties* result = (AttribDataCharacterProperties*)target;
  AttribData::dislocate(target);
  
  NMP::endianSwap(result->m_worldRootTransform);
  NMP::endianSwap(result->m_prevWorldRootTransform);
  NMP::endianSwap(result->m_groundContactTime);
  NMP::endianSwap(result->m_achievedRequestedMovement);
  NMP::endianSwap(result->m_physicsAndCharacterControllerUpdate);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataCharacterProperties::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(
      sizeof(AttribDataCharacterProperties), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataCharacterProperties::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  AttribDataHandle result;
  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);

  AttribDataCharacterProperties* attrib = (AttribDataCharacterProperties*)resource.ptr;
  attrib->setType(ATTRIB_TYPE_CHARACTER_PROPERTIES);
  attrib->setRefCount(refCount);
  attrib->m_allocator = allocator; // Store the allocator so we know where to free this attribData from when we destroy it.

  attrib->m_worldRootTransform.identity();
  attrib->m_prevWorldRootTransform.identity();
  attrib->m_groundContactTime = 0.0f;
  attrib->m_achievedRequestedMovement = false;
  attrib->m_physicsAndCharacterControllerUpdate = MR::PHYSICS_AND_CC_UPDATE_SEPARATE;

  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  result.m_attribData = attrib;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_DEBUG
void AttribDataCharacterProperties::relocate(AttribData* target, void* location)
{
  (void)target;
  (void)location;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCharacterControllerDef functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataCharacterControllerDef::locate(AttribData* target)
{
  AttribDataCharacterControllerDef* result = (AttribDataCharacterControllerDef*)target;
  AttribData::locate(target);

  NMP::endianSwap(result->m_characterControllerDef);

  // Character controller is optional.
  if (result->m_characterControllerDef)
  {
    result->m_characterControllerDef =
      (MR::CharacterControllerDef*)
      Manager::getInstance().getObjectPtrFromObjectID((ObjectID)result->m_characterControllerDef);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCharacterControllerDef::dislocate(AttribData* target)
{
  AttribDataCharacterControllerDef* result = (AttribDataCharacterControllerDef*)target;
  AttribData::dislocate(target);
 
  // Character controller is optional.
  if (result->m_characterControllerDef)
  {
    result->m_characterControllerDef =
      (MR::CharacterControllerDef*)Manager::getInstance().getObjectIDFromObjectPtr(result->m_characterControllerDef);
    NMP_ASSERT((ObjectID)result->m_characterControllerDef != INVALID_OBJECT_ID);
  }

  NMP::endianSwap(result->m_characterControllerDef);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataCharacterControllerDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(
             sizeof(AttribDataCharacterControllerDef), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataCharacterControllerDef* AttribDataCharacterControllerDef::init(
  NMP::Memory::Resource&  resource,
  CharacterControllerDef* characterControllerDef)
{
  // Placement new the resource to ensure the base constructor is called.
  NMP::Memory::Format format = getMemoryRequirements();
  AttribDataCharacterControllerDef* result = (AttribDataCharacterControllerDef*)resource.alignAndIncrement(format);

  result->setRefCount(IS_DEF_ATTRIB_DATA);
  result->setType(ATTRIB_TYPE_CHARACTER_CONTROLLER_DEF);
  result->m_characterControllerDef = characterControllerDef;

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
