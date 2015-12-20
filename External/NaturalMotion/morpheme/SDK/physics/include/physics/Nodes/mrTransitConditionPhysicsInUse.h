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
#ifndef MR_TRANSIT_CONDITION_PHYSICS_IN_USE_H
#define MR_TRANSIT_CONDITION_PHYSICS_IN_USE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefPhysicsInUse
/// \brief Transition condition definition for TransitConditionPhysicsInUse.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefPhysicsInUse : public TransitConditionDef
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
  NM_INLINE bool getOnInUse() const { return m_onInUse; }
  NM_INLINE void setOnInUse(bool onInUse) { m_onInUse = onInUse; }

protected:
  TransitConditionDefPhysicsInUse() { m_type = TRANSCOND_PHYSICS_IN_USE_ID; };

  bool m_onInUse;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionPhysicsInUse
/// \brief Condition satisfied when a physics body is assigned.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionPhysicsInUse : public TransitCondition
{
  friend class TransitConditionDefPhysicsInUse;

protected:
  TransitConditionPhysicsInUse();
  ~TransitConditionPhysicsInUse();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_PHYSICS_IN_USE_H
//----------------------------------------------------------------------------------------------------------------------
