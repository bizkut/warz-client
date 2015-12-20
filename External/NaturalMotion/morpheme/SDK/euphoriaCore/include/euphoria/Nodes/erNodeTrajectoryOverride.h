//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_TRAJECTORY_OVERRIDE_H
#define MR_NODE_TRAJECTORY_OVERRIDE_H

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"

//----------------------------------------------------------------------------------------------------------------------
namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
MR::Task* nodeTrajectoryOverrideQueueUpdate(
  MR::NodeDef*       node,
  MR::TaskQueue*     queue,
  MR::Network*       net,
  MR::TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Update of any node data that must happen before queue building or execution.
MR::NodeID nodeTrajectoryOverrideUpdateConnections(
  MR::NodeDef* node,
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
MR::NodeID nodeTrajectoryOverrideFindGeneratingNodeForSemantic(
  MR::NodeID             callingNodeID,
  bool                   fromParent,   // Was this query from a parent or child node.
  MR::AttribDataSemantic semantic,
  MR::NodeDef*           node,
  MR::Network*           network);

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_TRAJECTORY_OVERRIDE_H
//----------------------------------------------------------------------------------------------------------------------
