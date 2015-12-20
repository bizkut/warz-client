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
#include "NodeGunAimIKBuilder.h"
#include "morpheme/Nodes/mrNodeGunAimIK.h"
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
bool NodeGunAimIKBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeGunAimIKBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_GUN_AIM_IK);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeGunAimIKBuilder::getNodeDefInputConnections(
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

  // Control parameter node IDs.
  readDataPinChildNodeID(nodeDefDataBlock, "Target", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "BlendWeight", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeGunAimIKBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Target", 0, true, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "BlendWeight", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeGunAimIKBuilder::calcNodeDefInfo(
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
  sprintf_s(paramName, "GunBindJointIndex_%d", animSetIndex + 1);
  nodeDefDataBlock->readUInt(endJointIndex, paramName);
  //
  uint32_t rootJointIndex = 0;
  sprintf_s(paramName, "SpineRootJointIndex_%d", animSetIndex + 1);
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
NMP::Memory::Format NodeGunAimIKBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);
 
  //---------------------------
  // Allocate space for the NodeDef itself
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,                      // numChildren
    2,                      // numInputCPConnections
    numAnimSets,            // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Gun aim IK setup
  result += MR::AttribDataGunAimSetup::getMemoryRequirements();

  //---------------------------
  // Animation set dependent attribute data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[i];

    // We need to know the number of joints in the IK chain
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    uint32_t numJointsInChain;
    calcNodeDefInfo(nodeDefExport, animSetExport, animSetIndex, numJointsInChain);

    result += MR::AttribDataGunAimIKChain::getMemoryRequirements(numJointsInChain);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeGunAimIKBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_GUN_AIM_IK, "Expecting node type GUN_AIM_IK");
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
    1,                    // numChildren
    1,                    // max num active child nodes
    2,                    // numInputCPConnections
    0,                    // numOutputCPPins
    numAnimSets,          // numAnimSetEntries                     
    nodeDefExport);

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node IDs.
  MR::NodeID inputNodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", inputNodeID);
  nodeDef->setChildNodeID(0, inputNodeID);

  //---------------------------
  // Initialise the attrib datas.
  
  //---------------------------
  // Gun aim IK setup
  MR::AttribDataGunAimSetup* gunAimSetupAttrib = MR::AttribDataGunAimSetup::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,               // semantic,    
    0,                                                   // animSetIndex,
    gunAimSetupAttrib,                                   // attribData,  
    MR::AttribDataGunAimSetup::getMemoryRequirements()); // attribMemReqs

  // World up axis
  NMP::Vector3 worldUpAxis(-2.0f, -2.0f, -2.0f);
  nodeDefDataBlock->readFloat(worldUpAxis.x, "WorldUpAxisX");
  NMP_VERIFY(worldUpAxis.x >= -1.0f); // Check the attribute existed
  nodeDefDataBlock->readFloat(worldUpAxis.y, "WorldUpAxisY");
  NMP_VERIFY(worldUpAxis.y >= -1.0f); // Check the attribute existed
  nodeDefDataBlock->readFloat(worldUpAxis.z, "WorldUpAxisZ");
  NMP_VERIFY(worldUpAxis.z >= -1.0f); // Check the attribute existed
  gunAimSetupAttrib->m_worldUpAxis = worldUpAxis;

  // Keep upright
  bool keepUpright = false;
  nodeDefDataBlock->readBool(keepUpright, "KeepUpright");
  gunAimSetupAttrib->m_keepUpright = keepUpright;

  // Apply joint limits
  nodeDefDataBlock->readBool(gunAimSetupAttrib->m_applyJointLimits, "ApplyJointLimits");

  // WorldSpaceTarget
  nodeDefDataBlock->readBool(gunAimSetupAttrib->m_worldSpaceTarget, "WorldSpaceTarget");

  //---------------------------
  // Animation set dependent attribute data
  CHAR paramName[256];
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    // We need to know the number of joints in the IK chain
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    uint32_t numJointsInChain;
    calcNodeDefInfo(nodeDefExport, animSetExport, animSetIndex, numJointsInChain);

    // Add the per set MR::AttribDataGunAimIKChain.
    MR::AttribDataGunAimIKChain* gunAimChainAttrib =
      MR::AttribDataGunAimIKChain::init(memRes, numJointsInChain, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,        // semantic,    
      animSetIndex,                                          // animSetIndex,
      gunAimChainAttrib,                                     // attribData,  
      MR::AttribDataGunAimIKChain::getMemoryRequirements(numJointsInChain)); // attribMemReqs

    // GunJointIndex
    uint32_t gunJointIndex = 0;
    sprintf_s(paramName, "GunJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(gunJointIndex, paramName);
    gunAimChainAttrib->m_gunJointIndex = gunJointIndex;

    // GunBindJointIndex
    uint32_t gunBindJointIndex = 0;
    sprintf_s(paramName, "GunBindJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(gunBindJointIndex, paramName);
    gunAimChainAttrib->m_gunBindJointIndex = gunBindJointIndex;

    // GunPivotOffset
    NMP::Vector3  gunPivotOffset(0, 0, 0);
    sprintf_s(paramName, "GunPivotOffsetX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunPivotOffset.x, paramName);
    sprintf_s(paramName, "GunPivotOffsetY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunPivotOffset.y, paramName);
    sprintf_s(paramName, "GunPivotOffsetZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunPivotOffset.z, paramName);
    gunAimChainAttrib->m_gunPivotOffset = gunPivotOffset;

    // GunBarrelOffset
    NMP::Vector3  gunBarrelOffset(0, 0, 0);
    sprintf_s(paramName, "GunBarrelOffsetX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunBarrelOffset.x, paramName);
    sprintf_s(paramName, "GunBarrelOffsetY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunBarrelOffset.y, paramName);
    sprintf_s(paramName, "GunBarrelOffsetZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunBarrelOffset.z, paramName);
    gunAimChainAttrib->m_gunBarrelOffset = gunBarrelOffset;

    // GunPointingVector
    NMP::Vector3  gunPointingVector(1.0f, 0, 0);
    sprintf_s(paramName, "GunPointingVectorX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunPointingVector.x, paramName);
    sprintf_s(paramName, "GunPointingVectorY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunPointingVector.y, paramName);
    sprintf_s(paramName, "GunPointingVectorZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(gunPointingVector.z, paramName);
    gunPointingVector.normalise();
    gunAimChainAttrib->m_gunPointingVector = gunPointingVector;

    // PrimaryWristJointIndex
    uint32_t primaryWristJointIndex = 0;
    sprintf_s(paramName, "PrimaryWristJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(primaryWristJointIndex, paramName);
    gunAimChainAttrib->m_primaryWristJointIndex = primaryWristJointIndex;

    // FlipPrimaryHinge
    bool flipPrimaryHinge = false;
    sprintf_s(paramName, "FlipPrimaryHinge_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(flipPrimaryHinge, paramName);
    float flipOrNot = flipPrimaryHinge ? -1.0f : 1.0f;

    // PrimaryArmHingeAxis
    NMP::Vector3  primaryArmHingeAxis(0, 0, 1.0f);
    sprintf_s(paramName, "PrimaryArmHingeAxisX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(primaryArmHingeAxis.x, paramName);
    sprintf_s(paramName, "PrimaryArmHingeAxisY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(primaryArmHingeAxis.y, paramName);
    sprintf_s(paramName, "PrimaryArmHingeAxisZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(primaryArmHingeAxis.z, paramName);
    gunAimChainAttrib->m_primaryArmHingeAxis = flipOrNot * primaryArmHingeAxis;

    // UseSecondaryArm
    bool useSecondaryArm = false;
    sprintf_s(paramName, "UseSecondaryArm_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(useSecondaryArm, paramName);
    gunAimChainAttrib->m_useSecondaryArm = useSecondaryArm;

    // only set the secondary arm values if it is being used
    if (useSecondaryArm)
    {
      // SecondaryWristJointIndex
      uint32_t secondaryWristJointIndex = 0;
      sprintf_s(paramName, "SecondaryWristJointIndex_%d", animSetIndex + 1);
      nodeDefDataBlock->readUInt(secondaryWristJointIndex, paramName);
      gunAimChainAttrib->m_secondaryWristJointIndex = secondaryWristJointIndex;

      // FlipSecondaryHinge
      bool flipSecondaryHinge = false;
      sprintf_s(paramName, "FlipSecondaryHinge_%d", animSetIndex + 1);
      nodeDefDataBlock->readBool(flipSecondaryHinge, paramName);
      flipOrNot = flipSecondaryHinge ? -1.0f : 1.0f;

      // SecondaryArmHingeAxis
      NMP::Vector3  secondaryArmHingeAxis(0, 0, 1.0f);
      sprintf_s(paramName, "SecondaryArmHingeAxisX_%d", animSetIndex + 1);
      nodeDefDataBlock->readFloat(secondaryArmHingeAxis.x, paramName);
      sprintf_s(paramName, "SecondaryArmHingeAxisY_%d", animSetIndex + 1);
      nodeDefDataBlock->readFloat(secondaryArmHingeAxis.y, paramName);
      sprintf_s(paramName, "SecondaryArmHingeAxisZ_%d", animSetIndex + 1);
      nodeDefDataBlock->readFloat(secondaryArmHingeAxis.z, paramName);
      gunAimChainAttrib->m_secondaryArmHingeAxis = flipOrNot * secondaryArmHingeAxis;
    }

    // SpineBias
    float spineBias = 0.0f;
    sprintf_s(paramName, "SpineBias_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(spineBias, paramName);
    gunAimChainAttrib->m_spineBias = spineBias;

    // UpdateTargetByDeltas
    bool updateTargetByDelta = false;
    sprintf_s(paramName, "UpdateTargetByDeltas_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(updateTargetByDelta, paramName);
    gunAimChainAttrib->m_updateTargetByDeltas = updateTargetByDelta;

    // SpineRootJointIndex
    uint32_t spineRootJointIndex = 0;
    sprintf_s(paramName, "SpineRootJointIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(spineRootJointIndex, paramName);
    gunAimChainAttrib->m_spineRootJointIndex = spineRootJointIndex;

    // PointIK params - compiled from data we already know
    // Num aim joints - number of real joints, not IK joints
    gunAimChainAttrib->m_numAimJoints = numJointsInChain;
    //
    NMRU::PointIK::Params* ikParams = gunAimChainAttrib->m_ikParams;
    ikParams->pointingVector = gunPointingVector;
    ikParams->worldUpAxis = worldUpAxis;
    ikParams->keepUpright = keepUpright;
    ikParams->bias = 0; // We're going to do bias with joint weights
    // Set joint limits
    if (gunAimSetupAttrib->m_applyJointLimits)
    {
      initJointLimits(gunAimChainAttrib, animSetExport->getRig(), numJointsInChain, gunBindJointIndex);
    }
    // Fix the end effector 'joint'
    ikParams->perJointParams[numJointsInChain + 1].enabled = false;
    // Set the bias weights
    //  It's numJointsInChain + 2 because the pivot and end effector are also joints to PointIK
    ikParams->setLinearBiasCurve(numJointsInChain + 2, spineBias, true);
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
void NodeGunAimIKBuilder::initTaskQueuingFns(
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
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_TRANSFORM, FN_NAME(nodeGunAimIKQueueGunAimIKSetup), logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeGunAimIKQueueGunAimIKTransformBuffs), logger);
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeGunAimIKQueueGunAimIKTrajectoryDeltaAndTransformBuffs), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  //------------------------------------
  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsChildren1CompulsoryManyOptionalInputCPs), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeGunAimIKBuilder::initJointLimits(
  MR::AttribDataGunAimIKChain *chainAttrib,
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
