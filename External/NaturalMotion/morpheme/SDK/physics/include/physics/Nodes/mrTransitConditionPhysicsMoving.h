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
#ifndef MR_TRANSIT_CONDITION_MOVING_H
#define MR_TRANSIT_CONDITION_MOVING_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefPhysicsMoving
/// \brief Transition condition definition for TransitConditionPhysicsMoving.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefPhysicsMoving : public TransitConditionDef
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
  static void instanceReset(TransitCondition* tc, TransitConditionDef* tcDef);

  NM_INLINE float getVelocityThreshold() const { return m_VelocityThreshold; }
  NM_INLINE void setVelocityThreshold(float _fVelocityThreshold) { m_VelocityThreshold = _fVelocityThreshold; }
  NM_INLINE float getAngularVelocityThreshold() const { return m_AngularVelocityThreshold; }
  NM_INLINE void setAngularVelocityThreshold(float _fVelocityThreshold) { m_AngularVelocityThreshold = _fVelocityThreshold; }
  NM_INLINE uint32_t getMinFrameCount() const { return m_minFrameCountBelowThresholds; }
  NM_INLINE void setMinFrameCount(uint32_t _Value) { m_minFrameCountBelowThresholds = _Value; }
  NM_INLINE bool getOnNotSet() const { return m_onNotSet; }
  NM_INLINE void setOnNotSet(bool onNotSet) { m_onNotSet = onNotSet; }

protected:
  TransitConditionDefPhysicsMoving() { m_type = TRANSCOND_PHYSICS_MOVING_ID; };

  float         m_VelocityThreshold;                    ///< Might be used to tweak when the Moving-condition evaluates as 
                                                        ///< being fulfilled - the minimum velocity a joint has to have to
                                                        ///< flag the rig as "moving"
  float         m_AngularVelocityThreshold;
  uint32_t      m_minFrameCountBelowThresholds;         ///< the check routine gets reset whenever the current's frame average
                                                        ///< does not dip below thresholds
  bool          m_onNotSet;                             ///< Checking if currently not moving
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionPhysicsMoving
/// \brief Condition satisfied when source Nodes physics parts are dipping below the given threshold velocities for a given time.
///
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
///
/// Can be un-set after this if it continues to be monitored.
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionPhysicsMoving: public TransitCondition
{
  friend class TransitConditionDefPhysicsMoving;

protected:
  TransitConditionPhysicsMoving();
  ~TransitConditionPhysicsMoving();

protected:
  uint32_t m_numFramesVelocityConditionSatisfied;
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_MOVING_H
//----------------------------------------------------------------------------------------------------------------------
