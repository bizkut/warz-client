// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrAttribAddress.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
#include "sharedDefines/mCoreDebugInterface.h"
#include "NMGeomUtils/NMJointLimits.h"
#ifndef NM_HOST_CELL_SPU
  #include <stdlib.h>
#endif // NM_HOST_CELL_SPU

#ifdef NM_HOST_CELL_SPU
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif // NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#ifndef NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribAddress functions.
//----------------------------------------------------------------------------------------------------------------------
const char* AttribAddress::getAttribSemanticName(AttribDataSemantic smt)
{
  return Manager::getInstance().getAttributeSemanticName(smt);
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// DataRef functions.
//----------------------------------------------------------------------------------------------------------------------
void* DataRef::getData(NMP::Memory::Format sectionMemFormat, NMP::MemoryAllocator* allocator)
{
#ifdef NM_HOST_CELL_SPU
  // Allocate local memory to store the referenced data in.
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, sectionMemFormat);
  NMP_SPU_ASSERT(resource.ptr != NULL);

  // DMA anim from system mem to locally allocated result->m_anim.
  NMP::SPUDMAController::dmaGet(resource.ptr, (uint32_t)m_data, sectionMemFormat.size);
  NMP::SPUDMAController::dmaWaitAll();

  // Set the new data pointer.
  m_data = resource.ptr;
#else // NM_HOST_CELL_SPU
  (void) sectionMemFormat;
  (void) allocator;
#endif // NM_HOST_CELL_SPU

  return m_data;
}

//----------------------------------------------------------------------------------------------------------------------
// Debug output.
//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_NETWORK_LOGGING)
void netLogOutputTaskParam(uint32_t MR_USED_FOR_NETWORK_LOGGING(priority), Task* MR_USED_FOR_NETWORK_LOGGING(task), uint32_t index)
{
  if (index == 0)
  {
    NET_LOG_MESSAGE(priority, "      Attribute,                                            owner, target, validFrame, in, out, lifespan\n");
  }
  TaskParameter* param = &(task->m_params[index]);
  NET_LOG_MESSAGE(
    priority,
    "    %3i: %43s (%3i), %5i, %6i,  %9i,  %i,   %i, %8i\n",
    index,
    param->m_attribAddress.getAttribSemanticName(),
    param->m_attribAddress.m_semantic,
    param->m_attribAddress.m_owningNodeID,
    param->m_attribAddress.m_targetNodeID,
    param->m_attribAddress.m_validFrame,
    param->m_taskParamFlags.isSet(TPARAM_FLAG_INPUT),
    param->m_taskParamFlags.isSet(TPARAM_FLAG_OUTPUT),
    param->m_lifespan);
}

//----------------------------------------------------------------------------------------------------------------------
void netLogOutputTaskParams(uint32_t priority, Task* task)
{
  NMP_ASSERT(task);
  NET_LOG_MESSAGE(priority, "   Task params are:\n");

  for (uint32_t i = 0; i < task->m_numParams; i++)
  {
    netLogOutputTaskParam(priority, task, i);
  }
}
#endif // defined(MR_NETWORK_LOGGING)

