// Copyright (c) 2010 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
template <class _AttribDataTransitDef>
NM_INLINE void updatePhysicalTransition(
  NodeDef*               node,
  Network*               net,
  _AttribDataTransitDef* transitDef,
  NodeConnections*       nodeConnections,
  bool                   performDeadBlend,
  bool                   blendToPhysics)
{
  NodeID thisNodeID = node->getNodeID();

  // Currently we support only the default type of dead blend.
  NMP_ASSERT(node->getAttribData<AttribDataDeadBlendDef>(ATTRIB_SEMANTIC_DEAD_BLEND_DEF));

  // Get the dead blend state data.
  AttribDataDeadBlendState* deadBlendState = getDeadBlendState(node, net);

  // We need to deal with all the possible combinations here, and amend existing data when needed.
  bool startDeadBlend = false;
  if (deadBlendState)
  {
    if (deadBlendState->m_performDeadBlend)
    {
      // A dead blend from physics is already happening and this can't be changed (the source physics node has been deleted).
      // We just need to be sure to take care of possible changes in the destination.
      deadBlendState->m_isBlendingToPhysics = blendToPhysics;
    }
    else
    {
      if (performDeadBlend)
      {
        // The source has become physical (or become inactive) during the transition.
        // We need to amend the physics blend setup, but we also need to initialize
        // the data for the dead blend and to destroy the source node.
        startDeadBlend = true;
        deadBlendState->m_performDeadBlend = performDeadBlend;
      }
    }
  }
  else // if (deadBlendStateAttr)
  {
    // Prepare the dead blend state data.
    deadBlendState = AttribDataDeadBlendState::create(net->getPersistentMemoryAllocator());

    // Get some useful data from the transit node def.
    deadBlendState->m_blendMode = transitDef->m_blendMode;
    deadBlendState->m_slerpTrajectoryPosition = transitDef->m_slerpTrajectoryPosition;

    // Setup special case data.
    deadBlendState->m_isBlendingToPhysics = blendToPhysics;
    deadBlendState->m_performDeadBlend = performDeadBlend;

    AttribDataHandle handle = { deadBlendState, AttribDataDeadBlendState::getMemoryRequirements() };
    net->addAttribData(
      AttribAddress(ATTRIB_SEMANTIC_DEAD_BLEND_STATE, thisNodeID, INVALID_NODE_ID, VALID_FOREVER, net->getOutputAnimSetIndex(thisNodeID)),
      handle,
      LIFESPAN_FOREVER);

    startDeadBlend = performDeadBlend;
  }

  if (startDeadBlend)
  {
    NMP_ASSERT(deadBlendState);
    deadBlendState->m_duration = getDeadBlendDuration(net, thisNodeID, transitDef);
    if (net->isUsingCombinedTrajectoryAndTransforms())
    {
      initialiseTrajectoryDeltaAndTransformsDeadBlend(node, net);
    }
    else
    {
      initialiseDeadBlend(node, net);
    }
    NodeID sourceNodeID = nodeConnections->m_activeChildNodeIDs[0];
    NodeID destNodeID   = nodeConnections->m_activeChildNodeIDs[1];

    // From now on, the dead blend is in progress, so the source child needs to be removed.
    if ((net->getActiveNodesConnections(sourceNodeID)->m_activeParentNodeID == thisNodeID) &&
        !(destNodeID == sourceNodeID)) // Self-transition
    {
      // Only delete if it has not already been taken by a breakout to active state transition
      // Or is a self-transition (dest == source)
      net->deleteNodeInstance(sourceNodeID);
    }
    nodeConnections->m_numActiveChildNodes = 1;
    nodeConnections->m_activeChildNodeIDs[0] = destNodeID;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Transition has not completed yet so make sure the correct connections are updated.
template <class _AttribDataTransitDef>
NM_INLINE void updateTransition(
  NodeDef*               node,
  Network*               net,
  _AttribDataTransitDef* transitDef,
  NodeConnections*       nodeConnections)
{
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  if (transitDef->m_reversible)
  {
    // We need to update connections on boolean control parameter if transition is reversible.
    NMP_ASSERT(transitDef->m_reverseInputCPConnection.m_sourceNodeID != INVALID_NODE_ID);
    NMP_ASSERT(transitDef->m_reverseInputCPConnection.m_sourceNodeID != INVALID_NODE_ID);
    net->updateInputCPConnection<AttribDataBool>(&(transitDef->m_reverseInputCPConnection), animSet); // inputCPBoolReverse
  }

  // Update active child node connections and determine if any children are physical.
  bool performDeadBlend = false;
  bool blendToPhysics = false;

  if (nodeConnections->m_numActiveChildNodes == 2)
  {
    // When two children are active, the following can happen:
    // - This is the first time we update the connections
    // - Animation - Animation transition
    // - Animation - Physics transition

    // Always update the destination connection
    NodeID destNodeID = net->updateNodeInstanceConnections(nodeConnections->m_activeChildNodeIDs[1], animSet);
    nodeConnections->m_activeChildNodeIDs[1] = destNodeID;
    NodeID sourceNodeID = nodeConnections->m_activeChildNodeIDs[0];
    net->setActiveParentNodeID(destNodeID, node->getNodeID());
    blendToPhysics = net->isNodeOrActiveChildPhysical(net->getNetworkDef()->getNodeDef(destNodeID));

    // check if breakout to active state has occurred
    NodeID prevSourceNodeID = nodeConnections->m_activeChildNodeIDs[0];
    if ((net->getActiveParentNodeID(prevSourceNodeID) != node->getNodeID()) ||
        (destNodeID == sourceNodeID)) // Self-transition
    {
      // Child node has been re-parented or is a self transition (dest == source).
      // Don't update, initiate a dead blend
      performDeadBlend = true;
    }
    else
    {
      // If doing a physics->physics transition then we must make sure the source
      // doesn't get a chance to update its connections - because if it does then
      // when we have HK->ragdoll, the HK node will set the root control method
      // even though it will get deleted.
      performDeadBlend = net->isNodeOrActiveChildPhysical(net->getNetworkDef()->getNodeDef(sourceNodeID));

      if (!(blendToPhysics && performDeadBlend))
      {
        // This is not a physics->physics transition, so update the source connection
        NodeID newSourceNodeID = net->updateNodeInstanceConnections(nodeConnections->m_activeChildNodeIDs[0], animSet);
        nodeConnections->m_activeChildNodeIDs[0] = newSourceNodeID;
        net->setActiveParentNodeID(newSourceNodeID, node->getNodeID());
      }
    }
  }
  else
  {
    // There's just one active child. This means that a dead blend has already started.
    NodeID destNodeID = net->updateNodeInstanceConnections(nodeConnections->m_activeChildNodeIDs[0], animSet);
    nodeConnections->m_activeChildNodeIDs[0] = destNodeID;
    net->setActiveParentNodeID(destNodeID, node->getNodeID());
    blendToPhysics   = net->isNodeOrActiveChildPhysical(net->getNetworkDef()->getNodeDef(destNodeID));
    performDeadBlend = true;
  }

  // Update as a dead blend if any children are physical.
  if (performDeadBlend || blendToPhysics)
    updatePhysicalTransition(node, net, transitDef, nodeConnections, performDeadBlend, blendToPhysics);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
