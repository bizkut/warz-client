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
#include <tchar.h>
#include "NetworkDefBuilder.h"
#include "NodePhysicsTransitBuilder.h"
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrTransitDeadBlend.h"
#include "assetProcessor/TransitDeadBlendDefBuilder.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "physics/Nodes/mrNodePhysicsTransit.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodePhysicsTransitBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodePhysicsTransitBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_TRANSIT_PHYSICS);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DEAD_BLEND_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodePhysicsTransitBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  return getNodeTransitMemReqs(
    netDefCompilationInfo,
    nodeDefExport,
    netDefExport,
    processor);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodePhysicsTransitBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  MR::AttribDataTransitDef* transitAttribData = NULL;
  MR::NodeDef* nodeDef = initNodeTransit(
    memRes,
    netDefCompilationInfo,
    nodeDefExport,
    netDefExport,
    processor,
    &transitAttribData);

  // Physics nodes equire the trajectory delta for the entire network to have been computed before it can compute its transforms
  MR::NodeDef::NodeFlags nodeFlags = nodeDef->getNodeFlags();
  nodeFlags.set(MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS);
  nodeDef->setNodeFlags(nodeFlags);

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    transitAttribData,
    nodeDefDataBlock,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodePhysicsTransitBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  MR::AttribDataTransitDef*   transitAttribData,
  const ME::DataBlockExport*  nodeDefDataBlock,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  uint32_t deltaTrajSourceEnum;
  nodeDefDataBlock->readUInt(deltaTrajSourceEnum, "DeltaTrajSource");

  switch (deltaTrajSourceEnum)
  {
  case 2:
    // Source.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(queuePassThroughChild0IfNotPhysics), logger);
    break;

  case 1:
    // Destination.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(queuePassThroughChild1IfNotPhysics), logger);
    break;

  default:
    // Blend Both.
    if (transitAttribData->m_slerpTrajectoryPosition)
    {
      if (transitAttribData->m_blendMode == MR::BLEND_MODE_ADD_ATT_ADD_POS ||
          transitAttribData->m_blendMode == MR::BLEND_MODE_ADD_ATT_LEAVE_POS)
        nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(transitionQueueTrajectoryDeltaAddAttSlerpPos), logger);

      else
        nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(transitionQueueTrajectoryDeltaInterpAttSlerpPos), logger);
    }
    else
    {
      if (transitAttribData->m_blendMode == MR::BLEND_MODE_ADD_ATT_ADD_POS ||
          transitAttribData->m_blendMode == MR::BLEND_MODE_ADD_ATT_LEAVE_POS)
      {
        if (transitAttribData->m_blendMode == MR::BLEND_MODE_ADD_ATT_ADD_POS)
          nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(transitionQueueTrajectoryDeltaAddAttAddPos), logger);
        else
          nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(transitionQueueTrajectoryDeltaAddAttInterpPos), logger);
      }
      else
      {
        if (transitAttribData->m_blendMode == MR::BLEND_MODE_ADD_ATT_ADD_POS)
          nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(transitionQueueTrajectoryDeltaInterpAttAddPos), logger);
        else
          nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(transitionQueueTrajectoryDeltaInterpAttInterpPos), logger);
      }
    }
    break;
  }

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeTransitQueueUpdateTimePos), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_BLEND_WEIGHTS, FN_NAME(nodeTransitQueueUpdateTimePos), logger);    // Update time produces time and weighting attributes.
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  //-----------
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodePhysicsTransitQueueTransformBuffs), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE, FN_NAME(nodeTransitQueueDeadBlendCacheState), logger);

  //-----------
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeTransitBlend2QueueSampledEventsBuffers), logger);

  //-----------
  // Include handling of sync event related attributes so that if there is a transitMatchEvents somewhere in our parent
  // hierarchy this node will not crash out.
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(queuePassThroughChild0), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(queuePassThroughChild0), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeTransitDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeTransitUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeTransitFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
