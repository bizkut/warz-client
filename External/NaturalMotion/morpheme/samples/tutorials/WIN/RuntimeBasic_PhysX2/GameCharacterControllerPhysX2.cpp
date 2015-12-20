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
#include "GameCharacterControllerPhysX2.h"

#include "physics/mrPhysics.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{


//----------------------------------------------------------------------------------------------------------------------
// Controller hit report constructor
ControllerHitReport::ControllerHitReport(float maxTouchTime, TouchTimes& touchTimes) :
  m_maxPushForce(0.0f),
  m_dt(1.0f),
  m_maxTouchTime(maxTouchTime),
  m_touchTimes(touchTimes)
{
}

//----------------------------------------------------------------------------------------------------------------------
// Update pre Move
void ControllerHitReport::updatePreMove(float dt, float maxPushForce)
{
  m_dt = dt;
  m_maxPushForce = maxPushForce;
  for (TouchTimes::iterator it = m_touchTimes.begin() ; it != m_touchTimes.end() ; ++it)
  {
    it->second -= dt;
    if (it->second < 0.0f)
      it->second = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// On Shape Hit
NxControllerAction ControllerHitReport::onShapeHit(const NxControllerShapeHit& hit)
{
  if (hit.shape)
  {
    NxActor& actor = hit.shape->getActor();
    if (actor.isDynamic() && !actor.readBodyFlag(NX_BF_KINEMATIC))
    {
      float& touchTime = m_touchTimes[&actor];
      touchTime += 2.0f * m_dt; // twice since it will have been decremented if it's persistent
      if (touchTime > m_maxTouchTime)
        touchTime = m_maxTouchTime;
      NxF32 coeff = m_maxPushForce * touchTime / m_maxTouchTime;
      actor.addForceAtPos(hit.dir * coeff, NxVec3((float) hit.worldPos.x, (float) hit.worldPos.y,
                          (float) hit.worldPos.z), NX_FORCE);
    }
  }
  return NX_ACTION_NONE;
}

//---------------------------------------------------------------------------------------------------------------------
// On Controller Hit - Add actions here
NxControllerAction ControllerHitReport::onControllerHit(const NxControllersHit& NMP_UNUSED(hit))
{
  return NX_ACTION_NONE;
}

//----------------------------------------------------------------------------------------------------------------------
// class Game::PhysX2CharacterController
//----------------------------------------------------------------------------------------------------------------------
CharacterControllerPhysX2Basic* CharacterControllerPhysX2Basic::create(
  MR::Network* net,
  MR::PhysicsScenePhysX2* physicsScene,
  NxControllerManager* controllerManager)
{
  CharacterControllerPhysX2Basic* const instance = static_cast<CharacterControllerPhysX2Basic*>(NMPMemoryAlloc(sizeof(CharacterControllerPhysX2Basic)));
  new(instance) CharacterControllerPhysX2Basic();

  instance->init(net, physicsScene, controllerManager);

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2Basic::destroy(CharacterControllerPhysX2Basic* characterController)
{
  characterController->term();

  NMP::Memory::memFree(characterController);
}

//----------------------------------------------------------------------------------------------------------------------
bool CharacterControllerPhysX2Basic::init(
  MR::Network*                net,
  MR::PhysicsScenePhysX2*     physicsScene,
  NxControllerManager*        controllerManager)
{
  NMP_ASSERT(net);
  NMP_ASSERT(physicsScene);
  NMP_ASSERT(controllerManager);

  m_net = net;
  m_nxControllerManager = controllerManager;

  m_nxControllerEnabled = true;

  MR::AnimRigDef* rig = m_net->getNetworkDef()->getRig(0);
  MR::PhysicsRigDef* physicsRigDef = (MR::PhysicsRigDef*) getPhysicsRigDef(m_net->getNetworkDef(), 0);

  //----------------------------
  // Set the current character controller values from the network def data (i.e. no CC scaling is applied at the moment)
  m_controllerDef = *(net->getActiveCharacterControllerDef());

  if (physicsRigDef != NULL)
  {
    //----------------------------
    // physics in network
    NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(
                                                        MR::PhysicsRigPhysX2::getMemoryRequirements(physicsRigDef));
    NMP_ASSERT(resource.ptr);
    m_physicsRig = MR::PhysicsRigPhysX2::init(
                     resource,
                     physicsRigDef,
                     physicsScene,
                     rig,
                     getAnimToPhysicsMap(m_net->getNetworkDef(), m_net->getActiveAnimSetIndex()),
                     1 << MR::GROUP_CHARACTER_PART,
                     (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE));

    //----------------------------
    // set the position of the physical body to the 'graveyard', 1000 units below the origin.
    m_physicsRig->setKinematicPos(NMP::Vector3(0.0f, -1000.0f, 0.0f));
    setPhysicsRig(m_net, m_physicsRig);
  }
  else
  {
    //----------------------------
    // no physics in network
    NMP_ASSERT_FAIL();
    m_physicsRig = NULL;
    return false;
  }

  //----------------------------
  // Check a NxController hasn't already been created for this Physics Character Controller
  if (!m_nxController)
  {
    //----------------------------
    // Now set up the physX character controller object from the contents of the controllerParams.
    NxCapsuleControllerDesc desc;
    desc.interactionFlag = NXIF_INTERACTION_INCLUDE;
    desc.radius = m_controllerDef.getRadius();
    desc.height = m_controllerDef.getHeight();
    desc.upDirection = NX_Y;
    desc.slopeLimit = cosf(NMP::degreesToRadians(m_controllerDef.getMaxSlopeAngle()));
    desc.skinWidth = m_controllerDef.getSkinWidth();
    desc.stepOffset = m_controllerDef.getStepHeight();
    m_hitReport->m_maxPushForce = m_controllerDef.getMaxPushForce();
    desc.callback = m_hitReport;
    desc.position.set(m_positionWorld.x,
                      m_positionWorld.y + m_controllerOffset,
                      m_positionWorld.z);
    m_nxController = controllerManager->createController(physicsScene->getPhysXScene(), desc);
  }
  else
  {
    //----------------------------
    // Controller already exists for this object
    NMP_ASSERT_FAIL();
    return false;
  }

  if (m_nxController)
  {
    m_nxController->setCollision(false);
    setActor(m_nxController->getActor());
    m_net->setCharacterController(this);

    m_net->setCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_orientationWorld, m_positionWorld));

    MR::PhysicsRigPhysX2* physicsRigPhysX = (MR::PhysicsRigPhysX2*)m_physicsRig;
    physicsRigPhysX->setCharacterControllerActor(m_nxController->getActor());

    //----------------------------
    // Turn off collision detection between Character controller capsule and all body parts
    size_t nBones = m_physicsRig->getNumParts();
    for (size_t i = 0 ; i < nBones ; ++i)
    {
      MR::PhysicsRigPhysX2::PartPhysX* part = (MR::PhysicsRigPhysX2::PartPhysX*)m_physicsRig->getPart((uint32_t)i);
      physicsScene->getPhysXScene()->setActorPairFlags(*part->getActor(),
          *m_nxController->getActor(), NX_IGNORE_PAIR);
    }
    return true;
  }
  else
  {
    //----------------------------
    // m_controller was not created
    NMP_ASSERT_FAIL();
    return false;
  }
}

//---------------------------------------------------------------------------------------------------------------------
// Accessors for collisions being active
bool CharacterControllerPhysX2Basic::getCollisionEnabled()
{
  if (m_nxController->getActor())
    return !m_nxController->getActor()->readActorFlag(NX_AF_DISABLE_COLLISION);
  return false;
}

//---------------------------------------------------------------------------------------------------------------------
// Turn collisions on/off
void CharacterControllerPhysX2Basic::enableCollision(bool enable)
{
  if (enable == getCollisionEnabled())
    return;
  if (enable)
  {
    m_nxController->setCollision(true);

    //----------------------------
    // Transport CCM physX object back.
    NMP::Vector3 p(m_positionWorld + NMP::Vector3(0.0f, m_controllerOffset, 0.0f));
    m_nxController->setPosition(NxExtendedVec3(p.x, p.y, p.z));
    m_nxControllerEnabled = true;
  }
  else
  {
    m_nxController->setPosition(NxExtendedVec3(0.0f, -1000.0f, 0.0f));
    m_nxControllerEnabled = false;

    m_nxController->setCollision(false);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Check if the network has one of the physical flags set
bool CharacterControllerPhysX2Basic::isNetworkPhysical(const MR::Network* network)
{
  MR::NetworkDef* netDef = network->getNetworkDef();

  for (MR::NodeID i = 0; i < netDef->getNumNodeDefs(); ++i)
  {
    MR::NodeDef* node = netDef->getNodeDef(i);
    if (node->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_PHYSICAL)
      return true;

    if (node->getNodeFlags() & MR::NodeDef::NODE_FLAG_IS_PHYSICS_GROUPER)
      return true;
  }

  return false;
}

//---------------------------------------------------------------------------------------------------------------------
CharacterControllerPhysX2Basic::CharacterControllerPhysX2Basic():
  m_requestedDeltaWorldTranslation(NMP::Vector3::InitZero),
  m_requestedDeltaWorldOrientation(NMP::Quat::kIdentity),
  m_positionWorld(NMP::Vector3::InitZero),
  m_orientationWorld(NMP::Quat::kIdentity),
  m_actualDeltaWorldTranslation(NMP::Vector3::InitZero),
  m_actualDeltaWorldOrientation(NMP::Quat::kIdentity),
  m_nxController(NULL),
  m_nxControllerEnabled(false),
  m_controllerOffset(0.0f),
  m_physicsRig(NULL),
  m_net(NULL)
{
  //----------------------------
  m_hitReport = static_cast<ControllerHitReport*>(NMPMemoryAlloc(sizeof(ControllerHitReport)));
  new(m_hitReport) ControllerHitReport(0.3f, m_touchTimes);
}

//---------------------------------------------------------------------------------------------------------------------
// Release any memory setup in this class
bool CharacterControllerPhysX2Basic::term()
{
  // Destroy any PhysX character controller manager that we have created.
  if (m_nxController)
  {
    NMP_ASSERT(m_nxControllerManager);
    m_nxControllerManager->releaseController(*m_nxController);
    m_nxController = NULL;
  }

  // Wipe our reference to the PhysX controller manager.
  m_nxControllerManager = NULL;

  if (m_physicsRig)
  {
    NMP::Memory::memFree(m_physicsRig);
    m_physicsRig = NULL;
  }

  if (m_hitReport)
  {
    NMP::Memory::memFree(m_hitReport);
    m_hitReport = NULL;
  }

  return true;
}

//---------------------------------------------------------------------------------------------------------------------
// Indicate that an update is about to start and start setting up parameters before the acutal update
void CharacterControllerPhysX2Basic::updatePreController()
{
  if (m_net->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    enableCollision(false);
  }
  else
  {
    NET_LOG_MESSAGE(99, "Updating trajectory\n");
    if (!getCollisionEnabled())
    {
      enableCollision(true);
    }

    //----------------------------
    // We haven't been supplied with a delta transform so directly use the values calculated and stored in the Network.
    // The networks suggested delta translation is in the characters local space (relative to the characters current world orientation).
    // So to get the suggested delta translation in to world space rotate it by the characters current world orientation.
    m_requestedDeltaWorldTranslation = m_orientationWorld.rotateVector(m_net->getTranslationChange());
    m_requestedDeltaWorldOrientation = m_net->getOrientationChange();

    //----------------------------
    // Initialise the achieved world delta transform with the requested transform.
    m_actualDeltaWorldTranslation = m_requestedDeltaWorldTranslation;
    m_actualDeltaWorldOrientation = m_requestedDeltaWorldOrientation;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Update the character controller - includes a call to update the physx controller.
void CharacterControllerPhysX2Basic::updateController(float deltaTime)
{
  if (m_physicsRig->isReferenced() && m_net->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
    enableCollision(false);
  else
    enableCollision(true);

  if (m_nxControllerEnabled)
  {
    //----------------------------
    // Apply the delta orientation change.
    m_orientationWorld *= m_actualDeltaWorldOrientation;
    m_orientationWorld.normalise();

    //----------------------------
    // The controller is switched on, so update it according to Nx-controller semi-physics
    // requestedDeltaTranslation = the desired movement, as calculated from the animation system, that
    // we are trying to achieve

    //----------------------------
    // Execute physX's 'collide and slide' controller movement function.
    m_hitReport->updatePreMove(deltaTime, m_controllerDef.getMaxPushForce());
    NxU32 collisionGroupsFilterMask = (1 << MR::GROUP_COLLIDABLE_NON_PUSHABLE) | (1 << MR::GROUP_COLLIDABLE_PUSHABLE);
    NxU32 collisionFlags;
    m_nxController->move(
                      MR::nmVector3ToNxVec3(m_actualDeltaWorldTranslation),
                      collisionGroupsFilterMask,
                      0.00000001f,      // the minimum traveled distance to consider.
                      collisionFlags,   // Returned after move.
                      1.0f);            // Sharpness.

    //----------------------------
    // In our basic tutorial our character is always on the ground so always increment the contact time.
    m_net->setCharacterPropertiesGroundContactTime(m_net->getCharacterPropertiesGroundContactTime() + deltaTime);
  }

  //----------------------------
  // Global update of the PhysX character controller.  In our basic tutorial we only have one character so we can
  // wrap the update inside out CharacterControllerPhysX2Basic update.  For a full implementation see how the
  // CharacterControllerPhysX2 is updated in the Game Management library.
  m_nxControllerManager->updateControllers();

  //----------------------------
  // Store the new transform for the character on the Network so that the next network update has
  //  access to an accurate world transform on its next update.
  m_net->updateCharacterPropertiesWorldRootTransform(
    NMP::Matrix34(m_orientationWorld, m_positionWorld), 
    true);

}

//----------------------------------------------------------------------------------------------------------------------
// Update the GamePhysicsCharCtrl position, orientation and velocity
void CharacterControllerPhysX2Basic::updatePostController()
{
  if (m_net->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    //----------------------------
    // Update the root translation and orientation.
    // When using physics the root needs to be updated in updatePostPhysics so that the network
    // has the new root location, and can thus convert from world to local space. This required removing
    // setting from the root position from SceneObjectRecord::update (since that would be too late)
    m_actualDeltaWorldTranslation = m_orientationWorld.rotateVector(m_net->getTranslationChange());
    m_actualDeltaWorldOrientation = m_net->getOrientationChange();

    //----------------------------
    // Apply delta orientation change.
    m_orientationWorld *= m_actualDeltaWorldOrientation;
    m_orientationWorld.normalise();

    //----------------------------
    // make the root follow the ragdoll
    m_positionWorld += m_actualDeltaWorldTranslation;

    //----------------------------
    // Set the character controller's position.
    m_net->updateCharacterPropertiesWorldRootTransform(
      NMP::Matrix34(m_orientationWorld, m_positionWorld), 
      true);
  }
}

} // namespace Game
