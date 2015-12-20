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
#include "NodeBasicUnevenTerrainBuilder.h"
#include "NodePredictiveUnevenTerrainBuilder.h"
#include "morpheme/Nodes/mrNodeBasicUnevenTerrain.h"
#include "morpheme/Nodes/mrNodePredictiveUnevenTerrain.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodePredictiveUnevenTerrainBuilder::hasSemanticLookupTable()
{
  return true;
}
  
//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodePredictiveUnevenTerrainBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodePredictiveUnevenTerrainBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set child node IDs.
  MR::NodeID inputNodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", inputNodeID);
  childNodeIDs.push_back(inputNodeID);  

  //---------------------------
  // Control parameters
  readDataPinChildNodeID(nodeDefDataBlock, "IkHipsWeight", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "IkFkBlendWeight", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "PredictionEnable", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodePredictiveUnevenTerrainBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  //---------------------------
  // Input Control Param Connections.
  declareDataPin(netDefCompilationInfo, nodeDefExport, "IkHipsWeight", 0, true,  MR::ATTRIB_SEMANTIC_CP_FLOAT); 
  declareDataPin(netDefCompilationInfo, nodeDefExport, "IkFkBlendWeight",  1, true,  MR::ATTRIB_SEMANTIC_CP_FLOAT); 
  declareDataPin(netDefCompilationInfo, nodeDefExport, "PredictionEnable",  2, true,  MR::ATTRIB_SEMANTIC_CP_BOOL); 
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodePredictiveUnevenTerrainBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  const uint32_t numLimbs = 2;
  const uint32_t numChildren = 1;
  const uint32_t inputCPCount = 3;

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Allocate space for the NodeDef itself
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    numChildren,           // numChildren
    inputCPCount,          // numInputCPConnections
    numAnimSets,           // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Setup attributes
  result += MR::AttribDataBasicUnevenTerrainSetup::getMemoryRequirements();

  //---------------------------
  // Animation set dependent attribute data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    result += MR::AttribDataBasicUnevenTerrainChain::getMemoryRequirements(numLimbs);
    result += MR::AttribDataPredictiveUnevenTerrainPredictionDef::getMemoryRequirements(numLimbs);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodePredictiveUnevenTerrainBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN, "Expecting node type PREDICTIVE_UNEVEN_TERRAIN");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  const uint32_t numLimbs = 2;
  
  //---------------------------
  // Work out how many attrib datas we actually have.
  uint16_t numAnimSets = (uint16_t) animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    1,            // numChildren
    1,            // max num active child nodes
    3,            // numInputCPConnections
    0,            // numOutputCPPins
    numAnimSets,  // numAnimSetEntries                     
    nodeDefExport);

  MR::NodeDef::NodeFlags nodeFlags(0);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeFlags.set(MR::NodeDef::NODE_FLAG_IS_FILTER);

  // This node requires the trajectory delta for the entire network to have been computed before it can compute its transforms
  nodeFlags.set(MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS);

  nodeDef->setNodeFlags(nodeFlags);

  //---------------------------
  // Set child node IDs.
  MR::NodeID inputNodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", inputNodeID);
  nodeDef->setChildNodeID(0, inputNodeID);
  
 
  //---------------------------
  // Initialise the attrib dats.
  
  //---------------------------
  // Setup attributes
  MR::AttribDataBasicUnevenTerrainSetup* setupAttrib = MR::AttribDataBasicUnevenTerrainSetup::init(memRes, numLimbs, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                            // semantic,    
    0,                                                                // animSetIndex,
    setupAttrib,                                                      // attribData,  
    MR::AttribDataBasicUnevenTerrainSetup::getMemoryRequirements());  // attribMemReqs

  NodeBasicUnevenTerrainBuilder::initSetupAttrib(nodeDefExport, setupAttrib);

  //---------------------------
  // Animation set dependent attribute data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    //---------------------------
    // Chain definition data
    MR::AttribDataBasicUnevenTerrainChain* chainAttr =
      MR::AttribDataBasicUnevenTerrainChain::init(memRes, numLimbs, MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,                           // semantic,    
      animSetIndex,                                                             // animSetIndex,
      chainAttr,                                                                // attribData,  
      MR::AttribDataBasicUnevenTerrainChain::getMemoryRequirements(numLimbs));  // attribMemReqs

    NodeBasicUnevenTerrainBuilder::initChainAttrib(nodeDefExport, chainAttr, animSetIndex);
    
    //---------------------------
    // Prediction definition data
    MR::AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttr =
      MR::AttribDataPredictiveUnevenTerrainPredictionDef::init(memRes, numLimbs, MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_DEF,                                     // semantic,    
      animSetIndex,                                                                          // animSetIndex,
      predictionDefAttr,                                                                     // attribData,  
      MR::AttribDataPredictiveUnevenTerrainPredictionDef::getMemoryRequirements(numLimbs));  // attribMemReqs

    initPredictionDefAttrib(nodeDefExport, predictionDefAttr, animSetIndex);
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
void NodePredictiveUnevenTerrainBuilder::initPredictionDefAttrib(
  const ME::NodeExport* nodeDefExport,
  MR::AttribDataPredictiveUnevenTerrainPredictionDef* predictionDefAttr,
  uint32_t animSetIndex)
{
  NMP_VERIFY(nodeDefExport);
  NMP_VERIFY(predictionDefAttr);
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();

  CHAR paramName[256];

  //---------------------------
  // Atrtib data
  float footLiftingSlopeAngleLimit = 30.0f;
  sprintf_s(paramName, "PredictionSlopeAngleLimit_%d", animSetIndex + 1);
  nodeDefDatablock->readFloat(footLiftingSlopeAngleLimit, paramName);
  predictionDefAttr->m_footLiftingSlopeAngleLimit = tanf(NMP::degreesToRadians(footLiftingSlopeAngleLimit));

  float footLiftingLateralAngleLimit = 30.0f;
  sprintf_s(paramName, "PredictionLateralAngleLimit_%d", animSetIndex + 1);
  nodeDefDatablock->readFloat(footLiftingLateralAngleLimit, paramName);
  predictionDefAttr->m_footLiftingLateralAngleLimit = tanf(NMP::degreesToRadians(footLiftingLateralAngleLimit));

  float closeFootbaseTolFrac = 0.1f;
  sprintf_s(paramName, "PredictionCloseFootbaseTolFrac_%d", animSetIndex + 1);
  nodeDefDatablock->readFloat(closeFootbaseTolFrac, paramName);
  predictionDefAttr->m_closeFootbaseTolFrac = NMP::maximum(closeFootbaseTolFrac, 1e-2f);

  //---------------------------
  // Limb data
  CHAR* legNames[2] = { "Left", "Right" };
  for (uint32_t i = 0; i < 2; ++i)
  {
    MR::AttribDataPredictiveUnevenTerrainPredictionDef::LimbData* limbData = predictionDefAttr->m_limbInfo[i];
    NMP_ASSERT(limbData);

    int32_t eventTrackID = -1;
    sprintf_s(paramName, "%sEventTrackID_%d", legNames[i], animSetIndex + 1);
    nodeDefDatablock->readInt(eventTrackID, paramName);
    limbData->m_eventTrackID = (uint32_t)eventTrackID;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodePredictiveUnevenTerrainBuilder::initTaskQueuingFns(
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

  for (uint32_t i = 0; i < MR::ATTRIB_SEMANTIC_NM_MAX; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP, FN_NAME(nodePredictiveUnevenTerrainQueueIKSetup), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UNEVEN_TERRAIN_PREDICTION_STATE, FN_NAME(nodePredictiveUnevenTerrainQueueIKSetup), logger);
  
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET, FN_NAME(nodePredictiveUnevenTerrainQueueFootLiftingTargets), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodePredictiveUnevenTerrainQueueTransformBuffs), logger);

  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodePredictiveUnevenTerrainUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
