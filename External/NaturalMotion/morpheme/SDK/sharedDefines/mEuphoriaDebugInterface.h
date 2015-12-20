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
#ifndef M_EUPHORIA_DEBUG_INTERFACE_H
#define M_EUPHORIA_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
#include "mCoreDebugInterface.h"
#include "mPhysicsDebugInterface.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mEuphoriaDebugInterface.h
/// \brief 
/// \ingroup EuphoriaModule
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

// ensure all these structs are the same layout for all compilers
#pragma pack(push, 4)

const uint8_t INVALID_LIMB_INDEX = 0xFF;

//----------------------------------------------------------------------------------------------------------------------
class LimbControlAmounts
{
public:
  void setControlAmount(uint32_t control, float amount) {m_controlAmounts[control] = amount;}
  void setToZero();

  LimbControlAmounts operator*(float f) const;
  void operator*=(float f);

  LimbControlAmounts operator+(const LimbControlAmounts& other) const;
  void operator+=(const LimbControlAmounts& other);

  static LimbControlAmounts* endianSwap(void *buffer);

  /// Use a fixed array size here so that the size is know in builtin.types. In practice, only
  /// controls up to numControls will be processed.
  static const uint32_t MAX_NUM_DEBUG_CONTROLS = 28;
  float m_controlAmounts[MAX_NUM_DEBUG_CONTROLS];
};

//----------------------------------------------------------------------------------------------------------------------
enum EuphoriaColourOverrideTypeIds
{
  kLimbColourOverrideTypeId = MR::PhysicsPartOverrideColour::kNumOverrideTypeIDs,
  kNumEuphoriaOverrideTypeIds
};

//----------------------------------------------------------------------------------------------------------------------
enum EuphoriaObjectDataTypes
{
  kEuphoriaModuleDataType,
  kEuphoriaBodyDataType,
  kEuphoriaLimbDataType,
  kEuphoriaDebugControlDataType,
  kEuphoriaLimbControlAmountsDataType,
  kNumEuphoriaObjectDataTypes
};

