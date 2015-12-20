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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_TRANSIT_CONDITION_CROSSED_DURATION_FRACTION_H
#define MR_TRANSIT_CONDITION_CROSSED_DURATION_FRACTION_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribAddress.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

class AttribDataPlaybackPos;

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefCrossedDurationFraction
/// \brief Transition condition definition for TransitConditionCrossedDurationFraction.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefCrossedDurationFraction : public TransitConditionDef
{
public:
  static bool defDislocate(TransitConditionDef* tcDef);
  static bool defLocate(TransitConditionDef* tcDef);
  static NMP::Memory::Format instanceGetMemoryRequirements(TransitConditionDef* tcDef);
  static TransitCondition* instanceInit(
    TransitConditionDef*   tcDef,
    NMP::Memory::Resource& memRes,
    Network*               net,
    NodeID                 sourceNodeID);
  static bool instanceUpdate(
    TransitCondition*    tc,
    TransitConditionDef* tcDef,
    Network*             net,
    NodeID               smActiveNodeID);

  static void instanceQueueDeps(
    TransitCondition*    tc,
    TransitConditionDef* tcDef,
    Network*             net,
    NodeID               smActiveNodeID);
  static void instanceReset(TransitCondition* tc, TransitConditionDef* tcDef);

  NM_INLINE const AttribAddress* getSourceNodeFractionalPosAttribAddress() const { return &m_sourceNodeFractionalPosAttribAddress; }
  NM_INLINE void setSourceNodeFractionalPosAttribAddress(AttribAddress& address) { m_sourceNodeFractionalPosAttribAddress = address; }
  NM_INLINE float getTriggerFraction() const { return m_durationFractionTrigger; }
  NM_INLINE void setTriggerFraction(float durationFractionTrigger) { m_durationFractionTrigger = durationFractionTrigger; }

  /// \brief Evaluate the condition against the current time fraction and the condition state from the last frame.
  static bool evaluateCondition(
    float lastUpdateTimeFraction,
    float durationFractionTrigger,
    float currentTimeFraction,
    const AttribDataPlaybackPos* timeFractionAttrib);

protected:
  TransitConditionDefCrossedDurationFraction() { m_type = TRANSCOND_CROSSED_DURATION_FRACTION_ID; };

  AttribAddress m_sourceNodeFractionalPosAttribAddress; ///< Address of the fractional playback position attribute of the source node of the transition.
  float         m_durationFractionTrigger;              ///< What fraction through playback of source node must transition start at.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionCrossedDurationFraction
/// \brief Condition satisfied when source Nodes time fraction passes across a trigger boundary.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
///
/// Can be un-set after this if it continues to be monitored.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionCrossedDurationFraction : public TransitCondition
{
  friend class TransitConditionDefCrossedDurationFraction;

protected:
  TransitConditionCrossedDurationFraction();
  ~TransitConditionCrossedDurationFraction();

protected:
  float m_lastUpdateTimeFraction;  ///< The time fraction value from the last frames update.
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_CROSSED_DURATION_FRACTION_H
//----------------------------------------------------------------------------------------------------------------------
