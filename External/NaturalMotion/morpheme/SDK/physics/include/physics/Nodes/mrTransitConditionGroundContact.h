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
#ifndef MR_TRANSIT_CONDITION_GROUND_CONTACT_H
#define MR_TRANSIT_CONDITION_GROUND_CONTACT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefGroundContact
/// \brief Transition condition definition for TransitConditionGroundContact.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefGroundContact : public TransitConditionDef
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
  NM_INLINE float getTriggerTime() const { return m_triggerTime; }
  NM_INLINE void setTriggerTime(float triggerTime) { m_triggerTime = triggerTime; }
  NM_INLINE bool getOnGround() const { return m_onGround; }
  NM_INLINE void setOnGround(bool onGround) { m_onGround = onGround; }

protected:
  TransitConditionDefGroundContact() { m_type = TRANSCOND_GROUND_CONTACT_ID; };

  float m_triggerTime;
  bool  m_onGround;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionGroundContact
/// \brief Condition satisfied when Float Control Parameter value is less than our test value.
/// \ingroup NodesLibraryTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionGroundContact : public TransitCondition
{
  friend class TransitConditionDefGroundContact;

protected:
  TransitConditionGroundContact();
  ~TransitConditionGroundContact();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_GROUND_CONTACT_H
//----------------------------------------------------------------------------------------------------------------------
