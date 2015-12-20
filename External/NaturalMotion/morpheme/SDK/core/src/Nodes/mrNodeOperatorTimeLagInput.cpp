// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/Nodes/mrNodeOperatorTimeLagInput.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Explicitly specialised versions of the nodeOperatorTimeLagInputOutputCPUpdate template function.
template AttribData* nodeOperatorTimeLagInputOutputCPUpdate<float, AttribDataFloat, ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT> (
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net);
template AttribData* nodeOperatorTimeLagInputOutputCPUpdate<int32_t, AttribDataInt, ATTRIB_TYPE_TIME_LAG_INPUT_INT> (
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net);
template AttribData* nodeOperatorTimeLagInputOutputCPUpdate<NMP::Vector3, AttribDataVector3, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3> (
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net);
template AttribData* nodeOperatorTimeLagInputOutputCPUpdate<NMP::Quat, AttribDataVector4, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4> (
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net);
template AttribData* nodeOperatorTimeLagInputOutputCPUpdate<bool, AttribDataBool, ATTRIB_TYPE_TIME_LAG_INPUT_BOOL> (
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Explicitly specialised versions of the nodeOperatorTimeLagInputInitInstance template function.
template void nodeOperatorTimeLagInputInitInstance<float, AttribDataFloat, ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT> (NodeDef* node, Network* net);
template void nodeOperatorTimeLagInputInitInstance<int32_t, AttribDataInt, ATTRIB_TYPE_TIME_LAG_INPUT_INT> (NodeDef* node, Network* net);
template void nodeOperatorTimeLagInputInitInstance<NMP::Vector3, AttribDataVector3, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR3> (NodeDef* node, Network* net);
template void nodeOperatorTimeLagInputInitInstance<NMP::Quat, AttribDataVector4, ATTRIB_TYPE_TIME_LAG_INPUT_VECTOR4> (NodeDef* node, Network* net);
template void nodeOperatorTimeLagInputInitInstance<bool, AttribDataBool, ATTRIB_TYPE_TIME_LAG_INPUT_BOOL> (NodeDef* node, Network* net);

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTimeLagInputDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTimeLagInputDef::getMemoryRequirements()
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataTimeLagInputDef), MR_ATTRIB_DATA_ALIGNMENT);
    
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTimeLagInputDef* AttribDataTimeLagInputDef::init(
  NMP::Memory::Resource& resource,
  BufferingMode          bufferingMode,
  float                  sampleFrequency, 
  uint32_t               sampleBufferSize,
  uint16_t               refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataTimeLagInputDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataTimeLagInputDef* result = (AttribDataTimeLagInputDef*) resource.alignAndIncrement(memReqsHdr);

  if (sampleBufferSize == 0)
    sampleBufferSize = 32;
  if (sampleFrequency <= 0.0f)
    sampleFrequency = 10.0f;
  
  result->setType(ATTRIB_TYPE_TIME_LAG_DEF);
  result->setRefCount(refCount);
  
  result->m_bufferingMode = bufferingMode;

  result->m_sampleFrequency = sampleFrequency;
  result->m_sampleBufferSize = sampleBufferSize;
    
  result->m_maximumSamplingPeriod =  ((float)sampleFrequency) * sampleBufferSize;
  result->m_sampleInterval = 0.0f;
  result->m_sampleInterval = 1.0f / ((float)sampleFrequency);
        
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTimeLagInputDef::create(
  NMP::MemoryAllocator* allocator,
  BufferingMode         bufferingMode,
  float                 sampleFrequency, 
  uint32_t              sampleBufferSize,
  uint16_t              refCount)
{
  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);

  AttribDataHandle result;
  result.m_attribData = init(resource, bufferingMode, sampleFrequency, sampleBufferSize, refCount);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTimeLagInputDef::locate(AttribData* target)
{
  AttribDataTimeLagInputDef* result = (AttribDataTimeLagInputDef*) target;

  // Header
  AttribData::locate(target);

  NMP::endianSwap(result->m_bufferingMode);
  NMP::endianSwap(result->m_sampleFrequency);
  NMP::endianSwap(result->m_sampleInterval);
  NMP::endianSwap(result->m_maximumSamplingPeriod);
  NMP::endianSwap(result->m_sampleBufferSize);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTimeLagInputDef::dislocate(AttribData* target)
{
  AttribDataTimeLagInputDef* result = (AttribDataTimeLagInputDef*) target;

  NMP::endianSwap(result->m_bufferingMode);
  NMP::endianSwap(result->m_sampleFrequency);
  NMP::endianSwap(result->m_sampleInterval);
  NMP::endianSwap(result->m_maximumSamplingPeriod);
  NMP::endianSwap(result->m_sampleBufferSize);
  
  AttribData::dislocate(target);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
