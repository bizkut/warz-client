//----------------------------------------------------------------------------------------------------------------------
#include "NetworkDefBuilder.h"
#include "RigRetargetMappingBuilder.h"
#include "NodeRetargetBuilder.h"
#include "morpheme/Nodes/mrNodeRetarget.h"
#include "morpheme/Nodes/mrNodePassthrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool NodeRetargetBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* NodeRetargetBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_RETARGET);
  NMP_ASSERT(semanticLookupTable);

  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_ACTIVE_ANIM_SET_INDEX);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void NodeRetargetBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Set the child node ID.
  uint32_t inputNodeID = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(inputNodeID, "SourceNodeID");
  NMP_VERIFY(inputNodeID != 0xFFFFFFFF);
  childNodeIDs.push_back((MR::NodeID) inputNodeID);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeRetargetBuilder::preInit(
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NodeRetargetBuilder::getNodeDefMemoryRequirements(
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             NMP_UNUSED(processor))
{  
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint16_t numAnimationSets = (uint16_t)animLibraryExport->getNumAnimationSets();

  //---------------------------
  // Allocate space for the NodeDef itself
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo,
    1,                // numChildern
    0,                // numInputCPConnections
    numAnimationSets, // numAnimSetEntries
    nodeDefExport);

  const NodeDefDependency & nodeDefDep = netDefCompilationInfo->getNodeDefDependency(nodeDefExport->getNodeID());
  result += (MR::AttribDataUInt::getMemoryRequirements() * (uint32_t)nodeDefDep.m_animSets.size());

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* NodeRetargetBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_RETARGET, "Expecting node type RETARGET");
 
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
  uint16_t numAnimationSets = (uint16_t)animLibraryExport->getNumAnimationSets();

  //---------------------------
  // Initialise the NodeDef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    1,                // numChildren
    1,                // max num active child nodes
    0,                // numInputCPConnections
    0,                // numOutputCPPins
    numAnimationSets, // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Flag as being a filter node.
  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER | MR::NodeDef::NODE_FLAG_REQUIRES_SEPARATE_TRAJECTORY_AND_TRANSFORMS);

  //---------------------------
  // Set the child node ID.
  uint32_t inputNodeID = 0xFFFFFFFF;
  nodeDefDataBlock->readUInt(inputNodeID, "SourceNodeID");
  NMP_VERIFY(inputNodeID != 0xFFFFFFFF);
  nodeDef->setChildNodeID(0, (MR::NodeID) inputNodeID);

  //---------------------------
  // register interest in message if this is used in an asset manager network.
  bool assetManagerExport = false;
  nodeDefDataBlock->readBool(assetManagerExport, "AssetManagerExport");

  if (assetManagerExport)
  {
    netDefCompilationInfo->registerMessageInterest(0, nodeDef->getNodeID());
    netDefCompilationInfo->registerMessageInterest(1, nodeDef->getNodeID());
  }

  //---------------------------
  // Initialise the attrib datas.
  std::vector<MR::AnimSetIndex> animSets;
  getAnimSetsFromDataBlockExport( nodeDefDataBlock, animSets, numAnimationSets );

  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimationSets; ++animSetIndex)
  {
    MR::AnimSetIndex inputAnimSetIndex = animSets[ animSetIndex ];
    MR::AttribDataUInt* inputAnimSetIndexAttribData = MR::AttribDataUInt::init(memRes, inputAnimSetIndex, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_ACTIVE_ANIM_SET_INDEX,
      animSetIndex,
      inputAnimSetIndexAttribData,
      MR::AttribDataUInt::getMemoryRequirements());
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
void NodeRetargetBuilder::initTaskQueuingFns(
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

  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER, FN_NAME(nodeRetargetQueueUpdateTransforms), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM, FN_NAME(nodeRetargetQueueTrajectoryDeltaTransform), logger);
  nodeDef->setTaskQueuingFnId(taskQueuingFns, MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER, FN_NAME(nodeRetargetQueueTrajectoryDeltaAndTransforms), logger);

  // Register the shared task function tables
  nodeDef->registerTaskQueuingFns(taskQueuingFnTables, taskQueuingFns);
  nodeDef->registerEmptyOutputCPTasks(outputCPTaskFnTables);

  // Tidy up
  NMP::Memory::memFree(taskQueuingFns);

  // Other manager registered functions.
  nodeDef->setDeleteNodeInstanceId(FN_NAME(nodeShareDeleteInstanceWithChildren), logger);
  nodeDef->setUpdateConnectionsFnId(FN_NAME(nodeRetargetUpdateConnections), logger);
  nodeDef->setFindGeneratingNodeForSemanticFnId(FN_NAME(filterNodeFindGeneratingNodeForSemantic), logger);
  nodeDef->setMessageHandlerFnId(FN_NAME(nodeRetargetUpdateOffsetsAndScaleMessageHandler), logger);
}

//----------------------------------------------------------------------------------------------------------------------
void NodeRetargetBuilder::getAnimSetsFromDataBlockExport( const ME::DataBlockExport* nodeDefDataBlock, 
                                                          std::vector<MR::AnimSetIndex> & animSets,
                                                          uint32_t numAnimationSets )
{
  animSets.reserve( numAnimationSets );

  CHAR attributeName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimationSets; ++animSetIndex)
  {
    uint32_t inputAnimSetIndex = 0xFFFFFFFF;
    sprintf_s(attributeName, "InputAnimSetIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(inputAnimSetIndex, attributeName);
    NMP_VERIFY(inputAnimSetIndex != 0xFFFFFFFF);

    animSets.push_back( static_cast<uint16_t>( inputAnimSetIndex ) );
  }
}

}
//----------------------------------------------------------------------------------------------------------------------
