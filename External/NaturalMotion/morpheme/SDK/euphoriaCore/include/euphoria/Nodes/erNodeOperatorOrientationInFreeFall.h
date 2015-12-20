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
#ifdef _MSC_VER
#pragma once
#endif
#ifndef MR_NODE_OPERATOR_ORIENTATIONINFREEFALL_H
#define MR_NODE_OPERATOR_ORIENTATIONINFREEFALL_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "euphoria/erAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorOrientationInFreeFallDef
/// \brief Container for orientationInFreefall operator definition.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorOrientationInFreeFallDef : public MR::AttribData
{

public:
  static AttribDataOperatorOrientationInFreeFallDef* create(NMP::MemoryAllocator* allocator, uint16_t refCount);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorOrientationInFreeFallDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount);

  NM_INLINE AttribDataOperatorOrientationInFreeFallDef()
  {
    setType(ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_DEF);
    setRefCount(0);
  }

  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_DEF; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  static bool copy(MR::AttribData* source, MR::AttribData* dest);

  void endianSwap();

  // Attributes
  float m_startOrientationTime;
  float m_stopOrientationTime;

  float m_startOrientationTransitionTime;
  float m_stopOrientationTransitionTime;

  float m_weightOutputBeforeOrientation;
  float m_weightOutputDuringOrientation;
  float m_weightOutputAfterOrientation;

  bool m_startOrientationAtTimeBeforeImpact;
  bool m_stopOrientationAtTimeBeforeImpact;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorOrientationInFreeFallState
/// \brief Orientation in freefall operator state. Holds per node instance data that persists between frames.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorOrientationInFreeFallState : public MR::AttribData
{
public:
  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorOrientationInFreeFallState* init(
    NMP::Memory::Resource& resource,
    uint16_t refCount = 0);

  NM_INLINE AttribDataOperatorOrientationInFreeFallState()
  {
    setType(ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_STATE);
    setRefCount(0);
  }

  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_STATE; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  static MR::AttribDataHandle create(NMP::MemoryAllocator* allocator);
  void endianSwap();

  float m_timeInFreefall;
};


//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.
void nodeOperatorOrientationInFreeFallInitInstance(MR::NodeDef* node, MR::Network* net);

MR::AttribData* nodeOperatorOrientationInFreeFallOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, /// The output pin we have been asked to update. We have 1:
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the input control parameter pins.
enum NodeOperatorOrientationInFreeFallInCPPinIDs
{
  NODE_OPERATOR_ORIENTATIONINFREEFALL_IN_CP_PINID_ORIENTATION = 0,
  NODE_OPERATOR_ORIENTATIONINFREEFALL_IN_CP_PINID_TIMETOIMPACT = 1,

  NODE_OPERATOR_ORIENTATIONINFREEFALL_IN_CP_PINID_MAX
};

// Enumerating the output control parameter pins.
enum NodeOperatorOrientationInFreeFallOutCPPinIDs
{
  NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_ORIENTATION = 0,
  NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_WEIGHT = 1,

  NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_MAX
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_ORIENTATIONINFREEFALL_H
//----------------------------------------------------------------------------------------------------------------------
