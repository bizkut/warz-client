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
#include "morpheme/Nodes/mrNodeSwitch.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------  
namespace
{

//----------------------------------------------------------------------------------------------------------------------
void findActiveSourceIndex(
   float                                      interpolant,            // IN:Across any range.
   AttribDataFloatArray*                      attribChildNodeWeights, // IN:
   bool                                       wrapWeights,            // IN:
   AttribDataSwitchDef::InputSelectionMethod  weightRampingMethod,    // IN:
   uint16_t&                                  newActiveIndex)         // OUT: Index into array of source Nodes.
{
  NMP_ASSERT(attribChildNodeWeights);
  NodeID numSourceNodes = (NodeID)attribChildNodeWeights->m_numValues;
  NMP_ASSERT(numSourceNodes > 1);
  
  // First we work out whether the weight values are increasing or decreasing
  float delta = attribChildNodeWeights->m_values[numSourceNodes - 1] - attribChildNodeWeights->m_values[0];
  
  // now get the sign of the delta +1 if increasing, -1 if decreasing
  float signum = (delta >= 0) ? 1.0f : -1.0f;

  // Now we can multiply all things to do with blend weights by signum in order to reverse the logic, so
  // (signum * interp > signum * weight) is
  // (interp > weight) if signum = +1
  // (interp < weight) if signum = -1

  if ((signum * interpolant) <= (signum * attribChildNodeWeights->m_values[0]))
  {
    // Less than min value.
    newActiveIndex = 0;
  }
  else if ((signum * interpolant) >= (signum* attribChildNodeWeights->m_values[numSourceNodes - 1]))
  {
    // Beyond max value.
    newActiveIndex = numSourceNodes - 1;
  }
  else
  {
   // Work out which source input to activate.
   switch (weightRampingMethod) 
   {
   case AttribDataSwitchDef::kClosest:
      for (NodeID i = 1; i < numSourceNodes; i++)
      {
        newActiveIndex = i;          
        // Round to closest source
        float halfPrevRange = (attribChildNodeWeights->m_values[i] - attribChildNodeWeights->m_values[i - 1]) / 2;
        if ((signum * interpolant) < (signum *(attribChildNodeWeights->m_values[i] - halfPrevRange)))
        {
          // We've gone too far so the new active index must the the previous one.
          newActiveIndex = newActiveIndex - 1;
          break;
        }
      }
      break;
   case AttribDataSwitchDef::kFloor:
      for (NodeID i = 1; i < numSourceNodes; i++)
      {
        newActiveIndex = i;
        // Floor to previous source
        if ((signum * interpolant) < (signum * attribChildNodeWeights->m_values[i]))
        {
          // We've gone too far so the new active index must the the previous one.
          newActiveIndex = newActiveIndex - 1;
          break;
        }
      } 
      break;
   case AttribDataSwitchDef::kCeiling:
      for (NodeID i = 1; i < numSourceNodes; i++)
      {
        newActiveIndex = i;
        // Ceiling to previous source
        if ((signum * interpolant) > (signum * attribChildNodeWeights->m_values[i - 1]) && (signum * interpolant) <= (signum * attribChildNodeWeights->m_values[i]))
        {
          // No need to modify the newActiveIndex in case of a ceiling operation
          break;
        }
      }
      break;
   default:NMP_ASSERT_FAIL(); // We should never get here
    }
  } // else

  // Handle case where we're past the last source and interpolate back to the first
  if (wrapWeights && (newActiveIndex == (numSourceNodes - 1)))
    newActiveIndex = 0;
}
  
//----------------------------------------------------------------------------------------------------------------------
} // anonymous namespace 

