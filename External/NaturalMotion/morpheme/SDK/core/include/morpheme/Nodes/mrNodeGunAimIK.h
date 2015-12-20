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
#ifndef MR_NODE_GUN_AIM_IK_H
#define MR_NODE_GUN_AIM_IK_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
///
/// GUN AIM EXAMPLE
///
/// Will output modified quat channels for the joints of the specified IK chain such that the chosen
/// end effector (a point offset from the specified end joint by a specified amount) points towards
/// the given target along the specified pointing direction.  Since this is an underconstrained
/// problem, the result minimises the amount of overall motion.
///
/// The 'end joint index' and 'root joint index' identify the IK chain to be modified.  The root joint
/// index must be an ancestor of the end joint, or the node will fail to operate.
///
/// The 'pointing vector' is the direction, in the space of the end joint, which must end up passing
/// through the target, and the 'offset' vector allows you to specify an end effector that does not
/// coincide with the end joint.  This will let you, for instance, rotate a neck joint so that the
/// eyes point towards a target.  Such a distinction becomes noticeable the closer the target is.
///
/// The 'bias' parameter defines how motion is spread out along the chain (since, typically, every joint
/// could achieve the goal by itself).  If the bias is zero, the motion is spread evenly.  If 1 then
/// the weight increases linearly from zero at the root; if -1 it decreases linearly down to zero at the
/// end joint.  Note that at -1 and 1, one joint in your chain will not move at all from its source
/// orientation.  For natural-looking motion of the spine and neck you will want a bias value greater
/// than 1.
///
/// ALGORITHM
///
/// There are more details of the algorithm in the code.  Essentially the algorithm treats each joint
/// independently, calculating how to rotate it to move the end effector onto the circular locus which
/// defines all the positions that joint can put it in and still have it point at the target along the
/// chosen direction.  It swings the end effector onto that locus, then twists so the end effector
/// is pointing at the target.  Then it is free to rotate around the centreline of the locus in order
/// to minimise the magnitude of overall rotation.
///
/// The bias parameter is then used to define a weight for each joint, which essentially results in a
/// weighted blend between the source orientation of this joint, and the algorithm's result.  The
/// overall effect is to spread the motion out along the chain as desired.
///
//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations

/// \brief Queue task to copy previous frame's trajectory transform from the character controller (PPU only).
Task* nodeGunAimIKQueueGunAimIKSetup(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter);

/// \brief Queue task to update character transforms.
Task* nodeGunAimIKQueueGunAimIKTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

/// \brief Queue task to update character transforms.
Task* nodeGunAimIKQueueGunAimIKTrajectoryDeltaAndTransformBuffs(
  NodeDef*       node,
  TaskQueue*     queue,
  Network*       net,
  TaskParameter* dependentParameter);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_HEAD_LOOK_H
//----------------------------------------------------------------------------------------------------------------------
