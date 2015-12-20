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
#include "morpheme/Nodes/mrNodeSequence.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Update connections function.
NodeID nodeSequenceUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  //------------------------
  // check if it is time to see if there is a new source node.
  // if evalAtEndOfAnim is false then do this every update.
  bool checkForNewSourceNode = false;
  NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID());

  //------------------------
  // Have we just become active?
  if (nodeConnections->m_justBecameActive)
  {
    //------------------------
    // If we have just become active then make sure no children are active
    nodeConnections->m_numActiveChildNodes = 0;
    nodeConnections->m_activeChildNodeIDs[0] = INVALID_NODE_ID;
  }

  //------------------------
  // The node is initialized with no active children.
  // If there's an active child, the node is already active.
  if (nodeConnections->m_numActiveChildNodes > 0)
  {
    //------------------------
    // see what fraction of the child node's animation we are at
    NodeID currentActiveChild = nodeConnections->m_activeChildNodeIDs[0];
    NMP_ASSERT(currentActiveChild != INVALID_NODE_ID);

    //------------------------
    // note, there is only a valid time info for the previous frame
    const uint32_t previousFrame = currFrameNo - 1;
    NodeBinEntry* fractPosData =  net->getAttribDataNodeBinEntryRecurseFilterNodes(
                                    ATTRIB_SEMANTIC_FRACTION_POS,
                                    currentActiveChild,
                                    INVALID_NODE_ID,
                                    previousFrame);

    if (fractPosData)
    {
      AttribDataPlaybackPos* playbackPos = fractPosData->getAttribData<AttribDataPlaybackPos>();
      NMP_ASSERT(playbackPos);

      //------------------------
      // if the animation is expected to finish (or loop) next frame then it is time to check for a new source node
      const float timeFraction = playbackPos->m_currentPosAdj + playbackPos->m_delta + ERROR_LIMIT;

      if (timeFraction >= 1.0f)
      {
        //------------------------
        // Get the NodeDef for the currently active child
        NodeDef* activeChildNodeDef = node->getChildNodeDef(0);
        for (uint16_t i = 0; i <= (node->getNumChildNodes() - 1); ++i)
        {
          if (node->getChildNodeID(i) == currentActiveChild)
          {
            activeChildNodeDef = node->getChildNodeDef(i);
            break;
          }
        }

        //------------------------
        // Check if the active child is a sequence node. If it is then we need to make sure we let it get to
        // the end of its last connected source before moving to our next source.
        if (activeChildNodeDef->getNodeTypeID() == NODE_TYPE_SEQUENCE)
        {
          //------------------------
          // Check for update only if our child sequence node is at the end of its last source.
          NodeConnections* newNodeConnections = net->getActiveNodesConnections(activeChildNodeDef->getNodeID());
          NodeID newCurrentActiveChild = newNodeConnections->m_activeChildNodeIDs[0];
          if (activeChildNodeDef->getChildNodeID((activeChildNodeDef->getNumChildNodes() - 1)) == newCurrentActiveChild)
          {
            checkForNewSourceNode = true;
          }
        }
        else
        {
          //------------------------
          // Child is not a Sequence node
          checkForNewSourceNode = true;
        }
      }
    }
    else
    {
      //------------------------
      // if the network has nested Sequence nodes then this will happen as update connections recurses through the tree.
      // Check that the parent node is a Sequence node
      if (node->getParentNodeDef() &&
          node->getParentNodeDef()->getNodeTypeID() != NODE_TYPE_SEQUENCE)
      {
        NET_LOG_ERROR_MESSAGE("ATTRIB_SEMANTIC_FRACTION_POS is not being output by Sequence node's child. Cannot evaluate atEndOfAnim\n");
      }
    }
  }
  else
  {
    //------------------------
    // There's no active child.
    checkForNewSourceNode = true;
  }

  const MR::NodeID prevNode = nodeConnections->m_activeChildNodeIDs[0];
  if (checkForNewSourceNode)
  {
    //------------------------
    // Get the loop bool attrib
    NetworkDef* netDef = net->getNetworkDef();
    NMP_ASSERT(netDef);
    AttribDataHandle* handle = netDef->getAttribDataHandle(ATTRIB_SEMANTIC_CP_BOOL, node->getNodeID());
    NMP_ASSERT(handle);
    AttribDataBool* loop = (AttribDataBool*) handle->m_attribData;
    NMP_ASSERT(loop);

    NodeID myNewID = node->getChildNodeID(0);
    for (uint16_t i = 0; i < node->getNumChildNodes(); ++i)
    {
      //------------------------
      // Check we're looking at the active child id
      if (node->getChildNodeID(i) == nodeConnections->m_activeChildNodeIDs[0])
      {
        //------------------------
        // Are we looping the sequence?
        if (loop->m_value)
        {
          //------------------------
          // Check if our next source is within range. Increment if true break if not.
          if ((i + 1) < node->getNumChildNodes())
          {
            myNewID = node->getChildNodeID(i + 1);
          }
        }
        else
        {
          //------------------------
          // Return if the next source is out of range. This stops the node from looping the sequence
          if ((i + 1) >= node->getNumChildNodes())
          {
            //------------------------
            // Recurse to child.
            NMP_ASSERT(nodeConnections->m_activeChildNodeIDs[0] != INVALID_NODE_ID);
            net->updateNodeInstanceConnections(nodeConnections->m_activeChildNodeIDs[0], animSet);

            // We now have a correctly active child.
            nodeConnections->m_numActiveChildNodes = 1;
            return node->getNodeID();
          }
          else
          {
            myNewID = node->getChildNodeID(i + 1);
          }
        }
        break;
      }
    }

    nodeConnections->m_activeChildNodeIDs[0] = myNewID;

    //------------------------
    // Some nodes other than AnimSyncEvents add ATTRIB_SEMANTIC_FRACTION_POS with a zero lifespan.
    // This ensures the ATTRIB_SEMANTIC_FRACTION_POS data is available from last frame
    if (nodeConnections->m_numActiveChildNodes == 0 || prevNode != nodeConnections->m_activeChildNodeIDs[0])
    {
      //------------------------
      // add the fractional position attrib to the list of attributes that need to exist post queue execution.
      net->addPostUpdateAccessAttrib(nodeConnections->m_activeChildNodeIDs[0], ATTRIB_SEMANTIC_FRACTION_POS, 2);

      if (nodeConnections->m_numActiveChildNodes > 0)
      {
        //------------------------
        // remove the previous entry that was added to the post update list
        net->removePostUpdateAccessAttrib(prevNode, MR::ATTRIB_SEMANTIC_FRACTION_POS);
      }
    }
    nodeConnections->m_numActiveChildNodes = 1;
  }

  //------------------------
  // Recurse to child.
  NMP_ASSERT(nodeConnections->m_activeChildNodeIDs[0] != INVALID_NODE_ID);
  net->updateNodeInstanceConnections(nodeConnections->m_activeChildNodeIDs[0], animSet);

  // delete inactive child instance
  if (prevNode != INVALID_NODE_ID && prevNode != nodeConnections->m_activeChildNodeIDs[0])
  {
    net->deleteNodeInstance(prevNode);
  }

  return node->getNodeID();
}

//----------------------------------------------------------------------------------------------------------------------
void nodeSequenceAndSwitchDeleteInstance(const NodeDef* node, Network* net)
{
  nodeShareDeleteInstanceWithChildren(node, net);

  NodeConnections* connections = net->getActiveNodesConnections(node->getNodeID());

  for (uint32_t iChild = 0; iChild < connections->m_numActiveChildNodes; ++iChild)
  {
    net->removePostUpdateAccessAttrib(connections->m_activeChildNodeIDs[iChild], MR::ATTRIB_SEMANTIC_FRACTION_POS);
    connections->m_activeChildNodeIDs[iChild] = INVALID_NODE_ID;
  }

  connections->m_numActiveChildNodes = 0;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
