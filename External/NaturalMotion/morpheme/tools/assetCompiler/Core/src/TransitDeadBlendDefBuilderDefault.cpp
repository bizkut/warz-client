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
#include "TransitDeadBlendDefBuilderDefault.h"
#include "morpheme/mrTransitDeadBlend.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format TransitDeadBlendDefBuilderDefault::getNodeDefMemoryRequirements(
  const ME::NodeExport*        NMP_UNUSED(nodeExport),
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  return MR::AttribDataDeadBlendDef::getMemoryRequirements();
}

//----------------------------------------------------------------------------------------------------------------------
/// \brief Initialise the TransitDeadBlendDef from connects xml data.
MR::AttribData* TransitDeadBlendDefBuilderDefault::initAttribData(
  NMP::Memory::Resource&       memRes,
  const ME::NodeExport*        nodeExport,
  const ME::NetworkDefExport*  NMP_UNUSED(netDefExport),
  AssetProcessor*              NMP_UNUSED(processor))
{
  const ME::DataBlockExport* deadBlendDataBlock = nodeExport->getDataBlock();

  MR::AttribDataDeadBlendDef* result = MR::AttribDataDeadBlendDef::init(memRes, MR::IS_DEF_ATTRIB_DATA);

  bool useDeadReckoningWhenDeadBlending = true;
  bool blendToDestinationPhysicsBones = false;
  float deadReckoningAngularVelocityDamping = 0.0f;
  deadBlendDataBlock->readBool(useDeadReckoningWhenDeadBlending, "UseDeadReckoningWhenDeadBlending");
  deadBlendDataBlock->readFloat(deadReckoningAngularVelocityDamping, "DeadReckoningAngularVelocityDamping");
  deadBlendDataBlock->readBool(blendToDestinationPhysicsBones, "BlendToDestinationPhysicsBones");

  result->m_useDeadReckoningWhenDeadBlending = useDeadReckoningWhenDeadBlending;
  result->m_deadReckoningAngularVelocityDamping = deadReckoningAngularVelocityDamping;
  result->m_blendToDestinationPhysicsBones = blendToDestinationPhysicsBones;

  return result;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------

