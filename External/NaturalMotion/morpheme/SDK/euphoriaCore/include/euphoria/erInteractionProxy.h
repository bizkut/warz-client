// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef ER_CHARACTER_QUERY_PROXY_H
#define ER_CHARACTER_QUERY_PROXY_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "physics/PhysX3/mrPhysicsScenePhysX3.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{
class InteractionProxyDef;

struct InteractionProxySetup
{
  /// This creates default setup parameters based on a physics rig. If there is a physics rig
  /// available when creating the interaction proxy it's the simplest way to create it. Otherwise
  /// the members can be initialised individually. 
  InteractionProxySetup(const MR::PhysicsRigPhysX3* ownerPhysicsRig);

  InteractionProxySetup(
    float                   mass,
    const NMP::Matrix34&    initialTM,
    physx::PxScene*         physicsScene,
    physx::PxClientID       ownerClientID = physx::PX_DEFAULT_CLIENT,
    uint32_t clientBehaviourBits = physx::PxActorClientBehaviorBit::eREPORT_TO_FOREIGN_CLIENTS_CONTACT_NOTIFY | 
    physx::PxActorClientBehaviorBit::eREPORT_TO_FOREIGN_CLIENTS_SCENE_QUERY);

  /// This is the ID of the physics rig assigned to the character. It's used to prevent the
  /// interaction proxy from being detected by the character itself.
  int32_t                    m_ownerPhysicsRigID;
  /// Representative mass of the character, so other characters know how heavy it is if they need to
  /// brace etc.
  float                      m_mass;
  /// Initial position/orientation
  NMP::Matrix34              m_initialTM;
  /// The scene to which this proxy should be added.
  physx::PxScene*            m_physicsScene;
  /// See PhysX documentation for information on the ownerClientID.
  physx::PxClientID          m_ownerClientID;
  /// See PhysX documentation for information on the clientBehaviourBits.
  uint32_t                   m_clientBehaviourBits;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ER::InteractionProxy
/// \brief This helper class enables the use of a single capsule to represent each character
/// The capsule is used only in collision queries, not for physical collision during simulation. This query proxy is
/// usually a necessity when characters will be interacting with each other, or when several characters will be in 
/// close proximity and running behaviours simultaneously. 
/// The effect of this proxy is to simplify the awareness in autonomous behaviours like the protect, so other characters
/// are observed as a single capsule rather than as twenty or so collision shapes, one for each body part. 
/// The reaction to other characters is therefore more predictable and also there are fewer dynamic objects within each
/// character's field of view. Twenty objects will fill up the list of objects that the character can be aware of
//  so using a single capsule will improve the awareness of the dynamic environment around the character considerably.
//----------------------------------------------------------------------------------------------------------------------
class InteractionProxy
{
public:
  InteractionProxy() : m_interactionProxyDef(0), m_actor(0) {}

  /// Initialisation generates the interaction proxy capsule from the setup structure passed in.
  void init(
    const InteractionProxyDef*   interactionProxyDef,
    const InteractionProxySetup& interactionProxySetup);
  
  /// Removes and deallocates the proxy.
  void term();

  /// Update function for the interaction proxy. The capsule is moved according to the motion of the
  /// animation rig - at a frame given by frameCount, so this update needs to be called when that is
  /// available. The best time to do this is after the network has been updated, in which case
  /// network.getCurrentFrameNo() can be used. If calculateVelocities is true then velocities will
  /// be calculated based on the preview position, otherwise they will be set to zero.  If any animation channels
  /// from the interaction proxy's parent to the root are not set in the network output, the bind pose is used instead.
  void update(float deltaTime, const MR::Network& network, MR::FrameCount frameCount, bool calculateVelocities);

  /// If the physics rig associated with this character is changed, the interaction proxy needs to
  /// be informed.
  void setRigID(int32_t ownerPhysicsRigID);

protected:
  /// Update function for the proxy transforms etc. The shape is updated each frame directly by
  /// specifying the new world space transform, velocity and angular velocity for the capsule.
  void updateTM(const NMP::Matrix34& transform, const NMP::Vector3& velocity, const NMP::Vector3& angularVelocity);

  /// This is the previous world space transform of the proxy shape - we track it so we can update
  /// the velocity
  NMP::Matrix34 m_prevTransform;

  const InteractionProxyDef* m_interactionProxyDef;
  static physx::PxU32 s_clientBehaviourBits;
  physx::PxRigidDynamic* m_actor;
};

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
#endif // ER_CHARACTER_QUERY_PROXY_H
//----------------------------------------------------------------------------------------------------------------------
