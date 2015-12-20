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
#include "NMPlatform/NMBuffer.h"
#include "NMPlatform/NMMemoryAllocator.h"
//----------------------------------------------------------------------------------------------------------------------

namespace NMP
{

const uint32_t DataBuffer::POS_QUAT_BUFFER_POS_ELEMENT_INDEX = 0;   // Element index of the Quat in a PosQuat DataBuffer.
const uint32_t DataBuffer::POS_QUAT_BUFFER_QUAT_ELEMENT_INDEX = 1;  // Element index of the Quat in a PosQuat DataBuffer.
const uint32_t DataBuffer::POS_VEL_ANG_VEL_BUFFER_POS_VEL_ELEMENT_INDEX = 0;  // Element index of the Position Velocity in a PosVelAngVel DataBuffer.
const uint32_t DataBuffer::POS_VEL_ANG_VEL_BUFFER_ANG_VEL_ELEMENT_INDEX = 1;  // Element index of the Angular Velocity in a PosVelAngVel DataBuffer.

const DataBuffer::ElementDescriptor DataBuffer::POS_ELEMENT_DESCRIPTOR(DataBuffer::NMP_ELEMENT_TYPE_VEC3, sizeof(Vector3), 16);
const DataBuffer::ElementDescriptor DataBuffer::QUAT_ELEMENT_DESCRIPTOR(DataBuffer::NMP_ELEMENT_TYPE_QUAT, sizeof(Quat), 16);
const DataBuffer::ElementDescriptor DataBuffer::POS_VEL_ELEMENT_DESCRIPTOR(DataBuffer::NMP_ELEMENT_TYPE_POSVEL, sizeof(Vector3), 16);
const DataBuffer::ElementDescriptor DataBuffer::ANG_VEL_ELEMENT_DESCRIPTOR(DataBuffer::NMP_ELEMENT_TYPE_ANGVEL, sizeof(Vector3), 16);
const DataBuffer::ElementDescriptor DataBuffer::MATRIX_ELEMENT_DESCRIPTOR(DataBuffer::NMP_ELEMENT_TYPE_MATRIX, sizeof(Matrix34), 16);

// Some standard Buffer descriptions.
DataBuffer::ElementDescriptor DataBuffer::sm_PosQuatDescriptor[2] = { DataBuffer::POS_ELEMENT_DESCRIPTOR, DataBuffer::QUAT_ELEMENT_DESCRIPTOR };
DataBuffer::ElementDescriptor DataBuffer::sm_PosVelAngVelDescriptor[2] = { DataBuffer::POS_VEL_ELEMENT_DESCRIPTOR, DataBuffer::ANG_VEL_ELEMENT_DESCRIPTOR };

//----------------------------------------------------------------------------------------------------------------------
Memory::Format DataBuffer::getMemoryRequirements(
  uint32_t                 numElements,
  const ElementDescriptor* elements,
  uint32_t                 length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");
  NMP_ASSERT_MSG(numElements != 0, "Element count must be non-zero");

  Memory::Format memReq = Memory::Format(sizeof(DataBuffer), NMP_VECTOR_ALIGNMENT);

  // Used flags
  memReq += BitArray::getMemoryRequirements(length);

  // Elements
  memReq += Memory::Format(sizeof(ElementDescriptor) * numElements, NMP_NATURAL_TYPE_ALIGNMENT);

  // Data pointers
  memReq += Memory::Format(sizeof(void*) * numElements, NMP_NATURAL_TYPE_ALIGNMENT);

  // The data buffer itself, align to blocks of 4
  uint32_t length4 = (uint32_t)Memory::align(length, 4);
  for (uint32_t i = 0; i < numElements; i++)
  {
    memReq += Memory::Format(
                Memory::align(elements[i].m_size, elements[i].m_alignment) * length4,
                elements[i].m_alignment);
  }

  // Round the size up to a multiple of NMP_VECTOR_ALIGNMENT. Allows for fast copying and DMAing.
  memReq.align();

  return memReq;
}

//----------------------------------------------------------------------------------------------------------------------
DataBuffer* DataBuffer::create(
  MemoryAllocator*         allocator,
  const Memory::Format&    memoryReqs,
  uint32_t                 numElements,
  const ElementDescriptor* elements,
  uint32_t                 length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");
  NMP_ASSERT_MSG(numElements != 0, "Element count must be non-zero");
  NMP_ASSERT(memoryReqs == getMemoryRequirements(numElements, elements, length));
  Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memoryReqs);
  DataBuffer* buffer = (DataBuffer*) init(resource, memoryReqs, numElements, elements, length);
  return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
DataBuffer* DataBuffer::init(
  Memory::Resource&        resource,
  const Memory::Format&    memoryReqs,
  uint32_t                 numElements,
  const ElementDescriptor* elements,
  uint32_t                 length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");
  NMP_ASSERT_MSG(numElements != 0, "Element count must be non-zero");
  NMP_ASSERT(memoryReqs == getMemoryRequirements(numElements, elements, length));
  NMP_ASSERT(memoryReqs.alignment == NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(resource.format.size >= memoryReqs.size);
  resource.align(NMP_VECTOR_ALIGNMENT);
  DataBuffer* buffer = (DataBuffer*) resource.ptr;

  buffer->m_numElements = numElements;
  buffer->m_length = length;
  buffer->m_memoryReqs = memoryReqs;

  resource.increment(Memory::Format(sizeof(DataBuffer), NMP_VECTOR_ALIGNMENT));

  // Used flags
  buffer->m_usedFlags = BitArray::init(resource, length);

  // Set up elements
  Memory::Format elementsReq = Memory::Format(
                                 sizeof(ElementDescriptor) * numElements,
                                 NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(elementsReq);
  buffer->m_elements = (ElementDescriptor*)resource.ptr;
  resource.increment(elementsReq);

  memcpy(buffer->m_elements, elements, elementsReq.size);

  // Set up data pointers
  Memory::Format pointersReq = Memory::Format(sizeof(void*) * numElements, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(pointersReq);
  buffer->m_data = (void**)resource.ptr;
  resource.increment(pointersReq);

  // Set up data, align to blocks of 4
  uint32_t length4 = (uint32_t)Memory::align(length, 4);
  for (uint32_t i = 0; i < numElements; i++)
  {
    Memory::Format dataReq = Memory::Format(Memory::align(
        elements[i].m_size, elements[i].m_alignment) * length4,
                                            elements[i].m_alignment);
    resource.align(dataReq);
    buffer->m_data[i] = resource.ptr;
    resource.increment(dataReq);
  }

  // Mark all elements as unused
  buffer->m_usedFlags->clearAll();
  buffer->m_full = false;        // has to be set later

  // Round the size up to a multiple of NMP_VECTOR_ALIGNMENT. Allows for fast copying and DMAing.
  resource.align(NMP_VECTOR_ALIGNMENT);
  return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
Memory::Format DataBuffer::getPosQuatMemoryRequirements(uint32_t length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");

  Memory::Format memReq = Memory::Format(sizeof(DataBuffer), NMP_VECTOR_ALIGNMENT);

  // Used flags.
  memReq += BitArray::getMemoryRequirements(length);

  // Elements.
  memReq += Memory::Format(sizeof(ElementDescriptor) * 2, NMP_NATURAL_TYPE_ALIGNMENT);

  // Data pointers.
  memReq += Memory::Format(sizeof(void*) * 2, NMP_NATURAL_TYPE_ALIGNMENT);

  // The data buffer itself, align to blocks of 4
  uint32_t length4 = (uint32_t)Memory::align(length, 4);
  memReq += Memory::Format(
              Memory::align(POS_ELEMENT_DESCRIPTOR.m_size, POS_ELEMENT_DESCRIPTOR.m_alignment) * length4,
              POS_ELEMENT_DESCRIPTOR.m_alignment);
  memReq += Memory::Format(
              Memory::align(QUAT_ELEMENT_DESCRIPTOR.m_size, QUAT_ELEMENT_DESCRIPTOR.m_alignment) * length4,
              QUAT_ELEMENT_DESCRIPTOR.m_alignment);

  // Round the size up to a multiple of NMP_VECTOR_ALIGNMENT. Allows for fast copying and DMAing.
  memReq.align();

  return memReq;
}

//----------------------------------------------------------------------------------------------------------------------
DataBuffer* DataBuffer::createPosQuat(
  MemoryAllocator*      allocator,
  const Memory::Format& memoryReqs,
  uint32_t              length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");
  NMP_ASSERT(memoryReqs == getPosQuatMemoryRequirements(length));
  Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memoryReqs);
  DataBuffer* buffer = (DataBuffer*) initPosQuat(resource, memoryReqs, length);
  return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
DataBuffer* DataBuffer::initPosQuat(
  Memory::Resource&     resource,
  const Memory::Format& memoryReqs,
  uint32_t              length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");
  NMP_ASSERT(memoryReqs == getPosQuatMemoryRequirements(length));
  NMP_ASSERT(memoryReqs.alignment == NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(resource.format.size >= memoryReqs.size);
  resource.align(NMP_VECTOR_ALIGNMENT);
  DataBuffer* buffer = (DataBuffer*) resource.ptr;

  buffer->m_numElements = 2;
  buffer->m_length = length;
  buffer->m_memoryReqs = memoryReqs;

  resource.increment(Memory::Format(sizeof(DataBuffer), NMP_VECTOR_ALIGNMENT));

  // Used flags requirements.
  buffer->m_usedFlags = BitArray::init(resource, length);

  // Set up elements.
  Memory::Format elementsReq = Memory::Format(sizeof(ElementDescriptor) * 2, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(elementsReq);
  buffer->m_elements = (ElementDescriptor*)resource.ptr;
  resource.increment(elementsReq);

  buffer->m_elements[0] = POS_ELEMENT_DESCRIPTOR;
  buffer->m_elements[1] = QUAT_ELEMENT_DESCRIPTOR;

  // Set up data pointers.
  Memory::Format pointersReq = Memory::Format(sizeof(void*) * 2, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(pointersReq);
  buffer->m_data = (void**)resource.ptr;
  resource.increment(pointersReq);

  // Set up data, align to blocks of 4
  uint32_t length4 = (uint32_t)Memory::align(length, 4);

  Memory::Format dataReq = Memory::Format(
                             Memory::align(POS_ELEMENT_DESCRIPTOR.m_size, POS_ELEMENT_DESCRIPTOR.m_alignment) * length4,
                             POS_ELEMENT_DESCRIPTOR.m_alignment);
  resource.align(dataReq);
  buffer->m_data[0] = resource.ptr;
  resource.increment(dataReq);

  dataReq = Memory::Format(
              Memory::align(QUAT_ELEMENT_DESCRIPTOR.m_size, QUAT_ELEMENT_DESCRIPTOR.m_alignment) * length4,
              QUAT_ELEMENT_DESCRIPTOR.m_alignment);
  resource.align(dataReq);
  buffer->m_data[1] = resource.ptr;
  resource.increment(dataReq);

  // Round the size up to a multiple of NMP_VECTOR_ALIGNMENT. Allows for fast copying and DMAing.
  resource.align(NMP_VECTOR_ALIGNMENT);

  // Mark all elements as unused
  buffer->m_usedFlags->clearAll();
  buffer->m_full = false;     

  return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
Memory::Format DataBuffer::getPosVelAngVelMemoryRequirements(uint32_t length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");

  Memory::Format memReq = Memory::Format(sizeof(DataBuffer), NMP_VECTOR_ALIGNMENT);

  // Used flags.
  memReq +=  BitArray::getMemoryRequirements(length);

  // Elements.
  memReq += Memory::Format(sizeof(ElementDescriptor) * 2, NMP_NATURAL_TYPE_ALIGNMENT);

  // Data pointers.
  memReq += Memory::Format(sizeof(void*) * 2, NMP_NATURAL_TYPE_ALIGNMENT);

  // The data buffer itself, align to blocks of 4
  uint32_t length4 = (uint32_t)Memory::align(length, 4);
  memReq += Memory::Format(
              Memory::align(POS_VEL_ELEMENT_DESCRIPTOR.m_size, POS_VEL_ELEMENT_DESCRIPTOR.m_alignment) * length4,
              POS_VEL_ELEMENT_DESCRIPTOR.m_alignment);
  memReq += Memory::Format(
              Memory::align(ANG_VEL_ELEMENT_DESCRIPTOR.m_size, ANG_VEL_ELEMENT_DESCRIPTOR.m_alignment) * length4,
              ANG_VEL_ELEMENT_DESCRIPTOR.m_alignment);

  // Round the size up to a multiple of NMP_VECTOR_ALIGNMENT. Allows for fast copying and DMAing.
  memReq.align();

  return memReq;
}

//----------------------------------------------------------------------------------------------------------------------
DataBuffer* DataBuffer::createPosVelAngVel(
  MemoryAllocator*      allocator,
  const Memory::Format& memoryReqs,
  uint32_t              length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");
  NMP_ASSERT(memoryReqs == getPosVelAngVelMemoryRequirements(length));
  Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memoryReqs);
  DataBuffer* buffer = (DataBuffer*) initPosVelAngVel(resource, memoryReqs, length);
  return buffer;
}

//----------------------------------------------------------------------------------------------------------------------
DataBuffer* DataBuffer::initPosVelAngVel(
  Memory::Resource&     resource,
  const Memory::Format& memoryReqs,
  uint32_t              length)
{
  NMP_ASSERT_MSG(length != 0, "Buffer length must be non-zero");
  NMP_ASSERT(memoryReqs == getPosVelAngVelMemoryRequirements(length));
  NMP_ASSERT(memoryReqs.alignment == NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(resource.format.size >= memoryReqs.size);
  resource.align(NMP_VECTOR_ALIGNMENT);
  DataBuffer* buffer = (DataBuffer*) resource.ptr;

  buffer->m_numElements = 2;
  buffer->m_length = length;
  buffer->m_memoryReqs = memoryReqs;

  resource.increment(Memory::Format(sizeof(DataBuffer), NMP_VECTOR_ALIGNMENT));

  // Used flags requirements.
  buffer->m_usedFlags = BitArray::init(resource, length);

  // Set up elements.
  Memory::Format elementsReq = Memory::Format(sizeof(ElementDescriptor) * 2, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(elementsReq);
  buffer->m_elements = (ElementDescriptor*)resource.ptr;
  resource.increment(elementsReq);

  buffer->m_elements[0] = POS_VEL_ELEMENT_DESCRIPTOR;
  buffer->m_elements[1] = ANG_VEL_ELEMENT_DESCRIPTOR;

  // Set up data pointers.
  Memory::Format pointersReq = Memory::Format(sizeof(void*) * 2, NMP_NATURAL_TYPE_ALIGNMENT);
  resource.align(pointersReq);
  buffer->m_data = (void**)resource.ptr;
  resource.increment(pointersReq);

  // Set up data, align to blocks of 4
  uint32_t length4 = (uint32_t)Memory::align(length, 4);

  Memory::Format dataReq = Memory::Format(
                             Memory::align(POS_VEL_ELEMENT_DESCRIPTOR.m_size, POS_VEL_ELEMENT_DESCRIPTOR.m_alignment) * length4,
                             POS_VEL_ELEMENT_DESCRIPTOR.m_alignment);
  resource.align(dataReq);
  buffer->m_data[0] = resource.ptr;
  resource.increment(dataReq);

  dataReq = Memory::Format(
              Memory::align(ANG_VEL_ELEMENT_DESCRIPTOR.m_size, ANG_VEL_ELEMENT_DESCRIPTOR.m_alignment) * length4,
              ANG_VEL_ELEMENT_DESCRIPTOR.m_alignment);
  resource.align(dataReq);
  buffer->m_data[1] = resource.ptr;
  resource.increment(dataReq);

  // Round the size up to a multiple of NMP_VECTOR_ALIGNMENT. Allows for fast copying and DMAing.
  resource.align(NMP_VECTOR_ALIGNMENT);

  // Mark all elements as unused
  buffer->m_usedFlags->clearAll();
  buffer->m_full = false;        // has to be set later

  return buffer;
}

} // namespace NMP

//----------------------------------------------------------------------------------------------------------------------
