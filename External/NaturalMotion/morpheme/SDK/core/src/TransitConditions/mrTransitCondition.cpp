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
#include "morpheme/TransitConditions/mrTransitCondition.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// TransitConditionDef functions.
//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDef::locate(TransitConditionDef* tcDef)
{
  NMP_ASSERT(tcDef);
  NMP::endianSwap(tcDef->m_type);
  NMP::endianSwap(tcDef->m_invert);
  relocate(tcDef);

  NMP_ASSERT(tcDef->m_locateFn);
  tcDef->m_locateFn(tcDef);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDef::dislocate(TransitConditionDef* tcDef)
{
  NMP_ASSERT(tcDef);
  Manager::TransitCondRegistryEntry* transitRegEntry = Manager::getInstance().getTransitCondRegistryEntry(tcDef->m_type);
  NMP_ASSERT(transitRegEntry && transitRegEntry->m_dislocateFn);

  // reset the function pointers to NULL as they are reconstructed during locate(). This preserves binary invariance.
  tcDef->m_locateFn = NULL;
  tcDef->m_dislocateFn = NULL;
  tcDef->m_relocateFn = NULL;
  tcDef->m_getMemReqsFn = NULL;
  tcDef->m_initFn = NULL;
  tcDef->m_updateFn = NULL;
  tcDef->m_queueDepsFn = NULL;
  tcDef->m_resetFn = NULL;

  transitRegEntry->m_dislocateFn(tcDef);
  NMP::endianSwap(tcDef->m_type);
  NMP::endianSwap(tcDef->m_invert);

  // The function values are retrieved from the MR::Manager when the def is located so the value after dislocation isn't
  // used. Null the function pointers so that the binary created is always consistent.
  tcDef->m_locateFn = 0;
  tcDef->m_dislocateFn = 0;
  tcDef->m_relocateFn = 0;
  tcDef->m_getMemReqsFn = 0;
  tcDef->m_initFn = 0;
  tcDef->m_updateFn = 0;
  tcDef->m_queueDepsFn = 0;
  tcDef->m_resetFn = 0;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDef::relocate(TransitConditionDef* tcDef)
{
  NMP_ASSERT(tcDef);
  Manager::TransitCondRegistryEntry* transitRegEntry = Manager::getInstance().getTransitCondRegistryEntry(tcDef->m_type);
  NMP_ASSERT(transitRegEntry);

  tcDef->m_locateFn = transitRegEntry->m_locateFn;
  tcDef->m_dislocateFn = transitRegEntry->m_dislocateFn;
  tcDef->m_relocateFn = transitRegEntry->m_relocateFn;
  tcDef->m_getMemReqsFn = transitRegEntry->m_memReqsFn;
  tcDef->m_initFn = transitRegEntry->m_initFn;
  tcDef->m_updateFn = transitRegEntry->m_updateFn;
  tcDef->m_queueDepsFn = transitRegEntry->m_queueDepsFn;
  tcDef->m_resetFn = transitRegEntry->m_resetFn;

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDef::defaultInstanceReset(
  TransitCondition*    tc, 
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  NMP_ASSERT(tc);
  tc->condSetState(false);
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDef::defaultInstanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* NMP_UNUSED(tcDef),
  Network*             NMP_UNUSED(net),
  NodeID               NMP_UNUSED(smActiveNodeID))
{
  return tc->condIsSatisfied();
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDef::defaultInstanceQueueDeps(
  TransitCondition*    NMP_UNUSED(tc),
  TransitConditionDef* NMP_UNUSED(tcDef),
  Network*             NMP_UNUSED(net),
  NodeID               NMP_UNUSED(smActiveNodeID))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDef::defaultInstanceGetMemoryRequirements(
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(TransitCondition), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