//----------------------------------------------------------------------------------------------------------------------
// AttribData functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribData::locate(AttribData* target)
{
  NMP::endianSwap(target->m_typeRefCount);
  target->m_allocator = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribData::dislocate(AttribData* target)
{
  NMP::endianSwap(target->m_typeRefCount);
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
// AttribDataBool functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataBool::locate(AttribData* target)
{
  AttribDataBool* result = (AttribDataBool*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBool::dislocate(AttribData* target)
{
  AttribDataBool* result = (AttribDataBool*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_value);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBool::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataBool), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBool* AttribDataBool::init(NMP::Memory::Resource& resource, bool value, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBool), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBool* result = (AttribDataBool*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_BOOL);
  result->setRefCount(refCount);
  result->m_value = value;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBool::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  bool value = true;
  
  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBool::create(
  NMP::MemoryAllocator* allocator,
  bool                  value,
  uint16_t              refCount)
{
  AttribDataHandle result;
 
  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBool::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBool::copy(const AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_BOOL && dest->getType() == ATTRIB_TYPE_BOOL);
  ((AttribDataBool*) dest)->m_value = ((AttribDataBool*) source)->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataBool::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(AttribBoolOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribBoolOutputData* output = (AttribBoolOutputData*) outputBuffer;
    output->m_value = ((MR::AttribDataBool*)toOutput)->m_value;
    NMP::netEndianSwap(output->m_value);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataUInt functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataUInt::locate(AttribData* target)
{
  AttribDataUInt* result = (AttribDataUInt*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUInt::dislocate(AttribData* target)
{
  AttribDataUInt* result = (AttribDataUInt*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_value);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataUInt::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataUInt), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataUInt* AttribDataUInt::init(NMP::Memory::Resource& resource, uint32_t value, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataUInt), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataUInt* result = (AttribDataUInt*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_UINT);
  result->setRefCount(refCount);
  result->m_value = value;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUInt::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint16_t refCount = 0;
  uint32_t value = 0;
  
  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataUInt::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUInt::create(
  NMP::MemoryAllocator* allocator,
  uint32_t              value,
  uint16_t              refCount)
{
  AttribDataHandle result;
  
  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);
  
  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUInt::copy(const AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_UINT && dest->getType() == ATTRIB_TYPE_UINT);
  ((AttribDataUInt*) dest)->m_value = ((AttribDataUInt*) source)->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataUInt::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(AttribUIntOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribUIntOutputData* output = (AttribUIntOutputData*) outputBuffer;
    output->m_value = ((MR::AttribDataUInt*)toOutput)->m_value;
    NMP::netEndianSwap(output->m_value);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataInt functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataInt::locate(AttribData* target)
{
  AttribDataInt* result = (AttribDataInt*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataInt::dislocate(AttribData* target)
{
  AttribDataInt* result = (AttribDataInt*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_value);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataInt::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataInt), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataInt* AttribDataInt::init(NMP::Memory::Resource& resource, int32_t value, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataInt), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataInt* result = (AttribDataInt*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_INT);
  result->setRefCount(refCount);
  result->m_value = value;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataInt::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint16_t refCount = 0;
  int32_t value = 0;
  
  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataInt::create(NMP::MemoryAllocator* allocator, int32_t value, uint16_t refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataInt::copy(const AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_INT && dest->getType() == ATTRIB_TYPE_INT);
  ((AttribDataInt*) dest)->m_value = ((AttribDataInt*) source)->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataInt::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(AttribIntOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribIntOutputData* output = (AttribIntOutputData*) outputBuffer;
    output->m_value = ((MR::AttribDataInt*)toOutput)->m_value;
    NMP::netEndianSwap(output->m_value);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataFloat functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataFloat::locate(AttribData* target)
{
  AttribDataFloat* result = (AttribDataFloat*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataFloat::dislocate(AttribData* target)
{
  AttribDataFloat* result = (AttribDataFloat*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_value);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataFloat::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataFloat), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataFloat* AttribDataFloat::init(NMP::Memory::Resource& resource, float value, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataFloat), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataFloat* result = (AttribDataFloat*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_FLOAT);
  result->setRefCount(refCount);
  result->m_value = value;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataFloat::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  float value = 0.0f;

  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataFloat::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  float value = 0.0f;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataFloat::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataFloat::create(
  NMP::MemoryAllocator* allocator,
  float                 value,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, value, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataFloat::copy(const AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_FLOAT && dest->getType() == ATTRIB_TYPE_FLOAT);
  ((AttribDataFloat*) dest)->m_value = ((AttribDataFloat*) source)->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataFloat::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(AttribFloatOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribFloatOutputData* output = (AttribFloatOutputData*) outputBuffer;
    output->m_value = ((MR::AttribDataFloat*)toOutput)->m_value;
    NMP::netEndianSwap(output->m_value);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataVector3 functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataVector3::locate(AttribData* target)
{
  AttribDataVector3* result = (AttribDataVector3*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataVector3::dislocate(AttribData* target)
{
  AttribDataVector3* result = (AttribDataVector3*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_value);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataVector3::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataVector3), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataVector3* AttribDataVector3::init(
  NMP::Memory::Resource& resource,
  const NMP::Vector3&    value,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataVector3), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataVector3* result = (AttribDataVector3*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_VECTOR3);
  result->setRefCount(refCount);
  result->m_value = value;
#ifndef NM_HOST_64_BIT
  result->m_pad[0] = 0;
  result->m_pad[1] = 0;
#endif

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataVector3::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  NMP::Vector3 value(NMP::Vector3::InitZero);

  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    value,
    refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataVector3::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  NMP::Vector3 value(NMP::Vector3::InitZero);

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    value,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataVector3::create(
  NMP::MemoryAllocator* allocator,
  const NMP::Vector3&   value,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    value,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataVector3::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataVector3::copy(const AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_VECTOR3 && dest->getType() == ATTRIB_TYPE_VECTOR3);
  ((AttribDataVector3*) dest)->m_value = ((AttribDataVector3*) source)->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataVector3::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(AttribVector3OutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribVector3OutputData* output = (AttribVector3OutputData*) outputBuffer;
    output->m_value = ((MR::AttribDataVector3*)toOutput)->m_value;
    NMP::netEndianSwap(output->m_value);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataVector4 functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataVector4::locate(AttribData* target)
{
  AttribDataVector4* result = (AttribDataVector4*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataVector4::dislocate(AttribData* target)
{
  AttribDataVector4* result = (AttribDataVector4*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_value);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataVector4::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataVector4), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataVector4* AttribDataVector4::init(NMP::Memory::Resource& resource, const NMP::Quat& value, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataVector4), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataVector4* result = (AttribDataVector4*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_VECTOR4);
  result->setRefCount(refCount);
  result->m_value = value;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataVector4::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  NMP::Quat value(NMP::Quat::kIdentity);

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    value,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataVector4::create(NMP::MemoryAllocator* allocator, const NMP::Quat& value, uint16_t refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    value,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataVector4::copy(const AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_VECTOR4 && dest->getType() == ATTRIB_TYPE_VECTOR4);
  ((AttribDataVector4*) dest)->m_value = ((AttribDataVector4*) source)->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataVector4::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(AttribVector4OutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribVector4OutputData* output = (AttribVector4OutputData*) outputBuffer;
    output->m_value = ((MR::AttribDataVector4*)toOutput)->m_value;
    NMP::netEndianSwap(output->m_value);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBoolArray functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBoolArray::getMemoryRequirements(uint32_t numValues)
{
  NMP::Memory::Format result(sizeof(AttribDataBoolArray), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of bools.
  result += NMP::Memory::Format(sizeof(bool) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBoolArray* AttribDataBoolArray::init(NMP::Memory::Resource& resource, uint32_t numValues, uint16_t refCount)
{
  // Placement new the resource to ensure the base constructor is called.
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBoolArray), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBoolArray* result = (AttribDataBoolArray*)resource.alignAndIncrement(memReqsHdr);
  
  result->setType(ATTRIB_TYPE_BOOL_ARRAY);
  result->setRefCount(refCount);

  // Array of bools.
  NMP::Memory::Format memReqsData(sizeof(bool) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_values = (bool*) resource.alignAndIncrement(memReqsData);
  
  // Initialise.
  result->m_numValues = numValues;
  for (uint32_t i = 0; i < numValues; ++i)
  {
    result->m_values[i] = false;
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBoolArray::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  desc)
{
  AttribDataHandle result;
  
  NMP_ASSERT(desc);
  const AttribDataArrayCreateDesc* targetDesc = (AttribDataArrayCreateDesc*)desc;

  result.m_format = getMemoryRequirements(targetDesc->m_numEntries);
  NMP_ASSERT(resource.format.size >= result.m_format.size);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, targetDesc->m_numEntries, targetDesc->m_refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBoolArray::outputMemReqs(Network* network)
{
  // Bool arrays are used as output masks, so the size must be the number of bones.
  uint32_t numBones = network->getMaxTransformCount();
  return getMemoryRequirements(numBones);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBoolArray::create(NMP::MemoryAllocator* allocator, uint32_t numValues, uint16_t refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numValues);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, numValues, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void AttribDataBoolArray::locate(AttribData* target)
{
  AttribDataBoolArray* result = (AttribDataBoolArray*) target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_numValues);

  NMP::endianSwap(result->m_values);
  REFIX_PTR_RELATIVE(bool, result->m_values, result);

  // Fixup each of the actual bool values.
  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBoolArray::dislocate(AttribData* target)
{
  AttribDataBoolArray* result = (AttribDataBoolArray*) target;

  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }

  UNFIX_PTR_RELATIVE(bool, result->m_values, result);
  NMP::endianSwap(result->m_values);

  NMP::endianSwap(result->m_numValues);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBoolArray::relocate(AttribData* target, void* location)
{
  AttribDataBoolArray* result = (AttribDataBoolArray*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataBoolArray));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_values = (bool*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataBoolArray::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);
  AttribDataBoolArray* source = (AttribDataBoolArray*)toOutput;
  uint32_t dataSize =  sizeof(AttribBoolArrayOutputData) + (source->m_numValues * sizeof(bool));

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    // Initialise the output structure.
    AttribBoolArrayOutputData* output = (AttribBoolArrayOutputData*) outputBuffer;
    output->m_numValues = source->m_numValues;

    bool* values = NULL;
    if (output->m_numValues)
      values = (bool*) ((char*)outputBuffer + sizeof(AttribBoolArrayOutputData));

    for (uint32_t i = 0; i < output->m_numValues; ++i)
    {
      values[i] = source->m_values[i];
      NMP::netEndianSwap(values[i]);
    }

    NMP::netEndianSwap(output->m_numValues);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataIntArray functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataIntArray::getMemoryRequirements(uint32_t numValues)
{
  NMP::Memory::Format result(sizeof(AttribDataIntArray), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of bools.
  result += NMP::Memory::Format(sizeof(int32_t) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataIntArray* AttribDataIntArray::init(NMP::Memory::Resource& resource, uint32_t numValues, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataIntArray), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataIntArray* result = (AttribDataIntArray*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_INT_ARRAY);
  result->setRefCount(refCount);

  // Array of ints.
  NMP::Memory::Format memReqsData(sizeof(int32_t) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_values = (int32_t*) resource.alignAndIncrement(memReqsData);

  // Initialise.
  result->m_numValues = numValues;
  for (uint32_t i = 0; i < numValues; ++i)
  {
    result->m_values[i] = 0;
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataIntArray::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* desc)
{
  AttribDataHandle result;

  NMP_ASSERT(desc);
  const AttribDataArrayCreateDesc* targetDesc = (AttribDataArrayCreateDesc*)desc;

  result.m_format = getMemoryRequirements(targetDesc->m_numEntries);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, targetDesc->m_numEntries, targetDesc->m_refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataIntArray::create(NMP::MemoryAllocator* allocator, uint32_t numValues, uint16_t refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numValues);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, numValues, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataIntArray::locate(AttribData* target)
{
  AttribDataIntArray* result = (AttribDataIntArray*) target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_numValues);

  NMP::endianSwap(result->m_values);
  REFIX_PTR_RELATIVE(int32_t, result->m_values, result);

  // Fixup each of the actual values.
  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataIntArray::dislocate(AttribData* target)
{
  AttribDataIntArray* result = (AttribDataIntArray*) target;

  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }

  UNFIX_PTR_RELATIVE(int32_t, result->m_values, result);
  NMP::endianSwap(result->m_values);

  NMP::endianSwap(result->m_numValues);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataIntArray::relocate(AttribData* target, void* location)
{
  AttribDataIntArray* result = (AttribDataIntArray*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataIntArray));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_values = (int32_t*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataIntArray::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);
  AttribDataIntArray* source = (AttribDataIntArray*)toOutput;
  uint32_t dataSize =  sizeof(AttribIntArrayOutputData) + (source->m_numValues * sizeof(int32_t));

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    // Initialise the output structure.
    AttribIntArrayOutputData* output = (AttribIntArrayOutputData*) outputBuffer;
    output->m_numValues = source->m_numValues;

    int32_t* values = NULL;
    if (output->m_numValues)
      values = (int32_t*) ((char*)outputBuffer + sizeof(AttribIntArrayOutputData));

    for (uint32_t i = 0; i < output->m_numValues; ++i)
    {
      values[i] = source->m_values[i];
      NMP::netEndianSwap(values[i]);
    }

    NMP::netEndianSwap(output->m_numValues);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataUIntArray functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataUIntArray::getMemoryRequirements(uint32_t numValues)
{
  NMP::Memory::Format result(sizeof(AttribDataUIntArray), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of bools.
  result += NMP::Memory::Format(sizeof(uint32_t) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataUIntArray* AttribDataUIntArray::init(NMP::Memory::Resource& resource, uint32_t numValues, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataUIntArray), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataUIntArray* result = (AttribDataUIntArray*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_UINT_ARRAY);
  result->setRefCount(refCount);

  // Array of uints.
  NMP::Memory::Format memReqsData(sizeof(uint32_t) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_values = (uint32_t*) resource.alignAndIncrement(memReqsData);

  // Initialise.
  result->m_numValues = numValues;
  for (uint32_t i = 0; i < numValues; ++i)
  {
    result->m_values[i] = 0;
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUIntArray::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* desc)
{
  AttribDataHandle result;

  NMP_ASSERT(desc);
  const AttribDataArrayCreateDesc* targetDesc = (AttribDataArrayCreateDesc*)desc;

  result.m_format = getMemoryRequirements(targetDesc->m_numEntries);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, targetDesc->m_numEntries, targetDesc->m_refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUIntArray::create(NMP::MemoryAllocator* allocator, uint32_t numValues, uint16_t refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numValues);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, numValues, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataUIntArray::locate(AttribData* target)
{
  AttribDataUIntArray* result = (AttribDataUIntArray*) target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_numValues);

  NMP::endianSwap(result->m_values);
  REFIX_PTR_RELATIVE(uint32_t, result->m_values, result);

  // Fixup each of the actual uint values.
  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUIntArray::dislocate(AttribData* target)
{
  AttribDataUIntArray* result = (AttribDataUIntArray*) target;

  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }

  UNFIX_PTR_RELATIVE(uint32_t, result->m_values, result);
  NMP::endianSwap(result->m_values);

  NMP::endianSwap(result->m_numValues);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUIntArray::relocate(AttribData* target, void* location)
{
  AttribDataUIntArray* result = (AttribDataUIntArray*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataUIntArray));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_values = (uint32_t*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataUIntArray::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);
  AttribDataUIntArray* source = (AttribDataUIntArray*)toOutput;
  uint32_t dataSize =  sizeof(AttribUIntArrayOutputData) + (source->m_numValues * sizeof(uint32_t));

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    // Initialise the output structure.
    AttribUIntArrayOutputData* output = (AttribUIntArrayOutputData*) outputBuffer;
    output->m_numValues = source->m_numValues;

    uint32_t* values = NULL;
    if (output->m_numValues)
      values = (uint32_t*) ((char*)outputBuffer + sizeof(AttribUIntArrayOutputData));

    for (uint32_t i = 0; i < output->m_numValues; ++i)
    {
      values[i] = source->m_values[i];
      NMP::netEndianSwap(values[i]);
    }

    NMP::netEndianSwap(output->m_numValues);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataUInt64Array functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataUInt64Array::getMemoryRequirements(uint32_t numValues)
{
  NMP::Memory::Format result(sizeof(AttribDataUInt64Array), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of bools.
  result += NMP::Memory::Format(sizeof(uint64_t) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataUInt64Array* AttribDataUInt64Array::init(NMP::Memory::Resource& resource, uint32_t numValues, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataUInt64Array), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataUInt64Array* result = (AttribDataUInt64Array*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_UINT64_ARRAY);
  result->setRefCount(refCount);

  // Array of uints.
  NMP::Memory::Format memReqsData(sizeof(uint64_t) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_values = (uint64_t*) resource.alignAndIncrement(memReqsData);

  // Initialise.
  result->m_numValues = numValues;
  for (uint32_t i = 0; i < numValues; ++i)
  {
    result->m_values[i] = 0;
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUInt64Array::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* desc)
{
  AttribDataHandle result;

  NMP_ASSERT(desc);
  const AttribDataArrayCreateDesc* targetDesc = (AttribDataArrayCreateDesc*)desc;

  result.m_format = getMemoryRequirements(targetDesc->m_numEntries);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, targetDesc->m_numEntries, targetDesc->m_refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUInt64Array::create(NMP::MemoryAllocator* allocator, uint32_t numValues, uint16_t refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numValues);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, numValues, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataUInt64Array::locate(AttribData* target)
{
  AttribDataUInt64Array* result = (AttribDataUInt64Array*) target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_numValues);

  NMP::endianSwap(result->m_values);
  REFIX_PTR_RELATIVE(uint64_t, result->m_values, result);

  // Fixup each of the actual uint values.
  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUInt64Array::dislocate(AttribData* target)
{
  AttribDataUInt64Array* result = (AttribDataUInt64Array*) target;

  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }

  UNFIX_PTR_RELATIVE(uint64_t, result->m_values, result);
  NMP::endianSwap(result->m_values);

  NMP::endianSwap(result->m_numValues);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUInt64Array::relocate(AttribData* target, void* location)
{
  AttribDataUInt64Array* result = (AttribDataUInt64Array*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataUInt64Array));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_values = (uint64_t*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataFloatArray functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataFloatArray::getMemoryRequirements(uint32_t numValues)
{
  NMP::Memory::Format result(sizeof(AttribDataFloatArray), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of bools.
  result += NMP::Memory::Format(sizeof(float) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataFloatArray* AttribDataFloatArray::init(NMP::Memory::Resource& resource, uint32_t numValues, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataFloatArray), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataFloatArray* result = (AttribDataFloatArray*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_FLOAT_ARRAY);
  result->setRefCount(refCount);

  // Array of floats.
  NMP::Memory::Format memReqsData(sizeof(float) * numValues, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_values = (float*) resource.alignAndIncrement(memReqsData);

  // Initialise.
  result->m_numValues = numValues;
  for (uint32_t i = 0; i < numValues; ++i)
  {
    result->m_values[i] = 0.0f;
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataFloatArray::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* desc)
{
  AttribDataHandle result;

  NMP_ASSERT(desc);
  const AttribDataArrayCreateDesc* targetDesc = (AttribDataArrayCreateDesc*)desc;

  result.m_format = getMemoryRequirements(targetDesc->m_numEntries);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, targetDesc->m_numEntries, targetDesc->m_refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataFloatArray::create(
  NMP::MemoryAllocator* allocator,
  uint32_t              numValues,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numValues);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, numValues, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataFloatArray::locate(AttribData* target)
{
  AttribDataFloatArray* result = (AttribDataFloatArray*) target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_numValues);

  NMP::endianSwap(result->m_values);
  REFIX_PTR_RELATIVE(float, result->m_values, result);

  // Fixup each of the actual float values.
  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataFloatArray::dislocate(AttribData* target)
{
  AttribDataFloatArray* result = (AttribDataFloatArray*) target;

  for (uint32_t i = 0; i < result->m_numValues; ++i)
  {
    NMP::endianSwap(result->m_values[i]);
  }

  UNFIX_PTR_RELATIVE(float, result->m_values, result);
  NMP::endianSwap(result->m_values);

  NMP::endianSwap(result->m_numValues);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataFloatArray::relocate(AttribData* target, void* location)
{
  AttribDataFloatArray* result = (AttribDataFloatArray*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataFloatArray));
  ptr = (void*)(NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_VECTOR_ALIGNMENT));
  result->m_values = (float*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataFloatArray::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);
  AttribDataFloatArray* source = (AttribDataFloatArray*)toOutput;
  uint32_t dataSize =  sizeof(AttribFloatArrayOutputData) + (source->m_numValues * sizeof(uint32_t));

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    // Initialise the output structure.
    AttribFloatArrayOutputData* output = (AttribFloatArrayOutputData*) outputBuffer;
    output->m_numValues = source->m_numValues;

    float* values = NULL;
    if (output->m_numValues)
      values = (float*) ((char*)outputBuffer + sizeof(AttribFloatArrayOutputData));

    for (uint32_t i = 0; i < output->m_numValues; ++i)
    {
      values[i] = source->m_values[i];
      NMP::netEndianSwap(values[i]);
    }

    NMP::netEndianSwap(output->m_numValues);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataUpdatePlaybackPos functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataUpdatePlaybackPos::locate(AttribData* target)
{
  AttribDataUpdatePlaybackPos* result = (AttribDataUpdatePlaybackPos*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_isFraction);
  NMP::endianSwap(result->m_isAbs);
  NMP::endianSwap(result->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUpdatePlaybackPos::dislocate(AttribData* target)
{
  AttribDataUpdatePlaybackPos* result = (AttribDataUpdatePlaybackPos*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_isFraction);
  NMP::endianSwap(result->m_isAbs);
  NMP::endianSwap(result->m_value);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataUpdatePlaybackPos::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataUpdatePlaybackPos), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataUpdatePlaybackPos* AttribDataUpdatePlaybackPos::init(
  NMP::Memory::Resource& resource,
  bool                   isFraction,
  bool                   isAbs,
  float                  value,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataUpdatePlaybackPos), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataUpdatePlaybackPos* result = (AttribDataUpdatePlaybackPos*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_UPDATE_PLAYBACK_POS);
  result->setRefCount(refCount);
  result->m_isFraction = isFraction;
  result->m_isAbs = isAbs;
  result->m_value = value;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUpdatePlaybackPos::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    false,
    false,
    0.0f,
    refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUpdatePlaybackPos::create(
  NMP::MemoryAllocator* allocator,
  bool                  isFraction,
  bool                  isAbs,
  float                 value,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    isFraction,
    isAbs,
    value,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataUpdatePlaybackPos::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUpdatePlaybackPos::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_UPDATE_PLAYBACK_POS && dest->getType() == ATTRIB_TYPE_UPDATE_PLAYBACK_POS);
  AttribDataUpdatePlaybackPos* sourcePlaybackPos = (AttribDataUpdatePlaybackPos*) source;
  AttribDataUpdatePlaybackPos* destPlaybackPos = (AttribDataUpdatePlaybackPos*) dest;
  destPlaybackPos->m_isAbs = sourcePlaybackPos->m_isAbs;
  destPlaybackPos->m_isFraction = sourcePlaybackPos->m_isFraction;
  destPlaybackPos->m_value = sourcePlaybackPos->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataUpdatePlaybackPos::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  uint32_t dataSize = (uint32_t)sizeof(AttribUpdatePlaybackPosOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribUpdatePlaybackPosOutputData* output = (AttribUpdatePlaybackPosOutputData*) outputBuffer;
    AttribDataUpdatePlaybackPos* sourceAttrib = (AttribDataUpdatePlaybackPos*)toOutput;

    output->m_isFraction = sourceAttrib->m_isFraction;
    output->m_isAbs = sourceAttrib->m_isAbs;
    output->m_value = sourceAttrib->m_value;
    AttribUpdatePlaybackPosOutputData::endianSwap(outputBuffer);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataUpdatePlaybackPos functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataPlaybackPos::locate(AttribData* target)
{
  AttribDataPlaybackPos* result = (AttribDataPlaybackPos*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_currentPosAdj);
  NMP::endianSwap(result->m_previousPosAdj);
  NMP::endianSwap(result->m_currentPosReal);
  NMP::endianSwap(result->m_previousPosReal);
  NMP::endianSwap(result->m_setWithAbs);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPlaybackPos::dislocate(AttribData* target)
{
  AttribDataPlaybackPos* result = (AttribDataPlaybackPos*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_currentPosAdj);
  NMP::endianSwap(result->m_previousPosAdj);
  NMP::endianSwap(result->m_currentPosReal);
  NMP::endianSwap(result->m_previousPosReal);
  NMP::endianSwap(result->m_setWithAbs);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPlaybackPos::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataPlaybackPos), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPlaybackPos* AttribDataPlaybackPos::init(
  NMP::Memory::Resource& resource,
  float                  currentPosAdj,
  float                  previousPosAdj,
  float                  currentPosReal,
  float                  previousPosReal,
  bool                   setWithAbs,
  bool                   inclusiveStart,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPlaybackPos), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPlaybackPos* result = (AttribDataPlaybackPos*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_PLAYBACK_POS);
  result->setRefCount(refCount);
  result->m_currentPosAdj = currentPosAdj;
  result->m_previousPosAdj = previousPosAdj;
  result->m_currentPosReal = currentPosReal;
  result->m_previousPosReal = previousPosReal;
  result->m_setWithAbs = setWithAbs;
  result->m_inclusiveStart = inclusiveStart;
  result->m_delta = 0.0f;
  
  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPlaybackPos::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint16_t refCount = 0;
  
  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    0.0f,
    0.0f,
    0.0f,
    0.0f,
    false,
    false,
    refCount);
    
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPlaybackPos::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPlaybackPos::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_PLAYBACK_POS && dest->getType() == ATTRIB_TYPE_PLAYBACK_POS);
  AttribDataPlaybackPos* sourcePlaybackPos = (AttribDataPlaybackPos*) source;
  AttribDataPlaybackPos* destPlaybackPos = (AttribDataPlaybackPos*) dest;
  destPlaybackPos->m_currentPosAdj = sourcePlaybackPos->m_currentPosAdj;
  destPlaybackPos->m_previousPosAdj = sourcePlaybackPos->m_previousPosAdj;
  destPlaybackPos->m_currentPosReal = sourcePlaybackPos->m_currentPosReal;
  destPlaybackPos->m_previousPosReal = sourcePlaybackPos->m_previousPosReal;
  destPlaybackPos->m_delta = sourcePlaybackPos->m_delta;
  destPlaybackPos->m_setWithAbs = sourcePlaybackPos->m_setWithAbs;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataPlaybackPos::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  uint32_t dataSize = (uint32_t)sizeof(AttribPlaybackPosOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribPlaybackPosOutputData* output = (AttribPlaybackPosOutputData*) outputBuffer;
    AttribDataPlaybackPos* sourceAttrib = (AttribDataPlaybackPos*)toOutput;

    output->m_currentPosAdj = sourceAttrib->m_currentPosAdj;
    output->m_previousPosAdj = sourceAttrib->m_previousPosAdj;
    output->m_currentPosReal = sourceAttrib->m_currentPosReal;
    output->m_previousPosReal = sourceAttrib->m_previousPosReal;
    output->m_delta = sourceAttrib->m_delta;
    output->m_setWithAbs = sourceAttrib->m_setWithAbs;
    AttribPlaybackPosOutputData::endianSwap(outputBuffer);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataUpdateSyncEventPlaybackPos functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataUpdateSyncEventPlaybackPos::locate(AttribData* target)
{
  AttribDataUpdateSyncEventPlaybackPos* result = (AttribDataUpdateSyncEventPlaybackPos*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_isAbs);
  result->m_absPosAdj.locate();
  result->m_absPosReal.locate();
  result->m_deltaPos.locate();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUpdateSyncEventPlaybackPos::dislocate(AttribData* target)
{
  AttribDataUpdateSyncEventPlaybackPos* result = (AttribDataUpdateSyncEventPlaybackPos*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_isAbs);
  result->m_absPosAdj.dislocate();
  result->m_absPosReal.dislocate();
  result->m_deltaPos.dislocate();
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataUpdateSyncEventPlaybackPos::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataUpdateSyncEventPlaybackPos), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataUpdateSyncEventPlaybackPos* AttribDataUpdateSyncEventPlaybackPos::init(
  NMP::Memory::Resource& resource,
  bool                   thisIsAbs,
  const SyncEventPos&    absPosAdj,
  const SyncEventPos&    absPosReal,
  const SyncEventPos&    deltaPos,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataUpdateSyncEventPlaybackPos), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataUpdateSyncEventPlaybackPos* result = (AttribDataUpdateSyncEventPlaybackPos*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS);
  result->setRefCount(refCount);
  result->m_isAbs = thisIsAbs;
  result->m_absPosAdj = absPosAdj;
  result->m_absPosReal = absPosReal;
  result->m_deltaPos = deltaPos;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUpdateSyncEventPlaybackPos::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint16_t refCount = 0;
  
  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    true,
    SyncEventPos(0, 0.0f),
    SyncEventPos(0, 0.0f),
    SyncEventPos(0, 0.0f),
    refCount);
    
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUpdateSyncEventPlaybackPos::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    true,
    SyncEventPos(0, 0.0f),
    SyncEventPos(0, 0.0f),
    SyncEventPos(0, 0.0f),
    refCount);

  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataUpdateSyncEventPlaybackPos::create(
  NMP::MemoryAllocator* allocator,
  bool                  thisIsAbs,
  const SyncEventPos&   absPosAdj,
  const SyncEventPos&   absPosReal,
  const SyncEventPos&   deltaPos,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    thisIsAbs,
    absPosAdj,
    absPosReal,
    deltaPos,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataUpdateSyncEventPlaybackPos::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataUpdateSyncEventPlaybackPos::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT((source->getType() == ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS) &&
             (dest->getType() == ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS));
  AttribDataUpdateSyncEventPlaybackPos* sourcePlaybackPos = (AttribDataUpdateSyncEventPlaybackPos*) source;
  AttribDataUpdateSyncEventPlaybackPos* destPlaybackPos = (AttribDataUpdateSyncEventPlaybackPos*) dest;
  destPlaybackPos->m_isAbs = sourcePlaybackPos->m_isAbs;
  destPlaybackPos->m_absPosAdj = sourcePlaybackPos->m_absPosAdj;
  destPlaybackPos->m_absPosReal = sourcePlaybackPos->m_absPosReal;
  destPlaybackPos->m_deltaPos = sourcePlaybackPos->m_deltaPos;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataUpdateSyncEventPlaybackPos::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  uint32_t dataSize = sizeof(AttribUpdateSyncEventPlaybackPosOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribUpdateSyncEventPlaybackPosOutputData* output = (AttribUpdateSyncEventPlaybackPosOutputData*) outputBuffer;
    AttribDataUpdateSyncEventPlaybackPos* sourceAttrib = (AttribDataUpdateSyncEventPlaybackPos*)toOutput;

    output->m_isAbs              = sourceAttrib->m_isAbs;
    output->m_absPosAdjIndex     = sourceAttrib->m_absPosAdj.index();
    output->m_absPosAdjFraction  = sourceAttrib->m_absPosAdj.fraction();
    output->m_absPosRealIndex    = sourceAttrib->m_absPosReal.index();
    output->m_absPosRealFraction = sourceAttrib->m_absPosReal.fraction();
    output->m_deltaPosIndex      = sourceAttrib->m_deltaPos.index();
    output->m_deltaPosFraction   = sourceAttrib->m_deltaPos.fraction();
    AttribUpdateSyncEventPlaybackPosOutputData::endianSwap(outputBuffer);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING


//----------------------------------------------------------------------------------------------------------------------
// AttribDataEmittedMessagesMap functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataEmittedMessagesMap::locate(AttribData* target)
{
  AttribDataEmittedMessagesMap* result = (AttribDataEmittedMessagesMap*)target;
  AttribData::locate(result);

  NMP_ASSERT(result->getType() == AttribDataEmittedMessagesMap::getDefaultType());

  NMP::endianSwap(result->m_numEmittedMessage);
  REFIX_SWAP_PTR_RELATIVE(EmittedMessageMap, result->m_emittedMessages, result);
  REFIX_SWAP_PTR_RELATIVE(bool, result->m_messageUsed, result);

  for (uint32_t messageIndex = 0; messageIndex < result->m_numEmittedMessage; ++messageIndex)
  {
    NMP::endianSwap(result->m_emittedMessages[messageIndex].messageID);
    NMP::endianSwap(result->m_emittedMessages[messageIndex].stateMachineNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataEmittedMessagesMap::dislocate(AttribData* target)
{
  NMP_ASSERT(target->getType() == AttribDataEmittedMessagesMap::getDefaultType());

  AttribDataEmittedMessagesMap* result = (AttribDataEmittedMessagesMap*)target;
  AttribData::dislocate(result);

  for (uint32_t messageIndex = 0; messageIndex < result->m_numEmittedMessage; ++messageIndex)
  {
    NMP::endianSwap(result->m_emittedMessages[messageIndex].messageID);
    NMP::endianSwap(result->m_emittedMessages[messageIndex].stateMachineNodeID);
  }

  NMP::endianSwap(result->m_numEmittedMessage);
  UNFIX_SWAP_PTR_RELATIVE(EmittedMessageMap, result->m_emittedMessages, target);
  UNFIX_SWAP_PTR_RELATIVE(bool, result->m_messageUsed, target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataEmittedMessagesMap::relocate(AttribData* target)
{
  NMP_ASSERT(target->getType() == AttribDataEmittedMessagesMap::getDefaultType());
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));

  AttribDataEmittedMessagesMap* emittedMessageMap = (AttribDataEmittedMessagesMap*)target;

  void* targetData = (char*)target;

  NMP::Memory::Format fmt(sizeof(AttribDataEmittedMessagesMap), NMP_NATURAL_TYPE_ALIGNMENT);
  emittedMessageMap->m_emittedMessages = (EmittedMessageMap*)NMP::Memory::alignAndIncrement(targetData, fmt);

  fmt.set(sizeof(bool), NMP_NATURAL_TYPE_ALIGNMENT);
  emittedMessageMap->m_messageUsed = (bool*)NMP::Memory::alignAndIncrement(targetData, fmt);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataEmittedMessagesMap::getMemoryRequirements(uint32_t numEntries)
{
  NMP::Memory::Format memoryRequirements(sizeof(AttribDataEmittedMessagesMap), MR_ATTRIB_DATA_ALIGNMENT);
  memoryRequirements += NMP::Memory::Format(sizeof(EmittedMessageMap) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT);
  memoryRequirements += NMP::Memory::Format(sizeof(bool) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT);
  memoryRequirements.align();
  return memoryRequirements;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataEmittedMessagesMap* AttribDataEmittedMessagesMap::createAndInit(
  NMP::MemoryAllocator* allocator,
  uint32_t              numEntries,
  uint16_t              refCount)
{
  NMP::Memory::Format format = getMemoryRequirements(numEntries);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, format);
  AttribDataEmittedMessagesMap* messageMap = init(resource, numEntries, refCount);
  messageMap->m_allocator = allocator;

  return messageMap;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataEmittedMessagesMap* AttribDataEmittedMessagesMap::init(
  NMP::Memory::Resource&     resource,
  uint32_t                   numEntries,
  uint16_t                   refCount)
{
  NMP::Memory::Format fmt(sizeof(AttribDataEmittedMessagesMap), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataEmittedMessagesMap* messageMap = (AttribDataEmittedMessagesMap*)resource.alignAndIncrement(fmt);

  messageMap->setType(ATTRIB_TYPE_EMITTED_MESSAGE_MAP);
  messageMap->setRefCount(refCount);
  messageMap->m_numEmittedMessage = numEntries;

  fmt.set(sizeof(EmittedMessageMap) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT);
  messageMap->m_emittedMessages = (EmittedMessageMap*)resource.alignAndIncrement(fmt);

  fmt.set(sizeof(bool) * numEntries, NMP_NATURAL_TYPE_ALIGNMENT);
  messageMap->m_messageUsed = (bool*)resource.alignAndIncrement(fmt);

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  // Clear all emitted message slots
  for (uint32_t i = 0; i < messageMap->m_numEmittedMessage; ++i)
  {
    messageMap->setEmittedMessage(i, UNUSED, INVALID_NODE_ID, INVALID_MESSAGE_ID);
  }

  return messageMap;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataEmittedMessagesMap::setEmittedMessage(
  uint32_t    index,
  RequestType requestType,
  NodeID      targetStateMachineID,
  MessageID   messageID)
{
  NMP_ASSERT(index < m_numEmittedMessage);

  // Set default message status to True
  m_messageUsed[index] = true;

  switch (requestType)
  {
    case AttribDataEmittedMessagesMap::UNUSED:
      {
        m_emittedMessages[index].stateMachineNodeID = MR::INVALID_NODE_ID; // Mapping disabled.
        m_emittedMessages[index].messageID = MR::INVALID_MESSAGE_ID;
      }
      break;

    case AttribDataEmittedMessagesMap::EXTERNAL:
      {
        NMP_ASSERT(messageID != MR::INVALID_MESSAGE_ID);
        m_emittedMessages[index].stateMachineNodeID = MR::INVALID_NODE_ID; // Mapping disabled.
        m_emittedMessages[index].messageID = messageID;
      }
      break;

    case AttribDataEmittedMessagesMap::CLEAR:
      m_messageUsed[index] = false; // Set request status to False
      /* Deliberate fall-through */
    case AttribDataEmittedMessagesMap::SET:
      {
        NMP_ASSERT(targetStateMachineID != MR::INVALID_NODE_ID);
        NMP_ASSERT(messageID != MR::INVALID_MESSAGE_ID);
        m_emittedMessages[index].stateMachineNodeID = targetStateMachineID; // Broadcast is MR::NETWORK_NODE_ID
        m_emittedMessages[index].messageID = messageID;
      }
      break;

    case AttribDataEmittedMessagesMap::RESET:
      {
        NMP_ASSERT(targetStateMachineID != MR::INVALID_NODE_ID);
        m_emittedMessages[index].stateMachineNodeID = targetStateMachineID; // Broadcast is MR::NETWORK_NODE_ID
        m_emittedMessages[index].messageID = INVALID_MESSAGE_ID;
      }
      break;

    default:
      NMP_ASSERT(requestType == AttribDataEmittedMessagesMap::UNUSED); // False
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTransformBuffer functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransformBuffer::locate(AttribData* target)
{
  AttribDataTransformBuffer* result = (AttribDataTransformBuffer*)target;
  AttribData::locate(target);

  NMP::endianSwap(result->m_transformBuffer);
  REFIX_PTR_RELATIVE(NMP::DataBuffer, result->m_transformBuffer, result);
  result->m_transformBuffer->locate();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransformBuffer::dislocate(AttribData* target)
{
  AttribDataTransformBuffer* result = (AttribDataTransformBuffer*)target;
  AttribData::dislocate(target);

  result->m_transformBuffer->dislocate();
  UNFIX_PTR_RELATIVE(NMP::DataBuffer, result->m_transformBuffer, result);
  NMP::endianSwap(result->m_transformBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransformBuffer::relocate(AttribData* target, void* location)
{
  AttribDataTransformBuffer* result = (AttribDataTransformBuffer*) target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  size_t offset = NMP::Memory::align(sizeof(AttribDataTransformBuffer), NMP_VECTOR_ALIGNMENT);
  NMP::DataBuffer* transformBuffer = (NMP::DataBuffer*)((size_t)result + offset);
  result->m_transformBuffer = (NMP::DataBuffer*)((size_t)location + offset);
  NMP_ASSERT(NMP_IS_ALIGNED(transformBuffer, NMP_VECTOR_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(result->m_transformBuffer, NMP_VECTOR_ALIGNMENT));
  transformBuffer->relocate(result->m_transformBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataTransformBuffer::serializeTx(
  Network*    NMP_USED_FOR_ASSERTS(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput && net && net->getNetworkDef());

  // We only export PosQuat Buffers at the moment. This can be extended when connect is able to cope with other buffer formats.
  NMP::DataBuffer* transBuf = ((MR::AttribDataTransformBuffer*)toOutput)->m_transformBuffer;
  if (!transBuf->isPosQuat())
    return 0;

  uint32_t usedFlagsSize = (uint32_t) transBuf->getUsedFlags()->getMemoryRequirements().size;
  uint32_t transformSize = (uint32_t) sizeof(AttribTransformBufferOutputData::Transform);
  uint32_t dataSize = (uint32_t) (sizeof(AttribTransformBufferOutputData) +  NMP::Memory::align(usedFlagsSize + (transBuf->getLength() * transformSize), NMP_NATURAL_TYPE_ALIGNMENT));

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    NMP::Quat* qOut = transBuf->getPosQuatChannelQuat(0);
    NMP::Vector3* pOut = transBuf->getPosQuatChannelPos(0);

    // Initialise the output structure.
    AttribTransformBufferOutputData* output = (AttribTransformBufferOutputData*) outputBuffer;

    output->m_numTransforms = transBuf->getLength();
    NMP::BitArray* usedFlags = (NMP::BitArray*) ((char*)outputBuffer + sizeof(AttribTransformBufferOutputData));
    AttribTransformBufferOutputData::Transform* transforms = (AttribTransformBufferOutputData::Transform*) ((char*)outputBuffer + sizeof(AttribTransformBufferOutputData) + usedFlagsSize);

    // Copy the used bit flags
    memcpy(usedFlags, transBuf->getUsedFlags(), usedFlagsSize);

    // Copy the transforms.
    for (uint32_t i = 0; i < transBuf->getLength(); ++i)
    {
      transforms[i].m_pos = pOut[i];
      transforms[i].m_quat = qOut[i];
    }

    // Endian swap the bit flags and transforms themselves.
    uint32_t* block = (uint32_t*)(usedFlags);
    uint32_t numWords = (uint32_t) (NMP::Memory::align(usedFlagsSize + (transBuf->getLength() * transformSize), NMP_NATURAL_TYPE_ALIGNMENT) / 4);
    for (uint32_t i = 0; i < numWords; ++i)
    {
      NMP::netEndianSwap(block[i]);
    }

    // Endian swap the containing structure
    NMP::netEndianSwap(output->m_numTransforms);
    UNFIX_PTR_RELATIVE(NMP::BitArray, usedFlags, output);
    output->m_usedFlagsPtrOffset = (uint32_t)((ptrdiff_t)usedFlags);
    NMP::netEndianSwap(output->m_usedFlagsPtrOffset);
    UNFIX_PTR_RELATIVE(AttribTransformBufferOutputData::Transform, transforms, output);
    output->m_transformsPtrOffset = (uint32_t)((ptrdiff_t)transforms);
    NMP::netEndianSwap(output->m_transformsPtrOffset);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTransformBuffer::create(
  NMP::MemoryAllocator*                     allocator,
  const NMP::Memory::Format&                buffMemReqs,
  const NMP::Memory::Format&                internalBuffMemReqs,
  uint32_t                                  numElements,
  const NMP::DataBuffer::ElementDescriptor* elements,
  uint32_t                                  numEntries,
  uint16_t                                  refCount)
{
  AttribDataHandle handle;
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, buffMemReqs);
  NMP_ASSERT(resource.ptr);
  AttribDataTransformBuffer* attribData = init(resource, internalBuffMemReqs, numElements, elements, numEntries, refCount);
  attribData->m_allocator = allocator; // Back chain the allocator so we know what to free this attribData with when we destroy it.

  handle.m_format     = NMP::Memory::Format(NMP::Memory::align(sizeof(AttribDataTransformBuffer), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
  handle.m_attribData = attribData;

  return handle;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransformBuffer* AttribDataTransformBuffer::createPosQuat(
  NMP::MemoryAllocator*      allocator,
  const NMP::Memory::Format& buffMemReqs,
  const NMP::Memory::Format& internalBuffMemReqs,
  uint32_t                   numEntries,
  uint16_t                   refCount)
{
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, buffMemReqs);
  NMP_ASSERT(resource.ptr);
  AttribDataTransformBuffer* result = initPosQuat(resource, internalBuffMemReqs, numEntries, refCount);
  result->m_allocator = allocator; // Back chain the allocator so we know what to free this attribData with when we destroy it.
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransformBuffer* AttribDataTransformBuffer::createPosVelAngVel(
  NMP::MemoryAllocator*      allocator,
  const NMP::Memory::Format& buffMemReqs,
  const NMP::Memory::Format& internalBuffMemReqs,
  uint32_t                   numEntries,
  uint16_t                   refCount)
{
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, buffMemReqs);
  NMP_ASSERT(resource.ptr);
  AttribDataTransformBuffer* result = initPosVelAngVel(resource, internalBuffMemReqs, numEntries, refCount);
  result->m_allocator = allocator; // Back chain the allocator so we know what to free this attribData with when we destroy it.
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransformBuffer::getMemoryRequirements(
  uint32_t                                  numElements,
  const NMP::DataBuffer::ElementDescriptor* elements,
  uint32_t                                  numEntries,
  NMP::Memory::Format&                      buffMemReqs,
  NMP::Memory::Format&                      internalBuffMemReqs)
{
  internalBuffMemReqs = NMP::DataBuffer::getMemoryRequirements(numElements, elements, numEntries);
  buffMemReqs.set(sizeof(AttribDataTransformBuffer), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  buffMemReqs += internalBuffMemReqs;
  buffMemReqs.size = NMP::Memory::align(buffMemReqs.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransformBuffer::getPosQuatMemoryRequirements(
  uint32_t             numEntries,
  NMP::Memory::Format& buffMemReqs,
  NMP::Memory::Format& internalBuffMemReqs)
{
  internalBuffMemReqs = NMP::DataBuffer::getPosQuatMemoryRequirements(numEntries);
  buffMemReqs.set(sizeof(AttribDataTransformBuffer), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  buffMemReqs += internalBuffMemReqs;
  buffMemReqs.size = NMP::Memory::align(buffMemReqs.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransformBuffer::getPosVelAngVelMemoryRequirements(
  uint32_t             numEntries,
  NMP::Memory::Format& buffMemReqs,
  NMP::Memory::Format& internalBuffMemReqs)
{
  internalBuffMemReqs = NMP::DataBuffer::getPosVelAngVelMemoryRequirements(numEntries);
  buffMemReqs.set(sizeof(AttribDataTransformBuffer), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  buffMemReqs += internalBuffMemReqs;
  buffMemReqs.size = NMP::Memory::align(buffMemReqs.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTransformBuffer::getInstanceMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataTransformBuffer), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  result += m_transformBuffer->getInstanceMemoryRequirements();

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransformBuffer* AttribDataTransformBuffer::init(
  NMP::Memory::Resource&                    resource,
  const NMP::Memory::Format&                internalBuffMemReqs,
  uint32_t                                  numElements,
  const NMP::DataBuffer::ElementDescriptor* elements,
  uint32_t                                  numEntries,
  uint16_t                                  refCount)
{
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);
  AttribDataTransformBuffer* result = (AttribDataTransformBuffer*)resource.ptr;
  resource.increment(sizeof(AttribDataTransformBuffer));

  result->m_transformBuffer = NMP::DataBuffer::init(resource, internalBuffMemReqs, numElements, elements, numEntries);

  result->setType(ATTRIB_TYPE_TRANSFORM_BUFFER);
  result->setRefCount(refCount);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransformBuffer* AttribDataTransformBuffer::initPosQuat(
  NMP::Memory::Resource&     resource,
  const NMP::Memory::Format& internalBuffMemReqs,
  uint32_t                   numEntries,
  uint16_t                   refCount)
{
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);
  AttribDataTransformBuffer* result = (AttribDataTransformBuffer*)resource.ptr;
  resource.increment(sizeof(AttribDataTransformBuffer));

  result->m_transformBuffer = NMP::DataBuffer::initPosQuat(resource, internalBuffMemReqs, numEntries);

  result->setType(ATTRIB_TYPE_TRANSFORM_BUFFER);
  result->setRefCount(refCount);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransformBuffer* AttribDataTransformBuffer::initPosVelAngVel(
  NMP::Memory::Resource&     resource,
  const NMP::Memory::Format& internalBuffMemReqs,
  uint32_t                   numEntries,
  uint16_t                   refCount)
{
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);
  AttribDataTransformBuffer* result = (AttribDataTransformBuffer*)resource.ptr;
  resource.increment(sizeof(AttribDataTransformBuffer));

  result->m_transformBuffer = NMP::DataBuffer::initPosVelAngVel(resource, internalBuffMemReqs, numEntries);

  result->setType(ATTRIB_TYPE_TRANSFORM_BUFFER);
  result->setRefCount(refCount);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTransformBuffer::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  desc)
{
  AttribDataHandle result;

  NMP_ASSERT(desc);
  AttribDataTransformBufferCreateDesc* targetDesc = (AttribDataTransformBufferCreateDesc*)desc;

  NMP::Memory::Format internalBuffMemReqs;
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(targetDesc->m_size, result.m_format, internalBuffMemReqs);
  NMP_ASSERT(resource.format.size >= result.m_format.size);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = initPosQuat(resource, internalBuffMemReqs, targetDesc->m_size, 0);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTransformBuffer::outputMemReqs(Network* network)
{
  uint32_t numBones = network->getMaxTransformCount();
  NMP::Memory::Format internalBuffMemReqs;
  NMP::Memory::Format memReqs;
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(numBones, memReqs, internalBuffMemReqs);
  return memReqs;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTrajectoryDeltaTransform functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataTrajectoryDeltaTransform::locate(AttribData* target)
{
  AttribDataTrajectoryDeltaTransform* result = (AttribDataTrajectoryDeltaTransform*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_deltaAtt);
  NMP::endianSwap(result->m_deltaPos);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTrajectoryDeltaTransform::dislocate(AttribData* target)
{
  AttribDataTrajectoryDeltaTransform* result = (AttribDataTrajectoryDeltaTransform*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_deltaAtt);
  NMP::endianSwap(result->m_deltaPos);
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataTrajectoryDeltaTransform::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  uint32_t dataSize = (uint32_t)sizeof(AttribTrajectoryDeltaTransformOutputData);

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribTrajectoryDeltaTransformOutputData* output = (AttribTrajectoryDeltaTransformOutputData*) outputBuffer;
    AttribDataTrajectoryDeltaTransform* sourceAttrib = (AttribDataTrajectoryDeltaTransform*)toOutput;

    output->m_deltaPos = sourceAttrib->m_deltaPos;
    output->m_deltaAtt = sourceAttrib->m_deltaAtt;
    AttribTrajectoryDeltaTransformOutputData::endianSwap(outputBuffer);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTrajectoryDeltaTransform::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataTrajectoryDeltaTransform), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTrajectoryDeltaTransform* AttribDataTrajectoryDeltaTransform::init(
  NMP::Memory::Resource& resource,
  const NMP::Quat&       attitude,
  const NMP::Vector3&    position,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataTrajectoryDeltaTransform), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataTrajectoryDeltaTransform* result = (AttribDataTrajectoryDeltaTransform*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
  result->setRefCount(refCount);
  result->m_deltaAtt = attitude;
  result->m_deltaPos = position;
  result->m_filteredOut = false;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTrajectoryDeltaTransform::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  NMP::Quat attitude(NMP::Quat::kIdentity);
  NMP::Vector3 position(NMP::Vector3::InitZero);

  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    attitude,
    position,
    refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTrajectoryDeltaTransform::create(NMP::MemoryAllocator* allocator)
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  NMP::Quat attitude(NMP::Quat::kIdentity);
  NMP::Vector3 position(NMP::Vector3::InitZero);

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    attitude,
    position,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTrajectoryDeltaTransform::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTransform functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataTransform::locate(AttribData* target)
{
  AttribDataTransform* result = (AttribDataTransform*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_att);
  NMP::endianSwap(result->m_pos);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransform::dislocate(AttribData* target)
{
  AttribDataTransform* result = (AttribDataTransform*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_att);
  NMP::endianSwap(result->m_pos);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTransform::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataTransform), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransform* AttribDataTransform::init(
  NMP::Memory::Resource& resource,
  const NMP::Quat&       attitude,
  const NMP::Vector3&    position,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataTransform), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataTransform* result = (AttribDataTransform*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_TRANSFORM);
  result->setRefCount(refCount);
  result->m_att = attitude;
  result->m_pos = position;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTransform::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  NMP::Quat attitude(NMP::Quat::kIdentity);
  NMP::Vector3 position(NMP::Vector3::InitZero);

  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    attitude,
    position,
    refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTransform::create(
  NMP::MemoryAllocator* allocator,
  const NMP::Quat&      attitude,
  const NMP::Vector3&   position,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    attitude,
    position,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTransform::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataTransform::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  uint32_t dataSize = (uint32_t)sizeof(AttribTransformOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribTransformOutputData* output = (AttribTransformOutputData*) outputBuffer;
    AttribDataTransform* sourceAttrib = (AttribDataTransform*)toOutput;

    output->m_att = sourceAttrib->m_att;
    output->m_pos = sourceAttrib->m_pos;
    AttribTransformOutputData::endianSwap(outputBuffer);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataVelocity functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataVelocity::locate(AttribData* target)
{
  AttribDataVelocity* result = (AttribDataVelocity*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_linearVel);
  NMP::endianSwap(result->m_angularVel);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataVelocity::dislocate(AttribData* target)
{
  AttribDataVelocity* result = (AttribDataVelocity*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_linearVel);
  NMP::endianSwap(result->m_angularVel);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataVelocity::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataVelocity), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataVelocity* AttribDataVelocity::init(
  NMP::Memory::Resource& resource,
  const NMP::Vector3&    velocity,
  const NMP::Vector3&    angularVelocity,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataVelocity), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataVelocity* result = (AttribDataVelocity*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_VELOCITY);
  result->setRefCount(refCount);
  result->m_linearVel = velocity;
  result->m_angularVel = angularVelocity;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataVelocity::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint16_t refCount = 0;
  NMP::Vector3 vZero(NMP::Vector3::InitZero);
  
  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    vZero,
    vZero,
    refCount);
  
  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataVelocity::create(
  NMP::MemoryAllocator* allocator,
  const NMP::Vector3&   velocity,
  const NMP::Vector3&   angularVelocity,
  uint16_t              refCount)
{
  AttribDataHandle result;
  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  
  result.m_attribData = init(
    resource,
    velocity,
    angularVelocity,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataVelocity::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  uint32_t dataSize = (uint32_t)sizeof(AttribVelocityOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribVelocityOutputData* output = (AttribVelocityOutputData*) outputBuffer;
    AttribDataVelocity* sourceAttrib = (AttribDataVelocity*)toOutput;

    output->m_angularVel = sourceAttrib->m_angularVel;
    output->m_linearVel = sourceAttrib->m_linearVel;
    AttribVelocityOutputData::endianSwap(outputBuffer);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataSyncEventTrack functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataSyncEventTrack::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataSyncEventTrack), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSyncEventTrack* AttribDataSyncEventTrack::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataSyncEventTrack* result = (AttribDataSyncEventTrack*)resource.ptr;
  resource.increment(sizeof(AttribDataSyncEventTrack));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_SYNC_EVENT_TRACK);
  result->setRefCount(refCount);
  result->m_syncEventTrack.wipeDown();

  result->m_transitionOffset = 0;

  return result;
}

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void AttribDataSyncEventTrack::locate(AttribData* target)
{
  AttribDataSyncEventTrack* result = (AttribDataSyncEventTrack*)target;
  AttribData::locate(target);
  NMP::endianSwap(result->m_transitionOffset);
  result->m_syncEventTrack.locate();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSyncEventTrack::dislocate(AttribData* target)
{
  AttribDataSyncEventTrack* result = (AttribDataSyncEventTrack*)target;
  result->m_syncEventTrack.dislocate();
  NMP::endianSwap(result->m_transitionOffset);
  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataSyncEventTrack::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataSyncEventTrack::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataSyncEventTrack::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataSyncEventTrack::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  EventTrackSync* eventTrackSync = &(((AttribDataSyncEventTrack*)toOutput)->m_syncEventTrack);

  // m_NumEvents + DurationEvent buffer (float m_Start + float m_Duration).
  uint32_t dataSize =  sizeof(AttribSyncEventTrackOutputData) + (eventTrackSync->getNumEvents() * sizeof(AttribSyncEventTrackOutputData::Event));

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    // Initialise the output structure.
    AttribSyncEventTrackOutputData* output = (AttribSyncEventTrackOutputData*) outputBuffer;
    output->m_numEvents = eventTrackSync->getNumEvents();
    output->m_startEventIndex = eventTrackSync->getStartEventIndex();
    output->m_duration = eventTrackSync->getDuration();

    AttribSyncEventTrackOutputData::Event* events = NULL;
    if (output->m_numEvents)
      events = (AttribSyncEventTrackOutputData::Event*) ((char*)outputBuffer + sizeof(AttribSyncEventTrackOutputData));

    for (uint32_t i = 0; i < output->m_numEvents; ++i)
    {
      events[i].m_duration = eventTrackSync->getEvent(i)->getDuration();
      events[i].m_startTime = eventTrackSync->getEvent(i)->getStartTime();
      events[i].m_userData = eventTrackSync->getEvent(i)->getUserData();
      events[i].endianSwap();
    }

    NMP::netEndianSwap(output->m_duration);
    NMP::netEndianSwap(output->m_startEventIndex);
    NMP::netEndianSwap(output->m_numEvents);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataSampledEvents functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataSampledEvents::relocate(AttribData* target, void* location)
{
  AttribDataSampledEvents* result = (AttribDataSampledEvents*) target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_ATTRIB_DATA_ALIGNMENT));
  size_t offset = NMP::Memory::align(sizeof(AttribDataSampledEvents), NMP_NATURAL_TYPE_ALIGNMENT);
  TriggeredDiscreteEventsBuffer* discreteBuffer = (TriggeredDiscreteEventsBuffer*)(((size_t)result) + offset);
  result->m_discreteBuffer = (TriggeredDiscreteEventsBuffer*)(((size_t)location) + offset);
  discreteBuffer->relocate(result->m_discreteBuffer);

  NMP::Memory::Format format = TriggeredDiscreteEventsBuffer::getMemoryRequirements(
                                 discreteBuffer->getNumTriggeredEvents());
  offset += format.size;
  SampledCurveEventsBuffer* curveBuffer = (SampledCurveEventsBuffer*)(((size_t)result) + offset);
  result->m_curveBuffer = (SampledCurveEventsBuffer*)(((size_t)location) + offset);
  curveBuffer->relocate(result->m_curveBuffer);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataSampledEvents::getMemoryRequirements(
  uint32_t numDiscreteEvents,
  uint32_t numCurveEvents)
{
  NMP::Memory::Format result(sizeof(AttribDataSampledEvents), MR_ATTRIB_DATA_ALIGNMENT);

  result += TriggeredDiscreteEventsBuffer::getMemoryRequirements(numDiscreteEvents);
  result += SampledCurveEventsBuffer::getMemoryRequirements(numCurveEvents);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSampledEvents* AttribDataSampledEvents::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numDiscreteEvents,
  uint32_t                numCurveEvents,
  uint16_t                refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataSampledEvents), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataSampledEvents* result = (AttribDataSampledEvents*) resource.alignAndIncrement(memReqsHdr);
  
  result->setType(ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER);
  result->setRefCount(refCount);
  
  // Discrete buffer.
  result->m_discreteBuffer = TriggeredDiscreteEventsBuffer::init(resource, numDiscreteEvents);

  // Curve buffer.
  result->m_curveBuffer = SampledCurveEventsBuffer::init(resource, numCurveEvents);

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataSampledEvents::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  desc)
{
  AttribDataHandle result;

  NMP_ASSERT(desc);
  AttribDataSampledEventsCreateDesc* targetDesc = (AttribDataSampledEventsCreateDesc*)desc;

  result.m_format = getMemoryRequirements(targetDesc->m_numDiscreteEvents, targetDesc->m_numCurveEvents);
  // If this assertion triggers you may need to increase the size of MAX_NUM_DISCRETE_EVENTS or MAX_NUM_DURATION_EVENTS.
  NMP_ASSERT(resource.format.size >= result.m_format.size);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    targetDesc->m_numDiscreteEvents,
    targetDesc->m_numCurveEvents,
    targetDesc->m_refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataSampledEvents::create(
  NMP::MemoryAllocator* allocator,
  uint32_t              numDiscreteEvents,
  uint32_t              numCurveEvents,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numDiscreteEvents, numCurveEvents);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    numDiscreteEvents,
    numCurveEvents,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataSampledEvents::outputMemReqs(Network* NMP_UNUSED(network))
{
  // discrete events and curve events
  return getMemoryRequirements(MAX_NUM_DISCRETE_EVENTS, MAX_NUM_CURVE_EVENTS);
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataSampledEvents::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);
  AttribDataSampledEvents* source = (AttribDataSampledEvents*)toOutput;
  uint32_t numTriggeredEvents = 0;
  if (source->m_discreteBuffer)
    numTriggeredEvents = source->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t numSampledCurveEvents = 0;
  if (source->m_curveBuffer)
    numSampledCurveEvents = source->m_curveBuffer->getNumSampledEvents();

  uint32_t dataSize =  sizeof(AttribSampledEventsOutputData) +
                       (numTriggeredEvents * sizeof(AttribSampledEventsOutputData::TriggeredEvent)) +
                       (numSampledCurveEvents * sizeof(AttribSampledEventsOutputData::SampledEvent));
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    // Initialise the output structure.
    AttribSampledEventsOutputData* output = (AttribSampledEventsOutputData*) outputBuffer;

    // Triggered events.
    output->m_numTriggeredDiscreteEvents = numTriggeredEvents;
    output->m_triggeredDiscreteEventsPtrOffset = 0;
    if (output->m_numTriggeredDiscreteEvents)
    {
      AttribSampledEventsOutputData::TriggeredEvent* triggeredDiscreteEvents =
        (AttribSampledEventsOutputData::TriggeredEvent*)
        ((char*)outputBuffer + sizeof(AttribSampledEventsOutputData));
      for (uint32_t i = 0; i < output->m_numTriggeredDiscreteEvents; ++i)
      {
        TriggeredDiscreteEvent* triggeredEvent = source->m_discreteBuffer->getTriggeredEvent(i);
        NMP_ASSERT(triggeredEvent);
        triggeredDiscreteEvents[i].m_blendWeight = triggeredEvent->getBlendWeight();
        triggeredDiscreteEvents[i].m_sourceEventUserData = triggeredEvent->getSourceEventUserData();
        triggeredDiscreteEvents[i].m_sourceTrackUserData = triggeredEvent->getSourceTrackUserData();
        triggeredDiscreteEvents[i].endianSwap();
      }
      UNFIX_PTR_RELATIVE(AttribSampledEventsOutputData::TriggeredEvent, triggeredDiscreteEvents, output);
      output->m_triggeredDiscreteEventsPtrOffset = (uint32_t)((ptrdiff_t)triggeredDiscreteEvents);
      NMP::netEndianSwap(output->m_triggeredDiscreteEventsPtrOffset);
    }

    // Sampled events.
    output->m_numSampledCurveEvents = numSampledCurveEvents;
    output->m_sampledCurveEventsPtrOffset = 0;
    if (output->m_numSampledCurveEvents)
    {
      AttribSampledEventsOutputData::SampledEvent* sampledCurveEvents = (AttribSampledEventsOutputData::SampledEvent*)
          ((char*)outputBuffer +
           sizeof(AttribSampledEventsOutputData) +
           (numTriggeredEvents * sizeof(AttribSampledEventsOutputData::TriggeredEvent)));
      for (uint32_t i = 0; i < output->m_numSampledCurveEvents; ++i)
      {
        SampledCurveEvent* sampledEvent = source->m_curveBuffer->getSampledEvent(i);
        NMP_ASSERT(sampledEvent);
        sampledCurveEvents[i].m_blendWeight = sampledEvent->getBlendWeight();
        sampledCurveEvents[i].m_value = sampledEvent->getValue();
        sampledCurveEvents[i].m_sourceEventUserData = sampledEvent->getSourceEventUserData();
        sampledCurveEvents[i].m_sourceTrackUserData = sampledEvent->getSourceTrackUserData();
        sampledCurveEvents[i].endianSwap();
      }
      UNFIX_PTR_RELATIVE(AttribSampledEventsOutputData::SampledEvent, sampledCurveEvents, output);
      output->m_sampledCurveEventsPtrOffset = (uint32_t)((ptrdiff_t)sampledCurveEvents);
      NMP::netEndianSwap(output->m_sampledCurveEventsPtrOffset);
    }

    NMP::netEndianSwap(output->m_numTriggeredDiscreteEvents);
    NMP::netEndianSwap(output->m_numSampledCurveEvents);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataDurationEventTrackSet functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataDurationEventTrackSet::relocate(AttribData* target, void* location)
{
  AttribDataDurationEventTrackSet* result = (AttribDataDurationEventTrackSet*) target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_ATTRIB_DATA_ALIGNMENT));
  size_t offset = NMP::Memory::align(sizeof(AttribDataDurationEventTrackSet), NMP_NATURAL_TYPE_ALIGNMENT);
  EventTrackDurationSet* durationEventTrackSet = (EventTrackDurationSet*)(((size_t)result) + offset);
  result->m_durationEventTrackSet = (EventTrackDurationSet*)(((size_t)location) + offset);
  durationEventTrackSet->relocate(result->m_durationEventTrackSet);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataDurationEventTrackSet::getMemoryRequirements(
  uint32_t numEventTracks,
  uint32_t eventPoolSize)  // Number of events in each track.
{
  NMP::Memory::Format result(sizeof(AttribDataDurationEventTrackSet), MR_ATTRIB_DATA_ALIGNMENT);
  result += EventTrackDurationSet::getMemoryRequirements(numEventTracks, eventPoolSize);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataDurationEventTrackSet* AttribDataDurationEventTrackSet::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numEventTracks,
  uint32_t                eventPoolSize,
  uint16_t                refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataDurationEventTrackSet), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataDurationEventTrackSet* result = (AttribDataDurationEventTrackSet*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_DURATION_EVENT_TRACK_SET);
  result->setRefCount(refCount);
  
  result->m_durationEventTrackSet = EventTrackDurationSet::init(resource, numEventTracks, eventPoolSize);

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataDurationEventTrackSet::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  desc)
{
  AttribDataHandle result;
  
  NMP_ASSERT(desc);
  AttribDataDurationEventTrackSetCreateDesc* targetDesc = (AttribDataDurationEventTrackSetCreateDesc*)desc;
  
  result.m_format = getMemoryRequirements(targetDesc->m_numEventTracks, targetDesc->m_eventPoolSize);

  // If this assertion triggers you may need to increase the size of MAX_NUM_DURATION_EVENTS or MAX_NUM_DURATION_TRACKS.
  NMP_ASSERT(resource.format.size >= result.m_format.size);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    targetDesc->m_numEventTracks,
    targetDesc->m_eventPoolSize,
    targetDesc->m_refCount);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataDurationEventTrackSet::create(
  NMP::MemoryAllocator* allocator,
  uint32_t              numEventTracks,
  uint32_t              eventPoolSize,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numEventTracks, eventPoolSize);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    numEventTracks,
    eventPoolSize,
    refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataDurationEventTrackSet::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements(MAX_NUM_DURATION_TRACKS, MAX_NUM_DURATION_EVENTS);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataDurationEventTrackSet::getInstanceMemoryRequirements()
{
  NMP_ASSERT(m_durationEventTrackSet && m_durationEventTrackSet->getEventsPool());
  uint32_t eventsPoolMaxCount = m_durationEventTrackSet->getEventsPool()->m_size;
  uint32_t numEventTracks = m_durationEventTrackSet->getNumTracks();
  return getMemoryRequirements(numEventTracks, eventsPoolMaxCount);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataDurationEventTrackSet::getPackedInstanceMemoryRequirements()
{
  NMP_ASSERT(m_durationEventTrackSet);
  uint32_t numEventTracks = m_durationEventTrackSet->getNumTracks();
  uint32_t packedEventsCount = 0;
  for (uint32_t i = 0; i < numEventTracks; ++i)
  {
    NMP_ASSERT(m_durationEventTrackSet->getTrack(i));
    packedEventsCount += m_durationEventTrackSet->getTrack(i)->getNumEvents();
  }
  return getMemoryRequirements(numEventTracks, packedEventsCount);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataDurationEventTrackSet* AttribDataDurationEventTrackSet::copyAndPack(
  NMP::Memory::Resource& resource,
  NMP::MemoryAllocator*  allocator,
  uint16_t               refCount)
{
  NMP_ASSERT(resource.ptr);
  NMP_ASSERT(NMP_IS_ALIGNED(resource.ptr, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(resource.format.size >= getInstanceMemoryRequirements().size);
  AttribDataDurationEventTrackSet* outputAttrib = (AttribDataDurationEventTrackSet*) resource.ptr;
  resource.increment(NMP::Memory::align(sizeof(AttribDataDurationEventTrackSet), NMP_NATURAL_TYPE_ALIGNMENT));
  outputAttrib->setType(ATTRIB_TYPE_DURATION_EVENT_TRACK_SET);
  outputAttrib->m_durationEventTrackSet = m_durationEventTrackSet->copyAndPack(resource);
  outputAttrib->m_allocator = allocator;
  outputAttrib->setRefCount(refCount);

  return outputAttrib;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataDurationEventTrackSet* AttribDataDurationEventTrackSet::copyAndPack(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(resource.ptr);
  NMP_ASSERT(NMP_IS_ALIGNED(resource.ptr, MR_ATTRIB_DATA_ALIGNMENT));
  NMP_ASSERT(resource.format.size >= getInstanceMemoryRequirements().size);
  AttribDataDurationEventTrackSet* outputAttrib = (AttribDataDurationEventTrackSet*) resource.ptr;
  resource.increment(NMP::Memory::align(sizeof(AttribDataDurationEventTrackSet), NMP_NATURAL_TYPE_ALIGNMENT));
  outputAttrib->setType(ATTRIB_TYPE_DURATION_EVENT_TRACK_SET);
  outputAttrib->m_durationEventTrackSet = m_durationEventTrackSet->copyAndPack(resource);
  outputAttrib->setRefCount(refCount);

  return outputAttrib;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataDurationEventTrackSet::serializeTx(
  Network*    net,
  MR::NodeID  owningNodeID,
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);
  EventTrackDurationSet* setToOutput = ((AttribDataDurationEventTrackSet*)toOutput)->m_durationEventTrackSet;

  uint32_t dataSize = sizeof(AttribDurationEventTrackSetOutputData);
  dataSize += (sizeof(AttribDurationEventTrackSetOutputData::Track) * setToOutput->getNumTracks());
  for (uint32_t i = 0; i < setToOutput->getNumTracks(); ++i)
  {
    dataSize += (setToOutput->getTrack(i)->getNumEvents() * sizeof(AttribDurationEventTrackSetOutputData::Event));
  }

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    // Try and find the associated sync event track so that we can convert sync event info in to linear space.
    AttribDataSyncEventTrack* syncTrackAttribData = net->getOptionalAttribData<AttribDataSyncEventTrack>(
          ATTRIB_SEMANTIC_SYNC_EVENT_TRACK,
          owningNodeID,
          INVALID_NODE_ID,
          net->getCurrentFrameNo(),
          net->getOutputAnimSetIndex(owningNodeID));          
    EventTrackSync* syncTrack = NULL;
    if (syncTrackAttribData)
      syncTrack = &(syncTrackAttribData->m_syncEventTrack);

    // Initialise the output structure.
    AttribDurationEventTrackSetOutputData* output = (AttribDurationEventTrackSetOutputData*) outputBuffer;
    output->m_numEventTracks = setToOutput->getNumTracks();
    AttribDurationEventTrackSetOutputData::Track* tracks = 0;
    if (output->m_numEventTracks)
      tracks = (AttribDurationEventTrackSetOutputData::Track*) ((char*)outputBuffer + sizeof(AttribDurationEventTrackSetOutputData));

    // Copy the tracks.
    AttribDurationEventTrackSetOutputData::Event* currentEvent = (AttribDurationEventTrackSetOutputData::Event*)
        ((char*)tracks + (sizeof(AttribDurationEventTrackSetOutputData::Track) * output->m_numEventTracks));
    for (uint32_t i = 0; i < output->m_numEventTracks; ++i)
    {
      EventTrackDuration* sourceTrack = setToOutput->getTrack(i);
      tracks[i].m_numEvents = sourceTrack->getNumEvents();
      tracks[i].m_numSyncEvents = sourceTrack->getNumSyncEvents();
      tracks[i].m_runtimeID = sourceTrack->getRuntimeID();
      tracks[i].m_userData = sourceTrack->getUserData();
      AttribDurationEventTrackSetOutputData::Event* events = 0;
      if (tracks[i].m_numEvents)
        events = currentEvent;

      // Copy the events for the track.
      EventDuration* sourceEvent = sourceTrack->getEventsListHead();
      for (uint32_t j = 0; j < tracks[i].m_numEvents; ++j)
      {
        NMP_ASSERT(sourceEvent);
        events[j].m_weight = sourceEvent->getWeight();
        events[j].m_userData = sourceEvent->getUserData();
        events[j].m_syncEventSpaceStartPoint = sourceEvent->getSyncEventSpaceStartPoint();
        events[j].m_syncEventSpaceMidPoint = sourceEvent->getSyncEventSpaceMidPoint();
        events[j].m_syncEventSpaceDuration = sourceEvent->getSyncEventSpaceDuration();

        events[j].m_normalisedStartPoint = 0.0f;
        events[j].m_normalisedDuration = 0.0f;
        if (syncTrack)
        {
          events[j].m_normalisedStartPoint = syncTrack->getRealPosFractionFromAdjSyncEventPos(sourceEvent->getSyncEventSpaceStartPoint());
          float syncEventSpaceEndPoint = sourceEvent->getSyncEventSpaceStartPoint() + sourceEvent->getSyncEventSpaceDuration();
          float normalisedEndPoint = syncTrack->getRealPosFractionFromAdjSyncEventPos(syncEventSpaceEndPoint);
          events[j].m_normalisedDuration = normalisedEndPoint - events[j].m_normalisedStartPoint;
          if (events[j].m_normalisedDuration < 0.0f)
          {
            // Cope with wrap around.
            events[j].m_normalisedDuration = normalisedEndPoint + (1.0f - events[j].m_normalisedStartPoint);
          }
          else if ((((float)tracks[i].m_numSyncEvents) - events[j].m_syncEventSpaceDuration) < 0.001f)
          {
            // An event that last the length of the sync track.
            events[j].m_normalisedDuration = 1.0f;
          }
        }

        // Dislocate the event data.
        events[j].endianSwap();
        sourceEvent = sourceEvent->getNext(sourceTrack->getPool());
      }
      currentEvent = &(currentEvent[tracks[i].m_numEvents]);

      // Dislocate the track data.
      if (tracks[i].m_numEvents)
        UNFIX_PTR_RELATIVE(AttribDurationEventTrackSetOutputData::Event, events, &(tracks[i]));
      tracks[i].m_eventsPtrOffset = (uint32_t)((ptrdiff_t)events);
      NMP::netEndianSwap(tracks[i].m_eventsPtrOffset);
      NMP::netEndianSwap(tracks[i].m_userData);
      NMP::netEndianSwap(tracks[i].m_runtimeID);
      NMP::netEndianSwap(tracks[i].m_numSyncEvents);
      NMP::netEndianSwap(tracks[i].m_numEvents);
    }

    // Dislocate the track set data.
    if (output->m_numEventTracks)
      UNFIX_PTR_RELATIVE(AttribDurationEventTrackSetOutputData::Track, tracks, output);
    output->m_tracksPtrOffset = (uint32_t)((ptrdiff_t)tracks);
    NMP::netEndianSwap(output->m_tracksPtrOffset);
    NMP::netEndianSwap(output->m_numEventTracks);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataSourceEventTrackSet functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataSourceEventTrackSet::locate(AttribData* target)
{
  MR::Manager& manager = MR::Manager::getInstance();
  AttribDataSourceEventTrackSet* result = (AttribDataSourceEventTrackSet*)target;

  AttribData::locate(target);

  // Discrete events
  NMP::endianSwap(result->m_numDiscreteEventTracks);

  NMP::endianSwap(result->m_sourceDiscreteEventTracks);
  REFIX_PTR_RELATIVE(EventTrackDefDiscrete*, result->m_sourceDiscreteEventTracks, result);

  NMP::endianSwap(result->m_sourceDiscreteEventTrackSizes);
  REFIX_PTR_RELATIVE(uint32_t, result->m_sourceDiscreteEventTrackSizes, result);

  for (uint32_t i = 0; i < result->m_numDiscreteEventTracks; i++)
  {
    // Fixup each of the actual event tracks
    NMP::endianSwap(result->m_sourceDiscreteEventTracks[i]);
    result->m_sourceDiscreteEventTracks[i] = (EventTrackDefDiscrete*)
      manager.getObjectPtrFromObjectID((ObjectID)result->m_sourceDiscreteEventTracks[i]);

    NMP::endianSwap(result->m_sourceDiscreteEventTrackSizes[i]);
  }

  // Curve events
  NMP::endianSwap(result->m_numCurveEventTracks);

  NMP::endianSwap(result->m_sourceCurveEventTracks);
  REFIX_PTR_RELATIVE(EventTrackDefCurve*, result->m_sourceCurveEventTracks, result);

  NMP::endianSwap(result->m_sourceCurveEventTrackSizes);
  REFIX_PTR_RELATIVE(uint32_t, result->m_sourceCurveEventTrackSizes, result);

  for (uint32_t i = 0; i < result->m_numCurveEventTracks; i++)
  {
    // Fixup each of the actual event tracks
    NMP::endianSwap(result->m_sourceCurveEventTracks[i]);
    result->m_sourceCurveEventTracks[i] = (EventTrackDefCurve*)
      manager.getObjectPtrFromObjectID((ObjectID)result->m_sourceCurveEventTracks[i]);

    NMP::endianSwap(result->m_sourceCurveEventTrackSizes[i]);
  }

  // Duration events
  NMP::endianSwap(result->m_numDurEventTracks);

  NMP::endianSwap(result->m_sourceDurEventTracks);
  REFIX_PTR_RELATIVE(EventTrackDefDuration*, result->m_sourceDurEventTracks, result);

  NMP::endianSwap(result->m_sourceDurEventTrackSizes);
  REFIX_PTR_RELATIVE(uint32_t, result->m_sourceDurEventTrackSizes, result);

  for (uint32_t i = 0; i < result->m_numDurEventTracks; ++i)
  {
    // Fixup each of the actual event tracks
    NMP::endianSwap(result->m_sourceDurEventTracks[i]);
    result->m_sourceDurEventTracks[i] = (EventTrackDefDuration*)
      Manager::getInstance().getObjectPtrFromObjectID((ObjectID)result->m_sourceDurEventTracks[i]);
    NMP::endianSwap(result->m_sourceDurEventTrackSizes[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceEventTrackSet::dislocate(AttribData* target)
{
  MR::Manager& manager = MR::Manager::getInstance();
  AttribDataSourceEventTrackSet* result = (AttribDataSourceEventTrackSet*)target;

  // Discrete events
  for (uint32_t i = 0; i < result->m_numDiscreteEventTracks; i++)
  {
    result->m_sourceDiscreteEventTracks[i] = (EventTrackDefDiscrete*)
      manager.getObjectIDFromObjectPtr(result->m_sourceDiscreteEventTracks[i]);
    NMP_ASSERT((ObjectID)result->m_sourceDiscreteEventTracks[i] != INVALID_OBJECT_ID);
    NMP::endianSwap(result->m_sourceDiscreteEventTracks[i]);

    NMP::endianSwap(result->m_sourceDiscreteEventTrackSizes[i]);
  }

  UNFIX_PTR_RELATIVE(uint32_t, result->m_sourceDiscreteEventTrackSizes, result);
  NMP::endianSwap(result->m_sourceDiscreteEventTrackSizes);

  UNFIX_PTR_RELATIVE(EventTrackDefDiscrete*, result->m_sourceDiscreteEventTracks, result);
  NMP::endianSwap(result->m_sourceDiscreteEventTracks);

  NMP::endianSwap(result->m_numDiscreteEventTracks);

  // Curve events
  for (uint32_t i = 0; i < result->m_numCurveEventTracks; i++)
  {
    result->m_sourceCurveEventTracks[i] = (EventTrackDefCurve*)
      manager.getObjectIDFromObjectPtr(result->m_sourceCurveEventTracks[i]);
    NMP_ASSERT((ObjectID)result->m_sourceCurveEventTracks[i] != INVALID_OBJECT_ID);
    NMP::endianSwap(result->m_sourceCurveEventTracks[i]);

    NMP::endianSwap(result->m_sourceCurveEventTrackSizes[i]);
  }

  UNFIX_PTR_RELATIVE(uint32_t, result->m_sourceCurveEventTrackSizes, result);
  NMP::endianSwap(result->m_sourceCurveEventTrackSizes);

  UNFIX_PTR_RELATIVE(EventTrackDefCurve*, result->m_sourceCurveEventTracks, result);
  NMP::endianSwap(result->m_sourceCurveEventTracks);

  NMP::endianSwap(result->m_numCurveEventTracks);

  // Duration events
  for (uint32_t i = 0; i < result->m_numDurEventTracks; ++i)
  {
    result->m_sourceDurEventTracks[i] = (EventTrackDefDuration*)
      Manager::getInstance().getObjectIDFromObjectPtr(result->m_sourceDurEventTracks[i]);
    NMP_ASSERT((ObjectID)result->m_sourceDurEventTracks[i] != INVALID_OBJECT_ID);
    NMP::endianSwap(result->m_sourceDurEventTracks[i]);
    NMP::endianSwap(result->m_sourceDurEventTrackSizes[i]);
  }

  UNFIX_PTR_RELATIVE(uint32_t, result->m_sourceDurEventTrackSizes, result);
  NMP::endianSwap(result->m_sourceDurEventTrackSizes);

  UNFIX_PTR_RELATIVE(EventTrackDefDuration*, result->m_sourceDurEventTracks, result);
  NMP::endianSwap(result->m_sourceDurEventTracks);

  NMP::endianSwap(result->m_numDurEventTracks);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataSourceEventTrackSet::getMemoryRequirements(
  uint32_t numDiscreteEventTracks, 
  uint32_t numCurveEventTracks, 
  uint32_t numDurationEventTracks)
{
  NMP::Memory::Format result(sizeof(AttribDataSourceEventTrackSet), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of pointers.
  result += NMP::Memory::Format(sizeof(EventTrackDefDiscrete*) * numDiscreteEventTracks, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for memory requirements array.
  result += NMP::Memory::Format(sizeof(uint32_t) * numDiscreteEventTracks, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for the array of pointers
  result += NMP::Memory::Format(sizeof(EventTrackDefCurve*) * numCurveEventTracks, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for memory requirements array.
  result += NMP::Memory::Format(sizeof(uint32_t) * numCurveEventTracks, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for the array of pointers
  result += NMP::Memory::Format(sizeof(EventTrackDefDuration*) * numDurationEventTracks, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for memory requirements array.
  result += NMP::Memory::Format(sizeof(uint32_t) * numDurationEventTracks, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSourceEventTrackSet* AttribDataSourceEventTrackSet::init(
  NMP::Memory::Resource& resource,
  uint32_t               numDiscreteEventTracks,
  uint32_t               numCurveEventTracks,
  uint32_t               numDurationEventTracks,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements(numDiscreteEventTracks, numCurveEventTracks, numDurationEventTracks).alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataSourceEventTrackSet* result = (AttribDataSourceEventTrackSet*)resource.ptr;
  resource.increment(sizeof(AttribDataSourceEventTrackSet));

  result->setType(ATTRIB_TYPE_SOURCE_EVENT_TRACKS);
  result->setRefCount(refCount);

  // ------------------------
  // Discrete events
  result->m_numDiscreteEventTracks = numDiscreteEventTracks;

  // Array of pointers to event tracks.
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sourceDiscreteEventTracks = (EventTrackDefDiscrete**)resource.ptr;
  resource.increment(sizeof(EventTrackDefDiscrete*) * numDiscreteEventTracks);

  // Array of event track memory requirements.
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sourceDiscreteEventTrackSizes = (uint32_t*)resource.ptr;
  resource.increment(sizeof(uint32_t) * numDiscreteEventTracks);

  for (uint32_t i = 0; i < numDiscreteEventTracks; i++)
  {
    result->m_sourceDiscreteEventTracks[i] = NULL;
    result->m_sourceDiscreteEventTrackSizes[i] = 0;
  }

  // ------------------------
  // Curve events
  result->m_numCurveEventTracks = numCurveEventTracks;

  // Array of pointers to event tracks
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sourceCurveEventTracks = (EventTrackDefCurve**)resource.ptr;
  resource.increment(sizeof(EventTrackDefCurve*) * numCurveEventTracks);

  // Array of event track memory requirements.
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sourceCurveEventTrackSizes = (uint32_t*)resource.ptr;
  resource.increment(sizeof(uint32_t) * numCurveEventTracks);

  for (uint32_t i = 0; i < numCurveEventTracks; i++)
  {
    result->m_sourceCurveEventTracks[i] = NULL;
    result->m_sourceCurveEventTrackSizes[i] = 0;
  }

  // ------------------------
  // Duration events
  result->m_numDurEventTracks = numDurationEventTracks;

  // Array of pointers to event tracks.
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sourceDurEventTracks = (EventTrackDefDuration**)resource.ptr;
  resource.increment(sizeof(EventTrackDefDuration*) * numDurationEventTracks);

  // Array of event track memory requirements.
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_sourceDurEventTrackSizes = (uint32_t*)resource.ptr;
  resource.increment(sizeof(uint32_t) * numDurationEventTracks);

  // Initialise arrays.
  for (uint32_t i = 0; i < numDurationEventTracks; ++i)
  {
    result->m_sourceDurEventTracks[i] = NULL;
    result->m_sourceDurEventTrackSizes[i] = 0;
  }

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceEventTrackSet::relocate(AttribData* target, void* location)
{
  AttribDataSourceEventTrackSet* result = (AttribDataSourceEventTrackSet*) target;
  
  // Event track class itself.
  void* ptr = location;

  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataSourceEventTrackSet));

  // ------------------------
  // Discrete events
  // NOTE: This function alone cannot be used to relocate to a new memory space (SPU) as the pointed to
  // EventTrackDefDiscrete's are not relocated by this function.

  // Array of pointers to event tracks.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_sourceDiscreteEventTracks = (EventTrackDefDiscrete**)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(EventTrackDefDiscrete*) * result->m_numDiscreteEventTracks));

  // Array of event track memory requirements.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_sourceDiscreteEventTrackSizes = (uint32_t*)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(uint32_t) * result->m_numDiscreteEventTracks));

  // ------------------------
  // Curve events
  // NOTE: This function alone cannot be used to relocate to a new memory space (SPU) as the pointed to
  // EventTrackDefCurve's are not relocated by this function.

  // Array of pointers to event tracks.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_sourceCurveEventTracks = (EventTrackDefCurve**)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(EventTrackDefCurve*) * result->m_numCurveEventTracks));

  // Array of event track memory requirements.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_sourceCurveEventTrackSizes = (uint32_t*)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(uint32_t) * result->m_numCurveEventTracks));

  // ------------------------
  // Duration events
  // NOTE: This function alone cannot be used to relocate to a new memory space (SPU) as the pointed to
  // EventTrackDefDuration's are not relocated by this function.

  // Array of pointers to event tracks.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_sourceDurEventTracks = (EventTrackDefDuration**)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(EventTrackDefDuration*) * result->m_numDurEventTracks));

  // Array of event track memory requirements.
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_sourceDurEventTrackSizes = (uint32_t*)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(uint32_t) * result->m_numDurEventTracks));

}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_HOST_CELL_SPU
void AttribDataSourceEventTrackSet::prepDiscreteEventTrackSetForSPU(AttribData* target, NMP::MemoryAllocator* allocator)
{
  AttribDataSourceEventTrackSet* result = (AttribDataSourceEventTrackSet*)target;

  for (uint32_t i = 0; i < result->m_numDiscreteEventTracks; ++i)
  {
    // Allocate local memory to store the referenced duration event track.
    NMP::Memory::Resource resource =
      NMPAllocatorAllocateFromFormat(allocator, NMP::Memory::Format(result->m_sourceDiscreteEventTrackSizes[i], MR_ATTRIB_DATA_ALIGNMENT));
    NMP_ASSERT(resource.ptr);

    // DMA duration event track from system mem to locally allocated track.
    NMP::SPUDMAController::dmaGet(
      resource.ptr,
      (uint32_t) result->m_sourceDiscreteEventTracks[i],
      result->m_sourceDiscreteEventTrackSizes[i]);
    NMP::SPUDMAController::dmaWaitAll();

    // Set the new event track pointer pointer.
    result->m_sourceDiscreteEventTracks[i] = (EventTrackDefDiscrete*)resource.ptr;

    // Sort out all internal pointers in the event track.
    result->m_sourceDiscreteEventTracks[i]->relocate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceEventTrackSet::prepCurveEventTrackSetForSPU(AttribData* target, NMP::MemoryAllocator* allocator)
{
  AttribDataSourceEventTrackSet* result = (AttribDataSourceEventTrackSet*)target;

  for (uint32_t i = 0; i < result->m_numCurveEventTracks; ++i)
  {
    // Allocate local memory to store the referenced duration event track.
    NMP::Memory::Resource resource =
      NMPAllocatorAllocateFromFormat(allocator, NMP::Memory::Format(result->m_sourceCurveEventTrackSizes[i], MR_ATTRIB_DATA_ALIGNMENT));
    NMP_ASSERT(resource.ptr);

    // DMA duration event track from system mem to locally allocated track.
    NMP::SPUDMAController::dmaGet(
      resource.ptr,
      (uint32_t) result->m_sourceCurveEventTracks[i],
      result->m_sourceCurveEventTrackSizes[i]);
    NMP::SPUDMAController::dmaWaitAll();

    // Set the new event track pointer pointer.
    result->m_sourceCurveEventTracks[i] = (EventTrackDefCurve*)resource.ptr;

    // Sort out all internal pointers in the event track.
    result->m_sourceCurveEventTracks[i]->relocate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSourceEventTrackSet::prepDurationEventTrackSetForSPU(AttribData* target, NMP::MemoryAllocator* allocator)
{
  AttribDataSourceEventTrackSet* result = (AttribDataSourceEventTrackSet*)target;

  for (uint32_t i = 0; i < result->m_numDurEventTracks; ++i)
  {
    // Allocate local memory to store the referenced duration event track.
    NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, NMP::Memory::Format(result->m_sourceDurEventTrackSizes[i], MR_ATTRIB_DATA_ALIGNMENT));
    NMP_ASSERT(resource.ptr);

    // DMA duration event track from system mem to locally allocated track.
    NMP::SPUDMAController::dmaGet(
      resource.ptr,
      (uint32_t)result->m_sourceDurEventTracks[i],
      result->m_sourceDurEventTrackSizes[i]);
    NMP::SPUDMAController::dmaWaitAll();

    // Set the new event track pointer pointer.
    result->m_sourceDurEventTracks[i] = (EventTrackDefDuration*)resource.ptr;

    // Sort out all internal pointers in the event track.
    result->m_sourceDurEventTracks[i]->relocate();
  }
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataSourceEventTrackSet::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);
  AttribDataSourceEventTrackSet* setToOutput = (AttribDataSourceEventTrackSet*)toOutput;

  uint32_t dataSize = sizeof(AttribSourceEventTrackSetOutputData);
  dataSize += (sizeof(AttribSourceEventTrackSetOutputData::Track) * setToOutput->m_numDiscreteEventTracks);
  for (uint32_t i = 0; i < setToOutput->m_numDiscreteEventTracks; ++i)
  {
    dataSize += (setToOutput->m_sourceDiscreteEventTracks[i]->getNumEvents() * sizeof(AttribSourceEventTrackSetOutputData::Event));
  }

  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    // Initialise the output structure.
    AttribSourceEventTrackSetOutputData* output = (AttribSourceEventTrackSetOutputData*) outputBuffer;
    output->m_numEventTracks = setToOutput->m_numDiscreteEventTracks;
    AttribSourceEventTrackSetOutputData::Track* tracks = 0;
    if (output->m_numEventTracks)
      tracks = (AttribSourceEventTrackSetOutputData::Track*) ((char*)outputBuffer + sizeof(AttribSourceEventTrackSetOutputData));

    // Copy the tracks.
    AttribSourceEventTrackSetOutputData::Event* currentEvent = (AttribSourceEventTrackSetOutputData::Event*)
      ((char*)tracks + (sizeof(AttribSourceEventTrackSetOutputData::Track) * output->m_numEventTracks));
    for (uint32_t i = 0; i < output->m_numEventTracks; ++i)
    {
      EventTrackDefDiscrete* sourceTrack = setToOutput->m_sourceDiscreteEventTracks[i];
      tracks[i].m_numEvents = sourceTrack->getNumEvents();
      tracks[i].m_userData = sourceTrack->getUserData();
      AttribSourceEventTrackSetOutputData::Event* events = 0;
      if (tracks[i].m_numEvents)
        events = currentEvent;

      // Copy the events for the track.
      for (uint32_t j = 0; j < tracks[i].m_numEvents; ++j)
      {
        EventDefDiscrete* sourceEvent = sourceTrack->getEvent(j);
        NMP_ASSERT(sourceEvent);

        events[j].m_startTime = sourceEvent->getStartTime();
        events[j].m_userData = sourceEvent->getUserData();

        // Dislocate the event data.
        NMP::netEndianSwap(events[j].m_startTime);
        NMP::netEndianSwap(events[j].m_userData);
      }
      currentEvent = &(currentEvent[tracks[i].m_numEvents]);

      // Dislocate the track data.
      if (tracks[i].m_numEvents)
        UNFIX_PTR_RELATIVE(AttribSourceEventTrackSetOutputData::Event, events, &(tracks[i]));
      tracks[i].m_eventsPtrOffset = (uint32_t)((ptrdiff_t)events);
      NMP::netEndianSwap(tracks[i].m_eventsPtrOffset);
      NMP::netEndianSwap(tracks[i].m_userData);
      NMP::netEndianSwap(tracks[i].m_numEvents);
    }

    // Dislocate the track set data.
    if (output->m_numEventTracks)
      UNFIX_PTR_RELATIVE(AttribSourceEventTrackSetOutputData::Track, tracks, output);
    output->m_tracksPtrOffset = (uint32_t)((ptrdiff_t)tracks);
    NMP::netEndianSwap(output->m_tracksPtrOffset);
    NMP::netEndianSwap(output->m_numEventTracks);
  }

  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataClosestAnimDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataClosestAnimDef::ClosestAnimSourceData::getMemoryRequirements(uint32_t length, bool useVelocity)
{
  // Header
  NMP::Memory::Format result(sizeof(ClosestAnimSourceData), NMP_NATURAL_TYPE_ALIGNMENT);

  NMP::Memory::Format memReqsVec3 = CompressedDataBufferVector3::getMemoryRequirements(length);
  NMP::Memory::Format memReqsQuat = CompressedDataBufferQuat::getMemoryRequirements(length);
  
  // Transform data
  result += memReqsVec3;
  result += memReqsQuat;
  
  // Velocity data
  if (useVelocity)
  {
    result += memReqsVec3;
    result += memReqsVec3;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataClosestAnimDef::ClosestAnimSourceData* AttribDataClosestAnimDef::ClosestAnimSourceData::init(
  NMP::Memory::Resource& resource,
  uint32_t               length,
  bool                   useVelocity)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(ClosestAnimSourceData), NMP_NATURAL_TYPE_ALIGNMENT);
  ClosestAnimSourceData* result = (ClosestAnimSourceData*)resource.alignAndIncrement(memReqsHdr);

  // Transform data
  result->m_sourceTransformsPos = CompressedDataBufferVector3::init(resource, length);
  result->m_sourceTransformsAtt = CompressedDataBufferQuat::init(resource, length);
  
  // Velocity data
  if (useVelocity)
  {
    result->m_sourceTransformsPosVel = CompressedDataBufferVector3::init(resource, length);
    result->m_sourceTransformsAngVel = CompressedDataBufferVector3::init(resource, length);
  }
  else
  {
    result->m_sourceTransformsPosVel = NULL;
    result->m_sourceTransformsAngVel = NULL;
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataClosestAnimDef::ClosestAnimSourceData::locate(ClosestAnimSourceData* target)
{
  REFIX_SWAP_PTR_RELATIVE(CompressedDataBufferVector3, target->m_sourceTransformsPos, target);
  target->m_sourceTransformsPos->locate();
  
  REFIX_SWAP_PTR_RELATIVE(CompressedDataBufferQuat, target->m_sourceTransformsAtt, target);
  target->m_sourceTransformsAtt->locate();
  
  NMP::endianSwap(target->m_sourceTransformsPosVel);
  if (target->m_sourceTransformsPosVel)
  {
    REFIX_PTR_RELATIVE(CompressedDataBufferVector3, target->m_sourceTransformsPosVel, target);
    target->m_sourceTransformsPosVel->locate();
  }
  
  NMP::endianSwap(target->m_sourceTransformsAngVel);
  if (target->m_sourceTransformsAngVel)
  {
    REFIX_PTR_RELATIVE(CompressedDataBufferVector3, target->m_sourceTransformsAngVel, target);
    target->m_sourceTransformsAngVel->locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataClosestAnimDef::ClosestAnimSourceData::dislocate(ClosestAnimSourceData* target)
{
  if (target->m_sourceTransformsAngVel)
  {
    target->m_sourceTransformsAngVel->dislocate();
    UNFIX_PTR_RELATIVE(CompressedDataBufferVector3, target->m_sourceTransformsAngVel, target);
  }
  NMP::endianSwap(target->m_sourceTransformsAngVel);
  
  if (target->m_sourceTransformsPosVel)
  {
    target->m_sourceTransformsPosVel->dislocate();
    UNFIX_PTR_RELATIVE(CompressedDataBufferVector3, target->m_sourceTransformsPosVel, target);
  }
  NMP::endianSwap(target->m_sourceTransformsPosVel);

  target->m_sourceTransformsAtt->dislocate();
  UNFIX_SWAP_PTR_RELATIVE(CompressedDataBufferQuat, target->m_sourceTransformsAtt, target);
  
  target->m_sourceTransformsPos->dislocate();
  UNFIX_SWAP_PTR_RELATIVE(CompressedDataBufferVector3, target->m_sourceTransformsPos, target);
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataClosestAnimDef::locate(AttribData* target)
{
  AttribDataClosestAnimDef* result = (AttribDataClosestAnimDef*)target;
  AttribData::locate(target);

  // Root rotation blending
  NMP::endianSwap(result->m_rootRotationAxis);
  NMP::endianSwap(result->m_upAlignAtt);
  NMP::endianSwap(result->m_useRootRotationBlending);
  NMP::endianSwap(result->m_fractionThroughSource);
  NMP::endianSwap(result->m_maxRootRotationAngle);

  // Matching
  NMP::endianSwap(result->m_numAnimJoints);
  NMP::endianSwap(result->m_precomputeSourcesOffline);
  NMP::endianSwap(result->m_useVelocity);
  NMP::endianSwap(result->m_positionScaleFactor);
  NMP::endianSwap(result->m_orientationScaleFactor);
  NMP::endianSwap(result->m_influenceBetweenPosAndOrient);
  
  // Closest anim source data
  NMP::endianSwap(result->m_numSources);
  
  if (result->m_precomputeSourcesOffline)
  {
    REFIX_SWAP_PTR_RELATIVE(ClosestAnimSourceData*, result->m_sourceDataArray, target);
    for (uint32_t i = 0; i < result->m_numSources; ++i)
    {
      REFIX_SWAP_PTR_RELATIVE(ClosestAnimSourceData, result->m_sourceDataArray[i], target);
      ClosestAnimSourceData::locate(result->m_sourceDataArray[i]);
    }
  }

  NMP::endianSwap(result->m_numDescendants);

  if (result->m_numDescendants)
  {
    REFIX_SWAP_PTR_RELATIVE(NodeID, result->m_descendantIDs, target);
    NMP::endianSwapArray(result->m_descendantIDs, result->m_numDescendants, sizeof(NodeID));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataClosestAnimDef::dislocate(AttribData* target)
{
  AttribDataClosestAnimDef* result = (AttribDataClosestAnimDef*)target;

  if (result->m_numDescendants)
  {
    NMP::endianSwapArray(result->m_descendantIDs, result->m_numDescendants, sizeof(NodeID));
    UNFIX_SWAP_PTR_RELATIVE(NodeID, result->m_descendantIDs, target);
  }
  else
  {
    result->m_descendantIDs = NULL;
  }

  NMP::endianSwap(result->m_numDescendants);

  // Closest anim source data
  if (result->m_precomputeSourcesOffline)
  {
    for (uint32_t i = 0; i < result->m_numSources; ++i)
    {
      ClosestAnimSourceData::dislocate(result->m_sourceDataArray[i]);
      UNFIX_SWAP_PTR_RELATIVE(ClosestAnimSourceData, result->m_sourceDataArray[i], target);
    }
    UNFIX_SWAP_PTR_RELATIVE(ClosestAnimSourceData*, result->m_sourceDataArray, target);
  }
  NMP::endianSwap(result->m_numSources);

  // Matching
  NMP::endianSwap(result->m_influenceBetweenPosAndOrient);
  NMP::endianSwap(result->m_orientationScaleFactor);
  NMP::endianSwap(result->m_positionScaleFactor);
  NMP::endianSwap(result->m_useVelocity);
  NMP::endianSwap(result->m_precomputeSourcesOffline);
  NMP::endianSwap(result->m_numAnimJoints);

  // Root rotation blending
  NMP::endianSwap(result->m_maxRootRotationAngle);
  NMP::endianSwap(result->m_fractionThroughSource);
  NMP::endianSwap(result->m_useRootRotationBlending);
  NMP::endianSwap(result->m_upAlignAtt);
  NMP::endianSwap(result->m_rootRotationAxis);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataClosestAnimDef::getMemoryRequirements(
  bool                    precomputeSourcesOffline,
  uint32_t                numSources,
  uint32_t                numNodesUnderSources,
  uint32_t                numAnimChannelsPerSource,
  bool                    useVelocity)
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataClosestAnimDef), MR_ATTRIB_DATA_ALIGNMENT);

  if (precomputeSourcesOffline)
  {
    // Closest anim source data
    NMP_ASSERT(numSources > 0);
    // Pointers array
    NMP::Memory::Format memReqsDataArray(sizeof(ClosestAnimSourceData*) * numSources, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsDataArray;

    // Source data
    NMP::Memory::Format memReqsSource = ClosestAnimSourceData::getMemoryRequirements(numAnimChannelsPerSource, useVelocity);
    memReqsSource *= numSources;
    result += memReqsSource;
  }
  else
  {
    result += NMP::Memory::Format(sizeof(NodeID) * numNodesUnderSources);
  }

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataClosestAnimDef* AttribDataClosestAnimDef::init(
  NMP::Memory::Resource& resource,
  bool                   precomputeSourcesOffline,
  uint32_t               numSources,
  uint32_t               numDescendants,
  NodeID*                descendantIDs,
  uint32_t               numAnimChannelsPerSource,
  bool                   useVelocity,
  uint16_t               refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataClosestAnimDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataClosestAnimDef* result = (AttribDataClosestAnimDef*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_CLOSEST_ANIM_DEF);
  result->setRefCount(refCount);

  // Root rotation blending
  result->m_rootRotationAxis.set(0.0f, 1.0f, 0.0f);
  result->m_upAlignAtt.identity();
  result->m_useRootRotationBlending = true;
  result->m_fractionThroughSource = 0.0f;
  result->m_maxRootRotationAngle = NM_PI;

  // Matching  
  result->m_numAnimJoints = numAnimChannelsPerSource;
  result->m_precomputeSourcesOffline = precomputeSourcesOffline;
  result->m_useVelocity = useVelocity;
  result->m_positionScaleFactor = 1.0f;
  result->m_orientationScaleFactor = 1.0f;
  result->m_influenceBetweenPosAndOrient = 0.0f;
  
  // Closest anim source data
  NMP_ASSERT(numSources > 0);
  result->m_numSources = numSources;
  
  if (precomputeSourcesOffline)
  {
    NMP::Memory::Format memReqsDataArray(sizeof(ClosestAnimSourceData*) * numSources, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_sourceDataArray = (ClosestAnimSourceData**) resource.alignAndIncrement(memReqsDataArray);
    
    for (uint32_t i = 0; i < numSources; ++i)
    {
      result->m_sourceDataArray[i] = ClosestAnimSourceData::init(resource, numAnimChannelsPerSource, useVelocity);
    }
    result->m_descendantIDs = NULL;
    result->m_numDescendants = 0;
  }
  else
  {
    result->m_sourceDataArray = NULL;
    result->m_numDescendants = numDescendants;

    NMP::Memory::Format arrayReqs(sizeof(NodeID) * numDescendants, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_descendantIDs = (NodeID*)resource.alignAndIncrement(arrayReqs);
    for (uint32_t i = 0; i < numDescendants; ++i)
    {
      result->m_descendantIDs[i] = descendantIDs[i];
    }
  }

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataClosestAnimDefAnimSet functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataClosestAnimDefAnimSet::locate(AttribData* target)
{
  // Header
  AttribDataClosestAnimDefAnimSet* result = (AttribDataClosestAnimDefAnimSet*)target;
  AttribData::locate(target);

  NMP::endianSwap(result->m_numAnimWeights);
  NMP::endianSwap(result->m_numEntries);

  // Weights
  NMP::endianSwap(result->m_weights);
  REFIX_PTR_RELATIVE(float, result->m_weights, result);
  NMP::endianSwapArray(result->m_weights, result->m_numEntries);

  // Rig channels
  NMP::endianSwap(result->m_rigChannels);
  REFIX_PTR_RELATIVE(uint16_t, result->m_rigChannels, result);
  NMP::endianSwapArray(result->m_rigChannels, result->m_numEntries);
  
  // Anim channels
  NMP::endianSwap(result->m_animChannels);
  REFIX_PTR_RELATIVE(uint16_t, result->m_animChannels, result);
  NMP::endianSwapArray(result->m_animChannels, result->m_numEntries);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataClosestAnimDefAnimSet::dislocate(AttribData* target)
{
  AttribDataClosestAnimDefAnimSet* result = (AttribDataClosestAnimDefAnimSet*)target;

  // Anim channels
  NMP::endianSwapArray(result->m_animChannels, result->m_numEntries);
  UNFIX_PTR_RELATIVE(uint16_t, result->m_animChannels, result);
  NMP::endianSwap(result->m_animChannels);

  // Rig channels
  NMP::endianSwapArray(result->m_rigChannels, result->m_numEntries);
  UNFIX_PTR_RELATIVE(uint16_t, result->m_rigChannels, result);
  NMP::endianSwap(result->m_rigChannels);

  // Weights
  NMP::endianSwapArray(result->m_weights, result->m_numEntries);
  UNFIX_PTR_RELATIVE(float, result->m_weights, result);
  NMP::endianSwap(result->m_weights);

  // Header
  NMP::endianSwap(result->m_numEntries);
  NMP::endianSwap(result->m_numAnimWeights);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataClosestAnimDefAnimSet::getMemoryRequirements(uint32_t numEntries)
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataClosestAnimDefAnimSet), MR_ATTRIB_DATA_ALIGNMENT);

  // Weights
  NMP::Memory::Format memReqsWeights(sizeof(float) * numEntries, NMP_VECTOR_ALIGNMENT);
  result += memReqsWeights;
  
  // Rig channels
  NMP::Memory::Format memReqsRigChans(sizeof(uint16_t) * numEntries, NMP_VECTOR_ALIGNMENT);
  result += memReqsRigChans;
  
  // Anim channels
  result += memReqsRigChans;

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataClosestAnimDefAnimSet* AttribDataClosestAnimDefAnimSet::init(
  NMP::Memory::Resource&  resource,
  uint32_t                numEntries,
  uint16_t                refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataClosestAnimDefAnimSet), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataClosestAnimDefAnimSet* result = (AttribDataClosestAnimDefAnimSet*)resource.alignAndIncrement(memReqsHdr);
  
  result->setType(ATTRIB_TYPE_CLOSEST_ANIM_DEF_ANIM_SET);
  result->setRefCount(refCount);
  result->m_numEntries = numEntries;

  // Weights
  NMP::Memory::Format memReqsWeights(sizeof(float) * numEntries, NMP_VECTOR_ALIGNMENT);
  result->m_weights = (float*)resource.alignAndIncrement(memReqsWeights);

  // Rig channels
  NMP::Memory::Format memReqsRigChans(sizeof(uint16_t) * numEntries, NMP_VECTOR_ALIGNMENT);
  result->m_rigChannels = (uint16_t*)resource.alignAndIncrement(memReqsRigChans);
  
  // Anim channels
  result->m_animChannels = (uint16_t*)resource.alignAndIncrement(memReqsRigChans);

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataClosestAnimState::locate(AttribData* target)
{
  AttribDataClosestAnimState* result = (AttribDataClosestAnimState*)target;
  AttribData::locate(target);

  NMP::endianSwap(result->m_rootRotationOffset);
  NMP::endianSwap(result->m_fractionThroughSource);
  NMP::endianSwap(result->m_blendCompleteFlag);
  NMP::endianSwap(result->m_isPerformingSubNetworkEval);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataClosestAnimState::dislocate(AttribData* target)
{
  AttribDataClosestAnimState* result = (AttribDataClosestAnimState*)target;
  
  NMP::endianSwap(result->m_isPerformingSubNetworkEval);
  NMP::endianSwap(result->m_blendCompleteFlag);
  NMP::endianSwap(result->m_fractionThroughSource);
  NMP::endianSwap(result->m_rootRotationOffset);
  
  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataClosestAnimState::getMemoryRequirements()
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataClosestAnimState), MR_ATTRIB_DATA_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataClosestAnimState* AttribDataClosestAnimState::init(
  NMP::Memory::Resource&  resource,
  uint16_t                refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataClosestAnimState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataClosestAnimState* result = (AttribDataClosestAnimState*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_CLOSEST_ANIM_STATE);
  result->setRefCount(refCount);

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataClosestAnimState::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint16_t refCount = 0;
  
  result.m_format = AttribDataClosestAnimState::getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  AttribDataClosestAnimState* attrib = AttribDataClosestAnimState::init(resource, refCount);

  result.m_attribData = attrib;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataClosestAnimState::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;

  result.m_format = AttribDataClosestAnimState::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  AttribDataClosestAnimState* attrib = AttribDataClosestAnimState::init(resource, refCount);

  result.m_attribData = attrib;

  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataClosestAnimState::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataClosestAnimState::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_CLOSEST_ANIM_STATE && dest->getType() == ATTRIB_TYPE_CLOSEST_ANIM_STATE);
  
  AttribDataClosestAnimState* srcStateData = (AttribDataClosestAnimState*)source;
  AttribDataClosestAnimState* dstStateData = (AttribDataClosestAnimState*)dest;
  
  dstStateData->m_rootRotationOffset = srcStateData->m_rootRotationOffset;
  dstStateData->m_fractionThroughSource = srcStateData->m_fractionThroughSource;
  dstStateData->m_blendCompleteFlag = srcStateData->m_blendCompleteFlag;
  dstStateData->m_isPerformingSubNetworkEval = srcStateData->m_isPerformingSubNetworkEval;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataFunctionOperation functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataFunctionOperation::locate(AttribData* target)
{
  AttribDataFunctionOperation* result = (AttribDataFunctionOperation*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_operation);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataFunctionOperation::dislocate(AttribData* target)
{
  AttribDataFunctionOperation* result = (AttribDataFunctionOperation*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_operation);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataFunctionOperation::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataFunctionOperation), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataFunctionOperation* AttribDataFunctionOperation::init(NMP::Memory::Resource& resource, OperationType value, uint16_t refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataFunctionOperation* result = (AttribDataFunctionOperation*)resource.ptr;
  resource.increment(sizeof(AttribDataFunctionOperation));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_FLOAT_OPERATION);
  result->setRefCount(refCount);
  result->m_operation = value;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataArithmeticOperation functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataArithmeticOperation::locate(AttribData* target)
{
  AttribDataArithmeticOperation* result = (AttribDataArithmeticOperation*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_operation);
  NMP::endianSwap(result->m_constValue);
  NMP::endianSwap(result->m_constVector);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataArithmeticOperation::dislocate(AttribData* target)
{
  AttribDataArithmeticOperation* result = (AttribDataArithmeticOperation*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_operation);
  NMP::endianSwap(result->m_constValue);
  NMP::endianSwap(result->m_constVector);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataArithmeticOperation::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataArithmeticOperation), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataArithmeticOperation* AttribDataArithmeticOperation::init(
  NMP::Memory::Resource& resource,
  OperationType          value,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataArithmeticOperation* result = (AttribDataArithmeticOperation*)resource.ptr;
  resource.increment(sizeof(AttribDataArithmeticOperation));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_2_FLOAT_OPERATION);
  result->setRefCount(refCount);
  result->m_operation = value;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataArithmeticOperation* AttribDataArithmeticOperation::init(
  NMP::Memory::Resource& resource,
  OperationType          value,
  float                  constValue,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataArithmeticOperation* result = (AttribDataArithmeticOperation*)resource.ptr;
  resource.increment(sizeof(AttribDataArithmeticOperation));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_2_FLOAT_OPERATION);
  result->setRefCount(refCount);
  result->m_operation = value;
  result->m_constValue = constValue;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataArithmeticOperation* AttribDataArithmeticOperation::init(
  NMP::Memory::Resource& resource,
  OperationType          value,
  float                  constValue,
  float                  constX,
  float                  constY,
  float                  constZ,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataArithmeticOperation* result = (AttribDataArithmeticOperation*)resource.ptr;
  resource.increment(sizeof(AttribDataArithmeticOperation));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_2_FLOAT_OPERATION);
  result->setRefCount(refCount);
  result->m_operation = value;
  result->m_constValue = constValue;
  result->m_constVector.set(constX, constY, constZ);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBooleanOperation functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataBooleanOperation::locate(AttribData* target)
{
  AttribDataBooleanOperation* result = (AttribDataBooleanOperation*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_operation);
  NMP::endianSwap(result->m_invertResult);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBooleanOperation::dislocate(AttribData* target)
{
  AttribDataBooleanOperation* result = (AttribDataBooleanOperation*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_operation);
  NMP::endianSwap(result->m_invertResult);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBooleanOperation::getMemoryRequirements()
{
  return NMP::Memory::Format(
                    NMP::Memory::align(sizeof(AttribDataBooleanOperation), MR_ATTRIB_DATA_ALIGNMENT),
                    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBooleanOperation* AttribDataBooleanOperation::init(
  NMP::Memory::Resource& resource,
  OperationType          value,
  bool                   invertResult,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBooleanOperation* result = (AttribDataBooleanOperation*)resource.ptr;
  resource.increment(sizeof(AttribDataBooleanOperation));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_BOOLEAN_OPERATION);
  result->setRefCount(refCount);
  result->m_operation = value;
  result->m_invertResult = invertResult;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataValueCompareOperation functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataValueCompareOperation::locate(AttribData* target)
{
  AttribDataValueCompareOperation* result = (AttribDataValueCompareOperation*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_operation);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataValueCompareOperation::dislocate(AttribData* target)
{
  AttribDataValueCompareOperation* result = (AttribDataValueCompareOperation*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_operation);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataValueCompareOperation::getMemoryRequirements()
{
  return NMP::Memory::Format(
                    NMP::Memory::align(sizeof(AttribDataValueCompareOperation), MR_ATTRIB_DATA_ALIGNMENT),
                    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataValueCompareOperation* AttribDataValueCompareOperation::init(
  NMP::Memory::Resource& resource,
  OperationType          value,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataValueCompareOperation* result = (AttribDataValueCompareOperation*)resource.ptr;
  resource.increment(sizeof(AttribDataValueCompareOperation));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_VALUE_COMPARE_OPERATION);
  result->setRefCount(refCount);
  result->m_operation = value;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataSmoothFloatOperation functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataSmoothFloatOperation::locate(AttribData* target)
{
  AttribDataSmoothFloatOperation* result = (AttribDataSmoothFloatOperation*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_floatRateIncreasing);
  NMP::endianSwap(result->m_floatRateDecreasing);
  NMP::endianSwap(result->m_initialValueX);
  NMP::endianSwap(result->m_initialValueY);
  NMP::endianSwap(result->m_initialValueZ);
  NMP::endianSwap(result->m_vectorRate);
  NMP::endianSwap(result->m_smoothVel);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSmoothFloatOperation::dislocate(AttribData* target)
{
  AttribDataSmoothFloatOperation* result = (AttribDataSmoothFloatOperation*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_floatRateIncreasing);
  NMP::endianSwap(result->m_floatRateDecreasing);
  NMP::endianSwap(result->m_initialValueX);
  NMP::endianSwap(result->m_initialValueY);
  NMP::endianSwap(result->m_initialValueZ);
  NMP::endianSwap(result->m_vectorRate);
  NMP::endianSwap(result->m_smoothVel);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSmoothFloatOperation* AttribDataSmoothFloatOperation::create(
  NMP::MemoryAllocator* allocator,
  float                 rateInc,
  float                 rateDec,
  float                 initialValueX,
  float                 initialValueY,
  float                 initialValueZ,
  bool                  useInitValOnInit,
  bool                  smoothVel, 
  uint16_t              refCount)
{
  AttribDataSmoothFloatOperation* result;

  NMP::Memory::Format memReqs = AttribDataSmoothFloatOperation::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = AttribDataSmoothFloatOperation::init(
    resource, rateInc, rateDec, initialValueX, initialValueY, initialValueZ, useInitValOnInit, smoothVel, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataSmoothFloatOperation::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataSmoothFloatOperation), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSmoothFloatOperation* AttribDataSmoothFloatOperation::init(
  NMP::Memory::Resource& resource,
  float                  rateInc,
  float                  rateDec,
  float                  initialValueX,
  float                  initialValueY,
  float                  initialValueZ,
  bool                   useInitValOnInit,
  bool                   smoothVel, 
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataSmoothFloatOperation* result = (AttribDataSmoothFloatOperation*)resource.ptr;
  resource.increment(sizeof(AttribDataSmoothFloatOperation));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_SMOOTH_FLOAT_OPERATION);
  result->setRefCount(refCount);
  result->m_floatRateIncreasing = rateInc;
  result->m_floatRateDecreasing = rateDec;
  result->m_initialValueX = initialValueX;
  result->m_initialValueY = initialValueY;
  result->m_initialValueZ = initialValueZ;
  result->m_useInitValOnInit = useInitValOnInit;
  result->m_smoothVel = smoothVel;
  result->m_vectorRate.set(rateInc);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataSmoothFloatOperation::create(NMP::MemoryAllocator* allocator)
{
  AttribDataHandle result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, 0.0f, 0.0f, 0.0f, 0.0f, 0.0f, false, false, 0);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataRateOfChangeState functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataRateOfChangeState::locate(AttribData* target)
{
  AttribDataRateOfChangeState* result = (AttribDataRateOfChangeState*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_lastVector);
  NMP::endianSwap(result->m_lastFloat);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRateOfChangeState::dislocate(AttribData* target)
{
  AttribDataRateOfChangeState* result = (AttribDataRateOfChangeState*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_lastVector);
  NMP::endianSwap(result->m_lastFloat);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRateOfChangeState* AttribDataRateOfChangeState::create(
  NMP::MemoryAllocator* allocator,
  float                 initValue,
  uint16_t              refCount)
{
  AttribDataRateOfChangeState* result;

  NMP::Memory::Format memReqs = AttribDataRateOfChangeState::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = AttribDataRateOfChangeState::init(resource, initValue, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRateOfChangeState* AttribDataRateOfChangeState::create(
  NMP::MemoryAllocator* allocator,
  float                 x,
  float                 y,
  float                 z,
  uint16_t              refCount)
{
  AttribDataRateOfChangeState* result;

  NMP::Memory::Format memReqs = AttribDataRateOfChangeState::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = AttribDataRateOfChangeState::init(resource, x, y, z, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRateOfChangeState::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataRateOfChangeState), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRateOfChangeState* AttribDataRateOfChangeState::init(
  NMP::Memory::Resource& resource,
  float                  initValue,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataRateOfChangeState* result = (AttribDataRateOfChangeState*)resource.ptr;
  resource.increment(sizeof(AttribDataRateOfChangeState));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_RATE_OF_CHANGE_OPERATION);
  result->setRefCount(refCount);
  result->m_lastFloat = initValue;
  result->m_lastVector = NMP::Vector3::InitZero;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRateOfChangeState* AttribDataRateOfChangeState::init(
  NMP::Memory::Resource& resource,
  float                  x,
  float                  y,
  float                  z,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataRateOfChangeState* result = (AttribDataRateOfChangeState*)resource.ptr;
  resource.increment(sizeof(AttribDataRateOfChangeState));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_RATE_OF_CHANGE_OPERATION);
  result->setRefCount(refCount);
  result->m_lastFloat = 0.0f;
  result->m_lastVector.set(x,y,z);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataRateOfChangeState::create(NMP::MemoryAllocator* allocator)
{
  AttribDataHandle result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, 0.0f, 0);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataRandomFloatOperation functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataRandomFloatOperation::locate(AttribData* target)
{
  AttribDataRandomFloatOperation* result = (AttribDataRandomFloatOperation*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_generator);
  NMP::endianSwap(result->m_lastFrameTime);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRandomFloatOperation::dislocate(AttribData* target)
{
  AttribDataRandomFloatOperation* result = (AttribDataRandomFloatOperation*)target;

  NMP::endianSwap(result->m_generator);
  NMP::endianSwap(result->m_lastFrameTime);
  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataRandomFloatOperation::create(
  NMP::MemoryAllocator* allocator,
  uint32_t              seed,
  uint16_t              refCount)
{
  AttribDataHandle handle;

  NMP::Memory::Format memReqs = AttribDataRandomFloatOperation::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  AttribDataRandomFloatOperation* attribData = AttribDataRandomFloatOperation::init(resource, seed, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  attribData->m_allocator = allocator;

  handle.m_format     = AttribDataRandomFloatOperation::getMemoryRequirements();
  handle.m_attribData = attribData;

  return handle;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRandomFloatOperation::getMemoryRequirements()
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(AttribDataRandomFloatOperation), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRandomFloatOperation* AttribDataRandomFloatOperation::init(
  NMP::Memory::Resource& resource,
  uint32_t               seed,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);
  AttribDataRandomFloatOperation* result = (AttribDataRandomFloatOperation*)resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_RANDOM_FLOAT_OPERATION);
  result->setRefCount(refCount);
  result->m_lastFrameTime = 0.0f;
  result->m_generator.setSeed(seed);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataRandomFloatDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataRandomFloatDef::locate(AttribData* target)
{
  AttribDataRandomFloatDef* result = (AttribDataRandomFloatDef*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_min);
  NMP::endianSwap(result->m_max);
  NMP::endianSwap(result->m_duration);
  NMP::endianSwap(result->m_seed);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRandomFloatDef::dislocate(AttribData* target)
{
  AttribDataRandomFloatDef* result = (AttribDataRandomFloatDef*)target;

  NMP::endianSwap(result->m_min);
  NMP::endianSwap(result->m_max);
  NMP::endianSwap(result->m_duration);
  NMP::endianSwap(result->m_seed);
  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataRandomFloatDef::create(
  NMP::MemoryAllocator* allocator,
  float                 min,
  float                 max,
  float                 duration,
  uint32_t              seed,
  uint16_t              refCount)
{
  AttribDataHandle handle;

  NMP::Memory::Format memReqs = AttribDataRandomFloatDef::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  AttribDataRandomFloatDef* attribData = AttribDataRandomFloatDef::init(resource, min, max, duration, seed, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  attribData->m_allocator = allocator;

  handle.m_attribData = attribData;

  return handle;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRandomFloatDef::getMemoryRequirements()
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(AttribDataRandomFloatDef), MR_ATTRIB_DATA_ALIGNMENT), MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRandomFloatDef* AttribDataRandomFloatDef::init(
  NMP::Memory::Resource& resource,
  float                  min,
  float                  max,
  float                  duration,
  uint32_t               seed,
  uint16_t               refCount)
{
  NMP::Memory::Format format = getMemoryRequirements();
  resource.align(format);
  AttribDataRandomFloatDef* result = (AttribDataRandomFloatDef*)resource.ptr;
  resource.increment(format);

  result->setType(ATTRIB_TYPE_RANDOM_FLOAT_DEF);
  result->setRefCount(refCount);
  result->m_min = min;
  result->m_max = max;
  result->m_duration = duration;
  result->m_seed = seed;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataNoiseGenDef static data.
//----------------------------------------------------------------------------------------------------------------------

const uint32_t AttribDataNoiseGenDef::TABLE_1_SIZE = 7;
const uint32_t AttribDataNoiseGenDef::TABLE_2_SIZE = 19;
const uint32_t AttribDataNoiseGenDef::TABLE_3_SIZE = 31;

//----------------------------------------------------------------------------------------------------------------------
// AttribDataNoiseGenDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataNoiseGenDef::locate(AttribData* target)
{
  AttribDataNoiseGenDef* result = (AttribDataNoiseGenDef*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_flags);
  NMP::endianSwap(result->m_noiseSawFrequency);
  NMP::endianSwap(result->m_timeOffset);

  NMP::endianSwap(result->m_table1);
  REFIX_PTR_RELATIVE(float, result->m_table1, result);

  NMP::endianSwap(result->m_table2);
  REFIX_PTR_RELATIVE(float, result->m_table2, result);

  NMP::endianSwap(result->m_table3);
  REFIX_PTR_RELATIVE(float, result->m_table3, result);

  for (uint32_t i = 0; i < TABLE_1_SIZE; i++)
  {
    NMP::endianSwap(result->m_table1[i]);
  }

  for (uint32_t i = 0; i < TABLE_2_SIZE; i++)
  {
    NMP::endianSwap(result->m_table2[i]);
  }

  for (uint32_t i = 0; i < TABLE_3_SIZE; i++)
  {
    NMP::endianSwap(result->m_table3[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataNoiseGenDef::dislocate(AttribData* target)
{
  AttribDataNoiseGenDef* result = (AttribDataNoiseGenDef*)target;

  for (uint32_t i = 0; i < TABLE_3_SIZE; i++)
  {
    NMP::endianSwap(result->m_table3[i]);
  }

  for (uint32_t i = 0; i < TABLE_2_SIZE; i++)
  {
    NMP::endianSwap(result->m_table2[i]);
  }

  for (uint32_t i = 0; i < TABLE_1_SIZE; i++)
  {
    NMP::endianSwap(result->m_table1[i]);
  }

  UNFIX_PTR_RELATIVE(float, result->m_table3, result);
  NMP::endianSwap(result->m_table3);

  UNFIX_PTR_RELATIVE(float, result->m_table2, result);
  NMP::endianSwap(result->m_table2);

  UNFIX_PTR_RELATIVE(float, result->m_table1, result);
  NMP::endianSwap(result->m_table1);

  NMP::endianSwap(result->m_timeOffset);
  NMP::endianSwap(result->m_noiseSawFrequency);
  NMP::endianSwap(result->m_flags);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataNoiseGenDef::relocate(AttribData* target, void* location)
{
  AttribDataNoiseGenDef* result = (AttribDataNoiseGenDef*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));

  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataNoiseGenDef));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_table1 = (float*)ptr;

  ptr = (void*)(((size_t)ptr) + (sizeof(float) * (AttribDataNoiseGenDef::TABLE_1_SIZE)));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_table2 = (float*)ptr;

  ptr = (void*)(((size_t)ptr) + (sizeof(float) * (AttribDataNoiseGenDef::TABLE_2_SIZE)));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_table3 = (float*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataNoiseGenDef::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataNoiseGenDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the 3 tables of random floats.
  result += NMP::Memory::Format(sizeof(float) * TABLE_1_SIZE, NMP_NATURAL_TYPE_ALIGNMENT);
  result += NMP::Memory::Format(sizeof(float) * TABLE_2_SIZE, NMP_NATURAL_TYPE_ALIGNMENT);
  result += NMP::Memory::Format(sizeof(float) * TABLE_3_SIZE, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataNoiseGenDef* AttribDataNoiseGenDef::init(
  NMP::Memory::Resource& resource,
  uint32_t               flags,
  float                  noiseSawFrequency,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataNoiseGenDef* result = (AttribDataNoiseGenDef*) resource.ptr;
  resource.increment(sizeof(AttribDataNoiseGenDef));

  result->setType(ATTRIB_TYPE_NOISE_GEN_DEF);
  result->setRefCount(refCount);

  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_table1 = (float*) resource.ptr;
  resource.increment(sizeof(float) * TABLE_1_SIZE);

  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_table2 = (float*) resource.ptr;
  resource.increment(sizeof(float) * TABLE_2_SIZE);

  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_table3 = (float*) resource.ptr;
  resource.increment(sizeof(float) * TABLE_3_SIZE);

  fillTables(result->m_table1, result->m_table2, result->m_table3);

  result->m_flags = flags;
  result->m_noiseSawFrequency = noiseSawFrequency;
  result->m_timeOffset = (float)(rand() / (float)RAND_MAX);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataNoiseGenDef::fillTables(float* table1, float* table2, float* table3)
{
  for (uint32_t i = 0; i < TABLE_1_SIZE; ++i)
  {
    table1[i] = (float)(rand() / (float)RAND_MAX);
  }
  for (uint32_t i = 0; i < TABLE_2_SIZE; ++i)
  {
    table2[i] = (float)(rand() / (float)RAND_MAX);
  }
  for (uint32_t i = 0; i < TABLE_3_SIZE; ++i)
  {
    table3[i] = (float)(rand() / (float)RAND_MAX);
  }
}

#endif // NM_HOST_CELL_SPU

#ifndef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
// AttribDataSwitchDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataSwitchDef::locate(AttribData* target)
{
  AttribDataSwitchDef* result = (AttribDataSwitchDef*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_evalMode);
  NMP::endianSwap(result->m_inputSelectionMethod);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataSwitchDef::dislocate(AttribData* target)
{
  AttribDataSwitchDef* result = (AttribDataSwitchDef*)target;

  NMP::endianSwap(result->m_evalMode);
  NMP::endianSwap(result->m_inputSelectionMethod);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataSwitchDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataSwitchDef), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataSwitchDef* AttribDataSwitchDef::init(
  NMP::Memory::Resource& resource,
  EvaluateMode           evalMode,
  InputSelectionMethod   inputSelectionMethod,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataSwitchDef* result = (AttribDataSwitchDef*) resource.ptr;
  resource.increment(sizeof(AttribDataSwitchDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_SWITCH_DEF);
  result->setRefCount(refCount);

  result->m_evalMode = evalMode;
  result->m_inputSelectionMethod = (InputSelectionMethod)inputSelectionMethod;

  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataRayCastDef functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataRayCastDef::locate(AttribData* target)
{
  AttribDataRayCastDef* result = (AttribDataRayCastDef*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_rayStart);
  NMP::endianSwap(result->m_rayDelta);
  NMP::endianSwap(result->m_useLocalOrientation);
  NMP::endianSwap(result->m_upAxisIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataRayCastDef::dislocate(AttribData* target)
{
  AttribDataRayCastDef* result = (AttribDataRayCastDef*)target;

  NMP::endianSwap(result->m_rayStart);
  NMP::endianSwap(result->m_rayDelta);
  NMP::endianSwap(result->m_useLocalOrientation);
  NMP::endianSwap(result->m_upAxisIndex);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataRayCastDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataRayCastDef), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataRayCastDef* AttribDataRayCastDef::init(
  NMP::Memory::Resource& resource,
  bool                   useLocalOrientation,
  NMP::Vector3&          rayStart,
  NMP::Vector3&          rayDelta,
  uint32_t               upAxisIndex,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataRayCastDef* result = (AttribDataRayCastDef*) resource.ptr;
  resource.increment(sizeof(AttribDataRayCastDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_RAY_CAST_DEF);
  result->setRefCount(refCount);

  result->m_rayStart = rayStart;
  result->m_rayDelta = rayDelta;
  result->m_useLocalOrientation = useLocalOrientation;
  result->m_upAxisIndex = upAxisIndex;

  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicalEffectData functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicalEffectData* AttribDataPhysicalEffectData::create(
  NMP::MemoryAllocator* allocator,
  MR::Network* network,
  MR::NodeDef* nodeDef,
  uint16_t refCount)
{
  AttribDataPhysicalEffectData* result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
  result = init(resource, network, nodeDef, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicalEffectData::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataPhysicalEffectData), MR_ATTRIB_DATA_ALIGNMENT);
  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicalEffectData* AttribDataPhysicalEffectData::init(
  NMP::Memory::Resource& resource,
  MR::Network* network,
  MR::NodeDef* nodeDef,
  uint16_t refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);

  NMP::Memory::Format headerFormat(sizeof(AttribDataPhysicalEffectData), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPhysicalEffectData* result = (AttribDataPhysicalEffectData*)resource.alignAndIncrement(headerFormat);

  result->setType(ATTRIB_TYPE_PHYSICAL_EFFECT_DATA);
  result->setRefCount(refCount);
  result->m_network = network;
  result->m_nodeDef = nodeDef;

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsInfoDef functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataPhysicsInfoDef::locate(AttribData* target)
{
  AttribDataPhysicsInfoDef* result = (AttribDataPhysicsInfoDef*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_partIndex);
  NMP::endianSwap(result->m_outputInWorldSpace);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsInfoDef::dislocate(AttribData* target)
{
  AttribDataPhysicsInfoDef* result = (AttribDataPhysicsInfoDef*)target;

  NMP::endianSwap(result->m_partIndex);
  NMP::endianSwap(result->m_outputInWorldSpace);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsInfoDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataPhysicsInfoDef), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsInfoDef* AttribDataPhysicsInfoDef::init(
  NMP::Memory::Resource& resource,
  int                    partIndex,
  bool                   inWorldSpace,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPhysicsInfoDef* result = (AttribDataPhysicsInfoDef*) resource.ptr;
  resource.increment(sizeof(AttribDataPhysicsInfoDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_PHYSICS_INFO_DEF);
  result->setRefCount(refCount);

  result->m_partIndex = partIndex;
  result->m_outputInWorldSpace = inWorldSpace;

  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsObjectPointer functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataPhysicsObjectPointer::locate(AttribData* target)
{
  NMP_ASSERT_FAIL_MSG("Shouldn't be called");
  AttribDataPhysicsObjectPointer* result = (AttribDataPhysicsObjectPointer*)target;

  AttribData::locate(target);

  NMP::endianSwap(result->m_value);
  REFIX_PTR_RELATIVE(void*, result->m_value, result);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsObjectPointer::dislocate(AttribData* target)
{
  NMP_ASSERT_FAIL_MSG("Shouldn't be called");
  AttribDataPhysicsObjectPointer* result = (AttribDataPhysicsObjectPointer*)target;

  UNFIX_PTR_RELATIVE(void*, result->m_value, result);
  NMP::endianSwap(result->m_value);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsObjectPointer::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_PHYSICS_OBJECT_POINTER && dest->getType() == ATTRIB_TYPE_PHYSICS_OBJECT_POINTER);
  ((AttribDataPhysicsObjectPointer*) dest)->m_value = ((AttribDataPhysicsObjectPointer*) source)->m_value;
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataPhysicsObjectPointer::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  uint32_t dataSize = sizeof(AttribPhysicsObjectPointerOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribPhysicsObjectPointerOutputData* output = (AttribPhysicsObjectPointerOutputData*) outputBuffer;

    // Double cast prevents warning on sign extension of pointer.
    output->m_value = (int64_t) (int32_t)((ptrdiff_t)((MR::AttribDataPhysicsObjectPointer*)toOutput)->m_value);

    NMP::netEndianSwap(output->m_value);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsObjectPointer::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataPhysicsObjectPointer), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsObjectPointer* AttribDataPhysicsObjectPointer::init(
  NMP::Memory::Resource& resource,
  void*                  objectPointer,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPhysicsObjectPointer* result = (AttribDataPhysicsObjectPointer*) resource.ptr;
  resource.increment(sizeof(AttribDataPhysicsObjectPointer));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_PHYSICS_OBJECT_POINTER);
  result->setRefCount(refCount);

  result->m_value = objectPointer;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPhysicsObjectPointer::create(
  NMP::MemoryAllocator* allocator,
  void*                 physicsObjectPointer,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, physicsObjectPointer, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsSetup functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataPhysicsSetup::locate(AttribData* target)
{
  AttribDataPhysicsSetup* result = static_cast< AttribDataPhysicsSetup* >(target);
  AttribData::locate(target);

  NMP::endianSwap(result->m_enableCollision);
  NMP::endianSwap(result->m_useActiveAnimationAsKeyframeAnchor);
  NMP::endianSwap(result->m_useRootAsAnchor);
  NMP::endianSwap(result->m_outputSourceAnimation);
  NMP::endianSwap(result->m_hasAnimationChild);
  NMP::endianSwap(result->m_preserveMomentum);
  NMP::endianSwap(result->m_rootControlMethod);
  NMP::endianSwap(result->m_method);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsSetup::dislocate(AttribData* target)
{
  AttribDataPhysicsSetup* result = static_cast< AttribDataPhysicsSetup* >(target);

  NMP::endianSwap(result->m_enableCollision);
  NMP::endianSwap(result->m_useActiveAnimationAsKeyframeAnchor);
  NMP::endianSwap(result->m_useRootAsAnchor);
  NMP::endianSwap(result->m_outputSourceAnimation);
  NMP::endianSwap(result->m_hasAnimationChild);
  NMP::endianSwap(result->m_preserveMomentum);
  NMP::endianSwap(result->m_rootControlMethod);
  NMP::endianSwap(result->m_method);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsSetup::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataPhysicsSetup), MR_ATTRIB_DATA_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsSetup* AttribDataPhysicsSetup::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);

  NMP::Memory::Format format = getMemoryRequirements();
  AttribDataPhysicsSetup* result = (AttribDataPhysicsSetup*)resource.alignAndIncrement(format);

  result->setType(ATTRIB_TYPE_PHYSICS_SETUP);
  result->setRefCount(refCount);

  // Data member initialization.
  result->m_rootControlMethod = Network::ROOT_CONTROL_ANIMATION;
  result->m_method            = METHOD_INVALID;

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataPhysicsSetup::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_PHYSICS_SETUP && dest->getType() == ATTRIB_TYPE_PHYSICS_SETUP);
  *dest = *source;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsSetupPerAnimSet functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsSetupPerAnimSet::locate(AttribData* target)
{
  AttribDataPhysicsSetupPerAnimSet* result = static_cast< AttribDataPhysicsSetupPerAnimSet* >(target);
  AttribData::locate(target);

  NMP::endianSwap(result->m_disableSleeping);
  NMP::endianSwap(result->m_enableJointLimits);
  NMP::endianSwap(result->m_softKeyFramingMaxAccel);
  NMP::endianSwap(result->m_softKeyFramingMaxAngAccel);
  NMP::endianSwap(result->m_activeAnimationStrengthMultiplier);
  NMP::endianSwap(result->m_activeAnimationDampingMultiplier);
  NMP::endianSwap(result->m_activeAnimationInternalCompliance);
  NMP::endianSwap(result->m_activeAnimationExternalCompliance);
  NMP::endianSwap(result->m_jointProjectionIterations);
  NMP::endianSwap(result->m_jointProjectionLinearTolerance);
  NMP::endianSwap(result->m_jointProjectionAngularTolerance);
  NMP::endianSwap(result->m_massMultiplier);
  NMP::endianSwap(result->m_hardKeyframingEnableConstraint);
  NMP::endianSwap(result->m_gravityCompensation);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsSetupPerAnimSet::dislocate(AttribData* target)
{
  AttribDataPhysicsSetupPerAnimSet* result = static_cast< AttribDataPhysicsSetupPerAnimSet* >(target);

  NMP::endianSwap(result->m_disableSleeping);
  NMP::endianSwap(result->m_enableJointLimits);
  NMP::endianSwap(result->m_softKeyFramingMaxAccel);
  NMP::endianSwap(result->m_softKeyFramingMaxAngAccel);
  NMP::endianSwap(result->m_activeAnimationStrengthMultiplier);
  NMP::endianSwap(result->m_activeAnimationDampingMultiplier);
  NMP::endianSwap(result->m_activeAnimationInternalCompliance);
  NMP::endianSwap(result->m_activeAnimationExternalCompliance);
  NMP::endianSwap(result->m_jointProjectionIterations);
  NMP::endianSwap(result->m_jointProjectionLinearTolerance);
  NMP::endianSwap(result->m_jointProjectionAngularTolerance);
  NMP::endianSwap(result->m_massMultiplier);
  NMP::endianSwap(result->m_hardKeyframingEnableConstraint);
  NMP::endianSwap(result->m_gravityCompensation);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsSetupPerAnimSet::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataPhysicsSetupPerAnimSet), MR_ATTRIB_DATA_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsSetupPerAnimSet* AttribDataPhysicsSetupPerAnimSet::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPhysicsSetupPerAnimSet* result = static_cast< AttribDataPhysicsSetupPerAnimSet* >(resource.ptr);
  resource.increment(sizeof(AttribDataPhysicsSetupPerAnimSet));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  // Parent data member initialization.
  result->setType(ATTRIB_TYPE_PHYSICS_SETUP_ANIM_SET);
  result->setRefCount(refCount);

  // Data member initialization.
  result->m_disableSleeping                     = true;
  result->m_enableJointLimits                   = true;

  result->m_softKeyFramingMaxAccel              = -1.0f;  // Magic value indicating unlimited acceleration.
  result->m_softKeyFramingMaxAngAccel           = -1.0f;  // Magic value indicating unlimited acceleration.
  result->m_activeAnimationStrengthMultiplier   = 1.0f;
  result->m_activeAnimationDampingMultiplier    = 1.0f;
  result->m_activeAnimationInternalCompliance   = 1.0f;
  result->m_activeAnimationExternalCompliance   = 1.0f;
  result->m_jointProjectionIterations           = 0;
  result->m_jointProjectionLinearTolerance      = FLT_MAX;
  result->m_jointProjectionAngularTolerance     = NM_PI;
  result->m_massMultiplier                      = 10.0f;
  result->m_hardKeyframingEnableConstraint      = false;
  result->m_gravityCompensation                 = 0.0f;

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataPhysicsSetupPerAnimSet::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_PHYSICS_SETUP_ANIM_SET && dest->getType() == ATTRIB_TYPE_PHYSICS_SETUP_ANIM_SET);
  *dest = *source;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsState functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsState::locate(AttribData* target)
{
  AttribDataPhysicsState* result = (AttribDataPhysicsState*)target;
  AttribData::locate(target);

  NMP::endianSwap(result->m_previousChildTransforms);
  REFIX_PTR_RELATIVE(AttribDataTransformBuffer, result->m_previousChildTransforms, result);
  AttribDataTransformBuffer::locate(result->m_previousChildTransforms);

  NMP::endianSwap(result->m_previousDeltaTime);
  NMP::endianSwap(result->m_deltaTime);
  NMP::endianSwap(result->m_updateCounter);
  NMP::endianSwap(result->m_haveAddedPhysicsRigRef);
  NMP::endianSwap(result->m_needToInitialiseRoot);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsState::dislocate(AttribData* target)
{
  AttribDataPhysicsState* result = (AttribDataPhysicsState*)target;

  NMP::endianSwap(result->m_deltaTime);
  NMP::endianSwap(result->m_previousDeltaTime);
  NMP::endianSwap(result->m_updateCounter);
  NMP::endianSwap(result->m_haveAddedPhysicsRigRef);
  NMP::endianSwap(result->m_needToInitialiseRoot);

  AttribDataTransformBuffer::dislocate(result->m_previousChildTransforms);
  UNFIX_PTR_RELATIVE(AttribDataTransformBuffer, result->m_previousChildTransforms, result);
  NMP::endianSwap(result->m_previousChildTransforms);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsState::relocate(AttribData* target, void* location)
{
  AttribDataPhysicsState* result = (AttribDataPhysicsState*) target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  size_t offset = NMP::Memory::align(sizeof(AttribDataPhysicsState), MR_TRANSFORM_ATTRIB_ALIGNMENT);

  // Align for the transforms
  AttribDataTransformBuffer* previousChildTransforms = (AttribDataTransformBuffer*) (((size_t)result) + offset);
  result->m_previousChildTransforms = (AttribDataTransformBuffer*)(((size_t)location) + offset);
  NMP_ASSERT(NMP_IS_ALIGNED(previousChildTransforms, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(result->m_previousChildTransforms , MR_TRANSFORM_ATTRIB_ALIGNMENT));
  AttribDataTransformBuffer::relocate(previousChildTransforms, result->m_previousChildTransforms);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsState::getMemoryRequirements(uint32_t numAnimationJoints)
{
  NMP::Memory::Format result(sizeof(AttribDataPhysicsState), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(numAnimationJoints, buffMemReqs, internalBuffMemReqs);
  result += buffMemReqs;

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsState* AttribDataPhysicsState::init(
  NMP::Memory::Resource& resource,
  uint32_t               numAnimationJoints,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPhysicsState), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  AttribDataPhysicsState* result = (AttribDataPhysicsState*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_PHYSICS_STATE);
  result->setRefCount(refCount);

  result->m_deltaTime = 0.0f;
  result->m_previousDeltaTime = 0.0f;
  result->m_updateCounter = 0;
  result->m_haveAddedPhysicsRigRef = false;
  result->m_needToInitialiseRoot = true;

  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(numAnimationJoints, buffMemReqs, internalBuffMemReqs);
  result->m_previousChildTransforms = MR::AttribDataTransformBuffer::initPosQuat(
    resource,
    internalBuffMemReqs,
    numAnimationJoints);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPhysicsState::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  desc)
{
  AttribDataHandle result;
  
  NMP_ASSERT(desc);
  AttribDataArrayCreateDesc* targetDesc = (AttribDataArrayCreateDesc*)desc;
  
  result.m_format = getMemoryRequirements(targetDesc->m_numEntries);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, targetDesc->m_numEntries, targetDesc->m_refCount);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPhysicsState::create(
  NMP::MemoryAllocator*  allocator,
  uint32_t               numAnimationJoints,
  uint16_t               refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numAnimationJoints);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, numAnimationJoints, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsState::outputMemReqs(Network* network)
{
  uint32_t numBones = network->getMaxTransformCount();
  return getMemoryRequirements(numBones);
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataPhysicsState::copy(AttribData *source, AttribData *dest, bool copyTransforms)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_PHYSICS_STATE && dest->getType() == ATTRIB_TYPE_PHYSICS_STATE);

  AttribDataPhysicsState* s = (AttribDataPhysicsState*) source;
  AttribDataPhysicsState* d = (AttribDataPhysicsState*) dest;
  NMP_ASSERT(s->m_previousChildTransforms->m_transformBuffer->getLength() ==
             d->m_previousChildTransforms->m_transformBuffer->getLength());

  // save/restore the original pointer
  AttribDataTransformBuffer* previousChildTransforms = d->m_previousChildTransforms;
  *d = *s;

  d->m_previousChildTransforms = previousChildTransforms;

  if (copyTransforms)
    s->m_previousChildTransforms->m_transformBuffer->copyTo(d->m_previousChildTransforms->m_transformBuffer);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsInitialisation functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsInitialisation::locate(AttribData* target)
{
  AttribDataPhysicsInitialisation* result = (AttribDataPhysicsInitialisation*)target;
  AttribData::locate(target);

  NMP::endianSwap(result->m_transforms);
  REFIX_PTR_RELATIVE(AttribDataTransformBuffer, result->m_transforms, result);
  AttribDataTransformBuffer::locate(result->m_transforms);

  NMP::endianSwap(result->m_prevTransforms);
  REFIX_PTR_RELATIVE(AttribDataTransformBuffer, result->m_prevTransforms, result);
  AttribDataTransformBuffer::locate(result->m_prevTransforms);

  NMP::endianSwap(result->m_worldRoot);
  NMP::endianSwap(result->m_prevWorldRoot);
  NMP::endianSwap(result->m_deltaTime);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsInitialisation::dislocate(AttribData* target)
{
  AttribDataPhysicsInitialisation* result = (AttribDataPhysicsInitialisation*)target;

  NMP::endianSwap(result->m_worldRoot);
  NMP::endianSwap(result->m_prevWorldRoot);
  NMP::endianSwap(result->m_deltaTime);

  AttribDataTransformBuffer::dislocate(result->m_transforms);
  UNFIX_PTR_RELATIVE(AttribDataTransformBuffer, result->m_transforms, result);
  NMP::endianSwap(result->m_transforms);

  AttribDataTransformBuffer::dislocate(result->m_prevTransforms);
  UNFIX_PTR_RELATIVE(AttribDataTransformBuffer, result->m_prevTransforms, result);
  NMP::endianSwap(result->m_prevTransforms);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsInitialisation::relocate(AttribData* target, void* location)
{
  AttribDataPhysicsInitialisation* result = (AttribDataPhysicsInitialisation*) target;
  NMP_ASSERT(NMP_IS_ALIGNED(target, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(location, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  size_t offset = NMP::Memory::align(sizeof(AttribDataPhysicsInitialisation), MR_TRANSFORM_ATTRIB_ALIGNMENT);

  // Transforms.
  AttribDataTransformBuffer* transforms = (AttribDataTransformBuffer*) (((size_t)result) + offset);
  result->m_transforms = (AttribDataTransformBuffer*)(((size_t)location) + offset);
  NMP_ASSERT(NMP_IS_ALIGNED(transforms, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(result->m_transforms , MR_TRANSFORM_ATTRIB_ALIGNMENT));
  NMP::Memory::Format format = transforms->getInstanceMemoryRequirements();
  AttribDataTransformBuffer::relocate(transforms, result->m_transforms);
  offset += NMP::Memory::align(format.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);

  // Previous transforms.
  AttribDataTransformBuffer* prevTransforms = (AttribDataTransformBuffer*) (((size_t)result) + offset);
  result->m_prevTransforms = (AttribDataTransformBuffer*)(((size_t)location) + offset);
  NMP_ASSERT(NMP_IS_ALIGNED(prevTransforms, MR_TRANSFORM_ATTRIB_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(result->m_prevTransforms , MR_TRANSFORM_ATTRIB_ALIGNMENT));
  AttribDataTransformBuffer::relocate(prevTransforms, result->m_prevTransforms);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPhysicsInitialisation::create(
  NMP::MemoryAllocator* allocator,
  uint32_t              numAnimationJoints,
  uint16_t              refCount)
{
  AttribDataHandle handle;

  NMP::Memory::Format memReqs = getMemoryRequirements(numAnimationJoints);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  AttribDataPhysicsInitialisation* attribData = init(resource, numAnimationJoints, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  attribData->m_allocator = allocator;

  handle.m_attribData = attribData;

  return handle;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsInitialisation::getMemoryRequirements(uint32_t numAnimationJoints)
{
  NMP::Memory::Format result(sizeof(AttribDataPhysicsInitialisation), MR_TRANSFORM_ATTRIB_ALIGNMENT);
  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(numAnimationJoints, buffMemReqs, internalBuffMemReqs);
  result += buffMemReqs;
  result += buffMemReqs;

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_TRANSFORM_ATTRIB_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsInitialisation* AttribDataPhysicsInitialisation::init(
  NMP::Memory::Resource& resource,
  uint32_t               numAnimationJoints,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements(numAnimationJoints).alignment == MR_TRANSFORM_ATTRIB_ALIGNMENT);
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);
  AttribDataPhysicsInitialisation* result = (AttribDataPhysicsInitialisation*) resource.ptr;
  resource.increment(sizeof(AttribDataPhysicsInitialisation));

  result->setType(ATTRIB_TYPE_PHYSICS_INITIALISATION);
  result->setRefCount(refCount);

  result->m_worldRoot.identity();
  result->m_prevWorldRoot.identity();
  result->m_deltaTime = 0.0f;

  NMP::Memory::Format buffMemReqs;
  NMP::Memory::Format internalBuffMemReqs;
  AttribDataTransformBuffer::getPosQuatMemoryRequirements(numAnimationJoints, buffMemReqs, internalBuffMemReqs);
  result->m_transforms = MR::AttribDataTransformBuffer::initPosQuat(resource, internalBuffMemReqs, numAnimationJoints);
  result->m_prevTransforms = MR::AttribDataTransformBuffer::initPosQuat(resource, internalBuffMemReqs, numAnimationJoints);

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_TRANSFORM_ATTRIB_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPhysicsGrouperConfig functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsGrouperConfig::locate(AttribData* target)
{
  AttribDataPhysicsGrouperConfig* result = (AttribDataPhysicsGrouperConfig*)target;
  AttribData::locate(target);
  NMP::endianSwap(result->m_convertToPhysics[0]);
  NMP::endianSwap(result->m_convertToPhysics[1]);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPhysicsGrouperConfig::dislocate(AttribData* target)
{
  AttribDataPhysicsGrouperConfig* result = (AttribDataPhysicsGrouperConfig*)target;

  NMP::endianSwap(result->m_convertToPhysics[0]);
  NMP::endianSwap(result->m_convertToPhysics[1]);
  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPhysicsGrouperConfig::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  AttribDataHandle handle;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  AttribDataPhysicsGrouperConfig* attribData = init(resource, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  attribData->m_allocator = allocator;

  handle.m_attribData = attribData;
  handle.m_format     = AttribDataPhysicsGrouperConfig::getMemoryRequirements();

  return handle;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPhysicsGrouperConfig::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataPhysicsGrouperConfig), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPhysicsGrouperConfig* AttribDataPhysicsGrouperConfig::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPhysicsGrouperConfig* result = (AttribDataPhysicsGrouperConfig*) resource.ptr;
  resource.increment(sizeof(AttribDataPhysicsGrouperConfig));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_PHYSICS_GROUPER_CONFIG);
  result->setRefCount(refCount);

  result->m_convertToPhysics[0] = false;
  result->m_convertToPhysics[1] = false;

  return result;
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTransitDef functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataTransitDef::locate(AttribData* target)
{
  AttribDataTransitDef* result = (AttribDataTransitDef*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_duration);
  NMP::endianSwap(result->m_destinationInitMethod);
  NMP::endianSwap(result->m_destinationStartFraction);
  NMP::endianSwap(result->m_destinationStartSyncEvent);
  NMP::endianSwap(result->m_blendMode);
  NMP::endianSwap(result->m_slerpTrajectoryPosition);
  NMP::endianSwap(result->m_reversible);
  result->m_reverseInputCPConnection.endianSwap();
  
  if (result->m_nodeInitData)
  {
    NMP::endianSwap(result->m_nodeInitData);
    REFIX_PTR_RELATIVE(MR::NodeInitDataArrayDef, result->m_nodeInitData, result);
    result->m_nodeInitData->locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransitDef::dislocate(AttribData* target)
{
  AttribDataTransitDef* result = (AttribDataTransitDef*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_duration);
  NMP::endianSwap(result->m_destinationInitMethod);
  NMP::endianSwap(result->m_destinationStartFraction);
  NMP::endianSwap(result->m_destinationStartSyncEvent);
  NMP::endianSwap(result->m_blendMode);
  NMP::endianSwap(result->m_slerpTrajectoryPosition);
  NMP::endianSwap(result->m_reversible);
  result->m_reverseInputCPConnection.endianSwap();

  if (result->m_nodeInitData)
  {
    result->m_nodeInitData->dislocate();
    UNFIX_PTR_RELATIVE(MR::NodeInitDataArrayDef, result->m_nodeInitData, result);
    NMP::endianSwap(result->m_nodeInitData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTransitDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataTransitDef), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransitDef* AttribDataTransitDef::init(
  NMP::Memory::Resource&    resource,
  float                     duration,
  InitDestStartPointMethod  destinationInitMethod,
  float                     destinationStartFraction,
  float                     destinationStartSyncEvent,
  bool                      reversible,
  NodeID                    reverseControlParamNodeID,
  PinIndex                  reverseControlParamPinIndex,
  bool                      slerpTrajectoryPosition,
  uint32_t                  blendMode,
  bool                      freezeSource,
  bool                      freezeDest,
  uint16_t                  refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataTransitDef* result = (AttribDataTransitDef*)resource.ptr;
  resource.increment(sizeof(AttribDataTransitDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_TRANSIT_DEF);
  result->setRefCount(refCount);
  result->m_duration = duration;

  result->m_destinationInitMethod = destinationInitMethod;
  result->m_destinationStartFraction = destinationStartFraction;
  result->m_destinationStartSyncEvent = destinationStartSyncEvent;

  result->m_slerpTrajectoryPosition = slerpTrajectoryPosition;
  result->m_blendMode = blendMode;

  result->m_freezeSource = freezeSource;
  result->m_freezeDest = freezeDest;

  result->m_reversible = reversible;
  if (reversible)
  {
    NMP_ASSERT(reverseControlParamNodeID != INVALID_NODE_ID);
    result->m_reverseInputCPConnection.m_sourceNodeID = reverseControlParamNodeID;
    result->m_reverseInputCPConnection.m_sourcePinIndex = reverseControlParamPinIndex;
  }
  else
  {
    result->m_reverseInputCPConnection.m_sourceNodeID = INVALID_NODE_ID;
    result->m_reverseInputCPConnection.m_sourcePinIndex = INVALID_PIN_INDEX;
  }

  result->m_nodeInitData = 0;

  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTransitSyncEventsDef functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataTransitSyncEventsDef::locate(AttribData* target)
{
  AttribDataTransitSyncEventsDef* result = (AttribDataTransitSyncEventsDef*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_durationInEvents);
  NMP::endianSwap(result->m_destEventTrackOffset);
  NMP::endianSwap(result->m_destStartEventIndex);
  NMP::endianSwap(result->m_usingDestStartEventIndex);
  NMP::endianSwap(result->m_slerpTrajectoryPosition);
  NMP::endianSwap(result->m_blendMode);
  NMP::endianSwap(result->m_reversible);
  result->m_reverseInputCPConnection.endianSwap();

  if (result->m_nodeInitData)
  {
    NMP::endianSwap(result->m_nodeInitData);
    REFIX_PTR_RELATIVE(MR::NodeInitDataArrayDef, result->m_nodeInitData, result);
    result->m_nodeInitData->locate();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransitSyncEventsDef::dislocate(AttribData* target)
{
  AttribDataTransitSyncEventsDef* result = (AttribDataTransitSyncEventsDef*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_durationInEvents);
  NMP::endianSwap(result->m_destEventTrackOffset);
  NMP::endianSwap(result->m_destStartEventIndex);
  NMP::endianSwap(result->m_usingDestStartEventIndex);
  NMP::endianSwap(result->m_slerpTrajectoryPosition);
  NMP::endianSwap(result->m_blendMode);
  NMP::endianSwap(result->m_reversible);
  result->m_reverseInputCPConnection.endianSwap();

  if (result->m_nodeInitData)
  {
    result->m_nodeInitData->dislocate();
    UNFIX_PTR_RELATIVE(MR::NodeInitDataArrayDef, result->m_nodeInitData, result);
    NMP::endianSwap(result->m_nodeInitData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTransitSyncEventsDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataTransitSyncEventsDef), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransitSyncEventsDef* AttribDataTransitSyncEventsDef::init(
  NMP::Memory::Resource& resource,
  float                  durationInEvents,
  uint32_t               destEventSequenceOffset,
  uint32_t               destStartEventIndex,
  bool                   usingDestStartEventIndex,
  bool                   reversible,
  NodeID                 reverseControlParamNodeID,
  PinIndex               reverseControlParamPinIndex,
  bool                   slerpTrajectoryPosition,
  uint32_t               blendMode,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataTransitSyncEventsDef* result = (AttribDataTransitSyncEventsDef*)resource.ptr;
  resource.increment(sizeof(AttribDataTransitSyncEventsDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_TRANSIT_SYNC_EVENTS_DEF);
  result->setRefCount(refCount);
  result->m_durationInEvents         = durationInEvents;
  result->m_destEventTrackOffset     = destEventSequenceOffset;
  result->m_destStartEventIndex      = destStartEventIndex;
  result->m_usingDestStartEventIndex = usingDestStartEventIndex;

  result->m_slerpTrajectoryPosition  = slerpTrajectoryPosition;
  result->m_blendMode                = blendMode;

  result->m_reversible               = reversible;
  if (reversible)
  {
    NMP_ASSERT(reverseControlParamNodeID != INVALID_NODE_ID);
    result->m_reverseInputCPConnection.m_sourceNodeID = reverseControlParamNodeID;
    result->m_reverseInputCPConnection.m_sourcePinIndex = reverseControlParamPinIndex;
  }
  else
  {
    result->m_reverseInputCPConnection.m_sourceNodeID = INVALID_NODE_ID;
    result->m_reverseInputCPConnection.m_sourcePinIndex = INVALID_PIN_INDEX;
  }

  result->m_nodeInitData = 0;

  return result;
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataTransitSyncEvents functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataTransitSyncEvents::locate(AttribData* target)
{
  AttribDataTransitSyncEvents* result = (AttribDataTransitSyncEvents*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_transitionEventCount);
  NMP::endianSwap(result->m_completed);
  NMP::endianSwap(result->m_reversed);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataTransitSyncEvents::dislocate(AttribData* target)
{
  AttribDataTransitSyncEvents* result = (AttribDataTransitSyncEvents*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_transitionEventCount);
  NMP::endianSwap(result->m_completed);
  NMP::endianSwap(result->m_reversed);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTransitSyncEvents::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataTransitSyncEvents), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataTransitSyncEvents* AttribDataTransitSyncEvents::init(NMP::Memory::Resource& resource, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataTransitSyncEvents), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataTransitSyncEvents* result = (AttribDataTransitSyncEvents*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_TRANSIT_SYNC_EVENTS);
  result->setRefCount(refCount);
  result->m_transitionEventCount = 0.0f;
  result->m_completed = false;
  result->m_reversed = false;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTransitSyncEvents::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements();
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataTransitSyncEvents::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, refCount);

  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataTransitSyncEvents::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
#if defined(MR_OUTPUT_DEBUGGING)
uint32_t AttribDataTransitSyncEvents::serializeTx(
  Network*    NMP_UNUSED(net),
  MR::NodeID  NMP_UNUSED(owningNodeID),
  AttribData* toOutput,
  void*       outputBuffer,
  uint32_t    NMP_USED_FOR_ASSERTS(outputBufferSize))
{
  NMP_ASSERT(toOutput);

  uint32_t dataSize = (uint32_t)sizeof(AttribTransitSyncEventsOutputData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    AttribTransitSyncEventsOutputData* output = (AttribTransitSyncEventsOutputData*) outputBuffer;
    AttribDataTransitSyncEvents* sourceAttrib = (AttribDataTransitSyncEvents*)toOutput;

    output->m_completed = sourceAttrib->m_completed;
    output->m_reversed = sourceAttrib->m_reversed;
    output->m_transitionEventCount = sourceAttrib->m_transitionEventCount;
    AttribTransitSyncEventsOutputData::endianSwap(outputBuffer);
  }
  return dataSize;
}
#endif // MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPlaybackPosInit functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPlaybackPosInit::getMemoryRequirements()
{
  return NMP::Memory::Format(
           NMP::Memory::align(sizeof(AttribDataPlaybackPosInit), MR_ATTRIB_DATA_ALIGNMENT),
           MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPlaybackPosInit* AttribDataPlaybackPosInit::init(
  NMP::Memory::Resource& resource,
  bool                   initWithDurationFraction, // Indicates our initialisation method.
  float                  startValue,               // Either: Init the playback pos at a fraction of the nodes duration.
                                                   //     Or: Init the playback pos at the specified event position.
  int32_t                adjustStartEventIndex,    // Start sync event adjustment value, used independently of m_startValue;
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPlaybackPosInit* result = (AttribDataPlaybackPosInit*) resource.ptr;
  resource.increment(sizeof(AttribDataPlaybackPosInit));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_PLAYBACK_POS_INIT);
  result->setRefCount(refCount);

  result->m_initWithDurationFraction = initWithDurationFraction;
  result->m_startValue = startValue;
  result->m_adjustStartEventIndex = adjustStartEventIndex;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPlaybackPosInit::create(
  NMP::MemoryAllocator*      allocator,
  bool                       initWithDurationFraction, // Indicates our initialisation method.
  float                      startValue,               // Either: Init the playback pos at a fraction of the nodes duration.
                                                       //     Or: Init the playback pos at the specified event position.
  int32_t                    adjustStartEventIndex,    // Start sync event adjustment value, used independently of m_startValue;
  uint16_t                   refCount)
{
  AttribDataHandle handle;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  AttribDataPlaybackPosInit* attribData = init(resource, initWithDurationFraction, startValue, adjustStartEventIndex, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  attribData->m_allocator = allocator;

  handle.m_format     = AttribDataPlaybackPosInit::getMemoryRequirements();
  handle.m_attribData = attribData;

  return handle;
}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataPlaybackPosInit::copy(AttribData* source, AttribData* dest)
{
  NMP_ASSERT(source && dest);
  NMP_ASSERT(source->getType() == ATTRIB_TYPE_PLAYBACK_POS_INIT && dest->getType() == ATTRIB_TYPE_PLAYBACK_POS_INIT);
  AttribDataPlaybackPosInit* sourcePlaybackPosInit = (AttribDataPlaybackPosInit*) source;
  AttribDataPlaybackPosInit* destPlaybackPosInit = (AttribDataPlaybackPosInit*) dest;
  destPlaybackPosInit->m_initWithDurationFraction = sourcePlaybackPosInit->m_initWithDurationFraction;
  destPlaybackPosInit->m_adjustStartEventIndex = sourcePlaybackPosInit->m_adjustStartEventIndex;
  destPlaybackPosInit->m_startValue = sourcePlaybackPosInit->m_startValue;

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataPlaybackPosInit::locate(AttribData* target)
{
  AttribDataPlaybackPosInit* result = (AttribDataPlaybackPosInit*)target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_initWithDurationFraction);
  NMP::endianSwap(result->m_startValue);
  NMP::endianSwap(result->m_adjustStartEventIndex);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPlaybackPosInit::dislocate(AttribData* target)
{
  AttribDataPlaybackPosInit* result = (AttribDataPlaybackPosInit*)target;

  AttribData::dislocate(target);
  NMP::endianSwap(result->m_initWithDurationFraction);
  NMP::endianSwap(result->m_startValue);
  NMP::endianSwap(result->m_adjustStartEventIndex);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainSetup functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataBasicUnevenTerrainSetup::locate(AttribData* target)
{
  AttribDataBasicUnevenTerrainSetup* result = (AttribDataBasicUnevenTerrainSetup*)target;
  AttribData::locate(target);

  NMP::endianSwap(result->m_upAxis);
  NMP::endianSwap(result->m_numLimbs);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBasicUnevenTerrainSetup::dislocate(AttribData* target)
{
  AttribDataBasicUnevenTerrainSetup* result = (AttribDataBasicUnevenTerrainSetup*)target;

  NMP::endianSwap(result->m_numLimbs);
  NMP::endianSwap(result->m_upAxis);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainSetup::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainSetup), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainSetup* AttribDataBasicUnevenTerrainSetup::init(
  NMP::Memory::Resource& resource,
  uint32_t               numLimbs,
  uint16_t               refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainSetup), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainSetup* result = (AttribDataBasicUnevenTerrainSetup*) resource.alignAndIncrement(memReqsHdr);

  // Attrib data
  result->setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_SETUP);
  result->setRefCount(refCount);

  // Setup data
  result->m_upAxis.set(0.0f, 1.0f, 0.0f);
  result->m_numLimbs = numLimbs;

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainChain:ChainData functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataBasicUnevenTerrainChain::ChainData::locate(ChainData* target)
{
  AttribDataBasicUnevenTerrainChain::ChainData* result = (AttribDataBasicUnevenTerrainChain::ChainData*)target;

  // Leg IK solver options
  NMP::endianSwap(result->m_kneeRotationAxis);
  NMP::endianSwapArray(result->m_channelIDs, 5);
  NMP::endianSwap(result->m_numFootJoints);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBasicUnevenTerrainChain::ChainData::dislocate(ChainData* target)
{
  AttribDataBasicUnevenTerrainChain::ChainData* result = (AttribDataBasicUnevenTerrainChain::ChainData*)target;

  // Leg IK solver options
  NMP::endianSwap(result->m_numFootJoints);
  NMP::endianSwapArray(result->m_channelIDs, 5);
  NMP::endianSwap(result->m_kneeRotationAxis);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainChain::ChainData*
AttribDataBasicUnevenTerrainChain::ChainData::relocate(void*& ptr)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainChain::ChainData), NMP_VECTOR_ALIGNMENT);
  AttribDataBasicUnevenTerrainChain::ChainData* result =
    (AttribDataBasicUnevenTerrainChain::ChainData*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Multiple of the alignment
  ptr = (void*)NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainChain::ChainData::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainChain::ChainData), NMP_VECTOR_ALIGNMENT);

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainChain::ChainData*
AttribDataBasicUnevenTerrainChain::ChainData::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainChain::ChainData), NMP_VECTOR_ALIGNMENT);
  ChainData* result = (ChainData*)resource.alignAndIncrement(memReqsHdr);

  result->m_numFootJoints = 0;
  for (uint32_t i = 0; i < 5; ++i)
    result->m_channelIDs[i] = (uint32_t) - 1;

  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainChain functions
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataBasicUnevenTerrainChain::locate(AttribData* target)
{
  AttribDataBasicUnevenTerrainChain* result = (AttribDataBasicUnevenTerrainChain*)target;
  AttribData::locate(target);

  // Hips channel ID for the hip height control
  NMP::endianSwap(result->m_hipsChannelID);

  // Hips IK position limit clamping
  NMP::endianSwap(result->m_hipsHeightControlEnable);
  NMP::endianSwap(result->m_hipsPosVelLimitEnable);
  NMP::endianSwap(result->m_hipsPosAccelLimitEnable);
  NMP::endianSwap(result->m_hipsPosVelLimit);
  NMP::endianSwap(result->m_hipsPosAccelLimit);

  // Leg IK solver options
  NMP::endianSwap(result->m_straightestLegFactor);

  // Leg IK end joint limit clamping
  NMP::endianSwap(result->m_endJointPosVelLimitEnable);
  NMP::endianSwap(result->m_endJointPosAccelLimitEnable);
  NMP::endianSwap(result->m_endJointAngVelLimitEnable);
  NMP::endianSwap(result->m_endJointAngAccelLimitEnable);
  NMP::endianSwap(result->m_endJointPosVelLimit);
  NMP::endianSwap(result->m_endJointPosAccelLimit);
  NMP::endianSwap(result->m_endJointAngVelLimit);
  NMP::endianSwap(result->m_endJointAngAccelLimit);

  // Foot alignment with the terrain surface options
  NMP::endianSwap(result->m_useGroundPenetrationFixup);
  NMP::endianSwap(result->m_useTrajectorySlopeAlignment);
  NMP::endianSwap(result->m_footAlignToSurfaceAngleLimit);
  NMP::endianSwap(result->m_footAlignToSurfaceMaxSlopeAngle);

  // Foot lifting options
  NMP::endianSwap(result->m_footLiftingHeightLimit);

  // IK chain setup data for each limb
  NMP::endianSwap(result->m_numLimbs);
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    REFIX_SWAP_PTR_RELATIVE(AttribDataBasicUnevenTerrainChain::ChainData, result->m_chainInfo[i], target);
    AttribDataBasicUnevenTerrainChain::ChainData::locate(result->m_chainInfo[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBasicUnevenTerrainChain::dislocate(AttribData* target)
{
  AttribDataBasicUnevenTerrainChain* result = (AttribDataBasicUnevenTerrainChain*)target;

  // IK chain setup data for each limb
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    AttribDataBasicUnevenTerrainChain::ChainData::dislocate(result->m_chainInfo[i]);
    UNFIX_SWAP_PTR_RELATIVE(AttribDataBasicUnevenTerrainChain::ChainData, result->m_chainInfo[i], target);
  }
  NMP::endianSwap(result->m_numLimbs);

  // Foot lifting options
  NMP::endianSwap(result->m_footLiftingHeightLimit);

  // Foot alignment with the terrain surface options
  NMP::endianSwap(result->m_footAlignToSurfaceMaxSlopeAngle);
  NMP::endianSwap(result->m_footAlignToSurfaceAngleLimit);
  NMP::endianSwap(result->m_useTrajectorySlopeAlignment);
  NMP::endianSwap(result->m_useGroundPenetrationFixup);

  // Leg IK end joint limit clamping
  NMP::endianSwap(result->m_endJointAngAccelLimit);
  NMP::endianSwap(result->m_endJointAngVelLimit);
  NMP::endianSwap(result->m_endJointPosAccelLimit);
  NMP::endianSwap(result->m_endJointPosVelLimit);
  NMP::endianSwap(result->m_endJointAngAccelLimitEnable);
  NMP::endianSwap(result->m_endJointAngVelLimitEnable);
  NMP::endianSwap(result->m_endJointPosAccelLimitEnable);
  NMP::endianSwap(result->m_endJointPosVelLimitEnable);

  // Leg IK solver options
  NMP::endianSwap(result->m_straightestLegFactor);

  // Hips IK position limit clamping
  NMP::endianSwap(result->m_hipsPosAccelLimit);
  NMP::endianSwap(result->m_hipsPosVelLimit);
  NMP::endianSwap(result->m_hipsPosAccelLimitEnable);
  NMP::endianSwap(result->m_hipsPosVelLimitEnable);
  NMP::endianSwap(result->m_hipsHeightControlEnable);

  // Hips channel ID for the hip height control
  NMP::endianSwap(result->m_hipsChannelID);

  // AttribData
  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBasicUnevenTerrainChain::relocate(AttribData* target, void* location)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainChain), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainChain* result = (AttribDataBasicUnevenTerrainChain*)target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, memReqsHdr.alignment));
  ptr = NMP::Memory::increment(ptr, memReqsHdr.size);

  // Get the memory requirements for the chain info data
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    result->m_chainInfo[i] = AttribDataBasicUnevenTerrainChain::ChainData::relocate(ptr);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainChain::getMemoryRequirements(uint32_t numLimbs)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainChain), MR_ATTRIB_DATA_ALIGNMENT);

  // Get the memory requirements for the limb state data
  NMP::Memory::Format memReqsChain = AttribDataBasicUnevenTerrainChain::ChainData::getMemoryRequirements();
  result += (memReqsChain * numLimbs);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainChain* AttribDataBasicUnevenTerrainChain::init(
  NMP::Memory::Resource& resource,
  uint32_t               numLimbs,
  uint16_t               refCount)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainChain), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainChain* result = (AttribDataBasicUnevenTerrainChain*)resource.alignAndIncrement(memReqsHdr);

  // Attrib data
  result->setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_CHAIN);
  result->setRefCount(refCount);

  // Hips channel ID for the hip height control
  result->m_hipsChannelID = (uint32_t) - 1;

  // Chain data
  result->m_numLimbs = numLimbs;
  for (uint32_t i = 0; i < numLimbs; ++i)
  {
    result->m_chainInfo[i] = AttribDataBasicUnevenTerrainChain::ChainData::init(resource);
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainIKSetup::LimbState functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainIKSetup::LimbState::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainIKSetup::LimbState), NMP_VECTOR_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainIKSetup::LimbState*
AttribDataBasicUnevenTerrainIKSetup::LimbState::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainIKSetup::LimbState), NMP_VECTOR_ALIGNMENT);
  AttribDataBasicUnevenTerrainIKSetup::LimbState* result = (AttribDataBasicUnevenTerrainIKSetup::LimbState*) resource.alignAndIncrement(memReqsHdr);

  // Multiple of the attrib data alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainIKSetup::LimbState*
AttribDataBasicUnevenTerrainIKSetup::LimbState::relocate(void*& ptr)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainIKSetup::LimbState), NMP_VECTOR_ALIGNMENT);
  AttribDataBasicUnevenTerrainIKSetup::LimbState* result =
    (AttribDataBasicUnevenTerrainIKSetup::LimbState*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Multiple of the alignment
  ptr = (void*)NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainIKSetup functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataBasicUnevenTerrainIKSetup::relocate(AttribData* target, void* location)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainIKSetup), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainIKSetup* result = (AttribDataBasicUnevenTerrainIKSetup*)target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, memReqsHdr.alignment));
  ptr = NMP::Memory::increment(ptr, memReqsHdr.size);

  // Get the memory requirements for the chain info data
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    result->m_limbStates[i] = AttribDataBasicUnevenTerrainIKSetup::LimbState::relocate(ptr);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainIKSetup::getMemoryRequirements(uint32_t numLimbs)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainIKSetup), MR_ATTRIB_DATA_ALIGNMENT);

  // Get the memory requirements for the limb state data
  NMP::Memory::Format memReqsLimb = AttribDataBasicUnevenTerrainIKSetup::LimbState::getMemoryRequirements();
  result += (memReqsLimb * numLimbs);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainIKSetup* AttribDataBasicUnevenTerrainIKSetup::init(
  NMP::Memory::Resource& resource,
  uint32_t               numLimbs,
  uint16_t               refCount)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainIKSetup), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainIKSetup* result = (AttribDataBasicUnevenTerrainIKSetup*)resource.alignAndIncrement(memReqsHdr);

  // Attrib data
  result->setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_SETUP);
  result->setRefCount(refCount);

  result->m_enableIK = false;
  result->m_straightestLegLength = 0.0f;

  // Limb state data
  result->m_numLimbs = numLimbs;
  for (uint32_t i = 0; i < numLimbs; ++i)
  {
    result->m_limbStates[i] = AttribDataBasicUnevenTerrainIKSetup::LimbState::init(resource);
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBasicUnevenTerrainIKSetup::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint32_t numLimbs = 2;
  uint16_t refCount = 0;
  
  // Allocate the memory and initialise
  result.m_format = getMemoryRequirements(numLimbs);
  result.m_attribData = init(resource, numLimbs, refCount);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainIKSetup::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements(2);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData), NMP_VECTOR_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData*
AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData), NMP_VECTOR_ALIGNMENT);
  AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* result =
    (AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData*) resource.alignAndIncrement(memReqsHdr);

  result->m_targetWorldFootbaseLiftingValid = false;
  result->m_isFootInContactPhase = false;

  // Multiple of the attrib data alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData*
AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData::relocate(void*& ptr)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData), NMP_VECTOR_ALIGNMENT);
  AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData* result =
    (AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Multiple of the alignment
  ptr = (void*)NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainFootLiftingTarget functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataBasicUnevenTerrainFootLiftingTarget::relocate(AttribData* target, void* location)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainFootLiftingTarget), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainFootLiftingTarget* result = (AttribDataBasicUnevenTerrainFootLiftingTarget*)target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, memReqsHdr.alignment));
  ptr = NMP::Memory::increment(ptr, memReqsHdr.size);

  // Get the memory requirements for the chain info data
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    result->m_limbInfo[i] = AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData::relocate(ptr);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainFootLiftingTarget::getMemoryRequirements(uint32_t numLimbs)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainFootLiftingTarget), MR_ATTRIB_DATA_ALIGNMENT);

  // Get the memory requirements for the limb state data
  NMP::Memory::Format memReqsLimb = AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData::getMemoryRequirements();
  result += (memReqsLimb * numLimbs);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainFootLiftingTarget* AttribDataBasicUnevenTerrainFootLiftingTarget::init(
  NMP::Memory::Resource& resource,
  uint32_t               numLimbs,
  uint16_t               refCount)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainFootLiftingTarget), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainFootLiftingTarget* result = (AttribDataBasicUnevenTerrainFootLiftingTarget*) resource.alignAndIncrement(memReqsHdr);

  // Attrib data
  result->setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET);
  result->setRefCount(refCount);

  // Limb data
  result->m_numLimbs = numLimbs;
  for (uint32_t i = 0; i < numLimbs; ++i)
  {
    result->m_limbInfo[i] = AttribDataBasicUnevenTerrainFootLiftingTarget::LimbData::init(resource);
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBasicUnevenTerrainFootLiftingTarget::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint32_t numLimbs = 2;
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements(numLimbs);
  result.m_attribData = init(resource, numLimbs, refCount);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainFootLiftingTarget::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements(2);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainIKState::LimbState functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainIKState::LimbState::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainIKState::LimbState), NMP_VECTOR_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainIKState::LimbState*
AttribDataBasicUnevenTerrainIKState::LimbState::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainIKState::LimbState), NMP_VECTOR_ALIGNMENT);
  AttribDataBasicUnevenTerrainIKState::LimbState* result =
    (AttribDataBasicUnevenTerrainIKState::LimbState*) resource.alignAndIncrement(memReqsHdr);

  // Multiple of the attrib data alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainIKState::LimbState*
AttribDataBasicUnevenTerrainIKState::LimbState::relocate(void*& ptr)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainIKState::LimbState), NMP_VECTOR_ALIGNMENT);
  AttribDataBasicUnevenTerrainIKState::LimbState* result =
    (AttribDataBasicUnevenTerrainIKState::LimbState*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Multiple of the alignment
  ptr = (void*)NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBasicUnevenTerrainIKState functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataBasicUnevenTerrainIKState::relocate(AttribData* target, void* location)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainIKState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainIKState* result = (AttribDataBasicUnevenTerrainIKState*)target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, memReqsHdr.alignment));
  ptr = NMP::Memory::increment(ptr, memReqsHdr.size);

  // Get the memory requirements for the chain info data
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    result->m_limbStates[i] = AttribDataBasicUnevenTerrainIKState::LimbState::relocate(ptr);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainIKState::getMemoryRequirements(uint32_t numLimbs)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format result(sizeof(AttribDataBasicUnevenTerrainIKState), MR_ATTRIB_DATA_ALIGNMENT);

  // Get the memory requirements for the limb state data
  NMP::Memory::Format memReqsLimb = AttribDataBasicUnevenTerrainIKState::LimbState::getMemoryRequirements();
  result += (memReqsLimb * numLimbs);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBasicUnevenTerrainIKState* AttribDataBasicUnevenTerrainIKState::init(
  NMP::Memory::Resource& resource,
  uint32_t               numLimbs,
  uint16_t               refCount)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBasicUnevenTerrainIKState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBasicUnevenTerrainIKState* result = (AttribDataBasicUnevenTerrainIKState*) resource.alignAndIncrement(memReqsHdr);

  // Attrib data
  result->setType(ATTRIB_TYPE_BASIC_UNEVEN_TERRAIN_IK_STATE);
  result->setRefCount(refCount);

  // IK information
  result->m_isValidData = false;

  // Limb state data
  result->m_numLimbs = numLimbs;
  for (uint32_t i = 0; i < numLimbs; ++i)
  {
    result->m_limbStates[i] = AttribDataBasicUnevenTerrainIKState::LimbState::init(resource);
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBasicUnevenTerrainIKState::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint32_t numLimbs = 2;
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements(numLimbs);
  result.m_attribData = init(resource, numLimbs, refCount);
  
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBasicUnevenTerrainIKState::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements(2);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPredictiveUnevenTerrainPredictionDef:LimbData functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::locate(LimbData* target)
{
  AttribDataPredictiveUnevenTerrainPredictionDef::LimbData* result = (AttribDataPredictiveUnevenTerrainPredictionDef::LimbData*)target;

  NMP::endianSwap(result->m_eventTrackID);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::dislocate(LimbData* target)
{
  AttribDataPredictiveUnevenTerrainPredictionDef::LimbData* result = (AttribDataPredictiveUnevenTerrainPredictionDef::LimbData*)target;

  NMP::endianSwap(result->m_eventTrackID);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainPredictionDef::LimbData*
AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::relocate(void*& ptr)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainPredictionDef::LimbData), NMP_NATURAL_TYPE_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainPredictionDef::LimbData* result =
    (AttribDataPredictiveUnevenTerrainPredictionDef::LimbData*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Multiple of the alignment
  ptr = (void*)NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataPredictiveUnevenTerrainPredictionDef::LimbData), NMP_NATURAL_TYPE_ALIGNMENT);

  // Multiple of the alignment
  result.size = NMP::Memory::align(result.size, NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainPredictionDef::LimbData*
AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainPredictionDef::LimbData), NMP_NATURAL_TYPE_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainPredictionDef::LimbData* result =
    (AttribDataPredictiveUnevenTerrainPredictionDef::LimbData*) resource.alignAndIncrement(memReqsHdr);

  result->m_eventTrackID = (uint32_t)-1;

  // Multiple of the alignment
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPredictiveUnevenTerrainPredictionDef functions
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataPredictiveUnevenTerrainPredictionDef::locate(AttribData* target)
{
  AttribDataPredictiveUnevenTerrainPredictionDef* result = (AttribDataPredictiveUnevenTerrainPredictionDef*)target;
  AttribData::locate(target);

  // Foot lifting options dependent on the character trajectory
  NMP::endianSwap(result->m_footLiftingSlopeAngleLimit);
  NMP::endianSwap(result->m_footLiftingLateralAngleLimit);
  NMP::endianSwap(result->m_closeFootbaseTolFrac);

  // Limb data
  NMP::endianSwap(result->m_numLimbs);
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    REFIX_SWAP_PTR_RELATIVE(AttribDataPredictiveUnevenTerrainPredictionDef::LimbData, result->m_limbInfo[i], target);
    AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::locate(result->m_limbInfo[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainPredictionDef::dislocate(AttribData* target)
{
  AttribDataPredictiveUnevenTerrainPredictionDef* result = (AttribDataPredictiveUnevenTerrainPredictionDef*)target;

  // Limb data
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::dislocate(result->m_limbInfo[i]);
    UNFIX_SWAP_PTR_RELATIVE(AttribDataPredictiveUnevenTerrainPredictionDef::LimbData, result->m_limbInfo[i], target);
  }
  NMP::endianSwap(result->m_numLimbs);

  // Foot lifting options dependent on the character trajectory
  NMP::endianSwap(result->m_closeFootbaseTolFrac);
  NMP::endianSwap(result->m_footLiftingLateralAngleLimit);
  NMP::endianSwap(result->m_footLiftingSlopeAngleLimit);

  // Attrib data
  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainPredictionDef::relocate(AttribData* target, void* location)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainPredictionDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainPredictionDef* result = (AttribDataPredictiveUnevenTerrainPredictionDef*)target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, memReqsHdr.alignment));
  ptr = NMP::Memory::increment(ptr, memReqsHdr.size);

  // Get the memory requirements for the limb info data
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    result->m_limbInfo[i] = AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::relocate(ptr);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPredictiveUnevenTerrainPredictionDef::getMemoryRequirements(uint32_t numLimbs)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format result(sizeof(AttribDataPredictiveUnevenTerrainPredictionDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Get the memory requirements for the limb state data
  NMP::Memory::Format memReqsChain = AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::getMemoryRequirements();
  result += (memReqsChain * numLimbs);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainPredictionDef* AttribDataPredictiveUnevenTerrainPredictionDef::init(
  NMP::Memory::Resource& resource,
  uint32_t               numLimbs,
  uint16_t               refCount)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainPredictionDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainPredictionDef* result = (AttribDataPredictiveUnevenTerrainPredictionDef*)resource.alignAndIncrement(memReqsHdr);

  //---------------------------
  // Attrib data
  result->setType(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_PREDICTION_DEF);
  result->setRefCount(refCount);

  result->m_footLiftingSlopeAngleLimit = 0.0f;
  result->m_footLiftingLateralAngleLimit = 0.0f;
  result->m_closeFootbaseTolFrac = 0.0f;

  //---------------------------
  // limb data
  result->m_numLimbs = numLimbs;
  for (uint32_t i = 0; i < numLimbs; ++i)
  {
    result->m_limbInfo[i] = AttribDataPredictiveUnevenTerrainPredictionDef::LimbData::init(resource);
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPredictiveUnevenTerrainPredictionState::LimbState functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPredictiveUnevenTerrainPredictionState::LimbState::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataPredictiveUnevenTerrainPredictionState::LimbState), NMP_VECTOR_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainPredictionState::LimbState*
AttribDataPredictiveUnevenTerrainPredictionState::LimbState::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainPredictionState::LimbState), NMP_VECTOR_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainPredictionState::LimbState* result =
    (AttribDataPredictiveUnevenTerrainPredictionState::LimbState*) resource.alignAndIncrement(memReqsHdr);

  result->resetFootCycleTiming();
  result->resetFootCyclePrediction();

  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainPredictionState::LimbState*
AttribDataPredictiveUnevenTerrainPredictionState::LimbState::relocate(void*& ptr)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainPredictionState::LimbState), NMP_VECTOR_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainPredictionState::LimbState* result =
    (AttribDataPredictiveUnevenTerrainPredictionState::LimbState*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Multiple of the alignment
  ptr = (void*)NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainPredictionState::LimbState::resetFootCycleTiming()
{
  m_prevSyncEventPos = INVALID_EVENT_INDEX;
  m_nextSyncEventPos = INVALID_EVENT_INDEX;
  m_curLimbPhase = kInvalidPhase;
  m_hasFootCycleLooped = false;

  m_timeToPrevSyncEvent = 0.0f;
  m_timeToPrevEnd = 0.0f;
  m_timeToMid = 0.0f;
  m_timeToNextStart = 0.0f;
  m_timeToNextSyncEvent = 0.0f;

  m_timeFractionInFootCycle = 0.0f;
  m_timeFractionOfMidInFootCycle = 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainPredictionState::LimbState::resetFootCyclePrediction()
{
  m_initNextWorldTrajectoryPos.setToZero();
  m_initMidWorldTrajectoryPos.setToZero();
  m_initNextWorldTrajectoryPosValid = false;

  m_initNextWorldFootbasePos.setToZero();
  m_initMidWorldFootbasePos.setToZero();
  m_initNextWorldFootbasePosValid = false;

  m_footCyclePredictor.init();
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPredictiveUnevenTerrainPredictionState functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainPredictionState::relocate(AttribData* target, void* location)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainPredictionState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainPredictionState* result = (AttribDataPredictiveUnevenTerrainPredictionState*)target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, memReqsHdr.alignment));
  ptr = NMP::Memory::increment(ptr, memReqsHdr.size);

  // Get the memory requirements for the chain info data
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    result->m_limbStates[i] = AttribDataPredictiveUnevenTerrainPredictionState::LimbState::relocate(ptr);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPredictiveUnevenTerrainPredictionState::getMemoryRequirements(uint32_t numLimbs)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format result(sizeof(AttribDataPredictiveUnevenTerrainPredictionState), MR_ATTRIB_DATA_ALIGNMENT);

  // Get the memory requirements for the limb state data
  NMP::Memory::Format memReqsLimb = AttribDataPredictiveUnevenTerrainPredictionState::LimbState::getMemoryRequirements();
  result += (memReqsLimb * numLimbs);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainPredictionState* AttribDataPredictiveUnevenTerrainPredictionState::init(
  NMP::Memory::Resource& resource,
  uint32_t               numLimbs,
  uint16_t               refCount)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainPredictionState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainPredictionState* result = (AttribDataPredictiveUnevenTerrainPredictionState*)resource.alignAndIncrement(memReqsHdr);

  //---------------------------
  // Attrib data
  result->setType(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_IK_PREDICTION_STATE);
  result->setRefCount(refCount);

  // Common timing information
  result->m_isDataValid = false;
  result->m_isAbs = false;
  result->m_curSyncEventPos = 0.0f;

  // Trajectory sample history state
  result->resetTrajectoryPredictionHistory();

  //---------------------------
  // Limb state data
  result->m_numLimbs = numLimbs;
  for (uint32_t i = 0; i < numLimbs; ++i)
  {
    result->m_limbStates[i] = AttribDataPredictiveUnevenTerrainPredictionState::LimbState::init(resource);
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPredictiveUnevenTerrainPredictionState::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;
  
  // Defaults
  uint16_t refCount = 0;
  uint32_t numLimbs = 2;

  result.m_format = getMemoryRequirements(numLimbs);
  result.m_attribData = init(resource, numLimbs, refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPredictiveUnevenTerrainPredictionState::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements(2);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainPredictionState::resetTrajectoryPredictionHistory()
{
  m_trajNumSamples = 0;
  m_trajSampleIndx = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainPredictionState::appendTrajectorySample(
  float deltaTime,
  const NMP::Vector3& deltaPosVel,
  const NMP::Vector3& deltaAngVel)
{
  m_trajPosVel[m_trajSampleIndx] = deltaPosVel;
  m_trajAngVel[m_trajSampleIndx] = deltaAngVel;
  m_trajDeltaTime[m_trajSampleIndx] = deltaTime;

  m_trajSampleIndx = (m_trajSampleIndx + 1) & 0x03;
  m_trajNumSamples++;
  if (m_trajNumSamples > 4)
    m_trajNumSamples = 4;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData), NMP_VECTOR_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData*
AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData::init(NMP::Memory::Resource& resource)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData), NMP_VECTOR_ALIGNMENT);
  LimbData* result = (LimbData*)resource.alignAndIncrement(memReqsHdr);

  result->reset();

  // Multiple of the alignment
  resource.align(NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData*
AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData::relocate(void*& ptr)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData), NMP_VECTOR_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData* result =
    (AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData*)NMP::Memory::alignAndIncrement(ptr, memReqsHdr);

  // Multiple of the alignment
  ptr = (void*)NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData::reset()
{
  // Actual ray-cast hit valid flags
  m_curWorldFootbaseHitPosValid = false;
  m_midWorldFootbaseHitPosValid = false;
  m_nextWorldFootbaseHitPosValid = false;

  // Enable flags for foot lifting (indicates actual ray-cast entry can be used for lifting)
  m_midWorldHitDataEnabledForLifting = false;
  m_nextWorldHitDataEnabledForLifting = false;

  // Flags used to indicate the actual ray-cast hit normals are within the slope angle limit
  m_curWorldFootbaseHitNormalSlopeValid = false;
  m_midWorldFootbaseHitNormalSlopeValid = false;
  m_nextWorldFootbaseHitNormalSlopeValid = false;

  // Alternation state
  m_alternateState = AttribDataPredictiveUnevenTerrainFootLiftingState::kInvalidAlternationState;
  m_alternatePrediction = AttribDataPredictiveUnevenTerrainFootLiftingState::kInvalidPredictionState;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataPredictiveUnevenTerrainFootLiftingState functions
//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainFootLiftingState::relocate(AttribData* target, void* location)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainFootLiftingState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainFootLiftingState* result = (AttribDataPredictiveUnevenTerrainFootLiftingState*)target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, memReqsHdr.alignment));
  ptr = NMP::Memory::increment(ptr, memReqsHdr.size);

  // Get the memory requirements for the limb info data
  for (uint32_t i = 0; i < result->m_numLimbs; ++i)
  {
    result->m_limbInfo[i] = AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData::relocate(ptr);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPredictiveUnevenTerrainFootLiftingState::getMemoryRequirements(uint32_t numLimbs)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format result(sizeof(AttribDataPredictiveUnevenTerrainFootLiftingState), MR_ATTRIB_DATA_ALIGNMENT);

  // Get the memory requirements for the limb state data
  NMP::Memory::Format memReqsLimb = AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData::getMemoryRequirements();
  result += (memReqsLimb * numLimbs);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataPredictiveUnevenTerrainFootLiftingState* AttribDataPredictiveUnevenTerrainFootLiftingState::init(
  NMP::Memory::Resource& resource,
  uint32_t               numLimbs,
  uint16_t               refCount)
{
  NMP_ASSERT(numLimbs <= 2);
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataPredictiveUnevenTerrainFootLiftingState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataPredictiveUnevenTerrainFootLiftingState* result = (AttribDataPredictiveUnevenTerrainFootLiftingState*)resource.alignAndIncrement(memReqsHdr);

  // Attrib data
  result->setType(ATTRIB_TYPE_PREDICTIVE_UNEVEN_TERRAIN_FOOT_LIFTING_STATE);
  result->setRefCount(refCount);

  result->m_footLiftingBasisValid = false;

  // Limb state data
  result->m_numLimbs = numLimbs;
  for (uint32_t i = 0; i < numLimbs; ++i)
  {
    result->m_limbInfo[i] = AttribDataPredictiveUnevenTerrainFootLiftingState::LimbData::init(resource);
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataPredictiveUnevenTerrainFootLiftingState::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;
  uint32_t numLimbs = 2;

  result.m_format = getMemoryRequirements(numLimbs);
  result.m_attribData = init(resource, numLimbs, refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataPredictiveUnevenTerrainFootLiftingState::outputMemReqs(Network* NMP_UNUSED(network))
{
  return getMemoryRequirements(2);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataPredictiveUnevenTerrainFootLiftingState::reset()
{
  m_footLiftingBasisValid = false;

  for (uint32_t limbIndex = 0; limbIndex < m_numLimbs; ++limbIndex)
  {
    m_limbInfo[limbIndex]->reset();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBlendNxMDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBlendNxMDef::getMemoryRequirements(
  uint16_t                numWeightsX,
  uint16_t                numWeightsY,
  bool                    wrapWeightsX,
  bool                    wrapWeightsY)
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataBlendNxMDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Weights X
  uint16_t numElementsX = numWeightsX;
  if (wrapWeightsX)
    numElementsX++;
  result += NMP::Memory::Format(sizeof(float) * numElementsX, NMP_NATURAL_TYPE_ALIGNMENT);

  // Weights Y
  uint16_t numElementsY = numWeightsY;
  if (wrapWeightsY)
    numElementsY++;
  result += NMP::Memory::Format(sizeof(float) * numElementsY, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBlendNxMDef* AttribDataBlendNxMDef::init(
  NMP::Memory::Resource&  resource,
  uint16_t                numWeightsX,
  uint16_t                numWeightsY,
  bool                    wrapWeightsX,
  bool                    wrapWeightsY,
  uint16_t                refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBlendNxMDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBlendNxMDef* result = (AttribDataBlendNxMDef*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_BLEND_NXM_DEF);
  result->setRefCount(refCount);

  result->m_numWeightsX = numWeightsX;
  result->m_numWeightsY = numWeightsY;
  result->m_wrapWeightsX = wrapWeightsX;
  result->m_wrapWeightsY = wrapWeightsY;

  // Weights X
  uint16_t numElementsX = numWeightsX;
  if (wrapWeightsX)
    numElementsX++;
  NMP::Memory::Format memReqsDataX(sizeof(float) * numElementsX, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_weightsX = (float*) resource.alignAndIncrement(memReqsDataX);
  for (uint32_t i = 0; i < numElementsX; ++i)
  {
    result->m_weightsX[i] = 0.0f;
  }

  // Weights Y
  uint16_t numElementsY = numWeightsY;
  if (wrapWeightsY)
    numElementsY++;
  NMP::Memory::Format memReqsDataY(sizeof(float) * numElementsY, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_weightsY = (float*) resource.alignAndIncrement(memReqsDataY);
  for (uint32_t i = 0; i < numElementsY; ++i)
  {
    result->m_weightsY[i] = 0.0f;
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBlendNxMDef::create(
  NMP::MemoryAllocator*   allocator,
  uint16_t                numWeightsX,
  uint16_t                numWeightsY,
  bool                    wrapWeightsX,
  bool                    wrapWeightsY,
  uint16_t                refCount)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(numWeightsX, numWeightsY, wrapWeightsX, wrapWeightsY);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);

  AttribDataHandle result;
  result.m_attribData = init(resource, numWeightsX, numWeightsY, wrapWeightsX, wrapWeightsY, refCount);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataBlendNxMDef::locate(AttribData* target)
{
  AttribDataBlendNxMDef* result = (AttribDataBlendNxMDef*) target;

  // Header
  AttribData::locate(target);
  NMP::endianSwap(result->m_numWeightsX);
  NMP::endianSwap(result->m_numWeightsY);
  NMP::endianSwap(result->m_wrapWeightsX);
  NMP::endianSwap(result->m_wrapWeightsY);

  // Weights X
  REFIX_SWAP_PTR_RELATIVE(float, result->m_weightsX, result);
  NMP::endianSwapArray(result->m_weightsX, result->m_numWeightsX);

  // Weights Y
  REFIX_SWAP_PTR_RELATIVE(float, result->m_weightsY, result);
  NMP::endianSwapArray(result->m_weightsY, result->m_numWeightsY);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBlendNxMDef::dislocate(AttribData* target)
{
  AttribDataBlendNxMDef* result = (AttribDataBlendNxMDef*) target;

  // Weights Y
  NMP::endianSwapArray(result->m_weightsY, result->m_numWeightsY);
  UNFIX_SWAP_PTR_RELATIVE(float, result->m_weightsY, result);

  // Weights X
  NMP::endianSwapArray(result->m_weightsX, result->m_numWeightsX);
  UNFIX_SWAP_PTR_RELATIVE(float, result->m_weightsX, result);

  // Header
  NMP::endianSwap(result->m_wrapWeightsY);
  NMP::endianSwap(result->m_wrapWeightsX);
  NMP::endianSwap(result->m_numWeightsY);
  NMP::endianSwap(result->m_numWeightsX);
  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBlendNxMDef::relocate(AttribData* target, void* location)
{
  AttribDataBlendNxMDef* result = (AttribDataBlendNxMDef*) target;

  // Header
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = NMP::Memory::increment(ptr, sizeof(AttribDataBlendNxMDef));

  // Weights X
  NMP::Memory::Format memReqsDataX(sizeof(float) * result->m_numWeightsX, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_weightsX = (float*) NMP::Memory::alignAndIncrement(ptr, memReqsDataX);

  // Weights Y
  NMP::Memory::Format memReqsDataY(sizeof(float) * result->m_numWeightsY, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_weightsY = (float*) NMP::Memory::alignAndIncrement(ptr, memReqsDataY);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataJointLimits functions
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataJointLimits::getMemoryRequirements(
  uint32_t numRigJoints,
  uint32_t numLimitedJoints)
{
  // Shell
  NMP::Memory::Format result(sizeof(AttribDataJointLimits), MR_ATTRIB_DATA_ALIGNMENT);

  // Joint limit pointer array
  result += NMP::Memory::Format(sizeof(NMRU::JointLimits::Params*) * numRigJoints, NMP_NATURAL_TYPE_ALIGNMENT);

  // Joint limit data array
  result += NMP::Memory::Format(sizeof(NMRU::JointLimits::Params) * numLimitedJoints, NMP_VECTOR_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataJointLimits* AttribDataJointLimits::init(
  NMP::Memory::Resource& resource,
  uint32_t               numRigJoints,
  uint32_t               numLimitedJoints,
  uint16_t               refCount)
{
  NMP_ASSERT(resource.format.size >= getMemoryRequirements(numRigJoints, numLimitedJoints).size);

  // Initialise shell
  NMP::Memory::Format format(sizeof(AttribDataJointLimits), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataJointLimits* result = (AttribDataJointLimits*)resource.alignAndIncrement(format);

  // Shell data
  result->setType(ATTRIB_TYPE_JOINT_LIMITS);
  result->setRefCount(refCount);

  result->m_numJointLimits = numLimitedJoints;
  result->m_rigSize = numRigJoints;

  // Array of joint limits, initialised to NULL
  format = NMP::Memory::Format(sizeof(NMRU::JointLimits::Params*) * numRigJoints, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(format);
  result->m_jointLimits = (NMRU::JointLimits::Params**)resource.ptr;
  resource.increment(format);
  for (uint32_t j = 0; j < numRigJoints; ++j)
  {
    result->m_jointLimits[j] = 0;
  }

  // Array of joint limit data
  format = NMP::Memory::Format(sizeof(NMRU::JointLimits::Params) * numLimitedJoints, NMP_VECTOR_ALIGNMENT);
  resource.align(format);
  result->m_jointLimitData = (NMRU::JointLimits::Params*)resource.ptr;
  resource.increment(format);
  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  format = NMP::Memory::Format(0, MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(format);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataJointLimits::locate(AttribData* target)
{
  AttribDataJointLimits* result = (AttribDataJointLimits*) target;
  AttribData::locate(target);

  // Shell data
  NMP::endianSwap(result->m_rigSize);
  NMP::endianSwap(result->m_numJointLimits);
  NMP::endianSwap(result->m_jointLimits);
  NMP::endianSwap(result->m_jointLimitData);

  // Pointers
  REFIX_PTR_RELATIVE(NMRU::JointLimits::Params*, result->m_jointLimits, result);
  REFIX_PTR_RELATIVE(NMRU::JointLimits::Params, result->m_jointLimitData, result);

  // Pointed-to data
  for (uint32_t j = 0; j < result->m_rigSize; ++j)
  {
    NMP::endianSwap(result->m_jointLimits[j]);
    // If was NULL, keep as NULL, since that means there is no joint limit
    if (result->m_jointLimits[j] != 0)
    {
      REFIX_PTR_RELATIVE(NMRU::JointLimits::Params, result->m_jointLimits[j], result);
    }
  }
  for (uint32_t l = 0; l < result->m_numJointLimits; ++l)
  {
    result->m_jointLimitData[l].endianSwap();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataJointLimits::dislocate(AttribData* target)
{
  AttribDataJointLimits* result = (AttribDataJointLimits*) target;

  // Pointed-to data
  for (uint32_t l = 0; l < result->m_numJointLimits; ++l)
  {
    result->m_jointLimitData[l].endianSwap();
  }
  for (uint32_t j = 0; j < result->m_rigSize; ++j)
  {
    // If was NULL, keep as NULL, since that means there is no joint limit
    if (result->m_jointLimits[j] != 0)
    {
      UNFIX_PTR_RELATIVE(NMRU::JointLimits::Params, result->m_jointLimits[j], result);
    }
    NMP::endianSwap(result->m_jointLimits[j]);
  }

  // Pointers
  UNFIX_PTR_RELATIVE(NMRU::JointLimits::Params, result->m_jointLimitData, result);
  UNFIX_PTR_RELATIVE(NMRU::JointLimits::Params*, result->m_jointLimits, result);

  // Shell data
  NMP::endianSwap(result->m_jointLimitData);
  NMP::endianSwap(result->m_jointLimits);
  NMP::endianSwap(result->m_numJointLimits);
  NMP::endianSwap(result->m_rigSize);

  AttribData::dislocate(target);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataJointLimits::relocate(AttribData* target, void* location)
{
  AttribDataJointLimits* result = (AttribDataJointLimits*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));

  // Shell
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataJointLimits));

  // Joint limit pointer array
  ptr = (void*) NMP::Memory::align(ptr, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_jointLimits = (NMRU::JointLimits::Params**) ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(NMRU::JointLimits::Params*) * result->m_rigSize));

  // Joint limit data array
  ptr = (void*) NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT);
  result->m_jointLimitData = (NMRU::JointLimits::Params*) ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(NMRU::JointLimits::Params) * result->m_numJointLimits));
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataEmitMessageOnCPValueDef functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataEmitMessageOnCPValueDef::getMemoryRequirements(uint32_t numComparisons)
{
  NMP::Memory::Format result(sizeof(AttribDataEmitMessageOnCPValueDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of m_comparisonValues.
  result += NMP::Memory::Format(sizeof(float) * numComparisons, NMP_NATURAL_TYPE_ALIGNMENT);

  // Add space for the array of m_comparisonTypes.
  result += NMP::Memory::Format(sizeof(uint32_t) * numComparisons, NMP_NATURAL_TYPE_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataEmitMessageOnCPValueDef::create(
  NMP::MemoryAllocator* allocator,
  uint32_t              numComparisons,
  uint16_t              refCount)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(numComparisons);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);

  AttribDataHandle result;
  result.m_attribData = init(resource, numComparisons, refCount);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataEmitMessageOnCPValueDef* AttribDataEmitMessageOnCPValueDef::init(
  NMP::Memory::Resource& resource,
  uint32_t               numComparisons,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements(numComparisons).alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataEmitMessageOnCPValueDef* result = (AttribDataEmitMessageOnCPValueDef*) resource.ptr;
  resource.increment(sizeof(AttribDataEmitMessageOnCPValueDef));

  result->setType(ATTRIB_TYPE_EMIT_MESSAGE_ON_CP_VALUE);
  result->setRefCount(refCount);

  result->m_numComparisons = numComparisons;

  // Array of comparison values.
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_comparisonValues = (float*) resource.ptr;
  resource.increment(sizeof(float) * numComparisons);
  for (uint32_t i = 0; i < numComparisons; ++i)
  {
    result->m_comparisonValues[i] = 0.0f;
  }

  // Array of comparison types.
  resource.align(NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_comparisonTypes = (uint32_t*) resource.ptr;
  resource.increment(sizeof(uint32_t) * numComparisons);
  for (uint32_t i = 0; i < numComparisons; ++i)
  {
    result->m_comparisonTypes[i] = 0;
  }

  // Make sure resource pointer has been moved on by the size returned by getMemoryRequirements.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataEmitMessageOnCPValueDef::locate(AttribData* target)
{
  AttribDataEmitMessageOnCPValueDef* result = (AttribDataEmitMessageOnCPValueDef*) target;

  AttribData::locate(target);
  
  NMP::endianSwap(result->m_numComparisons);

  // Array of comparison values.
  NMP::endianSwap(result->m_comparisonValues);
  REFIX_PTR_RELATIVE(float, result->m_comparisonValues, result);
  NMP::endianSwapArray(result->m_comparisonValues, result->m_numComparisons, sizeof(float));

  // Array of comparison types.
  NMP::endianSwap(result->m_comparisonTypes);
  REFIX_PTR_RELATIVE(uint32_t, result->m_comparisonTypes, result);
  NMP::endianSwapArray(result->m_comparisonTypes, result->m_numComparisons, sizeof(uint32_t));
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataEmitMessageOnCPValueDef::dislocate(AttribData* target)
{
  AttribDataEmitMessageOnCPValueDef* result = (AttribDataEmitMessageOnCPValueDef*) target;

  // Array of comparison values.
  NMP::endianSwapArray(result->m_comparisonValues, result->m_numComparisons, sizeof(float));
  UNFIX_PTR_RELATIVE(float, result->m_comparisonValues, result);
  NMP::endianSwap(result->m_comparisonValues);

  // Array of comparison types.
  NMP::endianSwapArray(result->m_comparisonTypes, result->m_numComparisons, sizeof(uint32_t));
  UNFIX_PTR_RELATIVE(uint32_t, result->m_comparisonTypes, result);
  NMP::endianSwap(result->m_comparisonTypes);

  NMP::endianSwap(result->m_numComparisons);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataEmitMessageOnCPValueDef::relocate(AttribData* target)
{
  AttribDataEmitMessageOnCPValueDef* result = (AttribDataEmitMessageOnCPValueDef*) target;
  void* ptr = target;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataEmitMessageOnCPValueDef));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_comparisonValues = (float*)ptr;
  ptr = (void*)(((size_t)ptr) + (sizeof(float) * result->m_numComparisons));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_comparisonTypes = (uint32_t*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBlendFlags
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBlendFlags::getMemoryRequirements()
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataBlendFlags), MR_ATTRIB_DATA_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBlendFlags* AttribDataBlendFlags::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBlendFlags), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBlendFlags* result = (AttribDataBlendFlags*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_BLEND_FLAGS);
  result->setRefCount(refCount);

  // Blend flags
  result->m_alwaysBlendTrajectoryAndTransforms = true;
  result->m_alwaysCombineSampledEvents = true;

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBlendFlags::create(
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
#ifndef NM_HOST_CELL_SPU
void AttribDataBlendFlags::locate(AttribData* target)
{
  AttribDataBlendFlags* result = (AttribDataBlendFlags*) target;

  // Header
  AttribData::locate(target);
  NMP::endianSwap(result->m_alwaysBlendTrajectoryAndTransforms);
  NMP::endianSwap(result->m_alwaysCombineSampledEvents);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBlendFlags::dislocate(AttribData* target)
{
  AttribDataBlendFlags* result = (AttribDataBlendFlags*) target;

  // Header  
  NMP::endianSwap(result->m_alwaysBlendTrajectoryAndTransforms);
  NMP::endianSwap(result->m_alwaysCombineSampledEvents);
  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// AttribDataBlendWeights
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBlendWeights::getMemoryRequirements(uint16_t maxNumWeights)
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataBlendWeights), MR_ATTRIB_DATA_ALIGNMENT);

  // Blend weights
  if (maxNumWeights > 0)
  {
    NMP::Memory::Format memReqsWeights(sizeof(float) * maxNumWeights, NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsWeights; // transforms
    result += memReqsWeights; // events
    result += memReqsWeights; // sampled events
  }

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataBlendWeights* AttribDataBlendWeights::init(
  NMP::Memory::Resource& resource,
  uint16_t               maxNumWeights,
  uint16_t               refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataBlendWeights), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataBlendWeights* result = (AttribDataBlendWeights*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_BLEND_WEIGHTS);
  result->setRefCount(refCount);

  result->m_maxNumWeights = maxNumWeights;
  result->m_trajectoryAndTransformsNumWeights = 0;
  result->m_eventsNumWeights = 0;
  result->m_sampledEventsNumWeights = 0;

  // Blend weights
  if (maxNumWeights > 0)
  {
    NMP::Memory::Format memReqsWeights(sizeof(float) * maxNumWeights, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_trajectoryAndTransformsWeights = (float*)resource.alignAndIncrement(memReqsWeights);
    result->m_eventsWeights = (float*)resource.alignAndIncrement(memReqsWeights);
    result->m_sampledEventsWeights = static_cast<float*>( resource.alignAndIncrement(memReqsWeights) );
  }
  else
  {
    result->m_trajectoryAndTransformsWeights = NULL;
    result->m_eventsWeights = NULL;
    result->m_sampledEventsWeights = NULL;
  }

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBlendWeights::create(
  NMP::Memory::Resource& resource,
  AttribDataCreateDesc*  desc)
{
  AttribDataHandle result;

  NMP_ASSERT(desc);
  AttribDataBlendWeightsCreateDesc* targetDesc = (AttribDataBlendWeightsCreateDesc*)desc;

  result.m_format = getMemoryRequirements(targetDesc->m_maxNumWeights);
  NMP_ASSERT(resource.format.size >= result.m_format.size);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    targetDesc->m_maxNumWeights,
    targetDesc->m_refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataBlendWeights::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              maxNumWeights,
  uint16_t              refCount)
{
  NMP::Memory::Format memReqs = getMemoryRequirements(maxNumWeights);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);

  AttribDataHandle result;
  result.m_attribData = init(resource, maxNumWeights, refCount);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataBlendWeights::outputMemReqs(Network* NMP_UNUSED(Network))
{
  // As output it always is used with a single weight.
  return getMemoryRequirements(1);
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataBlendWeights::locate(AttribData* target)
{
  AttribDataBlendWeights* result = (AttribDataBlendWeights*) target;

  // Header
  AttribData::locate(target);

  NMP::endianSwap(result->m_maxNumWeights);
  NMP::endianSwap(result->m_trajectoryAndTransformsNumWeights);
  NMP::endianSwap(result->m_eventsNumWeights);
  NMP::endianSwap(result->m_sampledEventsNumWeights);

  // Blend weights
  if (result->m_maxNumWeights > 0)
  {
    REFIX_SWAP_PTR_RELATIVE(float, result->m_trajectoryAndTransformsWeights, result);
    NMP::endianSwapArray(result->m_trajectoryAndTransformsWeights, result->m_maxNumWeights);

    REFIX_SWAP_PTR_RELATIVE(float, result->m_eventsWeights, result);
    NMP::endianSwapArray(result->m_eventsWeights, result->m_maxNumWeights);

    REFIX_SWAP_PTR_RELATIVE(float, result->m_sampledEventsWeights, result);
    NMP::endianSwapArray(result->m_sampledEventsWeights, result->m_maxNumWeights);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBlendWeights::dislocate(AttribData* target)
{
  AttribDataBlendWeights* result = (AttribDataBlendWeights*) target;

  // Blend weights
  if (result->m_maxNumWeights > 0)
  {
    NMP::endianSwapArray(result->m_trajectoryAndTransformsWeights, result->m_maxNumWeights);
    UNFIX_SWAP_PTR_RELATIVE(float, result->m_trajectoryAndTransformsWeights, result);

    NMP::endianSwapArray(result->m_eventsWeights, result->m_maxNumWeights);
    UNFIX_SWAP_PTR_RELATIVE(float, result->m_eventsWeights, result);

    NMP::endianSwapArray(result->m_sampledEventsWeights, result->m_maxNumWeights);
    UNFIX_SWAP_PTR_RELATIVE(float, result->m_sampledEventsWeights, result);
  }

  // Header
  NMP::endianSwap(result->m_maxNumWeights);
  NMP::endianSwap(result->m_trajectoryAndTransformsNumWeights);
  NMP::endianSwap(result->m_eventsNumWeights);
  NMP::endianSwap(result->m_sampledEventsNumWeights);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataBlendWeights::relocate(AttribData* target, void* location)
{
  AttribDataBlendWeights* result = (AttribDataBlendWeights*) target;

  // Header
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = NMP::Memory::increment(ptr, sizeof(AttribDataBlendWeights));

  // Blend weights
  if (result->m_maxNumWeights > 0)
  {
    NMP::Memory::Format memReqsWeights(sizeof(float) * result->m_maxNumWeights, NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_trajectoryAndTransformsWeights = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsWeights);
    result->m_eventsWeights = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsWeights);
    result->m_sampledEventsWeights = static_cast<float*>(NMP::Memory::alignAndIncrement(ptr, memReqsWeights));
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataFeatherBlend2ChannelAlphas functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataFeatherBlend2ChannelAlphas::getMemoryRequirements(uint16_t numChannelAlphas)
{
  NMP::Memory::Format result(sizeof(AttribDataFeatherBlend2ChannelAlphas), MR_ATTRIB_DATA_ALIGNMENT);

  // Add space for the array of channel alphas.
  uint16_t numChannelAlphas4 = NMP::nmAlignedValue4(numChannelAlphas);
  result += NMP::Memory::Format(sizeof(float) * numChannelAlphas4, NMP_VECTOR_ALIGNMENT);

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataFeatherBlend2ChannelAlphas* AttribDataFeatherBlend2ChannelAlphas::init(NMP::Memory::Resource& resource, uint16_t numChannelAlphas, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataFeatherBlend2ChannelAlphas), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataFeatherBlend2ChannelAlphas* result = (AttribDataFeatherBlend2ChannelAlphas*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_FEATHER_BLEND2_CHANNEL_ALPHAS);
  result->setRefCount(refCount);
  result->m_trajectoryBoneIndex = 0;

  // Channel alpha weights
  uint16_t numChannelAlphas4 = NMP::nmAlignedValue4(numChannelAlphas);
  NMP::Memory::Format memReqsData(sizeof(float) * numChannelAlphas4, NMP_VECTOR_ALIGNMENT);
  result->m_channelAlphas = (float*) resource.alignAndIncrement(memReqsData);

  result->m_numChannelAlphas = numChannelAlphas;
  for (uint32_t i = 0; i < numChannelAlphas4; ++i)
  {
    result->m_channelAlphas[i] = 0.0f;
  }

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataFeatherBlend2ChannelAlphas::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* desc)
{
  AttribDataHandle result;

  NMP_ASSERT(desc);
  const AttribDataArrayCreateDesc* targetDesc = (AttribDataArrayCreateDesc*)desc;

  result.m_format = getMemoryRequirements((uint16_t)targetDesc->m_numEntries);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, (uint16_t)targetDesc->m_numEntries, targetDesc->m_refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataFeatherBlend2ChannelAlphas::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              numChannelAlphas,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements(numChannelAlphas);
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, result.m_format);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, numChannelAlphas, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataFeatherBlend2ChannelAlphas::locate(AttribData* target)
{
  AttribDataFeatherBlend2ChannelAlphas* result = (AttribDataFeatherBlend2ChannelAlphas*) target;

  AttribData::locate(target);
  NMP::endianSwap(result->m_trajectoryBoneIndex);
  NMP::endianSwap(result->m_numChannelAlphas);

  REFIX_SWAP_PTR_RELATIVE(float, result->m_channelAlphas, result);
  NMP::endianSwapArray(result->m_channelAlphas, result->m_numChannelAlphas);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataFeatherBlend2ChannelAlphas::dislocate(AttribData* target)
{
  AttribDataFeatherBlend2ChannelAlphas* result = (AttribDataFeatherBlend2ChannelAlphas*) target;

  NMP::endianSwapArray(result->m_channelAlphas, result->m_numChannelAlphas);
  UNFIX_SWAP_PTR_RELATIVE(float, result->m_channelAlphas, result);

  NMP::endianSwap(result->m_numChannelAlphas);
  NMP::endianSwap(result->m_trajectoryBoneIndex);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataFeatherBlend2ChannelAlphas::relocate(AttribData* target, void* location)
{
  AttribDataFeatherBlend2ChannelAlphas* result = (AttribDataFeatherBlend2ChannelAlphas*) target;
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = (void*)(((size_t)ptr) + sizeof(AttribDataFeatherBlend2ChannelAlphas));
  ptr = (void*)(NMP::Memory::align(ptr, NMP_VECTOR_ALIGNMENT));
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, NMP_VECTOR_ALIGNMENT));
  result->m_channelAlphas = (float*)ptr;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataScatterBlendAnalysisDef
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataScatterBlendAnalysisDef::getMemoryRequirements(
  uint16_t maxNumWeights,
  uint16_t maxNumNodeIDs)
{
  // Header
  NMP::Memory::Format result(sizeof(AttribDataScatterBlendAnalysisDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Weights
  NMP::Memory::Format memReqsWeights(sizeof(float) * maxNumWeights, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsWeights;

  // Node IDs
  NMP::Memory::Format memReqsNodeIDs(sizeof(NodeID) * maxNumNodeIDs, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsNodeIDs;

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataScatterBlendAnalysisDef* AttribDataScatterBlendAnalysisDef::init(
  NMP::Memory::Resource& resource,
  uint16_t               maxNumWeights,
  uint16_t               maxNumNodeIDs,
  uint16_t               refCount)
{
  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataScatterBlendAnalysisDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataScatterBlendAnalysisDef* result = (AttribDataScatterBlendAnalysisDef*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_SCATTER_BLEND_ANALYSIS_DEF);
  result->setRefCount(refCount);

  result->m_maxNumWeights = maxNumWeights;
  result->m_numWeights = 0;

  result->m_maxNumNodeIDs = maxNumNodeIDs;
  result->m_numNodeIDs = 0;

  // Blend weights  
  NMP::Memory::Format memReqsWeights(sizeof(float) * maxNumWeights, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_weights = (float*)resource.alignAndIncrement(memReqsWeights);

  // Node IDs
  NMP::Memory::Format memReqsNodeIDs(sizeof(NodeID) * maxNumNodeIDs, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_nodeIDs = (NodeID*)resource.alignAndIncrement(memReqsNodeIDs);

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataScatterBlendAnalysisDef::locate(AttribData* target)
{
  AttribDataScatterBlendAnalysisDef* result = (AttribDataScatterBlendAnalysisDef*) target;

  // Header
  AttribData::locate(target);

  NMP::endianSwap(result->m_maxNumWeights);
  NMP::endianSwap(result->m_maxNumNodeIDs);
  NMP::endianSwap(result->m_numWeights);
  NMP::endianSwap(result->m_numNodeIDs);

  // Weights  
  REFIX_SWAP_PTR_RELATIVE(float, result->m_weights, result);
  NMP::endianSwapArray(result->m_weights, result->m_maxNumWeights);

  // Node IDs
  REFIX_SWAP_PTR_RELATIVE(NodeID, result->m_nodeIDs, result);
  NMP::endianSwapArray(result->m_nodeIDs, result->m_maxNumNodeIDs);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScatterBlendAnalysisDef::dislocate(AttribData* target)
{
  AttribDataScatterBlendAnalysisDef* result = (AttribDataScatterBlendAnalysisDef*) target;

  // Node IDs
  NMP::endianSwapArray(result->m_nodeIDs, result->m_maxNumNodeIDs);
  UNFIX_SWAP_PTR_RELATIVE(NodeID, result->m_nodeIDs, result);

  // Weights    
  NMP::endianSwapArray(result->m_weights, result->m_maxNumWeights);
  UNFIX_SWAP_PTR_RELATIVE(float, result->m_weights, result);

  // Header  
  NMP::endianSwap(result->m_maxNumWeights);
  NMP::endianSwap(result->m_numWeights);

  NMP::endianSwap(result->m_maxNumNodeIDs);
  NMP::endianSwap(result->m_numNodeIDs);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScatterBlendAnalysisDef::relocate(AttribData* target, void* location)
{
  AttribDataScatterBlendAnalysisDef* result = (AttribDataScatterBlendAnalysisDef*) target;

  // Header
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = NMP::Memory::increment(ptr, sizeof(AttribDataScatterBlendAnalysisDef));

  // Blend weights
  NMP::Memory::Format memReqsWeights(sizeof(float) * result->m_maxNumWeights, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_weights = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsWeights);

  // Node IDs
  NMP::Memory::Format memReqsNodeIDs(sizeof(NodeID) * result->m_maxNumNodeIDs, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_nodeIDs = (NodeID*)NMP::Memory::alignAndIncrement(ptr, memReqsNodeIDs);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataScatterBlend1DDef
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataScatterBlend1DDef::getMemoryRequirements(
  uint32_t NMP_USED_FOR_ASSERTS(numSubDivisions),
  uint32_t numSubSamplesPerLine,
  uint32_t numVertices,
  uint32_t numLines,
  uint32_t numSubSamples)
{
  NMP_ASSERT(numSubDivisions < 3);
  NMP_ASSERT(numSubSamplesPerLine >= 2);
  NMP_ASSERT(numVertices >= 2);
  uint32_t numLines4 = NMP::nmAlignedValue4(numLines);

  // Header
  NMP::Memory::Format result(sizeof(AttribDataScatterBlend1DDef), MR_ATTRIB_DATA_ALIGNMENT);

  // Vertex source indices
  NMP::Memory::Format memReqsVS(sizeof(uint16_t) * numVertices, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsVS;

  // Samples data
  uint32_t numSamples = numVertices + numSubSamples;
  NMP::Memory::Format memReqsSamples(sizeof(float) * numSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsSamples;

  // Line sample indices
  NMP::Memory::Format memReqsLineSampleIndices(
    sizeof(uint16_t) * (numSubSamplesPerLine * numLines4),
    NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsLineSampleIndices;

  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataScatterBlend1DDef* AttribDataScatterBlend1DDef::init(
  NMP::Memory::Resource& resource,
  uint32_t               numSubDivisions,
  uint32_t               numSubSamplesPerLine,
  uint32_t               numVertices,
  uint32_t               numLines,
  uint32_t               numSubSamples)
{
  NMP_ASSERT(numSubDivisions < 3);
  NMP_ASSERT(numSubSamplesPerLine >= 2);
  NMP_ASSERT(numVertices >= 2);
  uint32_t numLines4 = NMP::nmAlignedValue4(numLines);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataScatterBlend1DDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataScatterBlend1DDef* result = (AttribDataScatterBlend1DDef*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_SCATTER_BLEND_1D_DEF);
  result->setRefCount(MR::IS_DEF_ATTRIB_DATA);

  // Information
  result->m_numSubDivisions = (uint16_t)numSubDivisions;
  result->m_numSubSamplesPerLine = (uint16_t)numSubSamplesPerLine;
  result->m_numVertices = (uint16_t)numVertices;
  result->m_numLines = (uint16_t)numLines;
  result->m_numSubSamples = (uint16_t)numSubSamples;

  // Vertex source indices
  NMP::Memory::Format memReqsVS(sizeof(uint16_t) * numVertices, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_vertexSourceIndices = (uint16_t*)resource.alignAndIncrement(memReqsVS);

  // Samples data
  uint32_t numSamples = numVertices + numSubSamples;
  NMP::Memory::Format memReqsSamples(sizeof(float) * numSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_samples = (float*)resource.alignAndIncrement(memReqsSamples);

  // Line sample indices
  NMP::Memory::Format memReqsLineSampleIndices(
    sizeof(uint16_t) * (numSubSamplesPerLine * numLines4),
    NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_lineSampleIndices = (uint16_t*)resource.alignAndIncrement(memReqsLineSampleIndices);

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataScatterBlend1DDef::locate(AttribData* target)
{
  AttribDataScatterBlend1DDef* result = (AttribDataScatterBlend1DDef*) target;
  AttribData::locate(target);

  // Information
  NMP::endianSwap(result->m_numSubDivisions);
  NMP::endianSwap(result->m_numSubSamplesPerLine);
  NMP::endianSwap(result->m_numVertices);
  NMP::endianSwap(result->m_numLines);
  NMP::endianSwap(result->m_numSubSamples);
  NMP_ASSERT(result->m_numVertices >= 2);
  NMP_ASSERT(result->m_numLines > 0);
  uint16_t numLines4 = NMP::nmAlignedValue4(result->m_numLines);

  // Unit scaling
  NMP::endianSwap(result->m_unitScale);

  // Vertex source indices
  REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_vertexSourceIndices, result);
  NMP::endianSwapArray(result->m_vertexSourceIndices, result->m_numVertices);

  // Samples data
  uint32_t numSamples = result->m_numVertices + result->m_numSubSamples;
  REFIX_SWAP_PTR_RELATIVE(float, result->m_samples, result);
  NMP::endianSwapArray(result->m_samples, numSamples);

  // Line sample indices
  REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_lineSampleIndices, result);
  NMP::endianSwapArray(result->m_lineSampleIndices, result->m_numSubSamplesPerLine * numLines4);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScatterBlend1DDef::dislocate(AttribData* target)
{
  AttribDataScatterBlend1DDef* result = (AttribDataScatterBlend1DDef*) target;

  NMP_ASSERT(result->m_numVertices >= 2);
  NMP_ASSERT(result->m_numLines > 0);
  uint16_t numLines4 = NMP::nmAlignedValue4(result->m_numLines);
  uint16_t numSamples = result->m_numVertices + result->m_numSubSamples;

  // Line sample indices
  NMP::endianSwapArray(result->m_lineSampleIndices, result->m_numSubSamplesPerLine * numLines4);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_lineSampleIndices, result);

  // Samples data
  NMP::endianSwapArray(result->m_samples, numSamples);
  UNFIX_SWAP_PTR_RELATIVE(float, result->m_samples, result);

  // Vertex source indices  
  NMP::endianSwapArray(result->m_vertexSourceIndices, result->m_numVertices);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_vertexSourceIndices, result);

  // Unit scaling
  NMP::endianSwap(result->m_unitScale);

  // Information
  NMP::endianSwap(result->m_numSubDivisions);
  NMP::endianSwap(result->m_numSubSamplesPerLine);
  NMP::endianSwap(result->m_numVertices);
  NMP::endianSwap(result->m_numLines);
  NMP::endianSwap(result->m_numSubSamples);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScatterBlend1DDef::relocate(AttribData* target, void* location)
{
  AttribDataScatterBlend1DDef* result = (AttribDataScatterBlend1DDef*) target;

  // Header
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = NMP::Memory::increment(ptr, sizeof(AttribDataScatterBlend1DDef));

  NMP_ASSERT(result->m_numVertices >= 2);
  NMP_ASSERT(result->m_numLines > 0);
  uint16_t numLines4 = NMP::nmAlignedValue4(result->m_numLines);
  uint16_t numSamples = result->m_numVertices + result->m_numSubSamples;

  // Vertex source indices
  NMP::Memory::Format memReqsVS(sizeof(uint16_t) * result->m_numVertices, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_vertexSourceIndices = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsVS);

  // Samples data
  NMP::Memory::Format memReqsSamples(sizeof(float) * numSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_samples = (float*)NMP::Memory::alignAndIncrement(ptr, memReqsSamples);

  // Line sample indices
  NMP::Memory::Format memReqsLineSampleIndices(
    sizeof(uint16_t) * (result->m_numSubSamplesPerLine * numLines4),
    NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_lineSampleIndices = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsLineSampleIndices);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataScatterBlend2DDef
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataScatterBlend2DDef::getMemoryRequirements(
  uint32_t numTriangleSubDivisions,
  uint32_t numSubSamplesPerTriangle,
  uint32_t numVertices,
  uint32_t numTriangles,
  uint32_t numSubSamples,
  uint32_t numExteriorEdgeFacets,
  uint32_t bspNumBranchNodes,
  uint32_t bspNumLeafNodes,
  uint32_t bspNumLeafNodeEntries)
{
  NMP_ASSERT(numTriangleSubDivisions < 3);
  NMP_ASSERT(numSubSamplesPerTriangle >= 3);
  NMP_ASSERT(numVertices >= 3);
  NMP_ASSERT(numTriangles >= 1);
  NMP_ASSERT(numExteriorEdgeFacets >= 3);

  // Header
  NMP::Memory::Format result(sizeof(AttribDataScatterBlend2DDef), MR_ATTRIB_DATA_ALIGNMENT);

  //------------------------
  // Samples data
  uint32_t numSamples = numVertices + numSubSamples;
  NMP::Memory::Format memReqsSamples(sizeof(uint16_t) * numSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result += (memReqsSamples * 2);

  // Triangle sample indices
  NMP::Memory::Format memReqsTriangleSampleIndices(
    sizeof(uint16_t) * numSubSamplesPerTriangle * numTriangles,
    NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsTriangleSampleIndices;

  // Exterior triangle facets
  uint32_t numExteriorEdgeFacets4 = NMP::nmAlignedValue4(numExteriorEdgeFacets);
  NMP::Memory::Format memReqsExtTriangles(sizeof(uint16_t) * numExteriorEdgeFacets4, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsExtTriangles;
  NMP::Memory::Format memReqsExtFacets(sizeof(uint8_t) * numExteriorEdgeFacets4, NMP_NATURAL_TYPE_ALIGNMENT);
  result += memReqsExtFacets;

  //------------------------
  // BSP Tree
  NMP::Memory::Format memReqsBSP = getBSPMemoryRequirements(
    numTriangleSubDivisions,
    numTriangles,
    bspNumBranchNodes,
    bspNumLeafNodes,
    bspNumLeafNodeEntries);
  result += memReqsBSP;

  //------------------------
  // Make sure size is a multiple of the alignment requirement.
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataScatterBlend2DDef::getBSPMemoryRequirements(
  uint32_t numTriangleSubDivisions,
  uint32_t numTriangles,
  uint32_t bspNumBranchNodes,
  uint32_t bspNumLeafNodes,
  uint32_t bspNumLeafNodeEntries)
{
  NMP::Memory::Format result(0, NMP_NATURAL_TYPE_ALIGNMENT);

  if (bspNumLeafNodes > 1)
  {
    // Note: We don't store any data if there is a single leaf node since we should just process
    // all triangles at runtime.
    NMP_ASSERT(bspNumLeafNodeEntries > 0);
    NMP::Memory::Format memReqsBspBranchNodes(sizeof(uint16_t) * bspNumBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Format memReqsBspLeafNodes(sizeof(uint16_t) * bspNumLeafNodes, NMP_NATURAL_TYPE_ALIGNMENT);

    // left, right
    result += (memReqsBspBranchNodes * 2);
    // Hyper-planes
    result += (memReqsBspBranchNodes * 2);
    // Leaf node entry offsets (+1 element required for forward difference)
    NMP::Memory::Format memReqsBspLeafEntryOffsets(sizeof(uint16_t) * (bspNumLeafNodes + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    result += memReqsBspLeafEntryOffsets;

    // Leaf node triangle entries
    if (numTriangles <= 256)
    {
      // 8-bits
      NMP::Memory::Format memReqsBspLeafTets(sizeof(uint8_t) * bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result += memReqsBspLeafTets;
    }
    else
    {
      // 16-bits
      NMP::Memory::Format memReqsBspLeafTets(sizeof(uint16_t) * bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result += memReqsBspLeafTets;
    }

    // Leaf node sub-triangle bit-masks
    if (numTriangleSubDivisions == 1)
    {
      // 8-bits (4 sub-triangles)
      NMP::Memory::Format memReqsBspSubTetFlags(bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result += memReqsBspSubTetFlags;
    }
    else if (numTriangleSubDivisions == 2)
    {
      // 16-bits (16 sub-triangles)
      NMP::Memory::Format memReqsBspSubTetFlags(2 * bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result += memReqsBspSubTetFlags;
    }
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataScatterBlend2DDef* AttribDataScatterBlend2DDef::init(
  NMP::Memory::Resource& resource,
  uint32_t               numTriangleSubDivisions,
  uint32_t               numSubSamplesPerTriangle,
  uint32_t               numVertices,
  uint32_t               numTriangles,
  uint32_t               numSubSamples,
  uint32_t               numExteriorEdgeFacets,
  uint32_t               bspNumBranchNodes,
  uint32_t               bspNumLeafNodes,
  uint32_t               bspNumLeafNodeEntries)
{
  NMP_ASSERT(numTriangleSubDivisions < 3);
  NMP_ASSERT(numSubSamplesPerTriangle >= 3);
  NMP_ASSERT(numVertices >= 3);
  NMP_ASSERT(numTriangles >= 1);
  NMP_ASSERT(numExteriorEdgeFacets >= 3);

  // Header
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataScatterBlend2DDef), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataScatterBlend2DDef* result = (AttribDataScatterBlend2DDef*)resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_SCATTER_BLEND_2D_DEF);
  result->setRefCount(MR::IS_DEF_ATTRIB_DATA);

  // Information
  result->m_numTriangleSubDivisions = (uint16_t)numTriangleSubDivisions;
  result->m_numSubSamplesPerTriangle = (uint16_t)numSubSamplesPerTriangle;
  result->m_numVertices = (uint16_t)numVertices;
  result->m_numTriangles = (uint16_t)numTriangles;
  result->m_numSubSamples = (uint16_t)numSubSamples;
  result->m_numExteriorEdgeFacets = (uint16_t)numExteriorEdgeFacets;
  result->m_bspNumBranchNodes = (uint16_t)bspNumBranchNodes;
  result->m_bspNumLeafNodes = (uint16_t)bspNumLeafNodes;
  result->m_bspNumLeafNodeEntries = (uint16_t)bspNumLeafNodeEntries;
  result->m_bspMaxNumSubTrianglesInPartition = 0;
  result->m_bspUnpackTriangleIndicesFn = 0;

  // Blend mode options
  result->m_projectionMode = MR::kScatterBlend2DInvalidProjectionMode;

  // Samples data
  uint32_t numSamples = numVertices + numSubSamples;
  NMP::Memory::Format memReqsSamples(sizeof(uint16_t) * numSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_samples[0] = (uint16_t*)resource.alignAndIncrement(memReqsSamples);
  result->m_samples[1] = (uint16_t*)resource.alignAndIncrement(memReqsSamples);

  // Triangle sample indices
  NMP::Memory::Format memReqsTriangleSampleIndices(
    sizeof(uint16_t) * numSubSamplesPerTriangle * numTriangles,
    NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_triangleSampleIndices = (uint16_t*)resource.alignAndIncrement(memReqsTriangleSampleIndices);

  // Exterior triangle facets
  uint32_t numExteriorEdgeFacets4 = NMP::nmAlignedValue4(numExteriorEdgeFacets);
  NMP::Memory::Format memReqsExtTriangles(sizeof(uint16_t) * numExteriorEdgeFacets4, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_exteriorTriangleIndices = (uint16_t*)resource.alignAndIncrement(memReqsExtTriangles);

  NMP::Memory::Format memReqsExtFacets(sizeof(uint8_t) * numExteriorEdgeFacets4, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_exteriorEdgeFacetIndices = (uint8_t*)resource.alignAndIncrement(memReqsExtFacets);

  //------------------------
  // BSP Tree
  result->m_bspBranchNodeLeft = NULL;
  result->m_bspBranchNodeRight = NULL;
  result->m_bspBranchNodeHyperPlaneSampleIndices[0] = NULL;
  result->m_bspBranchNodeHyperPlaneSampleIndices[1] = NULL;
  result->m_bspLeafNodeEntryOffsets = NULL;
  result->m_bspLeafNodeTriangleIndices = NULL;
  result->m_bspLeafNodeSubTriangleFlags = NULL;

  if (bspNumLeafNodes > 1)
  {
    // Note: We don't store any data if there is a single leaf node since we should just process
    // all triangles at runtime.
    NMP_ASSERT(bspNumLeafNodeEntries > 0);
    NMP::Memory::Format memReqsBspBranchNodes(sizeof(uint16_t) * bspNumBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Format memReqsBspLeafNodes(sizeof(uint16_t) * bspNumLeafNodes, NMP_NATURAL_TYPE_ALIGNMENT);

    // Left
    result->m_bspBranchNodeLeft = (uint16_t*)resource.alignAndIncrement(memReqsBspBranchNodes);
    // Right
    result->m_bspBranchNodeRight = (uint16_t*)resource.alignAndIncrement(memReqsBspBranchNodes);
    // Hyper-planes
    result->m_bspBranchNodeHyperPlaneSampleIndices[0] = (uint16_t*)resource.alignAndIncrement(memReqsBspBranchNodes);
    result->m_bspBranchNodeHyperPlaneSampleIndices[1] = (uint16_t*)resource.alignAndIncrement(memReqsBspBranchNodes);

    // Leaf node entry offsets (+1 element required for forward difference)
    NMP::Memory::Format memReqsBspLeafEntryOffsets(sizeof(uint16_t) * (bspNumLeafNodes + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_bspLeafNodeEntryOffsets = (uint16_t*)resource.alignAndIncrement(memReqsBspLeafEntryOffsets);

    // Leaf node triangle entries
    uint16_t idsIndex;
    if (numTriangles <= 256)
    {
      // 8-bits
      idsIndex = 0;
      NMP::Memory::Format memReqsBspLeafTets(sizeof(uint8_t) * bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_bspLeafNodeTriangleIndices = resource.alignAndIncrement(memReqsBspLeafTets);
    }
    else
    {
      // 16-bits
      idsIndex = 1;
      NMP::Memory::Format memReqsBspLeafTets(sizeof(uint16_t) * bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_bspLeafNodeTriangleIndices = resource.alignAndIncrement(memReqsBspLeafTets);
    }

    // Leaf node sub-triangle bit-masks
    uint16_t flagsIndex = result->m_numTriangleSubDivisions;
    if (numTriangleSubDivisions == 1)
    {
      // 8-bits (4 sub-triangles)
      NMP::Memory::Format memReqsBspSubTetFlags(bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_bspLeafNodeSubTriangleFlags = resource.alignAndIncrement(memReqsBspSubTetFlags);
    }
    else if (numTriangleSubDivisions == 2)
    {
      // 16-bits (16 sub-triangles)
      NMP::Memory::Format memReqsBspSubTetFlags(2 * bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_bspLeafNodeSubTriangleFlags = resource.alignAndIncrement(memReqsBspSubTetFlags);
    }

    // Set the unpacking function index
    result->m_bspUnpackTriangleIndicesFn = idsIndex * 3 + flagsIndex;
  }

  // Make sure size is a multiple of the alignment requirement.
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataScatterBlend2DDef::locate(AttribData* target)
{
  AttribDataScatterBlend2DDef* result = (AttribDataScatterBlend2DDef*) target;
  AttribData::locate(target);

  //------------------------
  // Information
  NMP::endianSwap(result->m_numTriangleSubDivisions);
  NMP::endianSwap(result->m_numSubSamplesPerTriangle);
  NMP::endianSwap(result->m_numVertices);
  NMP::endianSwap(result->m_numTriangles);
  NMP::endianSwap(result->m_numSubSamples);
  NMP::endianSwap(result->m_numExteriorEdgeFacets);
  NMP::endianSwap(result->m_bspNumBranchNodes);
  NMP::endianSwap(result->m_bspNumLeafNodes);
  NMP::endianSwap(result->m_bspNumLeafNodeEntries);
  NMP::endianSwap(result->m_bspMaxNumSubTrianglesInPartition);
  NMP::endianSwap(result->m_bspUnpackTriangleIndicesFn);

  // Blend mode options
  NMP::endianSwap(result->m_projectionMode);
  NMP::endianSwapArray(result->m_projectionVector, 2);

  // Display scales
  NMP::endianSwapArray(result->m_displayScale, 2);

  // Unit scaling
  NMP::endianSwapArray(result->m_unitScale, 2);

  // Samples data
  NMP::endianSwapArray(result->m_qScale, 2);
  NMP::endianSwapArray(result->m_qOffset, 2);
  uint32_t numSamples = result->m_numVertices + result->m_numSubSamples;
  REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_samples[0], result);
  REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_samples[1], result);
  NMP::endianSwapArray(result->m_samples[0], numSamples);
  NMP::endianSwapArray(result->m_samples[1], numSamples);

  // Triangle sample indices
  REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_triangleSampleIndices, result);
  NMP::endianSwapArray(result->m_triangleSampleIndices, result->m_numSubSamplesPerTriangle * result->m_numTriangles);

  // Exterior triangle facets
  uint32_t numExteriorEdgeFacets4 = NMP::nmAlignedValue4(result->m_numExteriorEdgeFacets);
  REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_exteriorTriangleIndices, result);
  NMP::endianSwapArray(result->m_exteriorTriangleIndices, numExteriorEdgeFacets4);
  REFIX_SWAP_PTR_RELATIVE(uint8_t, result->m_exteriorEdgeFacetIndices, result);
  NMP::endianSwapArray(result->m_exteriorEdgeFacetIndices, numExteriorEdgeFacets4);

  //------------------------
  // BSP Tree
  if (result->m_bspBranchNodeLeft)
  {
    NMP_ASSERT(result->m_bspNumBranchNodes > 0);
    // Left
    REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspBranchNodeLeft, result);
    NMP::endianSwapArray(result->m_bspBranchNodeLeft, result->m_bspNumBranchNodes);
    // Right
    REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspBranchNodeRight, result);
    NMP::endianSwapArray(result->m_bspBranchNodeRight, result->m_bspNumBranchNodes);
    // Hyper-planes
    REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspBranchNodeHyperPlaneSampleIndices[0], result);
    NMP::endianSwapArray(result->m_bspBranchNodeHyperPlaneSampleIndices[0], result->m_bspNumBranchNodes);
    REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspBranchNodeHyperPlaneSampleIndices[1], result);
    NMP::endianSwapArray(result->m_bspBranchNodeHyperPlaneSampleIndices[1], result->m_bspNumBranchNodes);

    // Leaf node entry offsets (+1 element required for forward difference)
    REFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspLeafNodeEntryOffsets, result);
    NMP::endianSwapArray(result->m_bspLeafNodeEntryOffsets, result->m_bspNumLeafNodes + 1);

    // Leaf node triangle entries
    if (result->m_numTriangles <= 256)
    {
      // 8-bits
      REFIX_SWAP_PTR_RELATIVE(void, result->m_bspLeafNodeTriangleIndices, result);
    }
    else
    {
      // 16-bits
      REFIX_SWAP_PTR_RELATIVE(void, result->m_bspLeafNodeTriangleIndices, result);
      uint16_t* leafNodeTriangleIndices = (uint16_t*)result->m_bspLeafNodeTriangleIndices;
      NMP::endianSwapArray(leafNodeTriangleIndices, result->m_bspNumLeafNodeEntries);
    }

    // Leaf node sub-triangle bit-masks
    if (result->m_numTriangleSubDivisions == 1)
    {
      // 8-bits (4 sub-triangles)
      REFIX_SWAP_PTR_RELATIVE(void, result->m_bspLeafNodeSubTriangleFlags, result);
    }
    else if (result->m_numTriangleSubDivisions == 2)
    {
      // 16-bits (16 sub-triangles)
      REFIX_SWAP_PTR_RELATIVE(void, result->m_bspLeafNodeSubTriangleFlags, result);
      uint16_t* subTriangleFlags = (uint16_t*)result->m_bspLeafNodeSubTriangleFlags;
      NMP::endianSwapArray(subTriangleFlags, result->m_bspNumLeafNodeEntries);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScatterBlend2DDef::dislocate(AttribData* target)
{
  AttribDataScatterBlend2DDef* result = (AttribDataScatterBlend2DDef*) target;

  //------------------------
  // BSP Tree
  if (result->m_bspBranchNodeLeft)
  {
    NMP_ASSERT(result->m_bspNumBranchNodes > 0);

    // Leaf node sub-triangle bit-masks
    if (result->m_numTriangleSubDivisions == 1)
    {
      // 8-bits (4 sub-triangles)
      UNFIX_SWAP_PTR_RELATIVE(void, result->m_bspLeafNodeSubTriangleFlags, result);
    }
    else if (result->m_numTriangleSubDivisions == 2)
    {
      // 16-bits (16 sub-triangles)
      uint16_t* subTriangleFlags = (uint16_t*)result->m_bspLeafNodeSubTriangleFlags;
      NMP::endianSwapArray(subTriangleFlags, result->m_bspNumLeafNodeEntries);
      UNFIX_SWAP_PTR_RELATIVE(void, result->m_bspLeafNodeSubTriangleFlags, result);
    }

    // Leaf node triangle entries
    if (result->m_numTriangles <= 256)
    {
      // 8-bits
      UNFIX_SWAP_PTR_RELATIVE(void, result->m_bspLeafNodeTriangleIndices, result);
    }
    else
    {
      // 16-bits      
      uint16_t* leafNodeTriangleIndices = (uint16_t*)result->m_bspLeafNodeTriangleIndices;
      NMP::endianSwapArray(leafNodeTriangleIndices, result->m_bspNumLeafNodeEntries);
      UNFIX_SWAP_PTR_RELATIVE(void, result->m_bspLeafNodeTriangleIndices, result);
    }

    // Leaf node entry offsets (+1 element required for forward difference)    
    NMP::endianSwapArray(result->m_bspLeafNodeEntryOffsets, result->m_bspNumLeafNodes + 1);
    UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspLeafNodeEntryOffsets, result);

    // Hyper-planes
    NMP::endianSwapArray(result->m_bspBranchNodeHyperPlaneSampleIndices[0], result->m_bspNumBranchNodes);
    UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspBranchNodeHyperPlaneSampleIndices[0], result);
    NMP::endianSwapArray(result->m_bspBranchNodeHyperPlaneSampleIndices[1], result->m_bspNumBranchNodes);
    UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspBranchNodeHyperPlaneSampleIndices[1], result);
    // Right
    NMP::endianSwapArray(result->m_bspBranchNodeRight, result->m_bspNumBranchNodes);
    UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspBranchNodeRight, result);
    // Left    
    NMP::endianSwapArray(result->m_bspBranchNodeLeft, result->m_bspNumBranchNodes);
    UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_bspBranchNodeLeft, result);
  }

  //------------------------
  // Exterior triangle facets
  uint32_t numExteriorEdgeFacets4 = NMP::nmAlignedValue4(result->m_numExteriorEdgeFacets);
  NMP::endianSwapArray(result->m_exteriorTriangleIndices, numExteriorEdgeFacets4);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_exteriorTriangleIndices, result);
  NMP::endianSwapArray(result->m_exteriorEdgeFacetIndices, numExteriorEdgeFacets4);
  UNFIX_SWAP_PTR_RELATIVE(uint8_t, result->m_exteriorEdgeFacetIndices, result);

  // Triangle sample indices
  NMP::endianSwapArray(result->m_triangleSampleIndices, result->m_numSubSamplesPerTriangle * result->m_numTriangles);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_triangleSampleIndices, result);

  // Samples data
  NMP::endianSwapArray(result->m_qScale, 2);
  NMP::endianSwapArray(result->m_qOffset, 2);
  uint32_t numSamples = result->m_numVertices + result->m_numSubSamples;
  NMP::endianSwapArray(result->m_samples[0], numSamples);
  NMP::endianSwapArray(result->m_samples[1], numSamples);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_samples[0], result);
  UNFIX_SWAP_PTR_RELATIVE(uint16_t, result->m_samples[1], result);

  // Unit scaling
  NMP::endianSwapArray(result->m_unitScale, 2);

  // Display scales
  NMP::endianSwapArray(result->m_displayScale, 2);

  // Blend mode options
  NMP::endianSwap(result->m_projectionMode);
  NMP::endianSwapArray(result->m_projectionVector, 2);

  // Information
  NMP::endianSwap(result->m_numTriangleSubDivisions);
  NMP::endianSwap(result->m_numSubSamplesPerTriangle);
  NMP::endianSwap(result->m_numVertices);
  NMP::endianSwap(result->m_numTriangles);
  NMP::endianSwap(result->m_numSubSamples);
  NMP::endianSwap(result->m_numExteriorEdgeFacets);
  NMP::endianSwap(result->m_bspNumBranchNodes);
  NMP::endianSwap(result->m_bspNumLeafNodes);
  NMP::endianSwap(result->m_bspNumLeafNodeEntries);
  NMP::endianSwap(result->m_bspMaxNumSubTrianglesInPartition);
  NMP::endianSwap(result->m_bspUnpackTriangleIndicesFn);

  AttribData::dislocate(target);
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScatterBlend2DDef::relocate(AttribData* target, void* location)
{
  AttribDataScatterBlend2DDef* result = (AttribDataScatterBlend2DDef*) target;

  //------------------------
  // Header
  void* ptr = location;
  NMP_ASSERT(NMP_IS_ALIGNED(ptr, MR_ATTRIB_DATA_ALIGNMENT));
  ptr = NMP::Memory::increment(ptr, sizeof(AttribDataScatterBlend2DDef)); 

  // Samples data
  uint32_t numSamples = result->m_numVertices + result->m_numSubSamples;
  NMP::Memory::Format memReqsSamples(sizeof(uint16_t) * numSamples, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_samples[0] = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsSamples);
  result->m_samples[1] = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsSamples);

  // Triangle sample indices
  NMP::Memory::Format memReqsTriangleSampleIndices(
    sizeof(uint16_t) * result->m_numSubSamplesPerTriangle * result->m_numTriangles,
    NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_triangleSampleIndices = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsTriangleSampleIndices);

  // Exterior triangle facets
  uint32_t numExteriorEdgeFacets4 = NMP::nmAlignedValue4(result->m_numExteriorEdgeFacets);
  NMP::Memory::Format memReqsExtTriangles(sizeof(uint16_t) * numExteriorEdgeFacets4, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_exteriorTriangleIndices = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsExtTriangles);

  NMP::Memory::Format memReqsExtFacets(sizeof(uint8_t) * numExteriorEdgeFacets4, NMP_NATURAL_TYPE_ALIGNMENT);
  result->m_exteriorEdgeFacetIndices = (uint8_t*)NMP::Memory::alignAndIncrement(ptr, memReqsExtFacets);

  //------------------------
  // BSP Tree
  if (result->m_bspBranchNodeLeft)
  {
    NMP_ASSERT(result->m_bspNumBranchNodes > 0);
    uint32_t bspNumNodes = result->m_bspNumBranchNodes + result->m_bspNumLeafNodes;
    NMP::Memory::Format memReqsBspBranchNodes(sizeof(uint16_t) * result->m_bspNumBranchNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Format memReqsBspLeafNodes(sizeof(uint16_t) * result->m_bspNumLeafNodes, NMP_NATURAL_TYPE_ALIGNMENT);
    NMP::Memory::Format memReqsBspNodes(sizeof(uint16_t) * bspNumNodes, NMP_NATURAL_TYPE_ALIGNMENT);

    // Left
    result->m_bspBranchNodeLeft = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsBspBranchNodes);
    // Right
    result->m_bspBranchNodeRight = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsBspBranchNodes);
    // Hyper-planes
    result->m_bspBranchNodeHyperPlaneSampleIndices[0] = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsBspBranchNodes);
    result->m_bspBranchNodeHyperPlaneSampleIndices[1] = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsBspBranchNodes);

    // Leaf node entry offsets (+1 element required for forward difference)
    NMP::Memory::Format memReqsBspLeafEntryOffsets(sizeof(uint16_t) * (result->m_bspNumLeafNodes + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    result->m_bspLeafNodeEntryOffsets = (uint16_t*)NMP::Memory::alignAndIncrement(ptr, memReqsBspLeafEntryOffsets);

    // Leaf node triangle entries
    if (result->m_numTriangles <= 256)
    {
      // 8-bits
      NMP::Memory::Format memReqsBspLeafTets(sizeof(uint8_t) * result->m_bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_bspLeafNodeTriangleIndices = NMP::Memory::alignAndIncrement(ptr, memReqsBspLeafTets);
    }
    else
    {
      // 16-bits
      NMP::Memory::Format memReqsBspLeafTets(sizeof(uint16_t) * result->m_bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_bspLeafNodeTriangleIndices = NMP::Memory::alignAndIncrement(ptr, memReqsBspLeafTets);
    }

    // Leaf node sub-triangle bit-masks
    if (result->m_numTriangleSubDivisions == 1)
    {
      // 8-bits (4 sub-triangles)
      NMP::Memory::Format memReqsBspSubTetFlags(result->m_bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_bspLeafNodeSubTriangleFlags = NMP::Memory::alignAndIncrement(ptr, memReqsBspSubTetFlags);
    }
    else if (result->m_numTriangleSubDivisions == 2)
    {
      // 16-bits (16 sub-triangles)
      NMP::Memory::Format memReqsBspSubTetFlags(2 * result->m_bspNumLeafNodeEntries, NMP_NATURAL_TYPE_ALIGNMENT);
      result->m_bspLeafNodeSubTriangleFlags = NMP::Memory::alignAndIncrement(ptr, memReqsBspSubTetFlags);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataScatterBlend2DState functions.
//----------------------------------------------------------------------------------------------------------------------
#ifndef NM_HOST_CELL_SPU
void AttribDataScatterBlend2DState::locate(AttribData* target)
{
  AttribDataScatterBlend2DState* result = (AttribDataScatterBlend2DState*)target;

  AttribData::locate(target);
  NMP::endianSwapArray(result->m_parameteriserInfo.m_childNodeIDs, 3, sizeof(NodeID));
  NMP::endianSwapArray(result->m_parameteriserInfo.m_childNodeWeights, 3, sizeof(float));
  NMP::endianSwapArray(result->m_parameteriserInfo.m_desiredParameters, 2, sizeof(float));
  NMP::endianSwapArray(result->m_parameteriserInfo.m_achievedParameters, 2, sizeof(float));
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataScatterBlend2DState::dislocate(AttribData* target)
{
  AttribDataScatterBlend2DState* result = (AttribDataScatterBlend2DState*)target;

  AttribData::dislocate(target);
  NMP::endianSwapArray(result->m_parameteriserInfo.m_childNodeIDs, 3, sizeof(NodeID));
  NMP::endianSwapArray(result->m_parameteriserInfo.m_childNodeWeights, 3, sizeof(float));
  NMP::endianSwapArray(result->m_parameteriserInfo.m_desiredParameters, 2, sizeof(float));
  NMP::endianSwapArray(result->m_parameteriserInfo.m_achievedParameters, 2, sizeof(float));
}
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataScatterBlend2DState::getMemoryRequirements()
{
  NMP::Memory::Format result(sizeof(AttribDataScatterBlend2DState), MR_ATTRIB_DATA_ALIGNMENT);

  // Multiple of the attrib data alignment
  result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataScatterBlend2DState* AttribDataScatterBlend2DState::init(NMP::Memory::Resource& resource, uint16_t refCount)
{
  NMP::Memory::Format memReqsHdr(sizeof(AttribDataScatterBlend2DState), MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataScatterBlend2DState* result = (AttribDataScatterBlend2DState*) resource.alignAndIncrement(memReqsHdr);

  result->setType(ATTRIB_TYPE_SCATTER_BLEND_2D_STATE);
  result->setRefCount(refCount);

  // Multiple of the attrib data alignment
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataScatterBlend2DState::create(
  NMP::MemoryAllocator* allocator,
  AttribDataCreateDesc* NMP_UNUSED(desc))
{
  AttribDataHandle result;

  // Defaults
  uint16_t refCount = 0;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(result.m_format NMP_MEMORY_TRACKING_ARGS);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataHandle AttribDataScatterBlend2DState::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  AttribDataHandle result;

  result.m_format = getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(result.m_format NMP_MEMORY_TRACKING_ARGS);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, refCount);

  // Backchain the allocator so we know what to free this attribData with when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
