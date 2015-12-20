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
#ifndef MR_NODE_TWO_BONE_IK_H
#define MR_NODE_TWO_BONE_IK_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
///
/// TWO-BONE IK EXAMPLE
///
/// In its most basic form, will output modified quat channels for the parent and grandparent joint
/// of a specified end-effector node, to ensure that that node reaches a given target position,
/// specified in character space.  If the target is out of reach the IK chain will 'point' towards it.
/// The middle joint of the chain is assumed to be a hinge and the axis of rotation of that hinge
/// are three parameters of the node.
///
/// The 'end joint', 'mid joint' and 'root joint' parameters allow you to set the indices of the
/// joints to be IKed, or you can set the 'end joint' alone, and check 'assume simple hierarchy' if
/// the mid and root joints are simply the parent and grandparent of the end joint.
///
/// If the 'target orientation' input node is connected it will be used to drive the desired
/// orientation of the end effector.  However, if 'keep end eff orientation' is switched on, it will
/// adjust the orientation of the end-effector to give it a character-space orientation unchanged
/// from the input.  If 'swivel contribution to orientation' is greater than zero then some of the
/// orientation will be achieved by swiveling the root joint around an axis from root to end joint.
/// A value of 1 means that the root joint will rotate as much as possible, but this doesn't mean the
/// desired orientation can be achieved from swivel alone.
///
/// The 'flipMidJointRotationDirection' parameter allows you to flip the hinge axis.  In some rigs,
/// this will mean that the same other parameters can be used for mirrored limbs, but it is not
/// always appropriate.
///
/// The 'swivel angle' parameter is another variable alongside the target position which manipulates
/// the one free degree of freedom in a two-bone-system: that is, once the end-effector is at the
/// target, the system can rotate freely about the axis running between the root of the IK chain and
/// the target.  You can define a fixed 'zero' for the swivel angle by setting a reference direction
/// for the hinge axis at the mid joint, using the 'Mid Joint Reference Axis' parameters.  This can
/// be in the root frame, or, if you check the 'Global Reference Axis' flag, it will be in character
/// space, which means you can always specify how much you want the elbow to point up, for instance.
/// However, most of the time you just want to add or subtract swivel from the pose the joints are
/// in after the shortest motion from the source anim.  To do this, just leave this as a zero vector.
///
/// An 'ik-fk weight' is also used to blend between the input and IK solutions in case smooth
/// motion between the two is desired.
///
/// The 'update target by deltas' parameter is a little complex.  If you are targetting something in
/// the world space, then you need to put it in character space before you pass it into this node.
/// However, depending on the way you have set up your runtime target, it is possible you do not
/// have access to the new character root transform at the point where you are setting your effector
/// target - you may only have access to the root transform for the last update.  If you turn on
/// the 'update target by deltas' flag, this node will automatically assume that this is the case,
/// and will adjust the target vector and orientation for you to account for motion since the last
/// update.
///
/// ALGORITHM
///
/// The details of the algorithm are documented in the code.  In summary, 2-bone IK uses trigonometry
/// to calculate the angle at the middle hinge joint.  Assuming that this joint is bent to the correct
/// angle, it then rotates the root joint by the shortest arc that takes the end-effector to the
/// target.  Finally, the desired swivel about the axis between root and target is applied to the
/// root (compounded on top of the previous swing rotation).
///
/// Reorienting the end-effector joint to preserve its character-local orientation is then just a
/// matter of undoing any rotation imparted by the rotation at the other joints.
///
//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations

/// \brief Queue task to copy previous frame's trajectory transform from the character controller (PPU only).
Task* nodeTwoBoneIKQueueTwoBoneIKSetup(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter);

/// \brief Queue task to update character transforms.
Task* nodeTwoBoneIKQueueTwoBoneIKTransformBuffs(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter);

/// \brief Queue task to update character transforms.
Task* nodeTwoBoneIKQueueTwoBoneIKTrajectoryDeltaAndTransformBuffs(
  NodeDef* node,
  TaskQueue* queue,
  Network* net,
  TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeTwoBoneIKUpdateConnections(
  NodeDef*           node,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_TWO_BONE_IK_H
//----------------------------------------------------------------------------------------------------------------------
