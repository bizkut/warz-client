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
#include "morpheme/TransitConditions/mrTransitConditionCrossedDurationFraction.h"
#include "morpheme/mrManager.h"
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefCrossedDurationFraction::defDislocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefCrossedDurationFraction* def = (TransitConditionDefCrossedDurationFraction*)tcDef;
  NMP::endianSwap(def->m_durationFractionTrigger);
  def->m_sourceNodeFractionalPosAttribAddress.endianSwap();
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefCrossedDurationFraction::defLocate(
  TransitConditionDef* tcDef)
{
  TransitConditionDefCrossedDurationFraction* def = (TransitConditionDefCrossedDurationFraction*)tcDef;
  NMP::endianSwap(def->m_durationFractionTrigger);
  def->m_sourceNodeFractionalPosAttribAddress.endianSwap();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefCrossedDurationFraction::instanceGetMemoryRequirements(
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(TransitConditionCrossedDurationFraction), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
TransitCondition* TransitConditionDefCrossedDurationFraction::instanceInit(
  TransitConditionDef*   tcDef,
  NMP::Memory::Resource& memRes,
  Network*               net,
  NodeID                 sourceNodeID)
{
  NMP_ASSERT(tcDef);
  TransitConditionCrossedDurationFraction* result = NULL;
  TransitConditionDefCrossedDurationFraction* def = static_cast <TransitConditionDefCrossedDurationFraction*> (tcDef);

  if (memRes.ptr)
  {
    NMP::Memory::Format memReqs = instanceGetMemoryRequirements(tcDef);
    memRes.align(memReqs);
    result = (TransitConditionCrossedDurationFraction*) memRes.ptr;
    memRes.increment(memReqs);

    result->condSetState(false);
    result->m_type = TRANSCOND_CROSSED_DURATION_FRACTION_ID;
    result->m_lastUpdateTimeFraction = -1.0f;
  }
  else
  {
    // Tell network to keep attrib data around after network update so that we can access it.
    NMP_ASSERT(sourceNodeID != INVALID_NODE_ID);
    AttribAddress address = *def->getSourceNodeFractionalPosAttribAddress();
    net->addPostUpdateAccessAttrib(sourceNodeID, address.m_semantic, 2);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefCrossedDurationFraction::evaluateCondition(
  float lastUpdateTimeFraction,
  float durationFractionTrigger,
  float currentTimeFraction,
  const AttribDataPlaybackPos* timeFractionAttrib)
{
  if (lastUpdateTimeFraction != -1.0f) // Not the initial call to this function
  {
    if ((lastUpdateTimeFraction < durationFractionTrigger) &&
        ((currentTimeFraction >= durationFractionTrigger) ||
         (currentTimeFraction < lastUpdateTimeFraction)))
    {
      // The trigger fraction lies after the last update fraction and on or before the current update
      // fraction or in case the fractional position has wrapped around if the current update fraction lies before the
      // last update fraction.
      return true;
    }
    else if ((lastUpdateTimeFraction > currentTimeFraction) &&
             (currentTimeFraction >= durationFractionTrigger))
    {
      // We have wrapped around and stepped over the trigger.
      return true;
    }
    else if (timeFractionAttrib->m_delta >= 1.0) 
    {
      // Here we detect a step bigger than the whole animation so condition must be true. 
      // Not caught by test below in first iteration (lastUpdateTimeFraction != -1.0f) 
      // as update time was absolute and delta was 0.
      return true;
    }
  }
  else if ((currentTimeFraction >= durationFractionTrigger) && // first frame, non looping and current time already over the trigger
           (timeFractionAttrib->m_previousPosAdj <= durationFractionTrigger))
  {
    return true;
  }
  else if (timeFractionAttrib->m_delta >= 1.0)  // first frame, looped, wrapped around and missed the trigger.
  {
    return true;
  }
  else if (currentTimeFraction == durationFractionTrigger) // Check precise fraction.
  {
    return true;
  }

  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool TransitConditionDefCrossedDurationFraction::instanceUpdate(
  TransitCondition*    tc,
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  NMP_ASSERT(tc && tcDef && net);
  TransitConditionCrossedDurationFraction*    condition = (TransitConditionCrossedDurationFraction*) (tc);
  TransitConditionDefCrossedDurationFraction* conditionDef = (TransitConditionDefCrossedDurationFraction*) (tcDef);

  // Get source node sampled events buffer attrib data.
  AttribAddress address = *conditionDef->getSourceNodeFractionalPosAttribAddress();
  address.m_validFrame = net->getCurrentFrameNo() - 1;
  address.m_animSetIndex = net->getActiveAnimSetIndex();
  address.m_owningNodeID = smActiveNodeID;
  AttribDataPlaybackPos* timeFractionAttrib = static_cast<AttribDataPlaybackPos*>(net->getAttribDataRecurseFilterNodes(address, true));

  condition->m_satisfied = false;
  if (timeFractionAttrib)
  {
    // Evaluate the transition condition
    condition->m_satisfied = evaluateCondition(
      condition->m_lastUpdateTimeFraction,
      conditionDef->m_durationFractionTrigger,
      timeFractionAttrib->m_currentPosAdj,
      timeFractionAttrib);

    condition->m_lastUpdateTimeFraction = timeFractionAttrib->m_currentPosAdj;
  }
  else
  {
    condition->m_lastUpdateTimeFraction = -1.0f;
  }

  return condition->m_satisfied;
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefCrossedDurationFraction::instanceQueueDeps(
  TransitCondition*    NMP_UNUSED(tc),
  TransitConditionDef* tcDef,
  Network*             net,
  NodeID               smActiveNodeID)
{
  TransitConditionDefCrossedDurationFraction* conditionDef;
  NMP_ASSERT(tcDef);
  conditionDef = (TransitConditionDefCrossedDurationFraction*) (tcDef);

  AttribAddress address = *conditionDef->getSourceNodeFractionalPosAttribAddress();
  address.m_owningNodeID = net->nodeFindGeneratingNodeForSemantic(smActiveNodeID, address.m_semantic, true, INVALID_NODE_ID);
  if (address.m_owningNodeID != INVALID_NODE_ID)
  {
    // Add attrib as dependent of root task to ensure it is updated next frame.
    net->addConditionAttrib(address, ATTRIB_TYPE_PLAYBACK_POS);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TransitConditionDefCrossedDurationFraction::instanceReset(
  TransitCondition*    tc,
  TransitConditionDef* NMP_UNUSED(tcDef))
{
  NMP_ASSERT(tc);
  TransitConditionCrossedDurationFraction* condition = static_cast <TransitConditionCrossedDurationFraction*> (tc);

  condition->m_satisfied = false;
  condition->m_lastUpdateTimeFraction = -1.0f;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
