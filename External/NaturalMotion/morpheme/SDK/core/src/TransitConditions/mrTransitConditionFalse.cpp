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
#include "morpheme/TransitConditions/mrTransitConditionFalse.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefFalse::defDislocate(TransitConditionDef* NMP_UNUSED(tcDef))
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefFalse::defLocate(TransitConditionDef* NMP_UNUSED(tcDef))
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
// Initialise this instance from the supplied TransitConditionDef
TransitCondition* TransitConditionDefFalse::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               NMP_UNUSED(net),
  NodeID                 NMP_UNUSED(sourceNodeID))
{
  NMP_ASSERT(tcDef);

  if (!memRes.ptr)
    return NULL;

  NMP::Memory::Format memReqs = TransitConditionDef::defaultInstanceGetMemoryRequirements(tcDef);
  memRes.align(memReqs);
  TransitConditionFalse* result = (TransitConditionFalse*) memRes.ptr;
  memRes.increment(memReqs);

  result->m_satisfied = false;
  result->m_type = TRANSCOND_FALSE_ID;

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
