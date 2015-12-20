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
#ifndef MR_TRANSIT_CONDITION_RAY_HIT_H
#define MR_TRANSIT_CONDITION_RAY_HIT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefRayHit
/// \brief Transition condition definition for TransitConditionRayHit.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefRayHit : public TransitConditionDef
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
  int32_t m_hitMode; // 0 = true if miss, 1 = true if hit, 2 = true if hit moving, 3 = true if hit stationary
  bool    m_useLocalOrientation;
  float   m_rayStartX;
  float   m_rayStartY;
  float   m_rayStartZ;
  float   m_rayDeltaX;
  float   m_rayDeltaY;
  float   m_rayDeltaZ;

protected:
  TransitConditionDefRayHit() { m_type = TRANSCOND_RAY_HIT_ID; };

};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionRayHit
/// \brief Condition satisfied when in/out of contact with the ground for a certain time.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionRayHit : public TransitCondition
{
  friend class TransitConditionDefRayHit;

protected:
  TransitConditionRayHit();
  ~TransitConditionRayHit();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_RAY_HIT_H
//----------------------------------------------------------------------------------------------------------------------
