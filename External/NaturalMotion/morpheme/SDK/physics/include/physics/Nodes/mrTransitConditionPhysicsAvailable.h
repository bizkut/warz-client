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
#ifndef MR_TRANSIT_CONDITION_PHYSICS_AVAILABLE_H
#define MR_TRANSIT_CONDITION_PHYSICS_AVAILABLE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefPhysicsAvailable
/// \brief Transition condition definition for TransitConditionPhysicsAvailable.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefPhysicsAvailable : public TransitConditionDef
{
public:
  static bool defDislocate(TransitConditionDef* tcDef);
  static bool defLocate(TransitConditionDef* tcDef);
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
  NM_INLINE bool getOnAvailable() const { return m_onAvailable; }
  NM_INLINE void setOnAvailable(bool onAvailable) { m_onAvailable = onAvailable; }

protected:
  TransitConditionDefPhysicsAvailable() { m_type = TRANSCOND_PHYSICS_AVAILABLE_ID; };

  bool m_onAvailable;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionPhysicsAvailable
/// \brief Condition satisfied when a physics body is available/assigned.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionPhysicsAvailable : public TransitCondition
{
  friend class TransitConditionDefPhysicsAvailable;

protected:
  TransitConditionPhysicsAvailable();
  ~TransitConditionPhysicsAvailable();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_PHYSICS_AVAILABLE_H
//----------------------------------------------------------------------------------------------------------------------
