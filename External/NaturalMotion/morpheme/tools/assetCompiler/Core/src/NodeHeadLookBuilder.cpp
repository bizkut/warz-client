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
#include "NodeHeadLookBuilder.h"
#include "morpheme/Nodes/mrNodeHeadLook.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------
#include "NMIK/NMPointIK.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeHeadLookBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeHeadLookBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_HEAD_LOOK);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeHeadLookBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set the child node ID.
  MR::NodeID inputNodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", inputNodeID);
  childNodeIDs.push_back(inputNodeID);

  readDataPinChildNodeID(nodeDefDataBlock, "BlendWeight", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "Target", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeHeadLookBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Target", 0, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "BlendWeight", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeHeadLookBuilder::calcNodeDefInfo(
  const ME::NodeExport* nodeDefExport,
  const ME::AnimationSetExport* animSetExport,
  uint32_t animSetIndex,
  uint32_t& numJointsInChain)
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::RigExport* rigExport = animSetExport->getRig();

  // Get the end joint and root joint indices
  CHAR paramName[256];
  uint32_t endJointIndex = 0;
  sprintf_s(paramName, "EndJointIndex_%d", animSetIndex + 1);
  nodeDefDataBlock->readUInt(endJointIndex, paramName);
  //
  uint32_t rootJointIndex = 0;
  sprintf_s(paramName, "RootJointIndex_%d", animSetIndex + 1);
  nodeDefDataBlock->readUInt(rootJointIndex, paramName);

  // Count chain joints
  int32_t j = (signed)endJointIndex;
  numJointsInChain = 1;
  while ((j = rigExport->getJoint(j)->getParentIndex()) >= (signed)rootJointIndex)
  {
    ++numJointsInChain;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeHeadLookBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint32_t inputCPCount = 2;

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Allocate space for the NodeDef itself
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,               // numChildren
    inputCPCount,    // numInputCPConnections
    numAnimSets,     // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Head look setup
  result += MR::AttribDataHeadLookSetup::getMemoryRequirements();

  //---------------------------
  // Animation set specific attribute data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[i];

    // We need to know the number of joints in the IK chain
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    uint32_t numJointsInChain;
    calcNodeDefInfo(nodeDefExport, animSetExport, animSetIndex, numJointsInChain);

    result += MR::AttribDataHeadLookChain::getMemoryRequirements(numJointsInChain);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeHeadLookBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_HEAD_LOOK, "Expecting node type HEAD_LOOK");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint16_t numAnimSets = (uint16_t) animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
  
  //---------------------------
  // Initialise the NodeDef itself
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    1,              // numChildren
    1,              // max num active child nodes
    2,              // numInputCPConnections
    0,              // numOutputCPPins
    numAnimSets,    // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set the child node ID.
  MR::NodeID inputNodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", inputNodeID);
  nodeDef->setChildNodeID(0, inputNodeID);

  //---------------------------
  // Initialise the attrib datas.
 
  //---------------------------
  // Head look setup
  MR::AttribDataHeadLookSetup* headLookSetupAttrib = MR::AttribDataHeadLookSetup::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                 // semantic,    
    0,                                                     // animSetIndex,
    headLookSetupAttrib,                                   // attribData,  
    MR::AttribDataHeadLookSetup::getMemoryRequirements()); // attribMemReqs

  // updateTargetByDeltas
  bool updateTargetByDelta = false;
  nodeDefDataBlock->readBool(updateTargetByDelta, "UpdateTargetByDeltas");
  headLookSetupAttrib->m_updateTargetByDeltas = updateTargetByDelta;

  // ApplyJointLimits
  bool applyJointLimits = false;
  nodeDefDataBlock->readBool(applyJointLimits, "ApplyJointLimits");

  // MinimiseRotation
  bool minimiseRotation = true;
  nodeDefDataBlock->readBool(minimiseRotation, "MinimiseRotation");

  // KeepUpright
  bool keepUpright = false;
  nodeDefDataBlock->readBool(keepUpright, "KeepUpright");

  // World up axis
  NMP::Vector3 worldUpAxis(-2.0f, -2.0f, -2.0f);
  nodeDefDataBlock->readFloat(worldUpAxis.x, "WorldUpAxisX");
  NMP_VERIFY(worldUpAxis.x >= -1.0f); // Check the attribute existed
  nodeDefDataBlock->readFloat(worldUpAxis.y, "WorldUpAxisY");
  NMP_VERIFY(worldUpAxis.y >= -1.0f); // Check the attribute existed
  nodeDefDataBlock->readFloat(worldUpAxis.z, "WorldUpAxisZ");
  NMP_VERIFY(worldUpAxis.z >= -1.0f); // Check the attribute existed

  // WorldSpaceTarget
  nodeDefDataBlock->readBool(headLookSetupAttrib->m_worldSpaceTarget, "WorldSpaceTarget");

  //---------------------------
  // Animation set specific attribute data
  CHAR paramName[256];
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    // We need to know the number of joints in the IK chain
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    uint32_t numJointsInChain;
    calcNodeDefInfo(nodeDefExport, animSetExport, animSetIndex, numJointsInChain);

    // Initialise the attrib data
    MR::AttribDataHeadLookChain* headLookChainAttrib =
      MR::AttribDataHeadLookChain::init(memRes, numJointsInChain, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,        // semantic,    
      animSetIndex,                                          // animSetIndex,
      headLookChainAttrib,                                   // attribData,  
      MR::AttribDataHeadLookChain::getMemoryRequirements(numJointsInChain)); // attribMemReqs

    // PointingVector.
    NMP::Vector3 pointingVector(0, 0, 0);
    sprintf_s(paramName, "PointingVectorX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(pointingVector.x, paramName);
    sprintf_s(paramName, "PointingVectorY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(pointingVector.y, paramName);
    sprintf_s(paramName, "PointingVectorZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(pointingVector.z, paramName);
    pointingVector.normalise();

    // EndEffectorOffset
    NMP::Vector3 endEffectorOffset(0, 0, 0);
    sprintf_s(paramName, "EndEffectorOffsetX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(endEffectorOffset.x, paramName);
    sprintf_s(paramName, "EndEffectorOffsetY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(endEffectorOffset.y, paramName);
    sprintf_s(paramName, "EndEffectorOffsetZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(endEffectorOffset.z, paramName);
    headLookChainAttrib->m_endEffectorOffset = endEffectorOffset;

    // Bias
    float bias = 0.0f;
    sprintf_s(paramName, "Bias_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(bias, paramName);

    // EndJointIndex
    uint32_t endJointIndex = 0;
    sprintf_s(paramName, "EndJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(endJointIndex, paramName);
    headLookChainAttrib->m_endJointIndex = endJointIndex;

    // RootJointIndex
    uint32_t rootJointIndex = 0;
    sprintf_s(paramName, "RootJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(rootJointIndex, paramName);
    headLookChainAttrib->m_rootJointIndex = rootJointIndex;

    // PointIK params - compiled from data we already know
    // Num joints - number of real joints, not IK joints (in PointIK the end effector is also a joint)
    headLookChainAttrib->m_numJoints = numJointsInChain;
    //
    NMRU::PointIK::Params* ikParams = headLookChainAttrib->m_ikParams;
    ikParams->pointingVector = pointingVector;
    ikParams->worldUpAxis = worldUpAxis;
    ikParams->keepUpright = keepUpright;
    ikParams->minimiseRotation = minimiseRotation;
    ikParams->bias = 0; // We're going to do bias with joint weights
    // Set joint limits
    if (applyJointLimits)
    {
      initJointLimits(headLookChainAttrib, animSetExport->getRig(), numJointsInChain, endJointIndex);
    }
    // Fix the end effector 'joint'
    ikParams->perJointParams[numJointsInChain].enabled = false;
    // Set the bias weights
    ikParams->setLinearBiasCurve(numJointsInChain + 1, bias, true);
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
void NodeHeadLookBuilder::initTaskQueuingFns(
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

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM, FN_NAME(nodeHeadLookQueueHeadLookSetup), logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeHeadLookQueueHeadLookTransformBuffs), logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeHeadLookQueueHeadLookTrajectoryDeltaAndTransformBuffs), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsChildrenInputCPs), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeHeadLookBuilder::initJointLimits(
  MR::AttribDataHeadLookChain *chainAttrib,
  const ME::RigExport *rigExport,
  uint32_t numJointsInChain,
  uint32_t endJointIndex)
{
  uint32_t numJointLimits = rigExport->getNumJointLimits();

  int32_t j = (signed)endJointIndex;
  for (int32_t i = numJointsInChain - 1; i >= 0; --i, j = rigExport->getJoint(j)->getParentIndex())
  {
    NMRU::IKJointParams& jointParams =  chainAttrib->m_ikParams->perJointParams[i];
    NMRU::JointLimits::Params& limitParams = jointParams.limits;
    jointParams.isLimited = false;

    // Search for a joint limit for this joint
    for (uint32_t l = 0; l != numJointLimits; ++l)
    {
      const ME::JointLimitExport* jointLimit = rigExport->getJointLimit(l);
      if ((signed)jointLimit->getJointIndex() == j)
      {
        jointParams.isLimited = true;
        jointLimit->getOrientation(limitParams.frame.x, limitParams.frame.y, limitParams.frame.z, limitParams.frame.w);
        jointLimit->getOffsetOrientation(
          limitParams.offset.x, limitParams.offset.y, limitParams.offset.z, limitParams.offset.w);

        // NOTE  We need to swap the limits over to make them comply with the nomenclature as used in
        // connect.  This is different from the way joint limits are implemented in the morpheme runtime.
        float twistLow = jointLimit->getTwistLow();
        float twistHigh = jointLimit->getTwistHigh();
        if (jointLimit->getLimitType() == ME::JointLimitExport::kHingeLimitType)
        {
          limitParams.setTwistLeanLimits(-twistHigh, -twistLow, 0, 0);
        }
        else
        {
          limitParams.setTwistLeanLimits(-twistHigh, -twistLow, jointLimit->getSwing2(), jointLimit->getSwing1());
        }
        break;
      }
    }
  }
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
