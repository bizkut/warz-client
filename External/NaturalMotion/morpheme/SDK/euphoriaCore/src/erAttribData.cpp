// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "euphoria/erAttribData.h"
#include "euphoria/erBodyDef.h"
#include "euphoria/erCharacter.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBodyDef functions
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataBodyDef::locate(AttribData* target)
{
  AttribDataBodyDef* result = (AttribDataBodyDef*)target;
  AttribData::locate(target);
  NMP::endianSwap(result->m_rigMemReqs);
  NMP::endianSwap(result->m_bodyDef);
  if (result->m_bodyDef)
    result->m_bodyDef = (ER::BodyDef*)MR::Manager::getInstance().getObjectPtrFromObjectID((MR::ObjectID)result->m_bodyDef);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBodyDef::dislocate(AttribData* target)
{
  AttribDataBodyDef* result = (AttribDataBodyDef*)target;
  AttribData::dislocate(target);
  NMP::endianSwap(result->m_rigMemReqs);
  if (result->m_bodyDef)
    result->m_bodyDef = (ER::BodyDef*)MR::Manager::getInstance().getObjectIDFromObjectPtr(result->m_bodyDef);
  NMP::endianSwap(result->m_bodyDef);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBodyDef::getMemoryRequirements()
{
  return NMP::Memory::Format(NMP::Memory::align(
    sizeof(AttribDataBodyDef), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBodyDef* AttribDataBodyDef::init(NMP::Memory::Resource& resource, ER::BodyDef* bodyDef)
{
  NMP::Memory::Format format = getMemoryRequirements();
  AttribDataBodyDef* result = (AttribDataBodyDef*)resource.alignAndIncrement(format);

  result->setRefCount(MR::IS_DEF_ATTRIB_DATA);
  result->setType((MR::AttribDataType)ATTRIB_TYPE_BODY_DEF);
  result->m_bodyDef = bodyDef;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataInteractionProxyDef functions
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataInteractionProxyDef::locate(AttribData* target)
{
  AttribDataInteractionProxyDef* result = (AttribDataInteractionProxyDef*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_interactionProxyDef);

  // Interaction Proxy is optional.
  if (result->m_interactionProxyDef)
  {
    result->m_interactionProxyDef =
      (ER::InteractionProxyDef*)
      MR::Manager::getInstance().getObjectPtrFromObjectID((MR::ObjectID)result->m_interactionProxyDef);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataInteractionProxyDef::dislocate(AttribData* target)
{
  AttribDataInteractionProxyDef* result = (AttribDataInteractionProxyDef*)target;

  AttribData::dislocate(target);

  // Interaction Proxy is optional.
  if (result->m_interactionProxyDef)
  {
    result->m_interactionProxyDef =
      (ER::InteractionProxyDef*)MR::Manager::getInstance().getObjectIDFromObjectPtr(result->m_interactionProxyDef);
  }

  NMP::endianSwap(result->m_interactionProxyDef);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataInteractionProxyDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(
    sizeof(AttribDataInteractionProxyDef), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataInteractionProxyDef* AttribDataInteractionProxyDef::init(
  NMP::Memory::Resource&   resource,
  ER::InteractionProxyDef* interactionProxy)
{
  NMP::Memory::Format format = getMemoryRequirements();
  AttribDataInteractionProxyDef* result = (AttribDataInteractionProxyDef*)resource.alignAndIncrement(format);

  result->setRefCount(MR::IS_DEF_ATTRIB_DATA);
  result->setType(ATTRIB_TYPE_INTERACTION_PROXY_DEF);
  result->m_interactionProxyDef = interactionProxy;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataCharacter functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataCharacter::locate(AttribData* target)
{
  AttribDataCharacter* result = (AttribDataCharacter*)target;

  AttribData::locate(target);

  result->m_character = 0; // Gets set later
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataCharacter::dislocate(AttribData* target)
{
  AttribDataCharacter* result = (AttribDataCharacter*)target;

  AttribData::dislocate(result);
  result->m_character = 0; // Gets set later
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataCharacter::getMemoryRequirements()
{
  return NMP::Memory::Format(sizeof(AttribDataCharacter), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataCharacter* AttribDataCharacter::create(
  NMP::MemoryAllocator*    allocator,
  Character*               character,
  uint16_t                 refCount)
{
  AttribDataCharacter* result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  result = init(resource, character, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataCharacter* AttribDataCharacter::init(
  NMP::Memory::Resource& resource,
  Character*             character,
  uint16_t                refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  AttribDataCharacter* result = (AttribDataCharacter*)resource.alignAndIncrement(format);

  result->setType(ATTRIB_TYPE_CHARACTER);
  result->setRefCount(refCount);
  result->m_character = character;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void networkSetCharacter(MR::Network* network, Character* character)
{
  MR::AttribAddress attribAddress(ATTRIB_SEMANTIC_CHARACTER, 0, MR::INVALID_NODE_ID);
  MR::NodeBinEntry* nodeBinEntry = network->getAttribDataNodeBinEntry(attribAddress);
  if (!nodeBinEntry)
  {
    AttribDataCharacter* attribDataCharacter = AttribDataCharacter::create(
      network->getPersistentMemoryAllocator(),
      character);

    MR::AttribDataHandle handle = { attribDataCharacter, AttribDataCharacter::getMemoryRequirements() };
    network->addAttribData(attribAddress, handle, MR::LIFESPAN_FOREVER);
  }
  else
  {
    AttribDataCharacter* attribDataCharacter =
      (AttribDataCharacter*) nodeBinEntry->m_attribDataHandle.m_attribData;

    // Make sure the character/euphoria references are clean.
    if (character && attribDataCharacter->m_character != character)
    {
      character->reset();
    }
    attribDataCharacter->m_character = character;
  }
}

//----------------------------------------------------------------------------------------------------------------------
Character* networkGetCharacter(MR::Network* network)
{
  MR::AttribAddress attribAddress(ATTRIB_SEMANTIC_CHARACTER, 0, MR::INVALID_NODE_ID);
  MR::NodeBinEntry* nodeBinEntry = network->getAttribDataNodeBinEntry(attribAddress);
  if (!nodeBinEntry)
  {
    return NULL;
  }
  else
  {
    AttribDataCharacter* character =
      (AttribDataCharacter*) nodeBinEntry->m_attribDataHandle.m_attribData;
    return character->m_character;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBehaviourParameters functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataBehaviourParameters::locate(MR::AttribData* target)
{
  AttribDataBehaviourParameters* result = (AttribDataBehaviourParameters*)target;
  MR::AttribData::locate(target);

  NMP::endianSwap(result->m_numInputCPInts);
  NMP::endianSwap(result->m_numInputCPFloats);
  NMP::endianSwap(result->m_numInputCPVector3s);
  NMP::endianSwap(result->m_numInputCPUInt64s);

  NMP::endianSwap(result->m_ints);
  REFIX_PTR_RELATIVE(MR::AttribDataIntArray, result->m_ints, result);
  MR::AttribDataIntArray::locate(result->m_ints);

  NMP::endianSwap(result->m_floats);
  REFIX_PTR_RELATIVE(MR::AttribDataFloatArray, result->m_floats, result);
  MR::AttribDataFloatArray::locate(result->m_floats);

  NMP::endianSwap(result->m_uint64s);
  REFIX_PTR_RELATIVE(MR::AttribDataUInt64Array, result->m_uint64s, result);
  MR::AttribDataUInt64Array::locate(result->m_uint64s);

  NMP::endianSwap(result->m_controlParameterInputsTypes);
  REFIX_PTR_RELATIVE(MR::AttribDataIntArray, result->m_controlParameterInputsTypes, result);
  MR::AttribDataIntArray::locate(result->m_controlParameterInputsTypes);

  NMP::endianSwap(result->m_controlParameterOutputsTypes);
  REFIX_PTR_RELATIVE(MR::AttribDataIntArray, result->m_controlParameterOutputsTypes, result);
  MR::AttribDataIntArray::locate(result->m_controlParameterOutputsTypes);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBehaviourParameters::dislocate(MR::AttribData* target)
{
  AttribDataBehaviourParameters* result = (AttribDataBehaviourParameters*)target;
  
  NMP::endianSwap(result->m_numInputCPInts);
  NMP::endianSwap(result->m_numInputCPFloats);
  NMP::endianSwap(result->m_numInputCPVector3s);
  NMP::endianSwap(result->m_numInputCPUInt64s);

  MR::AttribDataIntArray::dislocate(result->m_ints);
  UNFIX_PTR_RELATIVE(MR::AttribDataIntArray, result->m_ints, result);
  NMP::endianSwap(result->m_ints);

  MR::AttribDataFloatArray::dislocate(result->m_floats);
  UNFIX_PTR_RELATIVE(MR::AttribDataFloatArray, result->m_floats, result);
  NMP::endianSwap(result->m_floats);

  MR::AttribDataUInt64Array::dislocate(result->m_uint64s);
  UNFIX_PTR_RELATIVE(MR::AttribDataUInt64Array, result->m_uint64s, result);
  NMP::endianSwap(result->m_uint64s);

  MR::AttribDataIntArray::dislocate(result->m_controlParameterInputsTypes);
  UNFIX_PTR_RELATIVE(MR::AttribDataIntArray, result->m_controlParameterInputsTypes, result);
  NMP::endianSwap(result->m_controlParameterInputsTypes);

  MR::AttribDataIntArray::dislocate(result->m_controlParameterOutputsTypes);
  UNFIX_PTR_RELATIVE(MR::AttribDataIntArray, result->m_controlParameterOutputsTypes, result);
  NMP::endianSwap(result->m_controlParameterOutputsTypes);

  MR::AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBehaviourParameters::relocate(MR::AttribData* target, void* location)
{
  AttribDataBehaviourParameters* result = (AttribDataBehaviourParameters*) target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_ATTRIB_DATA_ALIGNMENT));

  size_t offset = NMP::Memory::align(sizeof(AttribDataBehaviourParameters), MR_ATTRIB_DATA_ALIGNMENT);
  NMP::Memory::Format format;
  
  MR::AttribDataIntArray* ints = (MR::AttribDataIntArray*)(((size_t)result) + offset);
  result->m_ints = (MR::AttribDataIntArray*)(((size_t)location) + offset);
  MR::AttribDataIntArray::relocate(ints, result->m_ints);
  format = MR::AttribDataIntArray::getMemoryRequirements(result->m_ints->m_numValues);
  offset += NMP::Memory::align(format.size, MR_ATTRIB_DATA_ALIGNMENT);

  MR::AttribDataFloatArray* floats = (MR::AttribDataFloatArray*)(((size_t)result) + offset);
  result->m_floats = (MR::AttribDataFloatArray*)(((size_t)location) + offset);
  MR::AttribDataFloatArray::relocate(floats, result->m_floats);
  format = MR::AttribDataFloatArray::getMemoryRequirements(result->m_floats->m_numValues);
  offset += NMP::Memory::align(format.size, MR_ATTRIB_DATA_ALIGNMENT);

  MR::AttribDataUInt64Array* uint64s = (MR::AttribDataUInt64Array*)(((size_t)result) + offset);
  result->m_uint64s = (MR::AttribDataUInt64Array*)(((size_t)location) + offset);
  MR::AttribDataUInt64Array::relocate(uint64s, result->m_uint64s);
  format = MR::AttribDataUInt64Array::getMemoryRequirements(result->m_uint64s->m_numValues);
  offset += NMP::Memory::align(format.size, MR_ATTRIB_DATA_ALIGNMENT);

  MR::AttribDataIntArray* controlParameterInputsTypes = (MR::AttribDataIntArray*)(((size_t)result) + offset);
  result->m_controlParameterInputsTypes = (MR::AttribDataIntArray*)(((size_t)location) + offset);
  MR::AttribDataIntArray::relocate(controlParameterInputsTypes, result->m_controlParameterInputsTypes);
  format = MR::AttribDataIntArray::getMemoryRequirements(result->m_controlParameterInputsTypes->m_numValues);
  offset += NMP::Memory::align(format.size, MR_ATTRIB_DATA_ALIGNMENT);

  MR::AttribDataIntArray* controlParameterOutputsTypes = (MR::AttribDataIntArray*)(((size_t)result) + offset);
  result->m_controlParameterOutputsTypes = (MR::AttribDataIntArray*)(((size_t)location) + offset);
  MR::AttribDataIntArray::relocate(controlParameterOutputsTypes, result->m_controlParameterOutputsTypes);
  format = MR::AttribDataIntArray::getMemoryRequirements(result->m_controlParameterOutputsTypes->m_numValues);
  offset += NMP::Memory::align(format.size, MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBehaviourParameters* AttribDataBehaviourParameters::create(
  NMP::MemoryAllocator* allocator,
  uint32_t numInts,
  uint32_t numFloats,
  uint32_t numUInt64s,
  uint32_t numInputCPInts,
  uint32_t numInputCPFloats,
  uint32_t numInputCPUInt64s,
  uint32_t numInputCPVector3s,
  uint32_t numControlParameterInputs,
  uint32_t numControlParameterOutputs,
  uint16_t refCount)
{
  AttribDataBehaviourParameters* result;

  NMP::Memory::Format memReqs = getMemoryRequirements(
    numInts, numFloats, numUInt64s, numControlParameterInputs, numControlParameterOutputs);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  result = init(
    resource,
    numInts,
    numFloats,
    numUInt64s,
    numInputCPInts,
    numInputCPFloats,
    numInputCPUInt64s,
    numInputCPVector3s,
    numControlParameterInputs,
    numControlParameterOutputs,
    refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBehaviourParameters::getMemoryRequirements(
  uint32_t numInts,
  uint32_t numFloats,
  uint32_t numUint64s,
  uint32_t numControlParameterInputs,
  uint32_t numControlParameterOutputs)
{
  NMP::Memory::Format result(sizeof(AttribDataBehaviourParameters), MR_ATTRIB_DATA_ALIGNMENT);

  result += MR::AttribDataIntArray::getMemoryRequirements(numInts);
  result += MR::AttribDataFloatArray::getMemoryRequirements(numFloats);
  result += MR::AttribDataUInt64Array::getMemoryRequirements(numUint64s);

  result += MR::AttribDataIntArray::getMemoryRequirements(numControlParameterInputs);  // types
  result += MR::AttribDataIntArray::getMemoryRequirements(numControlParameterOutputs); // types

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBehaviourParameters* AttribDataBehaviourParameters::init(
  NMP::Memory::Resource& resource,
  uint32_t numInts,
  uint32_t numFloats,
  uint32_t numUInt64s,
  uint32_t numInputCPInts,
  uint32_t numInputCPFloats,
  uint32_t numInputCPUInt64s,
  uint32_t numInputCPVector3s,
  uint32_t numControlParameterInputs,
  uint32_t numControlParameterOutputs,
  uint16_t refCount)
{
  NMP_ASSERT(getMemoryRequirements(
    numInts, numFloats, numUInt64s, numControlParameterInputs, numControlParameterOutputs).alignment == MR_ATTRIB_DATA_ALIGNMENT);

  NMP::Memory::Format headerFormat(sizeof(AttribDataBehaviourParameters), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBehaviourParameters* result = (AttribDataBehaviourParameters*)resource.alignAndIncrement(headerFormat);

  result->setType(ATTRIB_TYPE_BEHAVIOUR_PARAMETERS);
  result->setRefCount(refCount);

  result->m_ints   = MR::AttribDataIntArray::init(resource, numInts);
  result->m_floats = MR::AttribDataFloatArray::init(resource, numFloats);
  result->m_uint64s = MR::AttribDataUInt64Array::init(resource, numUInt64s);

  result->m_numInputCPInts = numInputCPInts;
  result->m_numInputCPFloats = numInputCPFloats;
  result->m_numInputCPUInt64s = numInputCPUInt64s;
  result->m_numInputCPVector3s = numInputCPVector3s;
  result->m_controlParameterInputsTypes = MR::AttribDataIntArray::init(resource, numControlParameterInputs);
  result->m_controlParameterOutputsTypes = MR::AttribDataIntArray::init(resource, numControlParameterOutputs);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBehaviourState functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
AttribDataBehaviourState* AttribDataBehaviourState::create(
  NMP::MemoryAllocator* allocator,
  uint32_t numInts,
  uint32_t numFloats,
  uint32_t numUInt64s,
  uint32_t numVector3s,
  uint16_t refCount
  )
{
  AttribDataBehaviourState* result;

  NMP::Memory::Format memReqs = getMemoryRequirements(numInts, numFloats, numUInt64s, numVector3s);

  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
  result = init(
    resource,
    numInts,
    numFloats,
    numUInt64s,
    numVector3s,
    refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBehaviourState::getMemoryRequirements(
    uint32_t numInts,
    uint32_t numFloats,
    uint32_t numUInt64s,
    uint32_t numVector3s)
{
  NMP::Memory::Format result(sizeof(AttribDataBehaviourState), MR_ATTRIB_DATA_ALIGNMENT);

  result += MR::AttribDataIntArray::getMemoryRequirements(numInts);
  result += MR::AttribDataFloatArray::getMemoryRequirements(numFloats);
  result += MR::AttribDataUInt64Array::getMemoryRequirements(numUInt64s);
  result += MR::AttribDataFloatArray::getMemoryRequirements(numVector3s * 4);
  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBehaviourState* AttribDataBehaviourState::init(
    NMP::Memory::Resource& resource,
    uint32_t numInts,
    uint32_t numFloats,
    uint32_t numUInt64s,
    uint32_t numVector3s,
    uint16_t refCount)
{
  NMP::Memory::Format headerFormat(sizeof(AttribDataBehaviourState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBehaviourState* result = (AttribDataBehaviourState*)resource.alignAndIncrement(headerFormat);

  result->setType(ATTRIB_TYPE_BEHAVIOUR_STATE);
  result->setRefCount(refCount);

  result->m_startedBehaviour = false;
  result->m_ints             = MR::AttribDataIntArray::init(resource, numInts);
  result->m_floats           = MR::AttribDataFloatArray::init(resource, numFloats);
  result->m_uint64s          = MR::AttribDataUInt64Array::init(resource, numUInt64s);
  result->m_vector3Data      = MR::AttribDataFloatArray::init(resource, numVector3s * 4);
  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBehaviourSetup functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataBehaviourSetup::locate(AttribData* target)
{
  AttribDataBehaviourSetup* result = (AttribDataBehaviourSetup*)target;
  AttribData::locate(target);
  NMP::endianSwap(result->m_behaviourID);

  NMP::endianSwap(result->m_childNodeIDs);
  REFIX_PTR_RELATIVE(MR::AttribDataIntArray, result->m_childNodeIDs, result);
  MR::AttribDataIntArray::locate(result->m_childNodeIDs);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBehaviourSetup::dislocate(AttribData* target)
{
  AttribDataBehaviourSetup* result = (AttribDataBehaviourSetup*)target;

  MR::AttribDataIntArray::dislocate(result->m_childNodeIDs);
  UNFIX_PTR_RELATIVE(MR::AttribDataIntArray, result->m_childNodeIDs, result);
  NMP::endianSwap(result->m_childNodeIDs);

  NMP::endianSwap(result->m_behaviourID);
  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBehaviourSetup::getMemoryRequirements(uint32_t numChildNodeIDs)
{
  NMP::Memory::Format result(sizeof(AttribDataBehaviourSetup), MR_ATTRIB_DATA_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  result += MR::AttribDataIntArray::getMemoryRequirements(numChildNodeIDs);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBehaviourSetup* AttribDataBehaviourSetup::init(
  NMP::Memory::Resource& resource,
  uint32_t               numChildNodeIDs,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements(numChildNodeIDs).alignment == MR_ATTRIB_DATA_ALIGNMENT);
  NMP::Memory::Format headerFormat(sizeof(AttribDataBehaviourSetup), MR_ATTRIB_DATA_ALIGNMENT);
  headerFormat.size = NMP::Memory::align(headerFormat.size, MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBehaviourSetup* result = (AttribDataBehaviourSetup*)resource.alignAndIncrement(headerFormat);

  result->setType(ATTRIB_TYPE_BEHAVIOUR_SETUP);
  result->setRefCount(refCount);

  result->m_childNodeIDs = MR::AttribDataIntArray::init(resource, numChildNodeIDs);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
