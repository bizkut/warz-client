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
#ifndef MR_NODE_OPERATOR_ROLLDOWNSTAIRS_H
#define MR_NODE_OPERATOR_ROLLDOWNSTAIRS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "euphoria/erAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorRollDownStairsDef
/// \brief Container for rollDownStairs operator definition.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorRollDownStairsDef : public MR::AttribData
{

public:
  static AttribDataOperatorRollDownStairsDef* create(NMP::MemoryAllocator* allocator, uint16_t refCount);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorRollDownStairsDef* init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount);

  NM_INLINE AttribDataOperatorRollDownStairsDef() { setType(ATTRIB_TYPE_OPERATOR_ROLLDOWNSTAIRS_DEF); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_ROLLDOWNSTAIRS_DEF; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  static bool copy(MR::AttribData* source, MR::AttribData* dest);

  // Attributes
  float m_minEnterAngleWithNormalToApplyRollPose;
  float m_maxEnterAngleWithNormalToApplyRollPose;
  float m_minExitAngleWithNormalToApplyRollPose;
  float m_maxExitAngleWithNormalToApplyRollPose;
  float m_minTangentialVelocityToApplyRollPose;
  float m_maxTangentialVelocityToApplyRollPose;
  float m_minAngularVelocityToApplyRollPose;
  float m_maxAngularVelocityToApplyRollPose;
  float m_minMotionRatioToBeConsideredRolling;
  float m_poseWeightMultiplier;
  float m_sideRollAmount;
};

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.
MR::AttribData* nodeOperatorRollDownStairsOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, /// The output pin we have been asked to update. We have 1:
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the output control parameter pins.
enum NodeOperatorRollDownStairsOutCPPinIDs
{
  NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_ROLLPOSEANIMWEIGHT = 0,
  NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_BALANCEDESIREDVELOCITY,
  NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_ISROLLING,

  NODE_OPERATOR_ROLLDOWNSTAIRS_OUT_CP_PINID_COUNT
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_ROLLDOWNSTAIRS_H
//----------------------------------------------------------------------------------------------------------------------
