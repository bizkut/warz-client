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
#include "NodePhysicsBuilder.h"
#include "physics/Nodes/mrNodePhysics.h"
#include "physics/Nodes/mrNodePhysicsGrouper.h"
#include "morpheme/Nodes/mrNodeFilterTransforms.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrCommonTaskQueuingFns.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodePhysicsBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodePhysicsBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_PHYSICS);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_FLOAT);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_BOOL);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodePhysicsBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", source0NodeID, true);
  if (source0NodeID != MR::INVALID_NODE_ID)
  {
    childNodeIDs.push_back(source0NodeID);
  }

  MR::NodeID physicalEffectID;
  readNodeID(nodeDefDataBlock, "PhysicalEffect", physicalEffectID, true);
  if (physicalEffectID != MR::INVALID_NODE_ID)
  {
    childNodeIDs.push_back(physicalEffectID);
  }

  // Control Parameters
  readDataPinChildNodeID(nodeDefDataBlock, "KWeightNodeID", childNodeIDs, true);
  readDataPinChildNodeID(nodeDefDataBlock, "AAWeightNodeID", childNodeIDs, true);
}

//----------------------------------------------------------------------------------------------------------------------
void NodePhysicsBuilder::preInit(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  //---------------------------
  // Input control parameters.
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "KWeightNodeID",   0, true, MR::ATTRIB_SEMANTIC_CP_FLOAT); // Optional control input
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "AAWeightNodeID",  1, true, MR::ATTRIB_SEMANTIC_CP_FLOAT); // Optional control input
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodePhysicsBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //---------------------------
  uint32_t numChildNodes = 0;
  uint32_t controlParamCount = 3;
 
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", source0NodeID, true);
  if (source0NodeID != MR::INVALID_NODE_ID)
  {
    numChildNodes = 1;
  }

  //---------------------------
  // Work out how many attrib datas we actually have.
  const uint16_t numAnimSets = static_cast< uint16_t >(animLibraryExport->getNumAnimationSets());
  NMP_VERIFY(numAnimSets > 0);

  //---------------------------
  // Allocate space for the NodeDef itself.
  // Note: we only allow space for AttribData in anim set 0, this is ok because we create no data for any other anim set.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    numChildNodes,        // numChildren
    controlParamCount,    // numInputCPConnections
    numAnimSets,          // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Setup attribute data
  result += MR::AttribDataFloat::getMemoryRequirements();
  result += MR::AttribDataBool::getMemoryRequirements();
  result += MR::AttribDataPhysicsSetup::getMemoryRequirements();

  for (uint16_t i(0); i < numAnimSets; ++i)
  {
    result += MR::AttribDataPhysicsSetupPerAnimSet::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodePhysicsBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_PHYSICS, "Expecting node type PHYSICS");
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  //---------------------------
  uint32_t numChildNodes = 0;
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "InputNodeID", source0NodeID, true);
  if (source0NodeID != MR::INVALID_NODE_ID)
  {
    ++numChildNodes;
  }

  //---------------------------
  // Work out how many attrib datas we actually have.
  const uint16_t numAnimSets = (uint16_t) animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,                  
    netDefCompilationInfo,   
    numChildNodes,            // numChildren
    (uint16_t)numChildNodes,  // numChildren
    3,                        // numInputCPConnections
    0,                        // numOutputCPPins
    numAnimSets,              // numAnimSetEntries
    nodeDefExport);

  MR::NodeDef::NodeFlags nodeFlags(0);

  // Flag node as being physical and as a filter node if we have a connected child.
  if (source0NodeID > 0)
  {
    nodeDef->setPassThroughChildIndex(0);
    nodeFlags.set(MR::NodeDef::NODE_FLAG_IS_PHYSICAL | MR::NodeDef::NODE_FLAG_IS_FILTER);
  }
  else
  {
    nodeFlags.set(MR::NodeDef::NODE_FLAG_IS_PHYSICAL);
  }

  // Physics nodes equire the trajectory delta for the entire network to have been computed before it can compute its transforms
  nodeFlags.set(MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS);

  nodeDef->setNodeFlags(nodeFlags);

  //---------------------------
  // Set child node IDs.
  if (source0NodeID != MR::INVALID_NODE_ID)
  {
    nodeDef->setChildNodeID(0, source0NodeID);
  }

  //---------------------------
  // Input control connections.
  readInputPinConnectionDetails(nodeDefDataBlock, "PhysicalEffect", nodeDef, 2, true); // Optional control input

  //---------------------------
  // Initialise the attrib datas.
  
  //---------------------------
  // Setup attribute data.
  MR::AttribDataFloat* fixedWeightAttribData = MR::AttribDataFloat::init(memRes, 1.0f, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_FLOAT,                   // semantic
    0,                                              // animSetIndex
    fixedWeightAttribData,                          // attribData
    MR::AttribDataFloat::getMemoryRequirements());  // attribMemReqs

  // Set up a constant input to use for waiting for tasks.
  MR::AttribDataBool* taskWaitAttribData = MR::AttribDataBool::init(memRes, true, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_BOOL,                    // semantic
    0,                                              // animSetIndex
    taskWaitAttribData,                             // attribData
    MR::AttribDataBool::getMemoryRequirements());   // attribMemReqs

  //---------------------------
  MR::AttribDataPhysicsSetup* physicsSetup = MR::AttribDataPhysicsSetup::init(memRes, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF,                // semantic
    0,                                                    // animSetIndex
    physicsSetup,                                         // attribData
    MR::AttribDataPhysicsSetup::getMemoryRequirements()); // attribMemReqs

  
  nodeDefDataBlock->readBool(physicsSetup->m_enableCollision, "EnableCollision");
  nodeDefDataBlock->readBool(physicsSetup->m_useActiveAnimationAsKeyframeAnchor, "UseActiveAnimationAsKeyframeAnchor");
  nodeDefDataBlock->readBool(physicsSetup->m_useRootAsAnchor, "UseRootAsAnchor");
  nodeDefDataBlock->readBool(physicsSetup->m_outputSourceAnimation, "OutputSourceAnimation");
  nodeDefDataBlock->readBool(physicsSetup->m_preserveMomentum, "PreserveMomentum");
  physicsSetup->m_hasAnimationChild = numChildNodes > 0;

  int method = MR::AttribDataPhysicsSetup::METHOD_INVALID;
  nodeDefDataBlock->readInt(method, "Method");
  physicsSetup->m_method = (MR::AttribDataPhysicsSetup::PhysicsMethod) method;

  // root control method
  switch (physicsSetup->m_method)
  {
  case MR::AttribDataPhysicsSetup::METHOD_RAGDOLL:
  case MR::AttribDataPhysicsSetup::METHOD_AA:
  case MR::AttribDataPhysicsSetup::METHOD_LOCAL_SK:
    {
      // Physics nodes that give us a root control method of ROOT_CONTROL_PHYSICS that have a node downstream
      // of them that create a dependency upstream on pre physics transforms caused a circular dependency.
      //
      // I've added the comment from Task* nodeNetworkQueueUpdateCharacterController() in mrNetwork.cpp for reference
      // // If physics-controlled root:
      // // add a dependency on every active physics node's pre-physics step. Just do this to make sure
      // // the pre-physics task gets executed before us - we don't need the result, but when the CC manager runs
      // // it will disable the CC for this network, and the pre-physics task needs to have completed before that.
      //
      // To avoid this circular dependency we look for downstream nodes creating a dependency on the network
      // node for prePhysicsTransforms if so we modify the root control method to avoid the creation of the
      // dependent task.
      const NodeDefDependency & nodeDep = netDefCompilationInfo->getNodeDefDependency( nodeDef->getNodeID() );
      if( doAnyDescendantsCreatePrePhysicsTransformsDependencyOnAnUpstreamNode( nodeDep ) )
      {
        physicsSetup->m_rootControlMethod = MR::Network::ROOT_CONTROL_PHYSICAL_ANIMATION;
      }
      else
      {
        physicsSetup->m_rootControlMethod = MR::Network::ROOT_CONTROL_PHYSICS;
      }
      break;
    }

  case MR::AttribDataPhysicsSetup::METHOD_SK:
  case MR::AttribDataPhysicsSetup::METHOD_HK:
  case MR::AttribDataPhysicsSetup::METHOD_SKAA:
    physicsSetup->m_rootControlMethod = MR::Network::ROOT_CONTROL_PHYSICAL_ANIMATION;
    break;

  default:
    physicsSetup->m_rootControlMethod = MR::Network::ROOT_CONTROL_ANIMATION;
    break;
  }
  NMP_VERIFY(physicsSetup->m_rootControlMethod != MR::Network::ROOT_CONTROL_ANIMATION);

  //---------------------------
  // Per Anim Set Attributes
  // Animation set dependent attribute data
  char attributeName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    MR::AttribDataPhysicsSetupPerAnimSet* physicsSetupPerAnimSet = MR::AttribDataPhysicsSetupPerAnimSet::init(memRes, MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,                 // semantic
      animSetIndex,                                                   // animSetIndex
      physicsSetupPerAnimSet,                                         // attribData
      MR::AttribDataPhysicsSetupPerAnimSet::getMemoryRequirements()); // attribMemReqs

    const MR::AnimSetIndex attributeNameIndex(animSetIndex + 1);

    sprintf_s(attributeName, "DisableSleeping_%d", attributeNameIndex);
    nodeDefDataBlock->readBool(physicsSetupPerAnimSet->m_disableSleeping, attributeName);
    
    sprintf_s(attributeName, "EnableJointLimits_%d", attributeNameIndex);
    nodeDefDataBlock->readBool(physicsSetupPerAnimSet->m_enableJointLimits, attributeName);

    sprintf_s(attributeName, "SoftKeyFramingMaxAccel_%d", attributeNameIndex);
    nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_softKeyFramingMaxAccel, attributeName);

    sprintf_s(attributeName, "SoftKeyFramingMaxAngAccel_%d", attributeNameIndex);
    nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_softKeyFramingMaxAngAccel, attributeName);

    sprintf_s(attributeName, "StrengthMultiplier_%d", attributeNameIndex);
    nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_activeAnimationStrengthMultiplier, attributeName);

    sprintf_s(attributeName, "DampingMultiplier_%d", attributeNameIndex);
    nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_activeAnimationDampingMultiplier, attributeName);

    sprintf_s(attributeName, "InternalCompliance_%d", attributeNameIndex);
    nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_activeAnimationInternalCompliance, attributeName);

    sprintf_s(attributeName, "ExternalCompliance_%d", attributeNameIndex);
    nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_activeAnimationExternalCompliance, attributeName);
    
    bool enableJointProjection = false;
    sprintf_s(attributeName, "EnableJointProjection_%d", attributeNameIndex);
    nodeDefDataBlock->readBool(enableJointProjection, attributeName);
    sprintf_s(attributeName, "EnableConstraint_%d", attributeNameIndex);
    nodeDefDataBlock->readBool(physicsSetupPerAnimSet->m_hardKeyframingEnableConstraint, attributeName);



    if (enableJointProjection)
    {
      sprintf_s(attributeName, "JointProjectionIterations_%d", attributeNameIndex);
      nodeDefDataBlock->readInt(physicsSetupPerAnimSet->m_jointProjectionIterations, attributeName);

      sprintf_s(attributeName, "JointProjectionLinearTolerance_%d", attributeNameIndex);
      nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_jointProjectionLinearTolerance, attributeName);

      sprintf_s(attributeName, "JointProjectionAngularTolerance_%d", attributeNameIndex);
      float degrees = 180.0f;
      nodeDefDataBlock->readFloat(degrees, attributeName);
      physicsSetupPerAnimSet->m_jointProjectionAngularTolerance = NMP::degreesToRadians(degrees);
    }
    else
    {
      physicsSetupPerAnimSet->m_jointProjectionIterations = 0;
      physicsSetupPerAnimSet->m_jointProjectionLinearTolerance = FLT_MAX;
      physicsSetupPerAnimSet->m_jointProjectionAngularTolerance = NM_PI;
    }

    sprintf_s(attributeName, "MassMultiplier_%d", attributeNameIndex);
    nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_massMultiplier, attributeName);

    sprintf_s(attributeName, "GravityCompensation_%d", attributeNameIndex);
    nodeDefDataBlock->readFloat(physicsSetupPerAnimSet->m_gravityCompensation, attributeName);
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
void NodePhysicsBuilder::initTaskQueuingFns(
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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodePhysicsQueueUpdateTransformsPostPhysics), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodePhysicsQueueUpdateTrajectory), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_PRE_PHYSICS_TRANSFORMS, FN_NAME(nodePhysicsQueueUpdateTransformsPrePhysics), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_OUTPUT_MASK, FN_NAME(grouperQueuePassThroughFromParent), logger);

  if (nodeDef->getNumChildNodes() == 0)
  {
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, FN_NAME(queueInitSampledEventsTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK, FN_NAME(queueInitUnitLengthSyncEventTrack), logger);
    nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, FN_NAME(queueInitEmptyEventTrackDurationSet), logger);
  }

  //---------------------------
  // Handle Data Pins
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
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodePhysicsDeleteInstance), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodePhysicsUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(nodePhysicsFindGeneratingNodeForSemantic), logger);
}

//----------------------------------------------------------------------------------------------------------------------
bool NodePhysicsBuilder::doAnyDescendantsCreatePrePhysicsTransformsDependencyOnAnUpstreamNode( const NodeDefDependency & nodeDep )
{
  std::vector< MR::NodeType > nodeTypes;
  NetworkDefBuilder::getNodeTypesOfAllDescendants( nodeDep, nodeTypes );

  if( std::find( nodeTypes.begin(), nodeTypes.end(), NODE_TYPE_BASIC_UNEVEN_TERRAIN ) != nodeTypes.end() )
  {
    return true;
  }

  if( std::find( nodeTypes.begin(), nodeTypes.end(), NODE_TYPE_PREDICTIVE_UNEVEN_TERRAIN ) != nodeTypes.end() )
  {
    return true;
  }

  return false;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
