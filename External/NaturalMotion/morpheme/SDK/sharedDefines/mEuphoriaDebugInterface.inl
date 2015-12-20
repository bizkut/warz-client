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
#ifndef M_EUPHORIA_DEBUG_INTERFACE_INL
#define M_EUPHORIA_DEBUG_INTERFACE_INL
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mEuphoriaDebugInterface.inl
/// \brief 
/// \ingroup EuphoriaModule
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

// ensure all these structs are the same layout for all compilers
#pragma pack(push, 4)

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaNetworkPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaNetworkPersistentData *EuphoriaNetworkPersistentData::endianSwap(void *buffer)
{
  EuphoriaNetworkPersistentData* persistentData = static_cast<EuphoriaNetworkPersistentData*>(buffer);

  NMP::netEndianSwap(persistentData->m_numModules);
  NMP::netEndianSwap(persistentData->m_numControlTypes);
  for (uint32_t  i = 0 ; i != LimbControlAmounts::MAX_NUM_DEBUG_CONTROLS ; ++i)
  {
    NMP::netEndianSwap(persistentData->m_controlTypeName);
  }

  return persistentData;
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaNetworkPersistentData *EuphoriaNetworkPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaModulePersistentData
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaModulePersistentData* EuphoriaModulePersistentData::endianSwap(void *buffer)
{
  EuphoriaModulePersistentData* persistentData = static_cast<EuphoriaModulePersistentData*>(buffer);

  NMP::netEndianSwap(persistentData->m_moduleName);
  NMP::netEndianSwap(persistentData->m_parentModuleName);

  return persistentData;
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaModulePersistentData* EuphoriaModulePersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaModulePersistentData
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaModuleFrameData *EuphoriaModuleFrameData::endianSwap(void *buffer)
{
  EuphoriaModuleFrameData* frameData = static_cast<EuphoriaModuleFrameData*>(buffer);
  NMP::netEndianSwap(frameData->m_moduleDebugEnabled);
  return frameData;
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaModuleFrameData *EuphoriaModuleFrameData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaLimbControlAmountsFrameData
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaLimbControlAmountsFrameData *EuphoriaLimbControlAmountsFrameData::endianSwap(void *buffer)
{
  EuphoriaLimbControlAmountsFrameData* frameData = static_cast<EuphoriaLimbControlAmountsFrameData*>(buffer);
  NMP::netEndianSwap(frameData->m_limbIndex);
  NMP::netEndianSwap(frameData->m_limbStiffnessFraction);
  LimbControlAmounts::endianSwap(&frameData->m_limbControlAmounts);
  return frameData;
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaLimbControlAmountsFrameData *EuphoriaLimbControlAmountsFrameData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaBodyPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaBodyPersistentData* EuphoriaBodyPersistentData::endianSwap(void *buffer)
{
  EuphoriaBodyPersistentData* persistentData = static_cast<EuphoriaBodyPersistentData*>(buffer);

  NMP::netEndianSwap(persistentData->m_rootLimbIndex);
  NMP::netEndianSwap(persistentData->m_numLimbs);
  NMP::netEndianSwap(persistentData->m_numArms);
  NMP::netEndianSwap(persistentData->m_numHeads);
  NMP::netEndianSwap(persistentData->m_numLegs);
  NMP::netEndianSwap(persistentData->m_numSpines);
  NMP::netEndianSwap(persistentData->m_mass);

  NMP::netEndianSwap(persistentData->m_selfAvoidanceRadius);

  return persistentData;
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaBodyPersistentData* EuphoriaBodyPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaLimbPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaLimbPersistentData* EuphoriaLimbPersistentData::endianSwap(void *buffer)
{
  EuphoriaLimbPersistentData* persistentData = static_cast<EuphoriaLimbPersistentData*>(buffer);

  NMP::netEndianSwap(persistentData->m_endEffectorlocalPose);
  NMP::netEndianSwap(persistentData->m_rootEffectorlocalPose);
  NMP::netEndianSwap(persistentData->m_name);
  NMP::netEndianSwap(persistentData->m_type);
  NMP::netEndianSwap(persistentData->m_rootIndex);
  NMP::netEndianSwap(persistentData->m_endIndex);
  NMP::netEndianSwap(persistentData->m_baseIndex);
  NMP::netEndianSwap(persistentData->m_isRootLimb);

  return persistentData;
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaLimbPersistentData* EuphoriaLimbPersistentData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaDebugControlPersistentData
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlPersistentData* EuphoriaDebugControlPersistentData::serialiseTx(
  void*                 buffer,
  MR::StringToken       name,
  uint8_t               id,
  DebugControlFlags     flags,
  DebugControlDataType  dataType)
{
  EuphoriaDebugControlPersistentData* packet = static_cast<EuphoriaDebugControlPersistentData*>(buffer);

  packet->m_controlFlags = flags;
  packet->m_controlName = name;
  packet->m_controlId = id;
  packet->m_controlDataType = static_cast<uint8_t>(dataType);

  return endianSwap(packet);
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlPersistentData* EuphoriaDebugControlPersistentData::deserialiseTx(void* buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlPersistentData* EuphoriaDebugControlPersistentData::endianSwap(void* buffer)
{
  EuphoriaDebugControlPersistentData* packet = static_cast<EuphoriaDebugControlPersistentData*>(buffer);

  NMP::netEndianSwap(packet->m_controlFlags);
  NMP::netEndianSwap(packet->m_controlName);

  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
/// EuphoriaDebugControlFrameData
//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlFrameData* EuphoriaDebugControlFrameData::serialiseTx(
  void*   buffer,
  uint8_t id,
  uint8_t limbIndex,
  bool    value)
{
  EuphoriaDebugControlFrameData* packet = static_cast<EuphoriaDebugControlFrameData*>(buffer);

  packet->m_value.m_bool = value;
  packet->m_controlId = id;
  packet->m_limbIndex = limbIndex;
  packet->m_controlDataType = kBoolControlDataType;

  return endianSwap(packet);
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlFrameData* EuphoriaDebugControlFrameData::serialiseTx(
  void*   buffer,
  uint8_t id,
  uint8_t limbIndex,
  int32_t value)
{
  EuphoriaDebugControlFrameData* packet = static_cast<EuphoriaDebugControlFrameData*>(buffer);

  packet->m_value.m_int32 = value;
  packet->m_controlId = id;
  packet->m_limbIndex = limbIndex;
  packet->m_controlDataType = kInt32ControlDataType;

  return endianSwap(packet);
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlFrameData* EuphoriaDebugControlFrameData::serialiseTx(
  void*    buffer,
  uint8_t  id,
  uint8_t  limbIndex,
  uint32_t value)
{
  EuphoriaDebugControlFrameData* packet = static_cast<EuphoriaDebugControlFrameData*>(buffer);

  packet->m_value.m_uint32 = value;
  packet->m_controlId = id;
  packet->m_limbIndex = limbIndex;
  packet->m_controlDataType = kUInt32ControlDataType;

  return endianSwap(packet);
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlFrameData* EuphoriaDebugControlFrameData::serialiseTx(
  void*   buffer,
  uint8_t id,
  uint8_t limbIndex,
  float   value)
{
  EuphoriaDebugControlFrameData* packet = static_cast<EuphoriaDebugControlFrameData*>(buffer);

  packet->m_value.m_float = value;
  packet->m_controlId = id;
  packet->m_limbIndex = limbIndex;
  packet->m_controlDataType = kFloatControlDataType;

  return endianSwap(packet);
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlFrameData* EuphoriaDebugControlFrameData::deserialiseTx(void* buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
inline EuphoriaDebugControlFrameData* EuphoriaDebugControlFrameData::endianSwap(void* buffer)
{
  EuphoriaDebugControlFrameData* packet = static_cast<EuphoriaDebugControlFrameData*>(buffer);

  switch (packet->m_controlDataType)
  {
  case kBoolControlDataType:
    NMP::netEndianSwap(packet->m_value.m_bool);
    break;
  case kInt32ControlDataType:
    NMP::netEndianSwap(packet->m_value.m_int32);
    break;
  case kUInt32ControlDataType:
    NMP::netEndianSwap(packet->m_value.m_uint32);
    break;
  case kFloatControlDataType:
    NMP::netEndianSwap(packet->m_value.m_float);
    break;
  case kNumControlDataTypes:
  default:
    NMP_ASSERT_FAIL_MSG("Unknown euphoria control data type");
    break;
  }

  return packet;
}

//----------------------------------------------------------------------------------------------------------------------
inline void LimbControlAmounts::setToZero()
{
  memset(m_controlAmounts, 0, sizeof(m_controlAmounts));
}

//----------------------------------------------------------------------------------------------------------------------
inline LimbControlAmounts LimbControlAmounts::operator*(float f) const
{
  LimbControlAmounts result = *this;
  result *= f;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
inline void LimbControlAmounts::operator*=(float f)
{
  for (uint32_t i = 0 ; i != MAX_NUM_DEBUG_CONTROLS ; ++i)
  {
    m_controlAmounts[i] *= f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline LimbControlAmounts LimbControlAmounts::operator+(const LimbControlAmounts& other) const
{
  LimbControlAmounts result = *this;
  result += other;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
inline void LimbControlAmounts::operator+=(const LimbControlAmounts& other)
{
  for (uint32_t i = 0 ; i != MAX_NUM_DEBUG_CONTROLS ; ++i)
  {
    m_controlAmounts[i] += other.m_controlAmounts[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
inline LimbControlAmounts* LimbControlAmounts::endianSwap(void *buffer)
{
  LimbControlAmounts* limbControlAmounts = static_cast<LimbControlAmounts*>(buffer);

  for (uint32_t i = 0 ; i != MAX_NUM_DEBUG_CONTROLS ; ++i)
  {
    NMP::netEndianSwap(limbControlAmounts->m_controlAmounts[i]);
  }
  return limbControlAmounts;
}



// restore the packing alignment set before this file
#pragma pack(pop)

}

#endif
