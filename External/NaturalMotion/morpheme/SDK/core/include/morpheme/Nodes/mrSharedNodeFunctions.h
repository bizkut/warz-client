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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_SHARED_NODE_FUNCTIONS_H
#define MR_SHARED_NODE_FUNCTIONS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Functions for use by all nodes.
//----------------------------------------------------------------------------------------------------------------------
// NULL Delete node instance function. Used by a node if it does not need to implement its own.
void nodeShareDeleteInstanceNULL(
  const NodeDef* node,
  Network*       net
);

//----------------------------------------------------------------------------------------------------------------------
// Default Delete node instance function. Used by a node if it does not need to implement its own.
void nodeShareDeleteInstanceNoChildren(
  const NodeDef* node,
  Network*       net
);

//----------------------------------------------------------------------------------------------------------------------
// Default Delete node instance function. Used by a node if it does not need to implement its own.
// Recurses the delete to all children before deleting all its own existing attributes.
void nodeShareDeleteInstanceWithChildren(
  const NodeDef* node,
  Network*       net
);

//----------------------------------------------------------------------------------------------------------------------
// Default node update connections functions.
//----------------------------------------------------------------------------------------------------------------------
/// \brief NULL Update connections function. Used by a node if it has no children and does not need to perform any immediate ops.
NodeID nodeShareUpdateConnectionsNULL(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Performs no operations on this node but iterates to the children.
/// Updates all active children.
NodeID nodeShareUpdateConnectionsChildren(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Performs no operations on this node but updates input control params and iterates to the children.
/// Updates all active children and all input control params are assumed non optional.
NodeID nodeShareUpdateConnectionsChildrenInputCPs(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Performs no operations on this node but updates input control params and iterates to the children.
/// Updates all active children and all input control params are assumed optional.
NodeID nodeShareUpdateConnectionsChildrenOptionalInputCPs(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Performs no operations on this node but updates input control params and iterates to the children.
/// Updates all active children.  The first input control param is considered compulsory and the
/// remainder optional.
NodeID nodeShareUpdateConnectionsChildren1CompulsoryManyOptionalInputCPs(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Performs no operations on this node but updates input control params and iterates to the children.
/// Updates all active children.  The first twp input control params are considered compulsory and the
/// remainder optional.
NodeID nodeShareUpdateConnectionsChildren2CompulsoryManyOptionalInputCPs(
  NodeDef*           node,
  Network*           net);

  //----------------------------------------------------------------------------------------------------------------------
/// \brief Performs no operations on this node but updates an input control param and iterates to the child.
/// There must be only 1 active child and the input control param is assumed non optional.
/// A little faster than the more general nodeUpdateConnectionsDefaultChildrenInputCPs.
NodeID nodeShareUpdateConnections1Child1InputCP(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Performs no operations on this node but updates an optional input control param and iterates to the child.
/// There must be only 1 active child and the input control param is optional.
NodeID nodeShareUpdateConnections1Child1OptionalInputCP(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
/// \brief Performs no operations on this node but updates 2 optional input control params and iterates to the child.
/// There must be only 1 active child and the input control params are optional.
NodeID nodeShareUpdateConnections1Child2InputCPs(
  NodeDef*           node,
  Network*           net);

//----------------------------------------------------------------------------------------------------------------------
// Default node init instance functions.
// These functions are called only when the network is instantiated.
//----------------------------------------------------------------------------------------------------------------------
/// \brief Sets all possible active child connections to INVALID_NODE_ID and sets the number of active children to zero.
void nodeShareInitInstanceInvalidateAllChildren(
  NodeDef* node,
  Network* net);

/// \brief Sets all possible active child connections to INVALID_NODE_ID and sets the number of active children to zero.
/// It also initializes any pin attrib data on the node
void nodeShareInitInstanceInvalidateAllChildrenInitPinAttrib(
  NodeDef* node,
  Network* net);

/// \brief Creates a float output attribute and sets it to 0.0f
void nodeShareInitInstanceCreateFloatOutputAttribute(
  NodeDef* node,
  Network* net);

/// \brief Creates a float output attribute and sets it to 0.0f. It also initializes any pin attrib data on the node
void nodeShareInitInstanceCreateFloatOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net);

/// \brief Creates a int output attribute and sets it to 0
void nodeShareInitInstanceCreateIntOutputAttribute(
  NodeDef* node,
  Network* net);

void nodeShareInitInstanceCreateIntOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net);

/// \brief Creates a float output attribute
void nodeShareInitInstanceCreateVector3OutputAttribute(
  NodeDef* node,
  Network* net);

/// \brief Creates a float output attribute. It also initializes any pin attrib data on the node
void nodeShareInitInstanceCreateVector3OutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net);

/// \brief Creates a bool output attribute
void nodeShareInitInstanceCreateBoolOutputAttribute(
  NodeDef* node,
  Network* net);

/// \brief Creates a bool output attribute. It also initializes any pin attrib data on the node
void nodeShareInitInstanceCreateBoolOutputAttributeInitPinAttrib(
  NodeDef* node, 
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
// An empty OutputCP function.
//  Every node that can emit requests must have an extra output CP function 
//  that actually deals with the update of emitted requests. 
//  The registered function can be empty if the requests are actually sent elsewhere.
//----------------------------------------------------------------------------------------------------------------------
AttribData* nodeNullEmitMessageOutputCPUpdate(
  NodeDef* node,
  PinIndex outputCPPinIndex, /// The output pin we have been asked to update.
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Initialize all the nodes pin attrib data from active animation set
void nodeInitPinAttribDataInstance(
  NodeDef* node,
  Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Update all the nodes pin attrib data from output animation set
void nodeUpdatePinAttribDataInstance(
  NodeDef* node,
  Network* net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_SHARED_NODE_FUNCTIONS_H
//----------------------------------------------------------------------------------------------------------------------
