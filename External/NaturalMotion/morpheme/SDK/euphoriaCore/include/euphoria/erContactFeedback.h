// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_CONTACTFEEDBACK_H
#define NM_CONTACTFEEDBACK_H
#include "mrPhysX3Includes.h"
#include "physics/mrPhysicsScene.h"

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
/// Base class so that users can register their own handler for collisions that are reported to and
/// registered by Euphoria.
//----------------------------------------------------------------------------------------------------------------------
class UserContactHandler
{
public:

  virtual ~UserContactHandler() {}

  /// This will be called when there is an active contact.
  virtual void onContact(
    const physx::PxContactPairHeader& pairHeader,
    const physx::PxContactPair*       pairs,
    physx::PxU32                      nbPairs) = 0;
};

// This class allows euphoria to access contact forces resulting from character collisions
class ContactFeedback : public physx::PxSimulationEventCallback
{
public:
  /// Need to pass in the clientID used for euphoria
  /// See PhysX documentation for information on the ownerClientID and clientBehaviourBits
  static void initialise(MR::PhysicsScene* physicsScene, physx::PxClientID ownerClientID);
  static void deinitialise(MR::PhysicsScene* physicsScene);

  static void setUserContactHandler(UserContactHandler* handler);

  // Debug Draw
  static bool getDrawContactsFlag();
  static void setDrawContactsFlag(bool drawEnabled);
  static bool getDrawDetailedContactsFlag();
  static void setDrawDetailedContactsFlag(bool drawEnabled);

protected:
  UserContactHandler* m_userContactHandler;
  static physx::PxClientID m_ownerClientID;

protected:
  void onContact(
    const physx::PxContactPairHeader& pairHeader, 
    const physx::PxContactPair* pairs, 
    physx::PxU32 nbPairs) NM_OVERRIDE;

  void onConstraintBreak(physx::PxConstraintInfo* NMP_UNUSED(constraints), physx::PxU32 NMP_UNUSED(count)) NM_OVERRIDE {}
  void onWake(physx::PxActor** NMP_UNUSED(actors), physx::PxU32 NMP_UNUSED(count)) NM_OVERRIDE {}
  void onSleep(physx::PxActor** NMP_UNUSED(actors), physx::PxU32 NMP_UNUSED(count)) NM_OVERRIDE {}
  void onTrigger(physx::PxTriggerPair* NMP_UNUSED(pairs), physx::PxU32 NMP_UNUSED(count)) NM_OVERRIDE {}
};
}

#endif // NM_CONTACTFEEDBACK_H
