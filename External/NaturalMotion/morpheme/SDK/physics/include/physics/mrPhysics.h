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
#ifndef MR_PHYSICS_H
#define MR_PHYSICS_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsAttribData.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

/// \brief This callback is used to inform the physics rig that the animation set has changed.  It's registered with
///  the MR::Network when the physics plugin is initialised.
void physicsOnAnimSetChanged(Network* net, AnimSetIndex animationSetIndex);

/// \brief Get the physical rig for the network - this function can return NULL if there is no PhysicsRig.
PhysicsRig* getPhysicsRig(Network* net);
PhysicsRig* getPhysicsRig(Network* net, AnimSetIndex animSetIndex);
PhysicsRig* getPhysicsRig(const Network* net);

/// \brief Sets the physical rig for the network - a physics rig should be set by the runtime if it
///        wants to use physics nodes with the network. It is legal to pass in NULL, to indicate
///        that no PhyscsRig is bound to the network.
void setPhysicsRig(Network* net, PhysicsRig* physicsRig);
void addPhysicsRig(Network* net, PhysicsRig* physicsRig, AnimSetIndex animationSetIndex);

/// \brief Get the PhysicsRigDef for the currently active animation set.
/// \return The PhysicsRigDef.
PhysicsRigDef* getActivePhysicsRigDef(Network* net);

/// \brief Get the PhysicsRigDef used by the specified animation set.
PhysicsRigDef* getPhysicsRigDef(NetworkDef* netDef, AnimSetIndex animSetIndex);

/// \brief Return the mapping between physics and animation rigs for the specified animation set.
AnimToPhysicsMap* getAnimToPhysicsMap(NetworkDef* netDef, AnimSetIndex animSetIndex);

/// \brief Return a pointer to the collision filter mask for this node or null if not found.
CollisionFilterMask* getCollisionFilterMask(NodeDef* nodeDef, Network* net);

/// \brief Sets the value of the collision filter mask for this node and returns true if successful.
bool setCollisionFilterMask(NodeDef* nodeDef, Network* net, const CollisionFilterMask& cfm);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSICS_H
//----------------------------------------------------------------------------------------------------------------------
