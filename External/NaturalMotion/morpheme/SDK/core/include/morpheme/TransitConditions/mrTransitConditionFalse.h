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
#ifndef MR_TRANSIT_CONDITION_FALSE_H
#define MR_TRANSIT_CONDITION_FALSE_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionDefFalse
/// \brief Transition condition definition for TransitConditionOnRequest.
/// \ingroup CoreTransitConditionsModule
/// \see MR::TransitConditionDef
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionDefFalse : public TransitConditionDef
{
public:
  static bool defDislocate(TransitConditionDef* tcDef);
  static bool defLocate(TransitConditionDef* tcDef);
  static TransitCondition* instanceInit(
    TransitConditionDef*   tcDef,
    NMP::Memory::Resource& memRes,
    Network*               net,
    NodeID                 sourceNodeID);

protected:
  TransitConditionDefFalse() { m_type = TRANSCOND_FALSE_ID; };
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::TransitConditionFalse
/// \brief Condition set or un-set directly by the user of the Network.
/// \ingroup CoreTransitConditionsModule
/// \see MR::TransitCondition
//----------------------------------------------------------------------------------------------------------------------
class TransitConditionFalse : public TransitCondition
{
  friend class TransitConditionDefFalse;

protected:
  TransitConditionFalse();
  ~TransitConditionFalse();
};

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_TRANSIT_CONDITION_FALSE_H
//----------------------------------------------------------------------------------------------------------------------
