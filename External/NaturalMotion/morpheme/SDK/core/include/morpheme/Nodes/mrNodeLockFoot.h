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
#ifndef MR_NODE_LOCK_FOOT_H
#define MR_NODE_LOCK_FOOT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
///
/// FOOTLOCKING NODE EXAMPLE
///
/// During a footfall duration event, this node modifies the source animation at the specified
/// joints in an attempt to lock the foot position during the footfall.  To do so it uses an analytic
/// Inverse Kinematics algorithm which assumes that the middle joint, or knee, is a hinge.  At the end
/// of the footfall duration, a controller brings the foot target back smoothly onto the source animation.
///
/// You can lock only the horizontal component of motion and allow the height to adapt to the source
/// anim, to avoid 'floating' artifacts caused by inaccuracies in the footfall event timing.  To do this
/// set the 'Up axis' parameter by enabling X, Y, or Z.
///
/// The 'IK-FK blend weight' control parameter provides an additional blend between the IK pose and the
/// source animation.
///
/// The 'Knee rotation axis' is the rotation axis of the knee in its parent's reference frame.
/// 'Flip knee rotation direction' is a quick way of negating this if the direction is wrong,
/// helpful when mirroring behaviour for different sides of the body (although not always relevant).
///
/// 'Hip index', 'Knee index', 'Ankle index' and 'Ball index' specify the channel indices of the
/// leg joints to be modified.  If 'Use ball joint' is set, it is the ball of the foot that will
/// be locked in position rather than the ankle joint.  If 'Assume simple hierarchy' is set, then
/// you only need to specify one joint index (the ankle index or the ball index), and the others
/// will be calculated from the rig assuming there is a direct parent relationship.
///
/// 'Footfall event ID' is the User Data of the footfall duration event that this node should respond to.
///
/// 'Catch-up speed factor' is a value between 0 and 1 which specifies how quickly the trailing leg
/// should lock back on to the incoming animation stream after the footfall event.  A value of 1 means
/// that it will do so instantaneously (i.e. the next frame, regardless of the time step).  0 means it
/// will track velocity only, and thus never 'catch up'.  Useful values are between 0.1 and 0.8.
/// 'Snap to source distance' is a distance in character space.  After the footstep, when the target
/// of the node's IK gets within this distance of the input pose, it snaps onto it.
///
/// 'Straightest leg factor' allows you to prevent the leg from being allowed to get too straight.  It
/// is the maximum allowed distance between the hip and ankle output from the algorithm, expressed
/// as a fraction of the maximum achievable distance (i.e. with a straight leg).  Hence the maximum
/// value should be 1, higher values make no sense and will produce artifacts.
///
/// 'Swivel contribution to orientation' is an optional input to the node.  If set to 1, the free
/// degree of freedom, rotation around the swivel axis between root and target, is used to help
/// achieve the correct (source) orientation of the ankle joint.  If less than 1 the contribution
/// from swivel is reduced (recommended, since allowing unrestricted use of swivel can lead to
/// some odd-looking results).  If unconnected or zero, no swivel is used.  This will result in the
/// least possible motion of hip and knee joints to achieve locking.
///
/// Finally, we have some related utilities for preventing ground penetration of the feet.  If
/// 'fix ground penetration' is selected then the 'lower height bound' is used to limit the height
/// of the end effector (ankle or ball joint).  If the end effector is the ball joint, we can
/// also fix the height of the toe using 'fix toe ground penetration', 'toe index', 'toe lower
/// height bound', and 'ball rotation axis'.  If the toe joint is still below the ground after
/// correcting the ball height, the ball joint is bent around the desired axis to lift the toe
/// up to its lower bound.
///
/// ALGORITHM
///
/// The details of the algorithm are documented in the code.  In summary, there is a closed-form
/// solution for the knee joint orientation required to get the correct distance between hip and
/// ankle.  The hip joint is then rotated through the shortest arc to bring the ankle onto the
/// correct position.  If we are targeting the ball joint, we simply calculate a new target for
/// the ankle, offset from the ball joint.  If desired, the hip joint is then given additional
/// rotation around the free swivel axis between hip and ankle to help achieve the correct
/// orientation of the foot.  We can then set the orientation of the ankle in closed form by
/// requiring its orientation in character space to be unchanged.  Finally, if requested and
/// necessary, we can rotate the ball joint around its hinge axis to lift the toe up to a
/// minimum height.
///
//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeLockFootQueueLockFootTransformBuffs(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* nodeLockFootQueueLockFootTrajectoryDeltaAndTransformsBuffs(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeLockFootUpdateConnections(
  NodeDef*           node,
  Network*           net);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_LOCK_FOOT_H
//----------------------------------------------------------------------------------------------------------------------
