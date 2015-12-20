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
#include "morpheme/Nodes/mrNodeBasicUnevenTerrain.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeBasicUnevenTerrainBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeBasicUnevenTerrainBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_BASIC_UNEVEN_TERRAIN);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);
  
  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBasicUnevenTerrainBuilder::getNodeDefInputConnections(
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
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBasicUnevenTerrainBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "IkHipsWeight", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "IkFkBlendWeight", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeBasicUnevenTerrainBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  const uint32_t numLimbs = 2;
  const uint32_t numChildren = 1;
  const uint32_t inputCPCount = 2;
 
  //---------------------------
  // Allocate space for the NodeDef itself
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    numChildren,    // numChildren
    inputCPCount,   // numInputCPConnections
    numAnimSets,    // numAnimSetEntries
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
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeBasicUnevenTerrainBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_BASIC_UNEVEN_TERRAIN, "Expecting node type BASIC_UNEVEN_TERRAIN");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  const uint32_t numLimbs = 2;
  const uint32_t numChildren = 1;
  const uint32_t inputCPCount = 2;
  const uint32_t outputCPCount = 0;

  //---------------------------
  // Initialise the NodeDef itself
  uint16_t numAnimSets = (uint16_t) animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    numChildren,           // numChildren
    numChildren,           // max num active child nodes
    inputCPCount,          // numInputCPConnections
    outputCPCount,         // numOutputCPPins
    numAnimSets,           // numAnimSetEntries                     
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
  // Initialise the attrib datas.
  
  //---------------------------
  // Setup attributes
  MR::AttribDataBasicUnevenTerrainSetup* setupAttrib = MR::AttribDataBasicUnevenTerrainSetup::init(memRes, numLimbs, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                            // semantic,    
    0,                                                                // animSetIndex,
    setupAttrib,                                                      // attribData,  
    MR::AttribDataBasicUnevenTerrainSetup::getMemoryRequirements());  // attribMemReqs
  initSetupAttrib(nodeDefExport, setupAttrib);

  //---------------------------
  // Animation set dependent attribute data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    MR::AttribDataBasicUnevenTerrainChain* chainAttr =
      MR::AttribDataBasicUnevenTerrainChain::init(memRes, numLimbs, MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,                           // semantic,    
      animSetIndex,                                                             // animSetIndex,
      chainAttr,                                                                // attribData,  
      MR::AttribDataBasicUnevenTerrainChain::getMemoryRequirements(numLimbs));  // attribMemReqs

    initChainAttrib(nodeDefExport, chainAttr, animSetIndex);
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
void NodeBasicUnevenTerrainBuilder::initSetupAttrib(
  const ME::NodeExport* nodeDefExport,
  MR::AttribDataBasicUnevenTerrainSetup* setupAttrib)
{
  NMP_VERIFY(nodeDefExport);
  NMP_VERIFY(setupAttrib);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  setupAttrib->m_numLimbs = 2;

  // Up Axis
  NMP::Vector3 upAxis(0.0f, 0.0f, 0.0f);
  uint32_t upAxisIndex = 1;
  nodeDefDataBlock->readUInt(upAxisIndex, "UpAxisIndex");
  upAxis[upAxisIndex] = 1.0f;
  setupAttrib->m_upAxis = upAxis;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBasicUnevenTerrainBuilder::initChainAttrib(
  const ME::NodeExport*                  nodeDefExport,
  MR::AttribDataBasicUnevenTerrainChain* chainAttr,
  uint32_t                               animSetIndex)
{
  NMP_VERIFY(nodeDefExport);
  NMP_VERIFY(chainAttr);
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  CHAR paramName[256];

  //---------------------------
  // Hips
  uint32_t hipsIndex = 0xFFFFFFFF;
  sprintf_s(paramName, "HipsIndex_%d", animSetIndex + 1);
  nodeDefDataBlock->readUInt(hipsIndex, paramName);
  chainAttr->m_hipsChannelID = hipsIndex;

  bool hipsHeightControlEnable = true;
  sprintf_s(paramName, "HipsHeightControlEnable_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(hipsHeightControlEnable, paramName);
  chainAttr->m_hipsHeightControlEnable = hipsHeightControlEnable;

  bool hipsPosVelLimitEnable = true;
  sprintf_s(paramName, "HipsPosVelLimitEnable_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(hipsPosVelLimitEnable, paramName);
  chainAttr->m_hipsPosVelLimitEnable = hipsPosVelLimitEnable;

  float hipsPosVelLimit = 0.5f; // Leg lengths per sec
  sprintf_s(paramName, "HipsPosVelLimit_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(hipsPosVelLimit, paramName);
  chainAttr->m_hipsPosVelLimit = hipsPosVelLimit;

  bool hipsPosAccelLimitEnable = true;
  sprintf_s(paramName, "HipsPosAccelLimitEnable_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(hipsPosAccelLimitEnable, paramName);
  chainAttr->m_hipsPosAccelLimitEnable = hipsPosAccelLimitEnable;

  float hipsPosAccelLimit = 40.0f; // Leg lengths per sec^2
  sprintf_s(paramName, "HipsPosAccelLimit_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(hipsPosAccelLimit, paramName);
  chainAttr->m_hipsPosAccelLimit = hipsPosAccelLimit;

  //---------------------------
  // Common leg options

  // Straightest leg factor
  float straightestLegFactor = 0.99f;
  sprintf_s(paramName, "StraightestLegFactor_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(straightestLegFactor, paramName);
  chainAttr->m_straightestLegFactor = straightestLegFactor;

  // Leg IK end joint limit clamping
  bool endJointPosVelLimitEnable = false;
  sprintf_s(paramName, "AnklePosVelLimitEnable_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(endJointPosVelLimitEnable, paramName);
  chainAttr->m_endJointPosVelLimitEnable = endJointPosVelLimitEnable;

  float endJointPosVelLimit = 1.0f; // Leg lengths per sec
  sprintf_s(paramName, "AnklePosVelLimit_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(endJointPosVelLimit, paramName);
  chainAttr->m_endJointPosVelLimit = endJointPosVelLimit;

  bool endJointPosAccelLimitEnable = true;
  sprintf_s(paramName, "AnklePosAccelLimitEnable_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(endJointPosAccelLimitEnable, paramName);
  chainAttr->m_endJointPosAccelLimitEnable = endJointPosAccelLimitEnable;

  float endJointPosAccelLimit = 40.0f; // Leg lengths per sec^2
  sprintf_s(paramName, "AnklePosAccelLimit_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(endJointPosAccelLimit, paramName);
  chainAttr->m_endJointPosAccelLimit = endJointPosAccelLimit;

  bool endJointAngVelLimitEnable = false;
  sprintf_s(paramName, "AnkleAngVelLimitEnable_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(endJointAngVelLimitEnable, paramName);
  chainAttr->m_endJointAngVelLimitEnable = endJointAngVelLimitEnable;

  float endJointAngVelLimit = 1.0f; // Revolutions per sec
  sprintf_s(paramName, "AnkleAngVelLimit_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(endJointAngVelLimit, paramName);
  endJointAngVelLimit *= (2 * NM_PI); // Radians per sec
  chainAttr->m_endJointAngVelLimit = endJointAngVelLimit;

  bool endJointAngAccelLimitEnable = true;
  sprintf_s(paramName, "AnkleAngAccelLimitEnable_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(endJointAngAccelLimitEnable, paramName);
  chainAttr->m_endJointAngAccelLimitEnable = endJointAngAccelLimitEnable;

  float endJointAngAccelLimit = 25.0f; // Revolutions per sec^2
  sprintf_s(paramName, "AnkleAngAccelLimit_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(endJointAngAccelLimit, paramName);
  endJointAngAccelLimit *= (2 * NM_PI); // Radians per sec^2
  chainAttr->m_endJointAngAccelLimit = endJointAngAccelLimit;

  // Use ground penetration fixup
  bool useGroundPenetrationFixup = true;
  sprintf_s(paramName, "UseGroundPenetrationFixup_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(useGroundPenetrationFixup, paramName);
  chainAttr->m_useGroundPenetrationFixup = useGroundPenetrationFixup;

  // Use the inclined trajectory motion plane or the canonical ground plane
  // to compute the relative height and alignment of the footbase
  bool useTrajectorySlopeAlignment = true;
  sprintf_s(paramName, "UseTrajectorySlopeAlignment_%d", animSetIndex + 1);
  nodeDefDataBlock->readBool(useTrajectorySlopeAlignment, paramName);
  chainAttr->m_useTrajectorySlopeAlignment = useTrajectorySlopeAlignment;

  // Foot alignment with the terrain surface options
  float footAlignToSurfaceAngleLimit = 30.0f; // degrees
  sprintf_s(paramName, "FootAlignToSurfaceAngleLimit_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(footAlignToSurfaceAngleLimit, paramName);
  footAlignToSurfaceAngleLimit = NMP::degreesToRadians(footAlignToSurfaceAngleLimit); // Radians
  // Cosine angle limit for comparing directly with w component of quats
  footAlignToSurfaceAngleLimit = NMP::clampValue(cosf(footAlignToSurfaceAngleLimit), 0.0f, 1.0f);
  chainAttr->m_footAlignToSurfaceAngleLimit = footAlignToSurfaceAngleLimit;

  float footAlignToSurfaceMaxSlopeAngle = 75.0f; // degrees
  sprintf_s(paramName, "FootAlignToSurfaceMaxSlopeAngle_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(footAlignToSurfaceMaxSlopeAngle, paramName);
  footAlignToSurfaceMaxSlopeAngle = NMP::degreesToRadians(footAlignToSurfaceMaxSlopeAngle); // Radians
  // Cosine angle limit for comparing directly with w component of quats
  footAlignToSurfaceMaxSlopeAngle = NMP::clampValue(cosf(footAlignToSurfaceMaxSlopeAngle), 0.0f, 1.0f);
  chainAttr->m_footAlignToSurfaceMaxSlopeAngle = footAlignToSurfaceMaxSlopeAngle;

  // Foot lifting options
  float footLiftingHeightLimit = 0.6f; // Leg lengths
  sprintf_s(paramName, "FootLiftingHeightLimit_%d", animSetIndex + 1);
  nodeDefDataBlock->readFloat(footLiftingHeightLimit, paramName);
  chainAttr->m_footLiftingHeightLimit = footLiftingHeightLimit;

  //---------------------------
  // Leg IK Chain options
  CHAR* legNames[2] = { "Left", "Right" };
  for (uint32_t i = 0; i < 2; ++i)
  {
    MR::AttribDataBasicUnevenTerrainChain::ChainData* chainData = chainAttr->m_chainInfo[i];

    // HipIndex
    uint32_t hipIndex = 0xFFFFFFFF;
    sprintf_s(paramName, "%sHipIndex_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readUInt(hipIndex, paramName);
    chainData->m_channelIDs[0] = hipIndex;

    // KneeIndex
    uint32_t kneeIndex = 0xFFFFFFFF;
    sprintf_s(paramName, "%sKneeIndex_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readUInt(kneeIndex, paramName);
    chainData->m_channelIDs[1] = kneeIndex;

    // AnkleIndex
    uint32_t ankleIndex = 0xFFFFFFFF;
    sprintf_s(paramName, "%sAnkleIndex_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readUInt(ankleIndex, paramName);
    chainData->m_channelIDs[2] = ankleIndex;

    // BallIndex
    uint32_t ballIndex = 0xFFFFFFFF;
    sprintf_s(paramName, "%sBallIndex_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readUInt(ballIndex, paramName);

    // ToeIndex
    uint32_t toeIndex = 0xFFFFFFFF;
    sprintf_s(paramName, "%sToeIndex_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readUInt(toeIndex, paramName);

    // Determine the foot model
    chainData->m_numFootJoints = 1; // Ankle

    if (ballIndex != 0xFFFFFFFF && toeIndex == 0xFFFFFFFF)
    {
      // Ankle and Ball
      chainData->m_numFootJoints = 2;
      chainData->m_channelIDs[3] = ballIndex;
    }

    if (ballIndex == 0xFFFFFFFF && toeIndex != 0xFFFFFFFF)
    {
      // Ankle and Toe
      chainData->m_numFootJoints = 2;
      chainData->m_channelIDs[3] = toeIndex;
    }

    if (ballIndex != 0xFFFFFFFF && toeIndex != 0xFFFFFFFF)
    {
      // Ankle and Ball and Toe
      chainData->m_numFootJoints = 3;
      chainData->m_channelIDs[3] = ballIndex;
      chainData->m_channelIDs[4] = toeIndex;
    }

    // Knee rotation axis
    NMP::Vector3 kneeRotationAxis(0, 0, 0);
    sprintf_s(paramName, "%sKneeRotationAxisX_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readFloat(kneeRotationAxis.x, paramName);
    sprintf_s(paramName, "%sKneeRotationAxisY_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readFloat(kneeRotationAxis.y, paramName);
    sprintf_s(paramName, "%sKneeRotationAxisZ_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readFloat(kneeRotationAxis.z, paramName);
    kneeRotationAxis.normalise();
    chainData->m_kneeRotationAxis = kneeRotationAxis;

    // Flip knee rotation direction
    bool flipKneeRotationDirection = false;
    sprintf_s(paramName, "%sFlipKneeRotationDirection_%d", legNames[i], animSetIndex + 1);
    nodeDefDataBlock->readBool(flipKneeRotationDirection, paramName);
    if (flipKneeRotationDirection)
      chainData->m_kneeRotationAxis *= -1.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NodeBasicUnevenTerrainBuilder::initTaskQueuingFns(
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

  //---------------------------
  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  for (uint32_t i = 0; i < MR::ATTRIB_SEMANTIC_NM_MAX; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UNEVEN_TERRAIN_IK_SETUP, FN_NAME(nodeBasicUnevenTerrainQueueIKSetupTransforms), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET, FN_NAME(nodeBasicUnevenTerrainQueueFootLiftingTargets), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeBasicUnevenTerrainQueueTransformBuffs), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnections1Child2InputCPs), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
