//----------------------------------------------------------------------------------------------------------------------
#include "NetworkDefBuilder.h"
#include "NodeLimbInfoBuilder.h"
#include "euphoria/erDefines.h"
#include "euphoria/Nodes/erNodeLimbInfo.h"
#include "euphoria/Nodes/erNodeLimbInfo.h"
#include "morpheme/Nodes/mrNodePassThrough.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "morpheme/mrNetworkDef.h"
#include "morpheme/mrAttribData.h"

//----------------------------------------------------------------------------------------------------------------------
namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
bool AP::NodeLimbInfoBuilder::hasSemanticLookupTable()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
MR::SemanticLookupTable* AP::NodeLimbInfoBuilder::initSemanticLookupTable(NMP::Memory::Resource& memRes)
{
  MR::SemanticLookupTable* semanticLookupTable = MR::SemanticLookupTable::init(memRes, NODE_TYPE_LIMB_INFO);
  NMP_ASSERT(semanticLookupTable);

  // Per anim set semantics.
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_UINT);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_VECTOR4);

  // Anim set independent semantics.
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_INT);
  semanticLookupTable->addLookupIndex(MR::ATTRIB_SEMANTIC_CP_BOOL);

  return semanticLookupTable;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeLimbInfoBuilder::getNodeDefInputConnections(
  std::vector<MR::NodeID>&    childNodeIDs,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  // Source node
  MR::NodeID source0NodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", source0NodeID);
  childNodeIDs.push_back(source0NodeID);
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeLimbInfoBuilder::calculateNodeDefMetrics(
  const ME::DataBlockExport* NMP_UNUSED(nodeDefDatablock),
  uint32_t& numberOfChildren,
  uint32_t& numberOfControlParams)
{
  // Decide on number of children/cparams, either statically or by reading data from nodeDefDatablock.
  numberOfChildren = 1;
  numberOfControlParams = 1;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AP::NodeLimbInfoBuilder::getNodeDefMemoryRequirements( 
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{
  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();

  //---------------------------
  // Work out how many attrib datas we actually have.
  uint32_t numAnimSets = 0;
  nodeDefDataBlock->readUInt(numAnimSets, "NumAnimSets");
  NMP_VERIFY(numAnimSets > 0);

  //---------------------------
  // Allocate space for the NodeDef itself.
  NMP::Memory::Format result = getCoreNodeDefMemoryRequirements(
    netDefCompilationInfo, 
    1,              // numChildren
    1,              // numInputCPConnections
    numAnimSets,    // numAnimSetEntries
    nodeDefExport);

  //---------------------------
  // Animation set dependent attribute data
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    result += MR::AttribDataUInt::getMemoryRequirements();    // Rig part index.
    result += MR::AttribDataVector4::getMemoryRequirements(); // Rig to limb transform quat.
  }

  //---------------------------
  // Attribute data
  result += MR::AttribDataInt::getMemoryRequirements();  // Output angle type.
  result += MR::AttribDataBool::getMemoryRequirements(); // Output angle in radians.

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeLimbInfoBuilder::preInit( 
  NetworkDefCompilationInfo*  NMP_UNUSED(netDefCompilationInfo),
  const ME::NodeExport*       NMP_UNUSED(nodeDefExport),
  const ME::NetworkDefExport* NMP_UNUSED(netDefExport),
  AssetProcessor*             NMP_UNUSED(processor))
{}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeDef* AP::NodeLimbInfoBuilder::init(
  NMP::Memory::Resource&      memRes,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  const ME::NodeExport*       nodeDefExport,
  const ME::NetworkDefExport* netDefExport,
  AssetProcessor*             processor)
{
  NMP_VERIFY_MSG(nodeDefExport->getTypeID() == NODE_TYPE_LIMB_INFO, "Expecting node type LIMB_INFO");

  const ME::DataBlockExport* nodeDefDataBlock = nodeDefExport->getDataBlock();
  uint32_t numChildren(0);
  uint32_t numControlParams(0);

  calculateNodeDefMetrics(nodeDefDataBlock, numChildren, numControlParams);

  const int16_t numAnimSets(readNumAnimSets(nodeDefDataBlock));

  // Initialise the nodedef itself.
  MR::NodeDef* nodeDef = initCoreNodeDef(
    memRes,
    netDefCompilationInfo,
    numChildren,
    (uint16_t) numChildren,
    numControlParams,
    ER::NODE_LIMB_INFO_OUT_PINID_COUNT,
    numAnimSets,
    nodeDefExport);

  nodeDef->setPassThroughChildIndex(0);
  nodeDef->setNodeFlags(MR::NodeDef::NODE_FLAG_IS_FILTER);

  //---------------------------
  // Set child node ID.
  MR::NodeID sourceNodeID;
  readNodeID(nodeDefDataBlock, "SourceNodeID", sourceNodeID);
  nodeDef->setChildNodeID(0, sourceNodeID);
  //---------------------------
  // Initialise the attrib datas.

  //---------------------------
  // Animation set dependent attribute data.
  CHAR paramName[256];
  for (MR::AnimSetIndex animSetIndex = 0; animSetIndex < numAnimSets; ++animSetIndex)
  {
    // Read the limb index.
    uint32_t limbIndex(0);
    sprintf_s(paramName, "LimbIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(limbIndex, paramName);

    // Read the part index.
    uint32_t limbPartIndex(0);
    sprintf_s(paramName, "PartIndex_%d", animSetIndex + 1);
    nodeDefDataBlock->readUInt(limbPartIndex, paramName);

    // Determine the rig joint index and transform from rig to limb part.
    uint32_t  jointIndex(0);
    NMP::Quat rigToLimbTransform(NMP::Quat::kIdentity);

    NMP_ASSERT(netDefExport); // Null check - validate function argument

    const ME::AnimationLibraryExport* animLibraryExport = netDefExport->getAnimationLibrary();
    const ME::AnimationSetExport* animSetExport = animLibraryExport->getAnimationSet(animSetIndex);

    const ME::BodyDefExport* bodyDefExport = animSetExport->getBodyDef();

    NMP_ASSERT(bodyDefExport); // Null check

    if (bodyDefExport)
    {
      const ME::LimbDefExport* limbDefExport = bodyDefExport->getLimb(limbIndex);

      NMP_ASSERT(limbDefExport); // Null check

      if (limbDefExport)
      {
        // Find the name of the limb part specified in the node attributes and the transform from rig part to limb part.
        const char* limbPartName(NULL);

        switch (limbPartIndex)
        {
        case ER::NODE_LIMB_INFO_LIMB_PART_ROOT:
          limbPartName = limbDefExport->getRootPartName();
          rigToLimbTransform = limbDefExport->getRootOffset().toQuat();
          break;
        case ER::NODE_LIMB_INFO_LIMB_PART_END:
          limbPartName = limbDefExport->getEndPartName();
          rigToLimbTransform = limbDefExport->getEndOffset().toQuat();
          break;
        default:
          NMP_ASSERT_FAIL(); // Invalid limb part index.
          break;
        }

        NMP_ASSERT(limbPartName); // Null check.

        // Linear search for limb part in anim rig.
        const ME::RigExport* animRig(animSetExport->getRig());
        const uint32_t numJoints(animRig->getNumJoints());

        for (jointIndex = 0; jointIndex < numJoints; ++jointIndex)
        {
          const ME::JointExport* jointExport(animRig->getJoint(jointIndex));

          const char* name(jointExport->getName());

          if (strcmp(name, limbPartName) == 0)
          {
            break;
          }
        }

        NMP_ASSERT(jointIndex < numJoints); // Assert if joint index not found.
      }
    }

    // Initialize and set the rig part index.
    MR::AttribDataUInt* jointIndexAttr = MR::AttribDataUInt::init(memRes, jointIndex, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_CP_UINT,                  // semantic,
      animSetIndex,                                 // animSetIndex,
      jointIndexAttr,                               // attribData,
      MR::AttribDataUInt::getMemoryRequirements()); // attribMemReqs

    NMP_ASSERT(jointIndexAttr); // Null check.

    jointIndexAttr->m_value = jointIndex;

    // Initialize and set the transform from rig part to limb part.
    MR::AttribDataVector4* rigToLimbTransformAttr
      = MR::AttribDataVector4::init(memRes, rigToLimbTransform, MR::IS_DEF_ATTRIB_DATA);
    initAttribEntry(
      nodeDef,
      MR::ATTRIB_SEMANTIC_CP_VECTOR4,                  // semantic,
      animSetIndex,                                    // animSetIndex,
      rigToLimbTransformAttr,                          // attribData,
      MR::AttribDataVector4::getMemoryRequirements()); // attribMemReqs

    NMP_ASSERT(rigToLimbTransformAttr); // Null check.

    rigToLimbTransformAttr->m_value = rigToLimbTransform;
  }

  // Angle type: [0] = "total", [1] = "eulerX", [2] = "eulerY", [3] = "eulerZ"
  MR::AttribDataInt* outputOrientationTypeAttr = MR::AttribDataInt::init(memRes, 0, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_INT,                  // semantic,    
    0,                                           // animSetIndex,
    outputOrientationTypeAttr,                   // attribData,  
    MR::AttribDataInt::getMemoryRequirements()); // attribMemReqs

  // Read and set the angle type.
  int32_t outputOrientationType = 0;
  nodeDefDataBlock->readInt(outputOrientationType, "OutputOrientationType");
  outputOrientationTypeAttr->m_value = outputOrientationType;

  // OutputAngleInRadians: true = radians, false = degrees
  MR::AttribDataBool* outputAngleInRadiansAttr = MR::AttribDataBool::init(memRes, true, MR::IS_DEF_ATTRIB_DATA);
  initAttribEntry(
    nodeDef,
    MR::ATTRIB_SEMANTIC_CP_BOOL,                  // semantic,    
    0,                                            // animSetIndex,
    outputAngleInRadiansAttr,                     // attribData,  
    MR::AttribDataBool::getMemoryRequirements()); // attribMemReqs

  // Read and set the measure units.
  bool outputAngleInRadians = true;
  nodeDefDataBlock->readBool(outputAngleInRadians, "OutputAngleInRadians");
  outputAngleInRadiansAttr->m_value = outputAngleInRadians;

  //---------------------------
  // Initialise the task function tables
  initTaskQueuingFns(nodeDef, netDefCompilationInfo, processor->getMessageLogger());

  return nodeDef;
}

//----------------------------------------------------------------------------------------------------------------------
void AP::NodeLimbInfoBuilder::initTaskQueuingFns(
  MR::NodeDef*                nodeDef,
  NetworkDefCompilationInfo*  netDefCompilationInfo,
  NMP::BasicLogger*           logger)
{
  //------------------------------------
  // Shared task function tables
  NMP_ASSERT(netDefCompilationInfo);
  MR::SharedTaskFnTables* taskQueuingFnTables = netDefCompilationInfo->getTaskQueuingFnTables();
  MR::SharedTaskFnTables* outputCPTaskFnTables = netDefCompilationInfo->getOutputCPTaskFnTables();
  MR::QueueAttrTaskFn* taskQueuingFns = reinterpret_cast< MR::QueueAttrTaskFn* >(
    MR::SharedTaskFnTables::createSharedTaskFnTable());

  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    // Most queuing requests get passed on to our child.
    nodeDef->setTaskQueuingFnId(taskQueuingFns, i, FN_NAME(queuePassThroughChild0), logger);
  }

  // Task queuing function for TRANSFORM_BUFFER semantic.
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER,
    ER_FN_NAME(nodeLimbInfoQueueTransforms),
    logger);

  // Task queuing function for TRAJECTORY_DELTA_TRANSFORM_BUFFER semantic.
  nodeDef->setTaskQueuingFnId(
    taskQueuingFns,
    MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER,
    ER_FN_NAME(nodeLimbInfoQueueTrajectoryDeltaAndTransforms),
    logger);

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
