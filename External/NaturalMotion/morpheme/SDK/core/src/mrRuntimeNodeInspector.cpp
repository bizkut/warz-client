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
#include "morpheme/mrNetwork.h"
#include "morpheme/mrRuntimeNodeInspector.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

#if defined(MR_OUTPUT_DEBUGGING)

//---------------------------------------------------------------------------------------------------------------------- 
namespace
{

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataSemantic convertNodeOutputDataTypeToAttribSemantic(MR::NodeOutputDataType type)
{
  MR::AttribDataSemantic result = MR::ATTRIB_SEMANTIC_NA;
  switch (type)
  {
  case NODE_OUTPUT_DATA_TYPE_TIME:
    result = MR::ATTRIB_SEMANTIC_TIME_POS;
    break;
  case NODE_OUTPUT_DATA_TYPE_TIME_UPDATE:
    result = MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS;
    break;
  case NODE_OUTPUT_DATA_TYPE_TRANSFORMS:
    result = MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER;
    break;
  case NODE_OUTPUT_DATA_TYPE_TRAJECTORY_CHANNEL_DELTAS:
    result = MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM;
    break;
  case NODE_OUTPUT_DATA_TYPE_TRAJECTORY_DELTA_TRANSFORMS:
    result = MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER;
    break;
  case NODE_OUTPUT_DATA_TYPE_SYNC_EVENT:
    result = MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS;
    break;
  case NODE_OUTPUT_DATA_TYPE_SYNC_EVENT_TRACK:
    result = MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK;
    break;
  case NODE_OUTPUT_DATA_TYPE_FLOAT:
    result = MR::ATTRIB_SEMANTIC_CP_FLOAT;
    break;
  case NODE_OUTPUT_DATA_TYPE_INT:
    result = MR::ATTRIB_SEMANTIC_CP_INT;
    break;
  case NODE_OUTPUT_DATA_TYPE_UINT:
    result = MR::ATTRIB_SEMANTIC_CP_UINT;
    break;
  case NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER:
    result = MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER;
    break;
  case NODE_OUTPUT_DATA_TYPE_BOOL:
    result = MR::ATTRIB_SEMANTIC_CP_BOOL;
    break;
  case NODE_OUTPUT_DATA_TYPE_VECTOR3:
    result = MR::ATTRIB_SEMANTIC_CP_VECTOR3;
    break;
  case NODE_OUTPUT_DATA_TYPE_VECTOR4:
    result = MR::ATTRIB_SEMANTIC_CP_VECTOR4;
    break;
  case NODE_OUTPUT_DATA_TYPE_CONDITION_SET_STATE:
    result = MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE;
    break;
  default:
    NMP_ASSERT_FAIL();
    break;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::NodeOutputDataType convertAttribSemanticToNodeOutputDataType(MR::AttribDataSemantic semantic)
{
  MR::NodeOutputDataType result = MR::INVALID_NODE_OUTPUT_DATA_TYPE;
  switch (semantic)
  {
  case MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE:
    result = NODE_OUTPUT_DATA_TYPE_CONDITION_SET_STATE;  // MORPH-21374: Remove this bad assumption.
    break;
  case MR::ATTRIB_SEMANTIC_TIME_POS:
    result = NODE_OUTPUT_DATA_TYPE_TIME;
    break;
  case MR::ATTRIB_SEMANTIC_UPDATE_TIME_POS:
    result = NODE_OUTPUT_DATA_TYPE_TIME_UPDATE;
    break;
  case MR::ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
    result = NODE_OUTPUT_DATA_TYPE_TRANSFORMS;
    break;
  case MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
    result = NODE_OUTPUT_DATA_TYPE_TRAJECTORY_CHANNEL_DELTAS;
    break;
  case MR::ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
    result = NODE_OUTPUT_DATA_TYPE_TRAJECTORY_DELTA_TRANSFORMS;
    break;
  case MR::ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS:
    result = NODE_OUTPUT_DATA_TYPE_SYNC_EVENT;
    break;
  case MR::ATTRIB_SEMANTIC_SYNC_EVENT_TRACK:
    result = NODE_OUTPUT_DATA_TYPE_SYNC_EVENT_TRACK;
    break;
  case MR::ATTRIB_SEMANTIC_CP_FLOAT:
    result = NODE_OUTPUT_DATA_TYPE_FLOAT;
    break;
  case MR::ATTRIB_SEMANTIC_CP_INT:
    result = NODE_OUTPUT_DATA_TYPE_INT;
    break;
  case MR::ATTRIB_SEMANTIC_CP_UINT:
    result = NODE_OUTPUT_DATA_TYPE_UINT;
    break;
  case MR::ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER:
    result = NODE_OUTPUT_DATA_TYPE_PHYSICS_OBJECT_POINTER;
    break;
  case MR::ATTRIB_SEMANTIC_CP_BOOL:
    result = NODE_OUTPUT_DATA_TYPE_BOOL;
    break;
  case MR::ATTRIB_SEMANTIC_CP_VECTOR3:
    result = NODE_OUTPUT_DATA_TYPE_VECTOR3;
    break;
  case MR::ATTRIB_SEMANTIC_CP_VECTOR4:
    result = NODE_OUTPUT_DATA_TYPE_VECTOR4;
    break;
  default:
    NMP_ASSERT_FAIL();
    break;
  }
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* getSourceAttribData(
  MR::Network*            net,
  MR::AttribDataSemantic  semantic,
  MR::NodeID              owningNodeID,
  MR::AnimSetIndex&       animSetIndex,   // What anim set this attrib data was created for.
  MR::FrameCount&         validFrame)     // Usually the frame it was created or MR::VALID_FOREVER.
{
  // Get the source from the network
  MR::AttribData* sourceAttrib;
 
  // First try to find a value that's flagged as valid forever
  //  If that doesn't work look for one for this specific frame
  MR::NodeBinEntry* sourceAttribEntry = net->getAttribDataNodeBinEntryRecurseFilterNodes(semantic, owningNodeID, MR::INVALID_NODE_ID, MR::VALID_FOREVER, net->getActiveAnimSetIndex());
  animSetIndex = net->getOutputAnimSetIndex(owningNodeID);
  validFrame = MR::VALID_FOREVER;

  if (!sourceAttribEntry)
  {
    sourceAttribEntry = net->getAttribDataNodeBinEntryRecurseFilterNodes(semantic, owningNodeID, MR::INVALID_NODE_ID, net->getCurrentFrameNo(), net->getActiveAnimSetIndex());
    validFrame = net->getCurrentFrameNo();
  }

  if (sourceAttribEntry)
  {
    sourceAttrib = sourceAttribEntry->getAttribData();
  }
  else
  {
    // if it's not in the network it may be in the def
    MR::AttribDataHandle* handle = net->getNetworkDef()->getOptionalAttribDataHandle(semantic, owningNodeID, net->getActiveAnimSetIndex());
    sourceAttrib = handle ? handle->m_attribData : NULL;
    validFrame = MR::VALID_FOREVER;
  }

  return sourceAttrib;
}

//----------------------------------------------------------------------------------------------------------------------
}

//----------------------------------------------------------------------------------------------------------------------
// NOTE: This function is to be removed when we have moved fully to the new system of filtering node output attrib data.
// The new filtering system will be fully user controlled. Navigating to different views of the running network
// will not change the data that is being buffered.
bool RuntimeNodeInspector::enableNodeDataBuffering(
  MR::Network*            net,
  MR::NodeID              nodeID,   //  Node Network ID to switch on or off.
  MR::NodeOutputDataType  dataType, //
  bool                    enable)   // Turn buffering on or off for this node
{
  // Control parameter node attributes are persistent and stored separately
  const MR::NodeDef *nodeDef = net->getNetworkDef()->getNodeDef(nodeID);
  if(nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
    return true;

  MR::AttribDataSemantic semantic = convertNodeOutputDataTypeToAttribSemantic(dataType);
  if (enable)
  {
    // Tell the network which output attributes we want to watch externally.
    net->addPostUpdateAccessAttrib(nodeID, semantic);
  }
  else
  {
    // Tell the network which output attributes to release.
    net->removePostUpdateAccessAttrib(nodeID, semantic);
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t RuntimeNodeInspector::getNodeBufferedDataTypeSize(
  MR::Network*            net,
  MR::NodeID              owningNodeID,   // Node Network ID.
  MR::NodeOutputDataType  outputType,     // Type we want to get the size of
  MR::NodeID&             targetNodeID,   // Returned - usually MR::INVALID_NODE_ID
  MR::AttribDataType&     attribType,     // Runtime attribute data type.
  MR::AttribDataSemantic& attribSemantic, // Runtime semantic identifier.
  MR::AnimSetIndex&       animSetIndex,   // What anim set this attrib data was created for.
  MR::FrameCount&         validFrame)     // Usually the frame it was created or MR::VALID_FOREVER.
{
  attribSemantic = convertNodeOutputDataTypeToAttribSemantic(outputType); // MORPH-21375: Stop using this innacurate approach.
  MR::AttribData* sourceAttrib = 0;

  // Control parameters are stored separately
  const MR::NodeDef *nodeDef = net->getNetworkDef()->getNodeDef(owningNodeID);
  if(nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
  {
    NodeBin* nodeBin = net->getNodeBin(owningNodeID);
    sourceAttrib = nodeBin->getOutputCPPin(MR::CONTROL_PARAMETER_NODE_PIN_0)->getAttribData();
  }
  else
  {
    sourceAttrib = getSourceAttribData(net, attribSemantic, owningNodeID, animSetIndex, validFrame);
  }

  // It's an error not to find the required attribute but we will do this for now.
  if (!sourceAttrib)
    return 0;

  MR::Manager& manager = MR::Manager::getInstance();
  attribType = sourceAttrib->getType();
  targetNodeID = MR::INVALID_NODE_ID;

  // call the serialize function with null buffer to ask it to return a required buffer size
  uint32_t bufferSize = 0;
  MR::AttribSerializeTxFn serializeFn = manager.getAttribSerializeTxFn(sourceAttrib->getType());
  if (serializeFn)
  {
    bufferSize = serializeFn(net, owningNodeID, sourceAttrib, 0, 0);
    NMP_ASSERT(bufferSize > 0);
  }

  return bufferSize;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* RuntimeNodeInspector::getNodeAttribDataForOutputDataType(
  MR::Network*            net,
  MR::NodeID              owningNodeID,
  MR::NodeOutputDataType  outputType)
{
  MR::AttribData* sourceAttrib = 0;

  // Control parameters are stored separately
  const MR::NodeDef *nodeDef = net->getNetworkDef()->getNodeDef(owningNodeID);
  if(nodeDef->getNodeFlags().isSet(MR::NodeDef::NODE_FLAG_IS_CONTROL_PARAM))
  {
    NodeBin* nodeBin = net->getNodeBin(owningNodeID);
    sourceAttrib = nodeBin->getOutputCPPin(MR::CONTROL_PARAMETER_NODE_PIN_0)->getAttribData();
  }
  else
  {
    MR::AttribDataSemantic attribSemantic = convertNodeOutputDataTypeToAttribSemantic(outputType);
    MR::AnimSetIndex animSetIndex;
    MR::FrameCount   validFrame;
    sourceAttrib = getSourceAttribData(net, attribSemantic, owningNodeID, animSetIndex, validFrame);
  }

  return sourceAttrib;
}

//----------------------------------------------------------------------------------------------------------------------
bool RuntimeNodeInspector::serialiseAttribData(
  MR::Network*            net,
  MR::NodeID              owningNodeID,
  MR::AttribData*         attribData,
  void*                   buffer,
  uint32_t                bufferSize)
{
  MR::Manager& manager = MR::Manager::getInstance();
  MR::AttribSerializeTxFn serializeFn = manager.getAttribSerializeTxFn(attribData->getType());
  if (serializeFn)
  {
#ifdef NMP_ENABLE_ASSERTS
    // Check the buffer is the correct size for the data we're going to put in it.
    uint32_t checkBufferSize = serializeFn(net, owningNodeID, attribData, 0, 0);
    NMP_ASSERT(bufferSize == checkBufferSize);
#endif // NMP_ENABLE_ASSERTS

    serializeFn(net, owningNodeID, attribData, buffer, bufferSize);
    return true;
  }

  NMP_ASSERT_FAIL();
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t RuntimeNodeInspector::getNodeNodeOutputDataTypes(
  MR::Network*            net,
  MR::NodeID              nodeID,             // Node Network ID.
  MR::NodeOutputDataType* typesArray,         // Array of types to be filled in.
  uint32_t                maxTypesArraySize)  // The size of the provided array (number of entries).
{
  MR::NodeDef* nodeDef = net->getNetworkDef()->getNodeDef(nodeID);
  uint32_t numTypes = 0;
  const uint32_t numEntries = MR::Manager::getInstance().getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; i++)
  {
    const MR::AttribDataSemantic semantic = (MR::AttribDataSemantic)i;
    if (nodeDef->getTaskQueueingFn(semantic)) // || nodeDef->getOutputCPTask(semantic))
    {
      typesArray[numTypes] = convertAttribSemanticToNodeOutputDataType(semantic);
      numTypes++;
      if (numTypes == maxTypesArraySize)
      {
        // We have filled the buffer
        break;
      }
    }
  }

  return numTypes;
}

#endif // MR_OUTPUT_DEBUGGING

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
