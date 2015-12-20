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
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Functions for use by all nodes.
//----------------------------------------------------------------------------------------------------------------------
void nodeShareDeleteInstanceNULL(
  const NodeDef* NMP_UNUSED(node),
  Network*       NMP_UNUSED(net))
{
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareDeleteInstanceNoChildren(
  const NodeDef* node,
  Network*       net)
{
  // Delete all our own existing attribute data.
  net->cleanNodeData(node->getNodeID());
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareDeleteInstanceWithChildren(
  const NodeDef* node,
  Network*       net)
{
  NET_LOG_MESSAGE(99, "Deleting default node %s\n", node->getName());
  // Recurse to our children first.
  NMP_ASSERT(net->getNetworkDef());
  for (uint32_t i = 0; i < node->getNumChildNodes(); ++i)
  {
    if(node->getChildNodeID(i) != INVALID_NODE_ID)
    {
      net->deleteNodeInstance(node->getChildNodeID(i));
    }
  }

  // Delete all our own existing attribute data.
  net->cleanNodeData(node->getNodeID());
}

//----------------------------------------------------------------------------------------------------------------------
// Default node update connections functions.
//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsNULL(
  NodeDef* node,
  Network* NMP_UNUSED(net))
{
  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildren(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);

  // Recurse to children.
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSet);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildrenInputCPs(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  for (PinIndex i = 0; i < node->getNumInputCPConnections(); ++i)
  {
    net->updateInputCPConnection(node->getInputCPConnection(i), animSet);
  }

  // Recurse to children.
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSet);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildrenOptionalInputCPs(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected control parameters.
  for (PinIndex i = 0; i < node->getNumInputCPConnections(); ++i)
  {
    net->updateOptionalInputCPConnection(node->getInputCPConnection(i), animSet);
  }

  // Recurse to children.
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSet);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildren1CompulsoryManyOptionalInputCPs(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the one compulsory control parameter, which must be the first one.
  uint32_t numInputCPConnections = node->getNumInputCPConnections();
  NMP_ASSERT(numInputCPConnections > 0);
  net->updateInputCPConnection(node->getInputCPConnection(0), animSetIndex);

  // Update the other control parameters - all optional
  for (PinIndex i = 1; i < numInputCPConnections; ++i)
  {
    net->updateOptionalInputCPConnection(node->getInputCPConnection(i), animSetIndex);
  }

  // Recurse to children.
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSetIndex);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnectionsChildren2CompulsoryManyOptionalInputCPs(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(connections);
  AnimSetIndex animSetIndex = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the compulsory control parameters, which must be the first ones.
  uint32_t numInputCPConnections = node->getNumInputCPConnections();
  NMP_ASSERT(numInputCPConnections > 1);
  net->updateInputCPConnection(node->getInputCPConnection(0), animSetIndex);
  net->updateInputCPConnection(node->getInputCPConnection(1), animSetIndex);

  // Update the other control parameters - all optional
  for (PinIndex i = 2; i < numInputCPConnections; ++i)
  {
    net->updateOptionalInputCPConnection(node->getInputCPConnection(i), animSetIndex);
  }

  // Recurse to children.
  for (uint32_t i = 0; i < connections->m_numActiveChildNodes; ++i)
  {
    net->updateNodeInstanceConnections(connections->m_activeChildNodeIDs[i], animSetIndex);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnections1Child1InputCP(
  NodeDef* node,
  Network* net)
{
  // Update the only connected control parameter.
  NMP_ASSERT(node->getNumInputCPConnections() == 1);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  net->updateInputCPConnection(node->getInputCPConnection(0), animSet);

  // Recurse to child.
  NMP_ASSERT(net->getNumActiveChildren(node->getNodeID()) == 1);
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnections1Child1OptionalInputCP(
  NodeDef* node,
  Network* net)
{
  // Update the only connected control parameter.
  NMP_ASSERT(node->getNumInputCPConnections() == 1);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  net->updateOptionalInputCPConnection(node->getInputCPConnection(0), animSet);

  // Recurse to child.
  NMP_ASSERT(net->getNumActiveChildren(node->getNodeID()) == 1);
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeShareUpdateConnections1Child2InputCPs(
  NodeDef* node,
  Network* net)
{
  // Update the only connected control parameter.
  NMP_ASSERT(node->getNumInputCPConnections() == 2);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());
  net->updateInputCPConnection(node->getInputCPConnection(0), animSet);
  net->updateInputCPConnection(node->getInputCPConnection(1), animSet);

  // Recurse to child.
  NMP_ASSERT(net->getNumActiveChildren(node->getNodeID()) == 1);
  NodeID activeChildNodeID = net->getActiveChildNodeID(node->getNodeID(), 0);
  net->updateNodeInstanceConnections(activeChildNodeID, animSet);

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
// Default node init instance functions.
// These functions are called only when the network is instantiated.
//----------------------------------------------------------------------------------------------------------------------

// Initialize all the nodes pin attrib datas from active animation set
void nodeInitPinAttribDataInstance(NodeDef* node, Network* net)
{
  // Control param attribute data does not exist in the Network, make sure it exists in the NetworkDef,
  // then make a new instance of it in the Network that will take priority over the Def instance.
  // Copy the def content into the new instance.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  for (uint32_t p = 0; p < node->getNumReflexiveCPPins(); ++p)
  {
    // Create a new attribute and add it to the output control param pin.
    PinAttribDataInfo* padi = node->getPinAttribDataInfo(p);
    OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(padi->m_pin);
    switch (padi->m_semantic)
    {
    case ATTRIB_SEMANTIC_CP_BOOL:
      {
        AttribDataBool* defAttribData = node->getPinAttribData<AttribDataBool>(p, net->getActiveAnimSetIndex());
        outputCPPin->m_attribDataHandle = AttribDataBool::create(net->getPersistentMemoryAllocator(), defAttribData->m_value);
      }
      break;
    case ATTRIB_SEMANTIC_CP_UINT:
      {
        AttribDataUInt* defAttribData = node->getPinAttribData<AttribDataUInt>(p, net->getActiveAnimSetIndex());
        outputCPPin->m_attribDataHandle = AttribDataUInt::create(net->getPersistentMemoryAllocator(), defAttribData->m_value);
      }
      break;
    case ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER:
      {
        AttribDataPhysicsObjectPointer* defAttribData = node->getPinAttribData<AttribDataPhysicsObjectPointer>(p, net->getActiveAnimSetIndex());
        outputCPPin->m_attribDataHandle = AttribDataPhysicsObjectPointer::create(net->getPersistentMemoryAllocator(), defAttribData->m_value);
      }
      break;
    case ATTRIB_SEMANTIC_CP_INT:
      {
        AttribDataInt* defAttribData = node->getPinAttribData<AttribDataInt>(p, net->getActiveAnimSetIndex());
        outputCPPin->m_attribDataHandle = AttribDataInt::create(net->getPersistentMemoryAllocator(), defAttribData->m_value);
      }
      break;
    case ATTRIB_SEMANTIC_CP_FLOAT:
      {
        AttribDataFloat* defAttribData = node->getPinAttribData<AttribDataFloat>(p, net->getActiveAnimSetIndex());
        outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), defAttribData->m_value);
      }
      break;
    case ATTRIB_SEMANTIC_CP_VECTOR3:
      {
        AttribDataVector3* defAttribData = node->getPinAttribData<AttribDataVector3>(p, net->getActiveAnimSetIndex());
        outputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator(), defAttribData->m_value);
      }
      break;
    case ATTRIB_SEMANTIC_CP_VECTOR4:
      {
        AttribDataVector4* defAttribData = node->getPinAttribData<AttribDataVector4>(p, net->getActiveAnimSetIndex());
        outputCPPin->m_attribDataHandle = AttribDataVector4::create(net->getPersistentMemoryAllocator(), defAttribData->m_value);
      }
      break;
    default:
      NMP_ASSERT_FAIL_MSG("Semantic not supported as Pin AttribData");
      break;
    }
    outputCPPin->m_lastUpdateFrame = VALID_FOREVER;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceInvalidateAllChildrenInitPinAttrib(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  for (uint32_t iChild = 0; iChild < connections->m_numActiveChildNodes; ++iChild)
    connections->m_activeChildNodeIDs[iChild] = INVALID_NODE_ID;

  connections->m_numActiveChildNodes = 0;
  
  // Initialise any pins that are using default data.
  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceInvalidateAllChildren(
  NodeDef* node,
  Network* net)
{
  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());
  for (uint32_t iChild = 0; iChild < connections->m_numActiveChildNodes; ++iChild)
    connections->m_activeChildNodeIDs[iChild] = INVALID_NODE_ID;

  connections->m_numActiveChildNodes = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateFloatOutputAttribute(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateFloatOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
  outputCPPin->m_lastUpdateFrame = 0;

  // Initialise any pins that are using default data.
  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateIntOutputAttribute(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataInt::create(net->getPersistentMemoryAllocator(), 0);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateIntOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataInt::create(net->getPersistentMemoryAllocator(), 0);
  outputCPPin->m_lastUpdateFrame = 0;

  // Initialise any pins that are using default data.
  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateVector3OutputAttribute(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator());
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateVector3OutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataVector3::create(net->getPersistentMemoryAllocator());
  outputCPPin->m_lastUpdateFrame = 0;

  // Initialise any pins that are using default data.
  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateBoolOutputAttribute(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataBool::create(net->getPersistentMemoryAllocator(), false);
  outputCPPin->m_lastUpdateFrame = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeShareInitInstanceCreateBoolOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net)
{
  // Create the output attribute
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(0);
  outputCPPin->m_attribDataHandle = AttribDataBool::create(net->getPersistentMemoryAllocator(), false);
  outputCPPin->m_lastUpdateFrame = 0;

  // Initialise any pins that are using default data.
  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// An empty OutputCP function.
//  Every node that can emit requests must have an extra output CP function 
//  that actually deals with the update of emitted requests. 
//  The registered function can be empty if the requests are actually sent elsewhere.
//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeNullEmitMessageOutputCPUpdate(
  NodeDef* NMP_UNUSED(node),
  PinIndex NMP_UNUSED(outputCPPinIndex), /// The output pin we have been asked to update.
  Network* NMP_UNUSED(net))
{
  return NULL;
}


//----------------------------------------------------------------------------------------------------------------------
// Update all the nodes pin attrib datas from output animation set
void nodeUpdatePinAttribDataInstance(NodeDef* node, Network* net)
{
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());

  for (uint32_t p = 0; p < node->getNumReflexiveCPPins(); ++p)
  {
    // Update the attribute on the reflexive control param pin.
    PinAttribDataInfo* padi = node->getPinAttribDataInfo(p);
    OutputCPPin *outputCPPin = nodeBin->getOutputCPPin(padi->m_pin);
    NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData);
    if (padi->m_perAnimSet)
    {    
      switch (padi->m_semantic)
      {
      case ATTRIB_SEMANTIC_CP_BOOL:
        {
          NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData->getType() == ATTRIB_TYPE_BOOL);
          AttribDataBool* defAttribData = node->getPinAttribData<AttribDataBool>(p, nodeBin->getOutputAnimSetIndex());
          ((AttribDataBool*)(outputCPPin->m_attribDataHandle.m_attribData))->m_value = defAttribData->m_value;
        }
        break;
      case ATTRIB_SEMANTIC_CP_UINT:
        {
          NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData->getType() == ATTRIB_TYPE_UINT);
          AttribDataUInt* defAttribData = node->getPinAttribData<AttribDataUInt>(p, nodeBin->getOutputAnimSetIndex());
          ((AttribDataUInt*)(outputCPPin->m_attribDataHandle.m_attribData))->m_value = defAttribData->m_value;
        }
        break;
      case ATTRIB_SEMANTIC_CP_PHYSICS_OBJECT_POINTER:
        {
          NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData->getType() == ATTRIB_TYPE_PHYSICS_OBJECT_POINTER);
          AttribDataPhysicsObjectPointer* defAttribData = node->getPinAttribData<AttribDataPhysicsObjectPointer>(p, nodeBin->getOutputAnimSetIndex());
          ((AttribDataPhysicsObjectPointer*)(outputCPPin->m_attribDataHandle.m_attribData))->m_value = defAttribData->m_value;
        }
        break;
      case ATTRIB_SEMANTIC_CP_INT:
        {
          NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData->getType() == ATTRIB_TYPE_INT);
          AttribDataInt* defAttribData = node->getPinAttribData<AttribDataInt>(p, nodeBin->getOutputAnimSetIndex());
          ((AttribDataInt*)(outputCPPin->m_attribDataHandle.m_attribData))->m_value = defAttribData->m_value;
        }
        break;
      case ATTRIB_SEMANTIC_CP_FLOAT:
        {
          NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData->getType() == ATTRIB_TYPE_FLOAT);
          AttribDataFloat* defAttribData = node->getPinAttribData<AttribDataFloat>(p, nodeBin->getOutputAnimSetIndex());
          ((AttribDataFloat*)(outputCPPin->m_attribDataHandle.m_attribData))->m_value = defAttribData->m_value;
        }
        break;
      case ATTRIB_SEMANTIC_CP_VECTOR3:
        {
          NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData->getType() == ATTRIB_TYPE_VECTOR3);
          AttribDataVector3* defAttribData = node->getPinAttribData<AttribDataVector3>(p, nodeBin->getOutputAnimSetIndex());
          ((AttribDataVector3*)(outputCPPin->m_attribDataHandle.m_attribData))->m_value = defAttribData->m_value;
        }
        break;
      case ATTRIB_SEMANTIC_CP_VECTOR4:
        {
          NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData->getType() == ATTRIB_TYPE_VECTOR4);
          AttribDataVector4* defAttribData = node->getPinAttribData<AttribDataVector4>(p, nodeBin->getOutputAnimSetIndex());
          ((AttribDataVector4*)(outputCPPin->m_attribDataHandle.m_attribData))->m_value = defAttribData->m_value;
        }
        break;
      default:
        NMP_ASSERT_FAIL_MSG("Semantic not supported as Pin AttribData");
        break;
      }
    }
  }
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
