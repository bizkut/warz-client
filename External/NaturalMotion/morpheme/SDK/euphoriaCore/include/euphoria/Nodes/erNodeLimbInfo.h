//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef ER_NODE_LIMB_INFO_H
#define ER_NODE_LIMB_INFO_H

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"

//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations

// Queue update function for Transform Buffer semantic
MR::Task* nodeLimbInfoQueueTransforms(
  MR::NodeDef*        node,
  MR::TaskQueue*      queue,
  MR::Network*        net,
  MR::TaskParameter*  dependentParameter);

// Queue update function for Trajectory Delta and Transform Buffer semantic
MR::Task* nodeLimbInfoQueueTrajectoryDeltaAndTransforms(
  MR::NodeDef*        node,
  MR::TaskQueue*      queue,
  MR::Network*        net,
  MR::TaskParameter*  dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the output control parameter pins.
enum NodeLimbInfoOutCPPinIDs
{
  NODE_LIMB_INFO_OUT_PINID_POSITION = 0,
  NODE_LIMB_INFO_OUT_PINID_AXIS,
  NODE_LIMB_INFO_OUT_PINID_ANGLE,

  NODE_LIMB_INFO_OUT_PINID_COUNT
};

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the limb parts (root, base, spine.
enum NodeLimbInfoLimbPartIndex
{
  NODE_LIMB_INFO_LIMB_PART_ROOT = 0,
  NODE_LIMB_INFO_LIMB_PART_END,

  NODE_LIMB_INFO_LIMB_PART_MAX
};


//----------------------------------------------------------------------------------------------------------------------
} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_NODE_LIMB_INFO_H
//----------------------------------------------------------------------------------------------------------------------
