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
#include "assetProcessor/NodeTransitBaseBuilder.h"
#include "assetProcessor/TransitDeadBlendDefBuilder.h"
#include "assetProcessor/BlendNodeBuilderUtils.h"
#include "assetProcessor/NodeBuilderUtils.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeTransit.h"
#include "morpheme/Nodes/mrNodeBlend2.h"
#include "morpheme/Nodes/mrNodeStateMachine.h"
#include "morpheme/mrBlendOps.h"
#include "morpheme/mrAttribData.h"
#include <tchar.h>

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
void NodeTransitBaseBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Source node: An unspecified value indicates that the destination node will be determined by the runtime.
  // i.e. an active state transition.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID, true);
  if (sourceNodeID != MR::INVALID_NODE_ID)
  {
    childNodeIDs.push_back(sourceNodeID);
  }

  // Destination node: An unspecified value indicates that the destination node will be determined by the runtime.
  MR::NodeID destNodeID;
  readNodeID(nodeDefDataBlock, "DestNodeID", destNodeID, true);
  if (destNodeID != MR::INVALID_NODE_ID)
  {
    childNodeIDs.push_back(destNodeID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeTransitBaseBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeTransitBaseBuilder::getNodeTransitMemReqs(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    2,               // numChildren
    0,               // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  // Add space for transit def attribute data.
  result += MR::AttribDataTransitDef::getMemoryRequirements();

  // Add space for dead blend attribute data.
  result += getAttribDataDeadBlendDefMemoryRequirements(
    nodeDefExport,
    netDefExport,
    processor);
  
  // Reserve space for the destination sub state init data.
  result += getTransitSubStateMemoryRequirements(nodeDefExport);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeTransitBaseBuilder::initNodeTransit(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor,
  MR::AttribDataTransitDef**  transitAttribDataRef)
{
#ifdef NMP_ENABLE_ASSERTS 
  MR::NodeType nodeTypeID = nodeDefExport->getTypeID();
  NMP_VERIFY_MSG(
    nodeTypeID == NODE_TYPE_TRANSIT || 
    nodeTypeID == NODE_TYPE_TRANSIT_PHYSICS,"Expecting node type TRANSIT or TRANSIT_PHYSICS");
#endif
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
 
  NMP::Memory::Format resultDeadBlend = getAttribDataDeadBlendDefMemoryRequirements(nodeDefExport, netDefExport, processor);
   
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    2,                      // numChildren
    2,                      // max num active child nodes
    0,                      // numInputCPConnections
    0,                      // numOutputCPPins
    1,                      // numAnimSetEntries                     
    nodeDefExport);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_TRANSITION);

  //---------------------------
  // Set child node IDs. In transitions source must always be 0 and dest must be 1.
  // An unspecified value indicates that the source node will be determined by the runtime.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID, true);
  nodeDef->setChildNodeID(0, sourceNodeID);

  // An unspecified value indicates that the destination node will be determined by the runtime.
  MR::NodeID destNodeID;
  readNodeID(nodeDefDataBlock, "DestNodeID", destNodeID, true);
  nodeDef->setChildNodeID(1, destNodeID);

  // Set up transit def attribute data.
  float transitDuration = 0.0f;
  float destStartFraction = 0.0f;
  float destStartSyncEvent = 0.0f;

  MR::AttribDataTransitDef::InitDestStartPointMethod destinationInitMethod =
    MR::AttribDataTransitDef::INIT_DEST_USE_START_DURATION_FRACTION;

  nodeDefDataBlock->readFloat(transitDuration, "DurationInTime");

  if (transitDuration == 0.f)
  {
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_TRANSITION_ZERO);
  }
  nodeDefDataBlock->readFloat(destStartFraction, "DestinationStartFraction");
  nodeDefDataBlock->readFloat(destStartSyncEvent, "DestinationStartSyncEvent");

  bool useDestinationStartFraction;
  bool useDestinationStartSyncEventIndex;
  bool useDestinationStartSyncEventFraction;
  nodeDefDataBlock->readBool(useDestinationStartFraction, "UseDestinationStartFraction");
  nodeDefDataBlock->readBool(useDestinationStartSyncEventIndex, "UseDestinationStartSyncEventIndex");
  nodeDefDataBlock->readBool(useDestinationStartSyncEventFraction, "UseDestinationStartSyncEventFraction");

  if (useDestinationStartFraction)
    destinationInitMethod = MR::AttribDataTransitDef::INIT_DEST_USE_START_DURATION_FRACTION;
  else if (useDestinationStartSyncEventIndex && useDestinationStartSyncEventFraction)
    destinationInitMethod = MR::AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_USE_EVENT_FRACTION;
  else if (useDestinationStartSyncEventIndex)
    destinationInitMethod = MR::AttribDataTransitDef::INIT_DEST_USE_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION;
  else if (useDestinationStartSyncEventFraction)
    destinationInitMethod = MR::AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_USE_EVENT_FRACTION;
  else
    destinationInitMethod = MR::AttribDataTransitDef::INIT_DEST_SYNC_ON_EVENT_INDEX_AND_SYNC_ON_EVENT_FRACTION;

  bool breakoutTransit = false;
  nodeDefDataBlock->readBool(breakoutTransit, "BreakoutTransit");
  if (breakoutTransit)
  {
    // Flag node as a breakout transition.
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_BREAKOUT_TRANSITION);
  }

  //reversible transit params
  bool reversible = false;
  int32_t reverseControlParamNodeID = 0xFFFFFFFF;
  MR::PinIndex reverseControlParamPinIndex = MR::CONTROL_PARAMETER_NODE_PIN_0; // Only connects to control parameters [MORPH-9125]
  nodeDefDataBlock->readBool(reversible, "isReversibleTransit");
  nodeDefDataBlock->readInt(reverseControlParamNodeID, "RuntimeNodeID");

  bool freezeSource = false;
  bool freezeDest = false;
  nodeDefDataBlock->readBool(freezeSource, "FreezeSource");
  nodeDefDataBlock->readBool(freezeDest, "FreezeDest");

  // Read the blending flags
  bool additiveBlendAtt = false;
  nodeDefDataBlock->readBool(additiveBlendAtt, "AdditiveBlendAttitude");
  bool additiveBlendPos = false;
  nodeDefDataBlock->readBool(additiveBlendPos, "AdditiveBlendPosition");
  bool slerpDeltaTrajTrans = false;
  nodeDefDataBlock->readBool(slerpDeltaTrajTrans, "SphericallyInterpolateTrajectoryPosition");

  uint32_t blendMode;
  if (additiveBlendAtt && additiveBlendPos)  // Additive blend attitude and position.
    blendMode = MR::BLEND_MODE_ADD_ATT_ADD_POS;
  else if (additiveBlendAtt)                 // Additive blend attitude and interpolate position.
    blendMode = MR::BLEND_MODE_ADD_ATT_LEAVE_POS;
  else if (additiveBlendPos)                 // Interpolate attitude and additive blend position.
    blendMode = MR::BLEND_MODE_INTERP_ATT_ADD_POS;
  else                                      // Interpolate attitude and position.
    blendMode = MR::BLEND_MODE_INTERP_ATT_INTERP_POS;

  bool deadblendBreakoutToSource = false;
  nodeDefDataBlock->readBool(deadblendBreakoutToSource, "DeadblendBreakoutToSource");
  if (deadblendBreakoutToSource && (sourceNodeID != destNodeID))
  {
    // Flag node as capable of active state dead blend, enable caching.
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND);
  }

  MR::AttribDataTransitDef* transitAttribData = MR::AttribDataTransitDef::init(
    memRes,
    transitDuration,
    destinationInitMethod,
    destStartFraction,
    destStartSyncEvent,
    reversible,
    (MR::NodeID)reverseControlParamNodeID,
    reverseControlParamPinIndex,
    slerpDeltaTrajTrans,
    blendMode,
    freezeSource,
    freezeDest,
    MR::IS_DEF_ATTRIB_DATA);

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,               // semantic
    0,                                                   // animSetIndex
    transitAttribData,                                   // attribData
    MR::AttribDataTransitDef::getMemoryRequirements());  // attribMemReqs

  if (resultDeadBlend.size != 0)
  {
    MR::AttribData* deadBlendAttribData = initAttribDataDeadBlendDef(
      memRes,
      nodeDefExport,
      netDefExport,
      processor);
    NMP_VERIFY(deadBlendAttribData);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_DEAD_BLEND_DEF,                   // semantic
      0,                                                    // animSetIndex
      deadBlendAttribData,                                  // attribData
      MR::AttribDataDeadBlendDef::getMemoryRequirements()); // attribMemReqs
  }

  //---------------------------
  // Initialise the sub state init data.
  initTransitSubStateParams(
    memRes,
    nodeDefExport,
    &transitAttribData->m_nodeInitData);

  // Output the transit attribute data pointer
  *transitAttribDataRef = transitAttribData;

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeTransitBaseBuilder::getNodeTransitSyncEventsMemReqs(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    2,               // numChildren
    0,               // numInputCPConnections
    1,               // numAnimSetEntries
    nodeDefExport);

  // Add space for transit def attribute data.
  result += MR::AttribDataTransitSyncEventsDef::getMemoryRequirements();

  // Reserve space for the loop flag.
  result += MR::AttribDataBool::getMemoryRequirements();

  // Reserve space for duration event blending mode.
  result += MR::AttribDataUInt::getMemoryRequirements();

  // Add space for dead blend attribute data.
  result += getAttribDataDeadBlendDefMemoryRequirements(
    nodeDefExport,
    netDefExport,
    processor);

  // Reserve space for the destination sub state init data.
  result += getTransitSubStateMemoryRequirements(nodeDefExport);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeTransitBaseBuilder::initNodeTransitSyncEvents(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor,
  MR::AttribDataTransitSyncEventsDef**  transitAttribDataRef)
{
#ifdef NMP_ENABLE_ASSERTS
  MR::NodeType nodeTypeID = nodeDefExport->getTypeID();
  NMP_VERIFY_MSG(
    nodeTypeID == NODE_TYPE_TRANSIT_SYNC_EVENTS ||
    nodeTypeID == NODE_TYPE_TRANSIT_SYNC_EVENTS_PHYSICS,
    "Expecting node type NODE_TYPE_TRANSIT_SYNC_EVENTS");
#endif
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();

  NMP::Memory::Format resultDeadBlend = getAttribDataDeadBlendDefMemoryRequirements(
    nodeDefExport,
    netDefExport,
    processor);
  
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    2,                      // numChildren
    2,                      // max num active child nodes
    0,                      // numInputCPConnections
    0,                      // numOutputCPPins
    1,                      // numAnimSetEntries                     
    nodeDefExport);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_TRANSITION);

  //---------------------------
  // Set child node IDs. In transitions source must always be 0 and dest must be 1.
  // An unspecified value indicates that the source node will be determined by the runtime.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDatablock, "SourceNodeID", sourceNodeID, true);
  nodeDef->setChildNodeID(0, sourceNodeID);

  // An unspecified value indicates that the destination node will be determined by the runtime.
  MR::NodeID destNodeID;
  readNodeID(nodeDefDatablock, "DestNodeID", destNodeID, true);
  nodeDef->setChildNodeID(1, destNodeID);

  //---------------------------
  // Set up transit def attribute data.
  float transitDurationInEvents;
  uint32_t destEventSequenceOffset;
  uint32_t destStartEventIndex;
  bool usingDestStartEventIndex;
  nodeDefDatablock->readFloat(transitDurationInEvents, "DurationInEvents");
  nodeDefDatablock->readUInt(destEventSequenceOffset, "DestEventSequenceOffset");
  nodeDefDatablock->readUInt(destStartEventIndex, "DestStartEventIndex");
  nodeDefDatablock->readBool(usingDestStartEventIndex, "UsingDestStartEventIndex");

  // Breakout transitions.
  bool breakoutTransit = false;
  nodeDefDatablock->readBool(breakoutTransit, "BreakoutTransit");
  if (breakoutTransit)
  {
    // Flag node as a breakout transition.
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_BREAKOUT_TRANSITION);
  }

  //reversible transit params
  bool     reversible = false;
  int32_t  reverseControlParamNodeID = 0xFFFFFFFF;
  MR::PinIndex reverseControlParamPinIndex = MR::CONTROL_PARAMETER_NODE_PIN_0; // Only connects to control parameters [MORPH-9125]
  nodeDefDatablock->readBool(reversible, "isReversibleTransit");
  nodeDefDatablock->readInt(reverseControlParamNodeID, "RuntimeNodeID");

  // Read the blending flags
  bool additiveBlendAtt = false;
  nodeDefDatablock->readBool(additiveBlendAtt, "AdditiveBlendAttitude");
  bool additiveBlendPos = false;
  nodeDefDatablock->readBool(additiveBlendPos, "AdditiveBlendPosition");
  bool slerpDeltaTrajTrans = false;
  nodeDefDatablock->readBool(slerpDeltaTrajTrans, "SphericallyInterpolateTrajectoryPosition");

  uint32_t blendMode;
  if (additiveBlendAtt && additiveBlendPos)  // Additive blend attitude and position.
    blendMode = MR::BLEND_MODE_ADD_ATT_ADD_POS;
  else if (additiveBlendAtt)                 // Additive blend attitude and interpolate position.
    blendMode = MR::BLEND_MODE_ADD_ATT_LEAVE_POS;
  else if (additiveBlendPos)                 // Interpolate attitude and additive blend position.
    blendMode = MR::BLEND_MODE_INTERP_ATT_ADD_POS;
  else                                      // Interpolate attitude and position.
    blendMode = MR::BLEND_MODE_INTERP_ATT_INTERP_POS;

  bool deadblendBreakoutToSource = false;
  nodeDefDatablock->readBool(deadblendBreakoutToSource, "DeadblendBreakoutToSource");
  if (deadblendBreakoutToSource && (sourceNodeID != destNodeID))
  {
    // Flag node as capable of active state dead blend, enable caching.
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_TRANSITION_CAN_DEAD_BLEND);
  }

  if (transitDurationInEvents == 0.0f)
  {
    nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_TRANSITION_ZERO);
  }

  MR::AttribDataTransitSyncEventsDef* transitAttribData = MR::AttribDataTransitSyncEventsDef::init(
    memRes,
    transitDurationInEvents,
    destEventSequenceOffset,
    destStartEventIndex,
    usingDestStartEventIndex,
    reversible,
    (MR::NodeID)reverseControlParamNodeID,
    reverseControlParamPinIndex,
    slerpDeltaTrajTrans,
    blendMode,
    MR::IS_DEF_ATTRIB_DATA);

  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                        // semantic
    0,                                                            // animSetIndex
    transitAttribData,                                            // attribData
    MR::AttribDataTransitSyncEventsDef::getMemoryRequirements()); // attribMemReqs

  //---------------------------
  // Always allow looping on transit sync events nodes.
  MR::AttribDataBool* loopable = MR::AttribDataBool::init(memRes, true, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_LOOP,                       // semantic,    
    0,                                              // animSetIndex,
    loopable,                                       // attribData,  
    MR::AttribDataBool::getMemoryRequirements());   // attribMemReqs

  //---------------------------
  // Duration event matching mode.
  uint32_t durationEventMatchingOp = determineEventMatchingTypeFromDataBlock(nodeDefDatablock);
  MR::AttribDataUInt* durationEventMatchingOpAttrib = MR::AttribDataUInt::init(memRes, durationEventMatchingOp, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP, // semantic
    0,                                              // animSetIndex
    durationEventMatchingOpAttrib,                  // attribData
    MR::AttribDataUInt::getMemoryRequirements());   // attribMemReqs

  if (resultDeadBlend.size != 0)
  {
    MR::AttribData* deadBlendAttribData = initAttribDataDeadBlendDef(
      memRes,
      netDefExport->getNode(nodeDef->getNodeID()),
      netDefExport,
      processor);

    NMP_VERIFY(deadBlendAttribData);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_DEAD_BLEND_DEF,                   // semantic
      0,                                                    // animSetIndex
      deadBlendAttribData,                                  // attribData
      MR::AttribDataDeadBlendDef::getMemoryRequirements()); // attribMemReqs
  }

  //---------------------------
  // Initialise the sub state init data.
  initTransitSubStateParams(
    memRes,
    netDefExport->getNode(nodeDef->getNodeID()),
    &transitAttribData->m_nodeInitData);

  // Output the transit attribute data pointer
  *transitAttribDataRef = transitAttribData;

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeTransitBaseBuilder::getTransitSubStateMemoryRequirements(const ME::NodeExport* nodeDefExport)
{
  NMP::Memory::Format result(0, NMP_NATURAL_TYPE_ALIGNMENT);

  const ME::DataBlockExport* datablock = nodeDefExport->getDataBlock();

  int32_t subStateCount = 0;
  datablock->readInt(subStateCount, "DestinationSubStateCount");

  if (subStateCount > 0)
  {
    result += MR::NodeInitDataArrayDef::getMemoryRequirements((uint16_t)subStateCount);

    for (uint16_t i = 0; i != (uint16_t) subStateCount; i++)
    {
      result += MR::StateMachineInitData::getMemoryRequirements();
    }
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeTransitBaseBuilder::initTransitSubStateParams(
  NMP::Memory::Resource&     memRes,
  const ME::NodeExport*      nodeDefExport,
  MR::NodeInitDataArrayDef** nodeInitDataArray)
{
  // init the transit to sub state information
  NMP::Memory::Format resultInitData = getTransitSubStateMemoryRequirements(nodeDefExport);
  if (resultInitData.size != 0)
  {
    int32_t subStateCount = 0;
    int32_t targetStateNodeID = 0;

    const ME::DataBlockExport* datablock = nodeDefExport->getDataBlock();

    datablock->readInt(subStateCount, "DestinationSubStateCount");
    (*nodeInitDataArray) = MR::NodeInitDataArrayDef::init(memRes, (uint16_t)subStateCount);

    //---------------------------
    // Set child node IDs.
    int32_t subStateNodeID;
    CHAR paramName[256];
    for (uint16_t i = 0; i != (uint16_t)subStateCount; i++)
    {
      sprintf_s(paramName, "DestinationSubStateID_%d", i);
      datablock->readInt(subStateNodeID, paramName);
      NMP_VERIFY(subStateNodeID > 0);

      sprintf_s(paramName, "DestinationSubStateParentID_%d", i);
      datablock->readInt(targetStateNodeID, paramName);
      NMP_VERIFY(targetStateNodeID > 0);

      MR::StateMachineInitData* initData = MR::StateMachineInitData::init(memRes, (MR::NodeID)subStateNodeID);
      initData->setTargetNodeID((MR::NodeID) targetStateNodeID);

      (*nodeInitDataArray)->m_nodeInitDataArray[i] = initData;
    }
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