//----------------------------------------------------------------------------------------------------------------------
// Update connections function.
NodeID nodeSwitchUpdateConnections(
  NodeDef*           node,
  Network*           net)
{
  FrameCount currFrameNo = net->getCurrentFrameNo();
  NMP_ASSERT(currFrameNo > 0);
  NMP_ASSERT(node->getNumInputCPConnections() > 0)
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  //------------------------
  // Update the only connected control parameter.
  float blendWeightValue = 0.0f;
  AttribDataFloat* inputCPFloat = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(0), animSet);
  blendWeightValue = inputCPFloat->m_value;

  //------------------------
  // Get the defs table of child node weight distributions.
  NetworkDef* netDef = net->getNetworkDef();
  NMP_ASSERT(netDef);
  NodeDef* nodeDef = netDef->getNodeDef(node->getNodeID());
  NMP_ASSERT(nodeDef);
  AttribDataFloatArray* childNodeWeights = nodeDef->getAttribData<AttribDataFloatArray>(
    ATTRIB_SEMANTIC_CHILD_NODE_WEIGHTS);

  NodeConnections* nodeConnections = net->getActiveNodesConnections(node->getNodeID());
  NMP_ASSERT(nodeConnections);

  //------------------------
  // get the Switch node's data
  AttribDataHandle* handle1 = netDef->getAttribDataHandle(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, node->getNodeID());
  NMP_ASSERT(handle1);
  AttribDataSwitchDef* switchData = (AttribDataSwitchDef*) handle1->m_attribData;
  NMP_ASSERT(switchData && switchData->getType() == ATTRIB_TYPE_SWITCH_DEF);

  //------------------------
  // Check if the the playback position has reached the end of the currently active animation
  // if evalAtEndOfAnim is false then do this every update.
  bool checkForNewSourceNode = true;

  if (!nodeConnections->m_justBecameActive)
  {
    if (switchData->m_evalMode == AttribDataSwitchDef::kEvalWhenActivating)
    {
      // We've already chosen our input animation, nothing more to do
      checkForNewSourceNode = false;
    }
    else if (switchData->m_evalMode == AttribDataSwitchDef::kEvalEveryFrame)
    {
      // Chose a new input
      checkForNewSourceNode = true;
    }
    else if (switchData->m_evalMode == AttribDataSwitchDef::kEvalOnLoop)
    {
      // Default to not choosing a new input, then test to see if we've looped
      checkForNewSourceNode = false;

      // The node is initialized with no active children.
      // If there's an active child, the node switch is already active.
      if (nodeConnections->m_numActiveChildNodes > 0)
      {
        //------------------------
        // see what fraction of the child node's animation we are at
        NodeID currentActiveChild = nodeConnections->m_activeChildNodeIDs[0];
        NMP_ASSERT(currentActiveChild != INVALID_NODE_ID);

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

          // if the animation is expected to finished or looped last frame then it is time to re-check the switch value
          if (playbackPos->m_currentPosAdj >= 1.0f - FLT_EPSILON)
          {
            // In the last frame, we reached the end of a non-looping input.
            checkForNewSourceNode = true;
          }
          if (playbackPos->m_currentPosAdj < playbackPos->m_previousPosAdj)
          {
            // In the last frame, the input looped around to the beginning.
            checkForNewSourceNode = true;
          }
        }
        else
        {
          // if the network has nested Switch nodes then this will happen as update connections recurses through the tree.
          // Check that the parent node is a switch node
          if (node->getParentNodeDef() &&
            node->getParentNodeDef()->getNodeTypeID() != NODE_TYPE_SWITCH)
          {
            NET_LOG_ERROR_MESSAGE("ATTRIB_SEMANTIC_FRACTION_POS is not being output by Switch node's child. Cannot evaluate atEndOfAnim\n");
          }
        }
      }
    }
    else
    {
      // Should never get here
      NMP_ASSERT_FAIL_MSG("Detected an invalid value for EvaluateMode: %i", switchData->m_evalMode);
    }
  }


  if (checkForNewSourceNode)
  {
    const MR::NodeID prevChildNode = nodeConnections->m_activeChildNodeIDs[0];

    // Determine the 2 active child node indexes and calculate the blend weighting between them.
    uint16_t activeChildIndex = 0;
    bool wrapEnabled = nodeDef->getNumChildNodes() != childNodeWeights->m_numValues;
    findActiveSourceIndex(
      blendWeightValue,
      childNodeWeights,
      wrapEnabled,
      switchData->m_inputSelectionMethod,
      activeChildIndex);

    nodeConnections->m_activeChildNodeIDs[0] = nodeDef->getChildNodeID(activeChildIndex);

    // Some nodes other than AnimSyncEvents add ATTRIB_SEMANTIC_FRACTION_POS with a zero lifespan.
    // This ensures the ATTRIB_SEMANTIC_FRACTION_POS data is available from last frame
    if (nodeConnections->m_numActiveChildNodes == 0 || prevChildNode != nodeConnections->m_activeChildNodeIDs[0])
    {
      // add the fractional position attrib to the list of attributes that need to exist post queue execution.
      net->addPostUpdateAccessAttrib(nodeConnections->m_activeChildNodeIDs[0], MR::ATTRIB_SEMANTIC_FRACTION_POS, 2);

      if (nodeConnections->m_numActiveChildNodes > 0)
      {
        // remove the previous entry that was added to the post update list
        net->removePostUpdateAccessAttrib(prevChildNode, MR::ATTRIB_SEMANTIC_FRACTION_POS);

        // Delete previous child node
        net->deleteNodeInstance(prevChildNode);
      }
    }

    nodeConnections->m_numActiveChildNodes = 1;
  }

  // Recurse to child.
  NMP_ASSERT(nodeConnections->m_activeChildNodeIDs[0] != INVALID_NODE_ID);
  net->updateNodeInstanceConnections(nodeConnections->m_activeChildNodeIDs[0], animSet);

  return node->getNodeID();
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
