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
#include "NodeLockFootBuilder.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/Nodes/mrNodeLockFoot.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeLockFootBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeLockFootBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_LOCK_FOOT);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeLockFootBuilder::getNodeDefInputConnections(
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

  // Control parameters
  readDataPinChildNodeID(nodeDefDataBlock, "IkFkBlendWeight", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "SwivelContributionToOrientation", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeLockFootBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "IkFkBlendWeight", 0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
  declareDataPin(netDefCompilationInfo, nodeDefExport, "SwivelContributionToOrientation", 1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeLockFootBuilder::getNodeDefMemoryRequirements(
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
  // Lock foot setup
  result += MR::AttribDataLockFootSetup::getMemoryRequirements();

  //---------------------------
  // Animation set specific attribute data
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t i=0; i < nodeDefDep.m_animSets.size(); ++i)
  {
    result += MR::AttribDataLockFootChain::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeLockFootBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_LOCK_FOOT, "Expecting node type LOCK_FOOT");
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
    1,             // numChildren
    1,             // max num active child nodes
    2,             // numInputCPConnections
    0,             // numOutputCPPins
    numAnimSets,   // numAnimSetEntries                     
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
  // Lock foot setup
  MR::AttribDataLockFootSetup* lockFootSetupAttr = MR::AttribDataLockFootSetup::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                 // semantic,    
    0,                                                     // animSetIndex,
    lockFootSetupAttr,                                     // attribData,  
    MR::AttribDataLockFootSetup::getMemoryRequirements()); // attribMemReqs

  // AssumeSimpleHierarchy
  bool assumeSimpleHierarchy = true;
  nodeDefDataBlock->readBool(assumeSimpleHierarchy, "AssumeSimpleHierarchy");
  lockFootSetupAttr->m_assumeSimpleHierarchy = assumeSimpleHierarchy;

  // UpAxisIndex
  nodeDefDataBlock->readUInt(lockFootSetupAttr->m_upAxis, "UpAxisIndex");

  // LockVerticalMotion
  nodeDefDataBlock->readBool(lockFootSetupAttr->m_lockVerticalMotion, "LockVerticalMotion");

  // TrackCharacterController
  nodeDefDataBlock->readBool(lockFootSetupAttr->m_trackCharacterController, "TrackCharacterController");

  //---------------------------
  // Animation set dependent attribute data
  CHAR paramName[256];
  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  for (uint32_t validAnimSetIdx = 0; validAnimSetIdx < nodeDefDep.m_animSets.size(); ++validAnimSetIdx)
  {
    MR::AnimSetIndex animSetIndex = nodeDefDep.m_animSets[validAnimSetIdx];

    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);
    const ME::RigExport* rigExport = animSetExport->getRig();

    MR::AttribDataLockFootChain* lockFootChainAttr = MR::AttribDataLockFootChain::init(memRes, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,        // semantic,    
      animSetIndex,                                          // animSetIndex,
      lockFootChainAttr,                                     // attribData,  
      MR::AttribDataLockFootChain::getMemoryRequirements()); // attribMemReqs

    // FlipKneeRotationDirection
    bool flipKneeRotationDirection;
    sprintf_s(paramName, "FlipKneeRotationDirection_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(flipKneeRotationDirection, paramName);

    // KneeRotationAxis
    NMP::Vector3  kneeRotationAxis(0, 0, 0);
    sprintf_s(paramName, "KneeRotationAxisX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(kneeRotationAxis.x, paramName);
    sprintf_s(paramName, "KneeRotationAxisY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(kneeRotationAxis.y, paramName);
    sprintf_s(paramName, "KneeRotationAxisZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(kneeRotationAxis.z, paramName);
    lockFootChainAttr->m_kneeRotationAxis =
      flipKneeRotationDirection ? -kneeRotationAxis : kneeRotationAxis;
    lockFootChainAttr->m_kneeRotationAxis.normalise();

    // BallRotationAxis
    NMP::Vector3  ballRotationAxis(0, 0, 0);
    sprintf_s(paramName, "BallRotationAxisX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(ballRotationAxis.x, paramName);
    sprintf_s(paramName, "BallRotationAxisY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(ballRotationAxis.y, paramName);
    sprintf_s(paramName, "BallRotationAxisZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(ballRotationAxis.z, paramName);
    lockFootChainAttr->m_ballRotationAxis = ballRotationAxis;
    lockFootChainAttr->m_ballRotationAxis.normalise();

    // FootLevelVector
    NMP::Vector3 footLevelVector(0, 0, 0);
    sprintf_s(paramName, "FootLevelVectorX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(footLevelVector.x, paramName);
    sprintf_s(paramName, "FootLevelVectorY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(footLevelVector.y, paramName);
    sprintf_s(paramName, "FootLevelVectorZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(footLevelVector.z, paramName);
    // Sanity check, it doesn't really matter what the default is
    if (footLevelVector.magnitudeSquared() < 1e-6f)
      footLevelVector.set(1.0f, 0, 0);
    else
      footLevelVector.normalise();
    lockFootChainAttr->m_footLevelVector = footLevelVector;

    // UseBallJoint
    bool useBallJoint;
    sprintf_s(paramName, "UseBallJoint_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(useBallJoint, paramName);
    lockFootChainAttr->m_useBallJoint = useBallJoint;

    // FixToeGroundPenetration
    bool fixToeGroundPenetration;
    sprintf_s(paramName, "FixToeGroundPenetration_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(fixToeGroundPenetration, paramName);
    lockFootChainAttr->m_fixToeGroundPenetration = fixToeGroundPenetration;

    // FixGroundPenetration
    bool fixGroundPenetration;
    sprintf_s(paramName, "FixGroundPenetration_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(fixGroundPenetration, paramName);
    lockFootChainAttr->m_fixGroundPenetration = fixGroundPenetration;

    // FixFootOrientation
    bool fixFootOrientation;
    sprintf_s(paramName, "FixFootOrientation_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(fixFootOrientation, paramName);
    lockFootChainAttr->m_fixFootOrientation = fixFootOrientation;

    // FootPivotResistance
    float footPivotResistance;
    sprintf_s(paramName, "FootPivotResistance_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(footPivotResistance, paramName);
    // Convert from a 0-1 resistance value to a cos-half-angle-radians value
    // Defines how the resistance factor is rescaled to give meaningful offset angles.
    // Tuning: higher values (say around 50) means that most of the useful parameter space is around lower
    // resistance and your numbers will have to be quite high to get strong friction.  Lower values (around 20)
    // mean the useful values are around higher resistance, and your numbers will have to be quite low to get
    // lots of slippage.
    const float rescaleFactor = 40.0f;
    const float tol = 1e-6f;
    if (footPivotResistance > 1.0f - tol)
    {
      lockFootChainAttr->m_footPivotOffsetThreshold = 0;
    }
    else
    {
      lockFootChainAttr->m_footPivotOffsetThreshold =
        1.0f + logf(1.0f - NMP::clampValue(footPivotResistance, 0.0f, 1.0f - tol)) / rescaleFactor;
    }
    NMP_VERIFY(lockFootChainAttr->m_footPivotOffsetThreshold * 0 == 0); // Sanity check for NaNs and Infs

    // CatchUpSpeedFactor
    float catchUpSpeedFactor;
    sprintf_s(paramName, "CatchUpSpeedFactor_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(catchUpSpeedFactor, paramName);
    lockFootChainAttr->m_catchUpSpeedFactor = catchUpSpeedFactor;

    // StraightestLegFactor
    float straightestLegFactor;
    sprintf_s(paramName, "StraightestLegFactor_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(straightestLegFactor, paramName);
    lockFootChainAttr->m_straightestLegFactor = straightestLegFactor;

    // SnapToSourceDistance
    float snapToSourceDistance;
    sprintf_s(paramName, "SnapToSourceDistance_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(snapToSourceDistance, paramName);
    lockFootChainAttr->m_snapToSourceDistance = snapToSourceDistance;

    // AnkleLowerHeightBound
    float ankleLowerHeightBound;
    sprintf_s(paramName, "AnkleLowerHeightBound_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(ankleLowerHeightBound, paramName);
    lockFootChainAttr->m_ankleLowerHeightBound = ankleLowerHeightBound;

    // LowerHeightBound
    float lowerHeightBound;
    sprintf_s(paramName, "LowerHeightBound_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(lowerHeightBound, paramName);
    lockFootChainAttr->m_lowerHeightBound = lowerHeightBound;

    // ToeLowerHeightBound
    float toeLowerHeightBound;
    sprintf_s(paramName, "ToeLowerHeightBound_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(toeLowerHeightBound, paramName);
    lockFootChainAttr->m_toeLowerHeightBound = toeLowerHeightBound;

    //-------------------------
    // Joint indices
    // We don't bother recovering the knee and hip indices - they are recovered using the
    // hierarchy.  The ankle index and ball index may also be ignored and recovered from
    // the hierarchy, depending on settings.
    // When we are no longer assuming a simple hierarchy, this can be modified.

    // AnkleIndex
    uint32_t ankleIndex = 0;
    sprintf_s(paramName, "AnkleIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(ankleIndex, paramName);

    // BallIndex
    uint32_t ballIndex = 0;
    sprintf_s(paramName, "BallIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(ballIndex, paramName);

    // ToeIndex
    uint32_t toeIndex = 0;
    sprintf_s(paramName, "ToeIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(toeIndex, paramName);

    // Recover the joint indices as a sequential hierarchy.
    uint32_t endJointPosition = 2;
    uint32_t endJointIndex = ankleIndex;
    // If we are using the ball joint and fixing toe penetration, get the entire
    // list from the toe index down to the root.
    if (useBallJoint && fixGroundPenetration && fixToeGroundPenetration)
    {
      endJointPosition = 4;
      endJointIndex = toeIndex;
    }
    // If we are using the ball joint but not fixing toe penetration, get everything
    // from the ball downwards.
    else if (useBallJoint)
    {
      endJointPosition = 3;
      endJointIndex = ballIndex;
    }
    // Otherwise, get everything from the ankle downwards.
    int32_t j = (signed)endJointIndex;
    for (int32_t i = endJointPosition; i >= 0; --i)
    {
      lockFootChainAttr->m_jointIndex[i] = j;
      j = rigExport->getJoint(j)->getParentIndex();
      NMP_VERIFY(j > 0);
    }

    // FootfallEventID
    int32_t footfallEventID = 0;
    sprintf_s(paramName, "FootfallEventID_%d", animSetIndex + 1);
    nodeDefDataBlock->readInt(footfallEventID, paramName);
    lockFootChainAttr->m_footfallEventID = footfallEventID;
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
void NodeLockFootBuilder::initTaskQueuingFns(
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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeLockFootQueueLockFootTransformBuffs), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeLockFootQueueLockFootTrajectoryDeltaAndTransformsBuffs), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    nodeDef->setInitNodeInstanceFnId(FN_NAME(nodeInitPinAttribDataInstance), logger);
  }

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeLockFootUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
