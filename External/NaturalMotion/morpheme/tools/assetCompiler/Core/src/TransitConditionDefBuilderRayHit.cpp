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
#include "TransitConditionDefBuilderRayHit.h"
#include "morpheme/TransitConditions/mrTransitConditionRayHit.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitConditionDefBuilderRayHit::getMemoryRequirements(
  const ME::ConditionExport*   NMP_UNUSED(condExport),
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  return NMP::Memory::Format(NMP::Memory::align(sizeof(MR::TransitConditionDefRayHit), NMP_NATURAL_TYPE_ALIGNMENT), NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitCondition from connects xml data.
MR::TransitConditionDef* TransitConditionDefBuilderRayHit::init(
  NMP::Memory::Resource&       memRes,
  const ME::ConditionExport*   condExport,
  const ME::NetworkDefExport*  netDefExport,
  AssetProcessor*              processor,
  MR::NodeID                   NMP_UNUSED(sourceNodeID))
{
  const ME::DataBlockExport* condDataBlock = condExport->getDataBlock();

  NMP::Memory::Format memReqs = getMemoryRequirements(condExport, netDefExport, processor);
  memRes.align(memReqs);
  MR::TransitConditionDefRayHit* result = (MR::TransitConditionDefRayHit*) memRes.ptr;
  memRes.increment(memReqs);
  result->setType(TRANSCOND_RAY_HIT_ID);

  // Initialize the static data
  int   intVal;
  bool  boolVal;
  float floatVal;

  condDataBlock->readInt(intVal, "HitMode");
  result->m_hitMode = intVal;
  condDataBlock->readBool(boolVal, "UseLocalOrientation");
  result->m_useLocalOrientation = boolVal;

  condDataBlock->readFloat(floatVal, "RayStartX");
  result->m_rayStartX = floatVal;
  condDataBlock->readFloat(floatVal, "RayStartY");
  result->m_rayStartY = floatVal;
  condDataBlock->readFloat(floatVal, "RayStartZ");
  result->m_rayStartZ = floatVal;
  condDataBlock->readFloat(floatVal, "RayDeltaX");
  result->m_rayDeltaX = floatVal;
  condDataBlock->readFloat(floatVal, "RayDeltaY");
  result->m_rayDeltaY = floatVal;
  condDataBlock->readFloat(floatVal, "RayDeltaZ");
  result->m_rayDeltaZ = floatVal;

  result->setInvertFlag(false);

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