//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaNetworkPersistentData
{
  uint32_t m_numModules;
  uint32_t m_numControlTypes;
  MR::StringToken m_controlTypeName[LimbControlAmounts::MAX_NUM_DEBUG_CONTROLS];

  /// \brief Endian swaps in place on buffer.
  static inline EuphoriaNetworkPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static inline EuphoriaNetworkPersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaModulePersistentData
{
  MR::StringToken m_moduleName;
  MR::StringToken m_parentModuleName;

  /// \brief Endian swaps in place on buffer.
  static EuphoriaModulePersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static EuphoriaModulePersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaModuleFrameData
{
  bool m_moduleDebugEnabled;

  /// \brief Endian swaps in place on buffer.
  static EuphoriaModuleFrameData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static EuphoriaModuleFrameData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaLimbControlAmountsFrameData
{
  ER::LimbControlAmounts m_limbControlAmounts;
  float m_limbStiffnessFraction;
  uint8_t m_limbIndex;

  /// \brief Endian swaps in place on buffer.
  static EuphoriaLimbControlAmountsFrameData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static EuphoriaLimbControlAmountsFrameData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Details of the Euphoria body
//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaBodyPersistentData
{
  uint32_t m_rootLimbIndex;
  uint32_t m_numLimbs;
  uint32_t m_numArms;
  uint32_t m_numHeads;
  uint32_t m_numLegs;
  uint32_t m_numSpines;
  float m_mass;

  float m_selfAvoidanceRadius;

  /// \brief Endian swaps in place on buffer.
  static EuphoriaBodyPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static EuphoriaBodyPersistentData *deserialiseTx(void *buffer);
};

enum EuphoriaLimbSide
{
  kEuphoriaCentreLimb,
  kEuphoriaLeftLimb,
  kEuphoriaRightLimb,
};
//----------------------------------------------------------------------------------------------------------------------
/// \brief Details of the Euphoria body
//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaLimbPersistentData
{
  NMP::Matrix34 m_endEffectorlocalPose;
  NMP::Matrix34 m_rootEffectorlocalPose;
  MR::StringToken m_name;

  uint32_t m_type;
  uint32_t m_limbSide;
  uint32_t m_rootIndex;
  uint32_t m_endIndex; 
  uint32_t m_baseIndex;
  uint32_t m_isRootLimb; 

  /// \brief Endian swaps in place on buffer.
  static EuphoriaLimbPersistentData *endianSwap(void *buffer);
  /// \brief Deserialises in place on buffer memory.
  static EuphoriaLimbPersistentData *deserialiseTx(void *buffer);
};

//----------------------------------------------------------------------------------------------------------------------
typedef uint8_t DebugControlID;

//----------------------------------------------------------------------------------------------------------------------
enum
{
  kNoControlFlag        = 0x00000000,
  kPerLimbControlFlag   = 0x00000001, ///< Indicates control is has a per limb value, if this is not specified
                                      ///  then it is assumed the control is per character
  kDebugDrawControlFlag = 0x00000002, ///< Indicates control is related to global debug draw options
  kNumControlFlags      = 0x00000004
};

typedef uint32_t DebugControlFlags;

//----------------------------------------------------------------------------------------------------------------------
enum DebugControlDataType
{
  kBoolControlDataType,
  kInt32ControlDataType,
  kUInt32ControlDataType,
  kFloatControlDataType,
  kNumControlDataTypes
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Persistent details of a Euphoria debug control
//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaDebugControlPersistentData
{
  // The member order is deliberate going from largest to smallest size member to help keep the packet smaller,
  // although inside a #pragma pack(4) region this is not strictly necessary.

  uint32_t        m_controlFlags;
  MR::StringToken m_controlName;
  uint8_t         m_controlId;
  uint8_t         m_controlDataType;

  /// \brief Serialises in place on buffer memory.
  static EuphoriaDebugControlPersistentData* serialiseTx(
    void*                buffer,
    MR::StringToken      name,
    DebugControlID       id,
    DebugControlFlags    flags,
    DebugControlDataType dataType);
  /// \brief Deserialises in place on buffer memory.
  static EuphoriaDebugControlPersistentData* deserialiseTx(void* buffer);

protected:
  /// \brief Endian swaps in place on buffer.
  static EuphoriaDebugControlPersistentData* endianSwap(void* buffer);
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Dynamic details of a Euphoria debug control. This structure will be sent from the runtime
/// to inform connect of the current value and sent to the runtime when connect sets a control value.
//----------------------------------------------------------------------------------------------------------------------
struct EuphoriaDebugControlFrameData
{
  // The member order is deliberate going from largest to smallest size member to help keep the
  // packet smaller, although inside a #pragma pack(4) region this is not strictly necessary.

  union
  {
    bool      m_bool;
    int32_t   m_int32;
    uint32_t  m_uint32;
    float     m_float;
  } m_value; //< Check m_control type to see which one is valid.

  uint8_t m_controlId;
  uint8_t m_limbIndex;
  uint8_t m_controlDataType; //< Can't get the type from the persistent packet as it is needed to endian swap.

  /// \brief Serialises in place on buffer memory for kBoolControlDataType.
  static EuphoriaDebugControlFrameData* serialiseTx(
    void*   buffer,
    uint8_t id,
    uint8_t limbIndex,
    bool    value);
  /// \overload for kInt32ControlDataType.
  static EuphoriaDebugControlFrameData* serialiseTx(
    void*   buffer,
    uint8_t id,
    uint8_t limbIndex,
    int32_t value);
  /// \overload for kUInt32ControlDataType.
  static EuphoriaDebugControlFrameData* serialiseTx(
    void*    buffer,
    uint8_t  id,
    uint8_t  limbIndex,
    uint32_t value);
  /// \overload for kFloatControlDataType.
  static EuphoriaDebugControlFrameData* serialiseTx(
    void*   buffer,
    uint8_t id,
    uint8_t limbIndex,
    float   value);

  /// \brief Deserialises in place on buffer memory.
  static EuphoriaDebugControlFrameData* deserialiseTx(void* buffer);

protected:
  /// \brief Endian swaps in place on buffer.
  static EuphoriaDebugControlFrameData* endianSwap(void* buffer);
};

// restore the packing alignment set before this file
#pragma pack(pop)

} // namespace ER

#include "sharedDefines/mEuphoriaDebugInterface.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // M_EUPHORIA_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
