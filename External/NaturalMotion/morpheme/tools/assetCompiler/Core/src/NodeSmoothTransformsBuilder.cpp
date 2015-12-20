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
#include "NodeSmoothTransformsBuilder.h"
#include "morpheme/mrAttribData.h"
#include "morpheme/Nodes/mrNodeSmoothTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
  
//----------------------------------------------------------------------------------------------------------------------
bool NodeSmoothTransformsBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeSmoothTransformsBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_SMOOTH_TRANSFORMS);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_BONE_WEIGHTS);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSmoothTransformsBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set child node IDs.
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", source0NodeID);
  childNodeIDs.push_back(source0NodeID);

  // Control parameters
  readDataPinChildNodeID(nodeDefDataBlock, "Multiplier", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSmoothTransformsBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Multiplier", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT); 
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeSmoothTransformsBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Allocate space for the NodeDef itself
  uint32_t inputCPCount = 1;
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,               // numChildren
    inputCPCount,    // numInputCPConnections
    numAnimSets,     // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Animation set dependent attribute data
  CHAR paramNumSmoothingStrengths[256];
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[i];

    sprintf_s(paramNumSmoothingStrengths, "numSmoothingStrengthsSet_%d", animSetIndex + 1);
    uint32_t numSmoothingStrengths;
    nodeDefDataBlock->readUInt(numSmoothingStrengths, paramNumSmoothingStrengths);

    result += MR::AttribDataFloatArray::getMemoryRequirements(numSmoothingStrengths);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeSmoothTransformsBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_SMOOTH_TRANSFORMS, "Expecting node type SMOOTH_TRANSFORMS");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint16_t numAnimSets = (uint16_t) animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    1,             // numChildren
    1,             // max num active child nodes
    1,             // numInputCPConnections
    0,             // numOutputCPPins
    numAnimSets,   // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node IDs.
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", source0NodeID);
  nodeDef->setChildNodeID(0, source0NodeID);

  //---------------------------
  // Initialise the attrib datas.
  
  // Animation set dependent attribute data
  CHAR paramNumSmoothingStrengths[256];
  CHAR paramSmoothingStrength[256];
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    sprintf_s(paramNumSmoothingStrengths, "numSmoothingStrengthsSet_%d", animSetIndex + 1);
    uint32_t numSmoothingStrengths;
    nodeDefDataBlock->readUInt(numSmoothingStrengths, paramNumSmoothingStrengths);

    MR::AttribDataFloatArray* smoothingStrengthsAttribData = MR::AttribDataFloatArray::init(memRes, numSmoothingStrengths, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_BONE_WEIGHTS,                                        // semantic,    
      animSetIndex,                                                            // animSetIndex,
      smoothingStrengthsAttribData,                                            // attribData,  
      MR::AttribDataFloatArray::getMemoryRequirements(numSmoothingStrengths)); // attribMemReqs
   
    float smoothingStrength;
    for (uint32_t i = 0; i < numSmoothingStrengths; ++i)
    {
      sprintf_s(paramSmoothingStrength, "SmoothingStrengths_%d_Set_%d", i + 1, animSetIndex + 1);
      nodeDefDataBlock->readFloat(smoothingStrength, paramSmoothingStrength);
      smoothingStrengthsAttribData->m_values[i] = smoothingStrength;
    }
  }

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeSmoothTransformsBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = (MR::QueueAttrTaskFn*)MR::SharedTaskFnTables::createSharedTaskFnTable();

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeSmoothTransformsQueueTransforms), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeSmoothTransformsQueueTrajectoryDeltaAndTransforms), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);
 
  // Handle Data Pins
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnections1Child1InputCP), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
