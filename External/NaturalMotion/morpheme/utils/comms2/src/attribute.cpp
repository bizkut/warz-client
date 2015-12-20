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
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "comms/mcomms.h"
#include "comms/attribute.h"
#include "comms/packet.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
//  Attribute
//----------------------------------------------------------------------------------------------------------------------
uint32_t Attribute::getDataTypeSize(Attribute::DataType dataType)
{
  switch (dataType)
  {
  case DATATYPE_LINE:
    return sizeof(MCOMMS::DebugLine);
  case DATATYPE_TRIANGLE:
    return sizeof(MCOMMS::DebugTriangle);

  case DATATYPE_MATRIX:
    return sizeof(NMP::Matrix34);

  case DATATYPE_VECTOR:
    return sizeof(NMP::Vector3);

  case DATATYPE_COLOUR:
    return sizeof(NMP::Colour);

  case DATATYPE_FLOAT:
  case DATATYPE_INT:
  case DATATYPE_UINT:
    return sizeof(uint32_t);

  case DATATYPE_CHAR:
    return sizeof(uint8_t);

  default:
    NMP_DEBUG_MSG("Attribute::DataType value %d not understood.", dataType);
    NMP_ASSERT_FAIL();
    break;
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool Attribute::copyDataFrom(const Attribute* src)
{
  if ((m_attrDesc != src->m_attrDesc) ||
      (m_dataSize != src->m_dataSize))
  {
    return false;
  }
  memcpy(getData(), src->getData(), m_dataSize);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::create(const Attribute::Descriptor* desc, const void* data)
{
  size_t dataCount = desc->getDataCount();
  size_t dataSize = Attribute::getDataTypeSize(desc->getDataType());
  void* dataCopy = NMPMemoryAllocAligned(dataSize * dataCount, 4);
  NMP_ASSERT(dataCopy);
  memcpy(dataCopy, data, dataCount * dataSize);

  Attribute* attribute = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attribute);
  attribute->init(*desc, dataCopy);

  return attribute;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createEmpty()
{
  Attribute* attribute = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attribute);
  return attribute;
}

//----------------------------------------------------------------------------------------------------------------------
void Attribute::destroy(Attribute* attribute)
{
  NMP::Memory::memFree(attribute);
}

//----------------------------------------------------------------------------------------------------------------------
void Attribute::endianSwapData(Attribute::Descriptor* desc, void* data)
{
  uint32_t dataElementSize = (uint32_t)Attribute::getDataTypeSize(desc->getDataType());
  uint32_t dataCount = (uint32_t)desc->getDataCount();

  switch (dataElementSize)
  {
  case 1:
    break;

  case 2:
    NMP::netEndianSwapArray((uint16_t*)data, dataCount);
    break;

  case 8:
    // May need to handle uint64_t case here
  default:
  {
    NMP::netEndianSwapArray((uint32_t*)data, (dataCount * dataElementSize) / sizeof(uint32_t));
  }
  break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void Attribute::endianSwapDesc(Attribute::Descriptor* desc)
{
  desc->endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
void Attribute::endianSwapData()
{
  endianSwapData(&m_attrDesc, getData());
}

//----------------------------------------------------------------------------------------------------------------------
// !! These could go in a separate file.  They are for tools only
Attribute* Attribute::createAttribute(
  float    value,
  Semantic semantic,
  bool     dynamic)
{
  Descriptor attrDesc(DATATYPE_FLOAT, 1, semantic, dynamic);
  float* data = (float*)NMPMemoryAllocAligned(sizeof(float), 4);
  NMP_ASSERT(data);
  *data = value;
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  int32_t  value,
  Semantic semantic,
  bool     dynamic)
{
  Descriptor attrDesc(DATATYPE_INT, 1, semantic, dynamic);
  int32_t* data = (int32_t*)NMPMemoryAllocAligned(sizeof(int32_t), 4);
  NMP_ASSERT(data);
  *data = value;
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  uint32_t value,
  Semantic semantic,
  bool     dynamic)
{
  Descriptor attrDesc(DATATYPE_UINT, 1, semantic, dynamic);
  uint32_t* data = (uint32_t*)NMPMemoryAllocAligned(sizeof(uint32_t), 4);
  NMP_ASSERT(data);
  *data = value;
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  bool     value,
  Semantic semantic,
  bool     dynamic)
{
  Descriptor attrDesc(DATATYPE_UINT, 1, semantic, dynamic);
  uint32_t* data = (uint32_t*)NMPMemoryAllocAligned(sizeof(uint32_t), 4);
  NMP_ASSERT(data);
  *data = (uint32_t)value;
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const NMP::Vector3& value,
  Semantic            semantic,
  bool                dynamic)
{
  Descriptor attrDesc(DATATYPE_VECTOR, 1, semantic, dynamic);
  NMP::Vector3* data = (NMP::Vector3*)NMPMemoryAllocAligned(sizeof(NMP::Vector3), NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(data);
  *data = value;
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const NMP::Matrix34& value,
  Semantic             semantic,
  bool                 dynamic)
{
  Descriptor attrDesc(DATATYPE_MATRIX, 1, semantic, dynamic);
  NMP::Matrix34* data = (NMP::Matrix34*)NMPMemoryAllocAligned(sizeof(NMP::Matrix34), NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(data);
  *data = value;
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const NMP::Colour& value,
  Semantic           semantic,
  bool               dynamic)
{
  Descriptor attrDesc(DATATYPE_COLOUR, 1, semantic, dynamic);
  NMP::Colour* data = (NMP::Colour*)NMPMemoryAllocAligned(sizeof(NMP::Colour), 16);
  NMP_ASSERT(data);
  *data = value;
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const float* values,
  uint32_t     numElements,
  Semantic     semantic,
  bool         dynamic)
{
  Descriptor attrDesc(DATATYPE_FLOAT, numElements, semantic, dynamic);
  uint32_t dataSize = sizeof(float) * numElements;
  float* data = (float*)NMPMemoryAllocAligned(dataSize, 4);
  NMP_ASSERT(data);
  memcpy(data, values, dataSize);
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const int32_t* values,
  uint32_t       numElements,
  Semantic       semantic,
  bool           dynamic)
{
  Descriptor attrDesc(DATATYPE_INT, numElements, semantic, dynamic);
  uint32_t dataSize = sizeof(int32_t) * numElements;
  int32_t* data = (int32_t*)NMPMemoryAllocAligned(dataSize, 4);
  NMP_ASSERT(data);
  memcpy(data, values, dataSize);
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const uint32_t* values,
  uint32_t        numElements,
  Semantic        semantic,
  bool            dynamic)
{
  Descriptor attrDesc(DATATYPE_UINT, numElements, semantic, dynamic);
  uint32_t dataSize = sizeof(uint32_t) * numElements;
  uint32_t* data = (uint32_t*)NMPMemoryAllocAligned(dataSize, 4);
  NMP_ASSERT(data);
  memcpy(data, values, dataSize);
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const NMP::Vector3* values,
  uint32_t            numElements,
  Semantic            semantic,
  bool                dynamic)
{
  Descriptor attrDesc(DATATYPE_VECTOR, numElements, semantic, dynamic);
  uint32_t dataSize = sizeof(NMP::Vector3) * numElements;
  NMP::Vector3* data = (NMP::Vector3*)NMPMemoryAllocAligned(dataSize, NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(data);
  memcpy(data, values, dataSize);
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const NMP::Matrix34* values,
  uint32_t             numElements,
  Semantic             semantic,
  bool                 dynamic)
{
  Descriptor attrDesc(DATATYPE_MATRIX, numElements, semantic, dynamic);
  uint32_t dataSize = sizeof(NMP::Matrix34) * numElements;
  NMP::Matrix34* data = (NMP::Matrix34*)NMPMemoryAllocAligned(dataSize, NMP_VECTOR_ALIGNMENT);
  NMP_ASSERT(data);
  memcpy(data, values, dataSize);
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const MCOMMS::DebugLine* values,
  uint32_t                 numElements,
  Semantic                 semantic,
  bool                     dynamic)
{
  Descriptor attrDesc(DATATYPE_LINE, numElements, semantic, dynamic);
  uint32_t dataSize = sizeof(MCOMMS::DebugLine) * numElements;
  MCOMMS::DebugLine* data = (MCOMMS::DebugLine*)NMPMemoryAllocAligned(dataSize, 4);
  NMP_ASSERT(data);
  memcpy(data, values, dataSize);
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const MCOMMS::DebugTriangle* values,
  uint32_t                     numElements,
  Semantic                     semantic,
  bool                         dynamic)
{
  Descriptor attrDesc(DATATYPE_TRIANGLE, numElements, semantic, dynamic);
  uint32_t dataSize = sizeof(MCOMMS::DebugTriangle) * numElements;
  MCOMMS::DebugTriangle* data = (MCOMMS::DebugTriangle*)NMPMemoryAllocAligned(dataSize, 4);
  NMP_ASSERT(data);
  memcpy(data, values, dataSize);
  Attribute* attr = (Attribute*)NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
Attribute* Attribute::createAttribute(
  const char* string,
  Semantic    semantic,
  bool        dynamic)
{
  size_t len = strlen(string);
  len++; // save space for a term char
  Descriptor attrDesc(DATATYPE_CHAR, (uint32_t)len, semantic, dynamic);
  size_t dataSize = sizeof(char) * len;
  char* data = (char*) NMPMemoryAllocAligned(dataSize, 4);
  NMP_ASSERT(data);
  memcpy(data, string, dataSize - 1);
  data[len - 1] = 0;
  Attribute* attr = (Attribute*) NMPMemoryAllocAligned(sizeof(Attribute), 16);
  NMP_ASSERT(attr);
  attr->init(attrDesc, data);
  return attr;
}

//----------------------------------------------------------------------------------------------------------------------
const char* Attribute::getSemanticDesc(Semantic semantic)
{
  const char* semanticStr = 0;
  switch (semantic)
  {
  case Attribute::SEMANTIC_PATH_HINT :
    semanticStr = "SEMANTIC_PATH_HINT";
    break;
  case Attribute::SEMANTIC_BONE_MATRICES :
    semanticStr = "SEMANTIC_BONE_MATRICES";
    break;
  case Attribute::SEMANTIC_TRANSFORM :
    semanticStr = "SEMANTIC_TRANSFORM";
    break;
  case Attribute::SEMANTIC_COLOUR :
    semanticStr = "SEMANTIC_COLOUR";
    break;
  case Attribute::SEMANTIC_NAME :
    semanticStr = "SEMANTIC_NAME";
    break;
  case Attribute::SEMANTIC_PARENT_OBJECT_ID :
    semanticStr = "SEMANTIC_PARENT_OBJECT_ID";
    break;
  case Attribute::SEMANTIC_PARENT_BONE_ID :
    semanticStr = "SEMANTIC_PARENT_BONE_ID";
    break;
  case Attribute::SEMANTIC_PARENT_OFFSET :
    semanticStr = "SEMANTIC_PARENT_OFFSET";
    break;
  case Attribute::SEMANTIC_STRING_HINT :
    semanticStr = "SEMANTIC_STRING_HINT";
    break;
  case Attribute::SEMANTIC_INSTANCEID :
    semanticStr = "SEMANTIC_INSTANCEID";
    break;
  case Attribute::SEMANTIC_ANIMSET_INDEX :
    semanticStr = "SEMANTIC_ANIMSET_INDEX";
    break;
  case Attribute::SEMANTIC_PHYSICS_SHAPE_TYPE :
    semanticStr = "SEMANTIC_PHYSICS_SHAPE_TYPE";
    break;
  case Attribute::SEMANTIC_LENGTH :
    semanticStr = "SEMANTIC_LENGTH";
    break;
  case Attribute::SEMANTIC_DEPTH :
    semanticStr = "SEMANTIC_DEPTH";
    break;
  case Attribute::SEMANTIC_HEIGHT :
    semanticStr = "SEMANTIC_HEIGHT";
    break;
  case Attribute::SEMANTIC_DENSITY :
    semanticStr = "SEMANTIC_DENSITY";
    break;
  case Attribute::SEMANTIC_RADIUS :
    semanticStr = "SEMANTIC_RADIUS";
    break;
  case Attribute::SEMANTIC_STATIC_FRICTION :
    semanticStr = "SEMANTIC_STATIC_FRICTION";
    break;
  case Attribute::SEMANTIC_DYNAMIC_FRICTION :
    semanticStr = "SEMANTIC_DYNAMIC_FRICTION";
    break;
  case Attribute::SEMANTIC_RESTITUTION :
    semanticStr = "SEMANTIC_RESTITUTION";
    break;
  case Attribute::SEMANTIC_SKIN_WIDTH :
    semanticStr = "SEMANTIC_SKIN_WIDTH";
    break;
  case Attribute::SEMANTIC_MAX_ANGULAR_VELOCITY :
    semanticStr = "SEMANTIC_LINEAR_VELOCITY";
    break;
  case Attribute::SEMANTIC_ANGULAR_DAMPING :
    semanticStr = "SEMANTIC_ANGULAR_DAMPING";
    break;
  case Attribute::SEMANTIC_LINEAR_DAMPING :
    semanticStr = "SEMANTIC_LINEAR_DAMPING";
    break;
  case Attribute::SEMANTIC_DYNAMIC_PHYSICS_OBJECT  :
    semanticStr = "SEMANTIC_DYNAMIC_PHYSICS_OBJECT ";
    break;
  case Attribute::SEMANTIC_CONSTRAINED_PHYSICS_OBJECT :
    semanticStr = "SEMANTIC_CONSTRAINED_PHYSICS_OBJECT";
    break;
  case Attribute::SEMANTIC_CONSTRAINT_GLOBAL_TRANSFORM :
    semanticStr = "SEMANTIC_CONSTRAINT_GLOBAL_TRANSFORM";
    break;
  case Attribute::SEMANTIC_CONSTRAINT_LOCAL_TRANSFORM :
    semanticStr = "SEMANTIC_CONSTRAINT_LOCAL_TRANSFORM";
    break;
  case Attribute::SEMANTIC_CONSTRAINT_STIFFNESS :
    semanticStr = "SEMANTIC_CONSTRAINT_STIFFNESS";
    break;
  case Attribute::SEMANTIC_CONSTRAINT_DAMPING :
    semanticStr = "SEMANTIC_CONSTRAINT_DAMPING";
    break;
  case Attribute::SEMANTIC_GRAVITY_ENABLED :
    semanticStr = "SEMANTIC_GRAVITY_ENABLED";
    break;
  case Attribute::SEMANTIC_ASSET_SCALE:
    semanticStr = "SEMANTIC_ASSET_SCALE";
    break;
  case Attribute::SEMANTIC_POSITION_SOLVER_ITERATION_COUNT :
    semanticStr = "SEMANTIC_POSITION_SOLVER_ITERATION_COUNT";
    break;
  case Attribute::SEMANTIC_VELOCITY_SOLVER_ITERATION_COUNT :
    semanticStr = "SEMANTIC_VELOCITY_SOLVER_ITERATION_COUNT";
    break;
  case Attribute::SEMANTIC_SLEEP_THRESHOLD:
    semanticStr = "SEMANTIC_SLEEP_THRESHOLD";
    break;
  case Attribute::SEMANTIC_GRAVITY_VECTOR:
    semanticStr = "SEMANTIC_GRAVITY_VECTOR";
    break;
  case Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE :
    semanticStr = "SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE";
    break;
  case Attribute::SEMANTIC_DEBUGLINES:
    semanticStr = "SEMANTIC_DEBUGLINES";
    break;

  default:
    semanticStr = "(unknown semantic)";
    break;
  }
  return semanticStr;
}

} // namespace MCCOMMS

//----------------------------------------------------------------------------------------------------------------------
