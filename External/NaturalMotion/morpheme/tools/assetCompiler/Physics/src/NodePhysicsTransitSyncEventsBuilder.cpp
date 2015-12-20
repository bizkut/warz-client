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
#include "NodePhysicsTransitSyncEventsBuilder.h"
#include "assetProcessor/TransitDeadBlendDefBuilder.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeTransitSyncEvents.h"
#include "morpheme/Nodes/mrNodeBlend2SyncEvents.h"
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "physics/Nodes/mrNodePhysicsTransit.h"
#include "morpheme/mrBlendOps.h"

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodePhysicsTransitSyncEventsBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodePhysicsTransitSyncEventsBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_DEAD_BLEND_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_LOOP);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodePhysicsTransitSyncEventsBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  return getNodeTransitSyncEventsMemReqs(
    netDefCompilationInfo,
    nodeDefExport,
    netDefExport,
    processor);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodePhysicsTransitSyncEventsBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  MR::AttribDataTransitSyncEventsDef* transitAttribData = NULL;
  MR::NodeDef* nodeDef = initNodeTransitSyncEvents(
    memRes,
    netDefCompilationInfo,
    nodeDefExport,
    netDefExport,
    processor,
    &transitAttribData);

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
void NodePhysicsTransitSyncEventsBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  MR::AttribDataTransitSyncEventsDef* transitAttribData,
  const ME::DataBlockExport*  nodeDefDatablock,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  uint32_t deltaTrajSourceEnum;
  nodeDefDatablock->readUInt(deltaTrajSourceEnum, "DeltaTrajSource");

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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TIME_POS, FN_NAME(nodeTransitSyncEventsQueueUpdateTimePos), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS, FN_NAME(nodeTransitSyncEventsQueueUpdateTimePos), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_FRACTION_POS, FN_NAME(nodeTransitSyncEventsQueueUpdateTimePos), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS, FN_NAME(nodeTransitSyncEventsQueueUpdateTimePos), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_BLEND_WEIGHTS, FN_NAME(nodeTransitSyncEventsQueueUpdateTimePos), logger);    // Update time produces time and weighting attributes.

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodePhysicsTransitSyncEventsQueueTransformBuffs), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_T_MINUS_ONE, FN_NAME(nodeTransitQueueDeadBlendCacheState), logger);

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(nodeTransitSyncEventsQueueSampledEventsBuffers), logger);

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_CP_UINT, FN_NAME(nodeTransitSyncEventsQueueSyncEventTrack), logger);

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(nodeTransitSyncEventsQueueBlend2DurationEventTrackSets), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(nodeTransitSyncEventsQueueSyncEventTrack), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeTransitDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeTransitSyncEventsUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodeTransitSyncEventsFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
