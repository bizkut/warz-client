//----------------------------------------------------------------------------------------------------------------------
#include "NetworkDefBuilder.h"
#include "NodeTrajectoryOverrideBuilder.h"
#include "morpheme/mrNetworkDef.h"
#include "physics/mrPhysicsAttribData.h"
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeTrajectoryOverride.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------
namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
bool NodeTrajectoryOverrideBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* AP::NodeTrajectoryOverrideBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_TRAJECTORY_OVERRIDE);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET);

  return semanticLookupTable;
}


//----------------------------------------------------------------------------------------------------------------------
void AP::NodeTrajectoryOverrideBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  MR::NodeID sourceParamID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceParamID, true);
  if (sourceParamID != MR::INVALID_NODE_ID)
  {
    childNodeIDs.push_back(sourceParamID);
  }

  MR::NodeID poseParamID;
  readNodeID(nodeDefDataBlock, "PoseNodeID", poseParamID, true);
  if (poseParamID != MR::INVALID_NODE_ID)
  {
    childNodeIDs.push_back(poseParamID);
  }

  readDataPinChildNodeID(nodeDefDataBlock, "Direction", childNodeIDs, false);
  readDataPinChildNodeID(nodeDefDataBlock, "Activate", childNodeIDs, true);

}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeTrajectoryOverrideBuilder::calculateNodeDefMetrics(
  const ME::DataBlockExport* nodeDefDatablock,
  uint32_t& numberOfChildren)
{
  numberOfChildren = 0;

  // Find number of children by reading data from nodeDefDatablock.
  MR::NodeID nodeID;
  if (readNodeID(nodeDefDatablock, "SourceNodeID", nodeID, true))
  {
    numberOfChildren++;
  }
  if (readNodeID(nodeDefDatablock, "PoseNodeID", nodeID, true))
  {
    numberOfChildren++;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AP::NodeTrajectoryOverrideBuilder::getNodeDefMemoryRequirements( 
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDatablock = nodeDefExport->getDataBlock();
  uint32_t numberOfChildren;
  calculateNodeDefMetrics(nodeDefDatablock, numberOfChildren);

  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();

  // Work out how many attrib datas we actually have.
  uint32_t numAnimSets = animLibraryExport->getNumAnimationSets();
  NMP_VERIFY(numAnimSets > 0);

  // Allocate space for the NodeDef.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    numberOfChildren,
    2,
    numAnimSets,
    nodeDefExport);

  // Reserve space for the attrib data for each anim set.
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    result += MR::AttribDataTrajectoryOverrideDefAnimSet::getMemoryRequirements();
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeTrajectoryOverrideBuilder::preInit( 
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  declareDataPin(netDefCompilationInfo, nodeDefExport, "Direction", 0, false, MR::ATTRIB_SEMANTIC_CP_VECTOR3);
  declareDataPin(netDefCompilationInfo, nodeDefExport,  "Activate",1, true, MR::ATTRIB_SEMANTIC_CP_BOOL);
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* AP::NodeTrajectoryOverrideBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_TRAJECTORY_OVERRIDE, "Expecting node type TRAJECTORY_OVERRIDE");

  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  uint32_t numberOfChildren(0);
  calculateNodeDefMetrics(nodeDefDataBlock, numberOfChildren);

  // Determine how many anim sets there are.
  const ME::AnimationLibraryExport* animLibraryExport(netDefExport->getAnimationLibrary());
  NMP_ASSERT(animLibraryExport); // Null check.

  const uint16_t numAnimSets(static_cast<uint16_t>(animLibraryExport->getNumAnimationSets()));
  NMP_VERIFY(numAnimSets > 0);

  // Initialise the nodedef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    numberOfChildren,             // numChildren
    2,                            // max num active child nodes
    2,                            // numInputCPConnections
    0,                            // numberOfOutputControlParams
    numAnimSets,
    nodeDefExport);

  // Functional Pin - Source
  MR::NodeID SourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", SourceNodeID);
  if (SourceNodeID != MR::INVALID_NODE_ID)
  {
    nodeDef->setChildNodeID(0, SourceNodeID);
  }

  // Functional Pin - Pose
  MR::NodeID PoseNodeID;
  readNodeID(nodeDefDataBlock, "PoseNodeID", PoseNodeID, true);
  if (PoseNodeID != MR::INVALID_NODE_ID)
  {
    nodeDef->setChildNodeID(1, PoseNodeID);
  }

  // Flag node as a filter.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER|MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS);

  //---------------------------
  // Initialise attrib data.

  // Animation set dependent attribute data
  CHAR paramName[256];

  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    MR::AttribDataTrajectoryOverrideDefAnimSet* attribData =
      MR::AttribDataTrajectoryOverrideDefAnimSet::init(memRes, MR::IS_DEF_ATTRIB_DATA);

    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF_ANIM_SET,                       // semantic,
      animSetIndex,                                                         // animSetIndex,
      attribData,                                                           // attribData,
      MR::AttribDataTrajectoryOverrideDefAnimSet::getMemoryRequirements()); // attribMemReqs

    // Read attribute values from serialized data.

    // Reference part values
    sprintf_s(paramName, "PhysicsPartIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(attribData->m_physicsPartIndex, paramName);

    // Edge forward direction in character space values.
    sprintf_s(paramName, "DirectionInCharacterSpaceX_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(attribData->m_directionInCharacterSpace.x, paramName);

    sprintf_s(paramName, "DirectionInCharacterSpaceY_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(attribData->m_directionInCharacterSpace.y, paramName);

    sprintf_s(paramName, "DirectionInCharacterSpaceZ_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(attribData->m_directionInCharacterSpace.z, paramName);

    // Ensure input is normalised.
    attribData->m_directionInCharacterSpace.normalise();

    // Project onto ground values.
    sprintf_s(paramName, "ProjectOntoGround_%d", animSetIndex + 1);
    nodeDefDataBlock->readBool(attribData->m_projectOntoGround, paramName);

    sprintf_s(paramName, "ProjectOntoGroundDistance_%d", animSetIndex + 1);
    nodeDefDataBlock->readFloat(attribData->m_projectOntoGroundDistance, paramName);
  }

  //---------------------------
  // Initialise task queuing functions.
  initTaskQueuingFns(
    nodeDef,
    netDefCompilationInfo,
    processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeTrajectoryOverrideBuilder::initTaskQueuingFns(
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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, ER_FN_NAME(nodeTrajectoryOverrideQueueUpdate), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeShareUpdateConnectionsChildren), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
}

//----------------------------------------------------------------------------------------------------------------------
} // namespace AP
