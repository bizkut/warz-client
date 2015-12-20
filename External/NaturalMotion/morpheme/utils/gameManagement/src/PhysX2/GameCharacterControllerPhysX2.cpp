// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "GameManagement/PhysX2/GameCharacterControllerPhysX2.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

// If the character movement should "hug" the ground at the top of slopes etc.
#define STICK_TO_GROUND

/// When the character controller is in contact with another body
///  this is the time taken for the controller force to reach its maximum.
///  (This is force the controller applies to the body it is contacting)
float g_maxForceTime = 0.3f;

//----------------------------------------------------------------------------------------------------------------------
/// \class ControllerHitReport
/// \brief Controller hit report for PhysX2. 
///
/// This class is used to apply forces to bodies that the character controller
/// comes into contact with. The force is ramped up depending on how long the body has been in contact with the
/// character controller.
//----------------------------------------------------------------------------------------------------------------------
class ControllerHitReport : public NxUserControllerHitReport
{
public:
  ControllerHitReport(float maxTouchTime, TouchTimes& touchTimes);

  void updatePreMove(float dt, float maxPushForce);

  virtual NxControllerAction onShapeHit(const NxControllerShapeHit& hit);
  virtual NxControllerAction onControllerHit(const NxControllersHit& NMP_UNUSED(hit));

  float      m_maxPushForce; ///< The maximum push force which can be applied by the character controller.
  float      m_dt;           ///< The timestep since the last character controller update.
  float      m_maxTouchTime; ///< The amount of time the controller will touch something before it ramps up to full push force.
  TouchTimes m_touchTimes;   ///< A map of Shape pointers to touch time (in seconds). 
};
  
//----------------------------------------------------------------------------------------------------------------------
// CharacterControllerPhysX functions
//----------------------------------------------------------------------------------------------------------------------
CharacterControllerPhysX2::CharacterControllerPhysX2() :
m_positionWorld(NMP::Vector3::InitZero),
  m_orientationWorld(NMP::Quat::kIdentity),
  m_orientationWorldPrevFrame(NMP::Quat::kIdentity),
  m_actualDeltaWorldTranslation(NMP::Vector3::InitZero),
  m_actualDeltaWorldOrientation(NMP::Quat::kIdentity),
  m_physXController(NULL),
  m_gravityEnabled(false),
  m_physXControllerEnabled(false),
  m_onGround (false),
  m_onGroundPrevFrame (false),
  m_verticalStepVector(NMP::Vector3::InitZero),
  m_velocityInGravityDirection(NMP::Vector3::InitZero),
  m_originOffset(NMP::Vector3::InitZero),
  m_hitReport(NULL),
  m_activeControllerDef(NULL)
{
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerPhysX2::~CharacterControllerPhysX2()
{
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerPhysX2* CharacterControllerPhysX2::create(
  MR::Network*            net,
  const NMP::Vector3&     initialPosition,     ///< Starting position.
  const NMP::Quat&        initialOrientation,  ///< Starting orientation.
  const NMP::Vector3&     graveyardPosition,   ///< Where to put the PhysX character controller object when we want to
                                               ///<  temporarily disable collision with it.
  MR::PhysicsScenePhysX2* physicsScene,
  NxControllerManager*    controllerManager)
{
  CharacterControllerPhysX2* const instance = static_cast<CharacterControllerPhysX2*>(NMPMemoryAlloc(sizeof(CharacterControllerPhysX2)));
  new(instance) CharacterControllerPhysX2();

  instance->init(net, initialPosition, initialOrientation, graveyardPosition, physicsScene, controllerManager);

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::destroy(CharacterControllerPhysX2* characterController)
{
  NMP_ASSERT(characterController);

  characterController->term();
  NMP::Memory::memFree(characterController);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::init(
  MR::Network*            net,
  const NMP::Vector3&     initialPosition,
  const NMP::Quat&        initialOrientation,
  const NMP::Vector3&     graveyardPosition,
  MR::PhysicsScenePhysX2* physicsScene,
  NxControllerManager*    controllerManager)
{  
  NMP_ASSERT(net && controllerManager);
  m_net = net;
  MR::NetworkDef* netDef = m_net->getNetworkDef();
  m_physXControllerManager = controllerManager;
  
  m_physXControllerEnabled = false;
  m_gravityEnabled = true;  // We are making a character with a physics system character controller, so assume gravity is on.

  // Get the active physics and animation rig defs.
  MR::AnimRigDef* animRigDef = m_net->getActiveRig();
  MR::PhysicsRigDef* physicsRigDef = getActivePhysicsRigDef(m_net);
    
  //----------------------------
  // Store a pointer to the current character controller values from the network def data.
  m_activeControllerDef = net->getActiveCharacterControllerDef();

  //----------------------------
  // Store a pointer to the physics scene.
  setPhysicsScene(physicsScene);
  
  // Get the world up direction from the physics scene.
  NMP::Vector3 worldUpDirection = physicsScene->getWorldUpDirection();
 
  //----------------------------
  // Set the starting transform.
  //  Nudge the start position up very slightly so that, in the typical case with the ground plane at
  //  zero height etc, we don't start borderline intersecting the ground.
  m_positionWorld = initialPosition + (worldUpDirection * 1e-5f);
  m_orientationWorld = initialOrientation;
  m_positionWorldPrevFrame = m_positionWorld;
  m_orientationWorldPrevFrame = m_orientationWorld;

  //----------------------------
  // Set the graveyard of the nxController physical body.
  m_graveyardPosition = graveyardPosition;

  //----------------------------
  // Zero deltas and velocities.
  m_actualDeltaWorldTranslation.setToZero();
  m_actualDeltaWorldOrientation.identity();
  m_requestedDeltaWorldTranslation.setToZero();
  m_requestedDeltaWorldOrientation.identity();
  m_velocityInGravityDirection.setToZero();
    
  //----------------------------
  MR::PhysicsRigPhysX2* physicsRig = NULL;
  if (physicsRigDef)
  {
    // Create a physics rig and and store a pointer to it.
    NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(MR::PhysicsRigPhysX2::getMemoryRequirements(physicsRigDef));
    NMP_ASSERT(resource.ptr);
    physicsRig = MR::PhysicsRigPhysX2::init(
                                resource,
                                physicsRigDef,
                                physicsScene,
                                animRigDef,
                                getAnimToPhysicsMap(netDef, m_net->getActiveAnimSetIndex()),
                                1 << MR::GROUP_CHARACTER_PART,
                                (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE));

    // set the position of the physical body to the 'graveyard'.
    physicsRig->setKinematicPos(m_graveyardPosition);
  }
  // Note even if the Network has no physics rig we will still run a physical character controller.
  setPhysicsRig(m_net, physicsRig);
  

  //----------------------------
  // Recalculate the offset from the origin of the character controller.
  m_originOffset = calculateOriginOffset(worldUpDirection);
   
  //----------------------------
  // Now set up the physX character controller object from the contents of the controller definition.
  createPhysXController(physicsScene);

  // Tell the Network instance that this is the character controller that it should be communicating with.
  m_net->setCharacterController(this);

  // Set the networks position for the character controller.
  m_net->setCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_orientationWorld, m_positionWorld));

  //----------------------------
  if (physicsRig)
  {
    // Tell the physics rig about the character controller capsule that it is working with.
    physicsRig->setCharacterControllerActor(m_physXController->getActor());

    // Turn off collision detection between Character controller capsule and all body parts.
    size_t nBones = physicsRig->getNumParts();
    for (size_t i = 0 ; i < nBones ; ++i)
    {
      MR::PhysicsRigPhysX2::PartPhysX* part = (MR::PhysicsRigPhysX2::PartPhysX*)physicsRig->getPart((uint32_t)i);
      physicsScene->getPhysXScene()->setActorPairFlags(
                                                *part->getActor(),
                                                *m_physXController->getActor(),
                                                NX_IGNORE_PAIR);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::term()
{
  // Destroy any PhysX character controller that we have created.
  if (m_physXController)
  {
    NMP_ASSERT(m_physXControllerManager);
    m_physXControllerManager->releaseController(*m_physXController);
    m_physXController = NULL;
  }

  // Wipe our reference to the PhysX controller manager.
  m_physXControllerManager = NULL;
  
  // Destroy any created physics rig.
  NMP_ASSERT(m_net);
  MR::PhysicsRig* physicsRig = getPhysicsRig(m_net);
  if (physicsRig)
  {
    physicsRig->term();
    NMP::Memory::memFree(physicsRig);
    setPhysicsRig(m_net, NULL);
  }

  // Tidy the hit report structure.
  if (m_hitReport)
  {
    NMP::Memory::memFree(m_hitReport);
    m_hitReport = NULL;
  }

  // Wipe our reference to the global PhysX2 scene.
  setPhysicsScene(NULL);

  // Clean up some reference pointers etc.
  m_activeControllerDef = NULL;
  m_physXControllerEnabled = false;
  m_net = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 CharacterControllerPhysX2::calculateOriginOffset(
  const NMP::Vector3& worldUpDirection)
{
  NMP_ASSERT(m_activeControllerDef);
  NMP::Vector3 offset(NMP::Vector3::InitTypeOne);
  switch (m_activeControllerDef->getShape())
  {
    case MR::CharacterControllerDef::CapsuleShape:
    {
      offset = 
        worldUpDirection *
        ((m_activeControllerDef->getHeight() * 0.5f) + // Half height of the cylinder that forms the central body of the capsule.
         m_activeControllerDef->getRadius() +          // Sphere on bottom of character capsule.
         m_activeControllerDef->getSkinWidth());       // Skin width around the whole character controller.
      break;
    }
    case MR::CharacterControllerDef::BoxShape:
    {
      offset = 
        worldUpDirection *
        ((m_activeControllerDef->getHeight() * 0.5f) + // Half height of the box.
         m_activeControllerDef->getSkinWidth());       // Skin width around the whole character controller.
      break;
    }
    default:
      NMP_ASSERT_FAIL(); // Not a valid shape type
  }

  return offset;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::createPhysXController(MR::PhysicsScenePhysX2* physicsScene)
{
  //----------------------------
  // Check a NxController hasn't already been created for this Physics Character Controller
  NMP_ASSERT(!m_physXController)

  //----------------------------
  // Initialise a PhysX controller description structure.
  NxControllerDesc* desc = NULL;
  

  //----------------------------
  // Collision shape specific initialisation of the character controller.
  NxCapsuleControllerDesc capsuleDesc;
  NxBoxControllerDesc boxDesc;
  NMP_ASSERT(m_activeControllerDef);
  switch (m_activeControllerDef->getShape())
  {
    case MR::CharacterControllerDef::CapsuleShape:
    {
      capsuleDesc.radius = m_activeControllerDef->getRadius();
      capsuleDesc.height = m_activeControllerDef->getHeight();
      desc = &capsuleDesc;
      break;
    }
    case MR::CharacterControllerDef::BoxShape:
    {
      // The box extents are half-extents.
      boxDesc.extents.x = m_activeControllerDef->getWidth() * 0.5f;
      boxDesc.extents.y = m_activeControllerDef->getHeight() * 0.5f;
      boxDesc.extents.z = m_activeControllerDef->getDepth() * 0.5f;
      desc = &boxDesc;
      break;
    }
    default:
      NMP_ASSERT_FAIL_MSG("An unsupported CharacterController type has been selected for PhysX!");
  }
  NMP_ASSERT(desc);
   
  desc->interactionFlag = NXIF_INTERACTION_INCLUDE;

  // Set the up direction specified in the physics scene.
  // NOTE: PhysX character controller can only cope with the up direction being in one of the primary world axies.
  NMP_ASSERT(physicsScene);
  NMP::Vector3 worldUpDirection = physicsScene->getWorldUpDirection();
  NMP_ASSERT(worldUpDirection.x > 0.99f || worldUpDirection.z > 0.99f || worldUpDirection.y > 0.99f);
  if (worldUpDirection.x > 0.99f)
    desc->upDirection = NX_X;
  else if (worldUpDirection.z > 0.99f)
    desc->upDirection = NX_Z;
  else
    desc->upDirection = NX_Y;

  desc->slopeLimit = cosf(NMP::degreesToRadians(m_activeControllerDef->getMaxSlopeAngle()));
  desc->skinWidth = m_activeControllerDef->getSkinWidth();
  desc->stepOffset = m_activeControllerDef->getStepHeight();

  //----------------------------
  m_hitReport = static_cast<ControllerHitReport*>(NMPMemoryAlloc(sizeof(ControllerHitReport)));
  new(m_hitReport) ControllerHitReport(g_maxForceTime, m_touchTimes);

  m_hitReport->m_maxPushForce = m_activeControllerDef->getMaxPushForce();
  desc->callback = m_hitReport;

  // Set the starting position for the physical controller object.
  NMP::Vector3 controllerPos = m_positionWorld + computeWorldSpaceCCOriginOffset();
  desc->position.set(controllerPos.x, controllerPos.y, controllerPos.z);

  //----------------------------
  // Create the physical controller object.
  m_physXController = m_physXControllerManager->createController(physicsScene->getPhysXScene(), *desc);
  NMP_ASSERT(m_physXController);

  m_physXController->setCollision(false);
  setActor(m_physXController->getActor());
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::updateControllerMove(
  float               deltaTime,
  const NMP::Vector3* requestedDeltaWorldTranslation,
  const NMP::Quat*    requestedDeltaWorldOrientation)
{
  NMP_ASSERT(m_net && m_physXController && m_activeControllerDef);

  //----------------------------
  // Update the last frames transform.
  m_orientationWorldPrevFrame = m_orientationWorld;
  m_positionWorldPrevFrame = m_positionWorld;

  MR::PhysicsRigPhysX2* physicsRig = (MR::PhysicsRigPhysX2*) getPhysicsRig(m_net);
  if (physicsRig && physicsRig->isReferenced() && (m_net->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS))
  {
    //----------------------------
    // The character root position is being controlled by the position of the physics rig.

    // Disable the physics controller object.
    disablePhysXController();

    // Delta transform values come from the physics update so zero them for now.
    m_actualDeltaWorldTranslation.setToZero();
    m_actualDeltaWorldOrientation.identity();
    m_requestedDeltaWorldTranslation.setToZero();
    m_requestedDeltaWorldOrientation.identity();

    // The on ground flag is only maintained when the character controller physical object is active.
    m_onGroundPrevFrame = false;
    m_onGround = false;
    m_net->setCharacterPropertiesGroundContactTime(0.0f);
    m_net->setCharacterPropertiesAchievedRequestedMovement(true);
  }
  else
  {
    //----------------------------
    // The character root position is being controlled by a combination of a requested delta movement and the PhysX controller semi-physics.
    // Enable the controller and attempt to move it by the requested amount.

    // Enable the physics controller object.
    enablePhysXController();

    // Calculate and store requested world delta transform.
    initRequestedWorldDeltaTransform(requestedDeltaWorldTranslation, requestedDeltaWorldOrientation);

    // Add affect of gravity to the requested delta translation.
    addAccelerationDueToGravity(deltaTime);

    // Initialise the achieved world delta transform with the requested transform.
    m_actualDeltaWorldTranslation = m_requestedDeltaWorldTranslation;
    m_actualDeltaWorldOrientation = m_requestedDeltaWorldOrientation;

    // Apply the delta orientation change.
    m_orientationWorld *= m_requestedDeltaWorldOrientation;
    m_orientationWorld.normalise();
    
    // Calculate what component of the requested delta translation is horizontal.
    calculateRequestedHorizontalDeltaTranslation();

    // If the controller was in ground contact last frame try and keep it in contact this frame by adding
    //  a vertical step down to this frames requested delta movement of our character.
    addStickToGroundMovement();
        
    // Execute physX's 'collide and slide' controller movement function.
    m_hitReport->updatePreMove(deltaTime, m_activeControllerDef->getMaxPushForce());
    NxU32 collisionGroupsFilterMask = (1 << MR::GROUP_COLLIDABLE_NON_PUSHABLE) | (1 << MR::GROUP_COLLIDABLE_PUSHABLE);
    NxU32 collisionFlags;
    m_physXController->move(
                      MR::nmVector3ToNxVec3(m_requestedDeltaWorldTranslation),
                      collisionGroupsFilterMask,
                      0.00000001f,      // the minimum traveled distance to consider.
                      collisionFlags,   // Returned after move.
                      1.0f);            // Sharpness.
                      

    // Determine whether character is on the ground or in the air after the update,
    //  and if it has just transitioned from one state to the other.
    m_onGround = (collisionFlags & NXCC_COLLISION_DOWN) != 0 ;

    // Update the duration of our ground contact.
    if (m_onGround && m_onGroundPrevFrame)
      m_net->setCharacterPropertiesGroundContactTime(m_net->getCharacterPropertiesGroundContactTime() + deltaTime); // on ground
    else if (!m_onGround && !m_onGroundPrevFrame)
      m_net->setCharacterPropertiesGroundContactTime(m_net->getCharacterPropertiesGroundContactTime() - deltaTime); // in air
    else
      m_net->setCharacterPropertiesGroundContactTime(0.0f); // transition from/to ground to/from air
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::initRequestedWorldDeltaTransform(
  const NMP::Vector3* requestedDeltaWorldTranslation,
  const NMP::Quat*    requestedDeltaWorldOrientation)
{
  if (requestedDeltaWorldTranslation && requestedDeltaWorldOrientation)
  {
    // We have been supplied with a delta transform so use it.
    m_requestedDeltaWorldTranslation = *requestedDeltaWorldTranslation;
    m_requestedDeltaWorldOrientation = *requestedDeltaWorldOrientation;
  }
  else
  {
    // We haven't been supplied with a delta transform so directly use the values calculated and stored in the Network.
    // The networks suggested delta translation is in the characters local space (relative to the characters current world orientation).
    // So to get the suggested delta translation in to world space rotate it by the characters current world orientation.
    m_requestedDeltaWorldTranslation = m_orientationWorld.rotateVector(m_net->getTranslationChange());
    m_requestedDeltaWorldOrientation = m_net->getOrientationChange();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::addAccelerationDueToGravity(float deltaTime)
{
  if (m_gravityEnabled)
  {
    // Add acceleration due to gravity to translation.
    NMP::Vector3 gravityAccel = getPhysicsScene()->getGravity();
    m_velocityInGravityDirection += (gravityAccel * deltaTime);
    m_requestedDeltaWorldTranslation += (m_velocityInGravityDirection * deltaTime);   
  }
  else
  {
    // Don't apply gravity to the CC but set the velocity in gravity direction to the component of the animation
    // delta in the gravity direction so that CC will inherit anim's velocity when switching back to the default
    // (gravity enabled) state.
    NMP::Vector3 gravityDirection(getPhysicsScene()->getGravity());
    gravityDirection.fastNormalise();
    m_velocityInGravityDirection = gravityDirection * m_requestedDeltaWorldTranslation.dot(gravityDirection) * deltaTime;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::calculateRequestedHorizontalDeltaTranslation()
{
  NMP_ASSERT(getPhysicsScene());
  NMP::Vector3 worldUpDirection = getPhysicsScene()->getWorldUpDirection();
  float amountInUpDirection = worldUpDirection.dot(m_requestedDeltaWorldTranslation);
  NMP::Vector3 vectorInUpDirection = worldUpDirection * amountInUpDirection;
  m_requestedHorizontalDeltaTranslation = m_requestedDeltaWorldTranslation - vectorInUpDirection;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::addStickToGroundMovement()
{
#ifdef STICK_TO_GROUND
  if (m_onGround)
  {
    // Move character down by the maximum slope amount over the distance moved horizontally.
    float requestedDeltaHorizontalDistance = m_requestedHorizontalDeltaTranslation.magnitude();
    NMP_ASSERT(getPhysicsScene());
    NMP::Vector3 worldUpDirection = getPhysicsScene()->getWorldUpDirection();
    m_verticalStepVector = worldUpDirection * (-requestedDeltaHorizontalDistance * tanf(NMP::degreesToRadians(m_activeControllerDef->getMaxSlopeAngle())));
    m_requestedDeltaWorldTranslation += m_verticalStepVector;
  }
#endif // STICK_TO_GROUND
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::updateControllerMoveAdjustingStepping()
{
#ifdef STICK_TO_GROUND
  if (m_physXControllerEnabled && m_onGroundPrevFrame && !m_onGround)
  {
    // We were on the ground, but are no longer on it,
    //  undo the extra vertical movement that we added to keep our character in ground contact (when going down hill).
    NxU32 collisionGroupsFilterMask = (1 << MR::GROUP_COLLIDABLE_NON_PUSHABLE) | (1 << MR::GROUP_COLLIDABLE_PUSHABLE);
    NxU32 collisionFlags;
    m_physXController->move(
                      NxVec3(-m_verticalStepVector.x, -m_verticalStepVector.y, -m_verticalStepVector.z),
                      collisionGroupsFilterMask,
                      0.00000001f,      // the minimum traveled distance to consider.
                      collisionFlags,   // Returned after move.
                      1.0f);            // Sharpness.
  }
#endif // STICK_TO_GROUND
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::updateCachedTransformsFromCharacterControllerPosition(float deltaTime)
{
  //----------------------------
  // The NxController::move(desired) function above has changed the position of the NxController according to the
  //  desired change from animation filtered through the NxController's semi-physical collision detection.
  // So we now need to update our cached world transform to reflect this new value,
  //  and also set the root position for the network's animation data.
  if (m_physXControllerEnabled)
  {
    // Get and store the updated position of the NxController.
    NxExtendedVec3 newWorldPos = m_physXController->getPosition();
    m_positionWorld.set((float) newWorldPos.x, (float) newWorldPos.y, (float) newWorldPos.z);

    // Take off the offset value to translate the character position down from the center of
    //  the controller body to where the feet should be.
    m_positionWorld -= computeWorldSpaceCCOriginOffset();

    // Determine whether the character achieved the requested delta movement this frame.
    didControllerAchieveRequestedMovement();
    
    // Record the vertical velocity of the character.
    computeVerticalVelocity(deltaTime);
        
    // Store the new transform for the character on the Network so that the next network update has
    //  access to an accurate world transform on its next update.
    m_net->updateCharacterPropertiesWorldRootTransform(
                NMP::Matrix34(m_orientationWorld, m_positionWorld), 
                true);
  }

  // We have finished with the working transform request values so wipe them down.
  m_requestedDeltaWorldTranslation.setToZero();
  m_requestedHorizontalDeltaTranslation.setToZero();
  m_requestedDeltaWorldOrientation.identity();
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::computeVerticalVelocity(float deltaTime)
{
  // Calculate the component of the actual translation in the gravity direction.
  NMP::Vector3 gravityDirection(getPhysicsScene()->getGravity());
  gravityDirection.fastNormalise();
  float actualTranslationInGravityDirection = gravityDirection.dot(m_actualDeltaWorldTranslation);

  // Calculate the component of the requested translation in the gravity direction.
  float requestedTranslationInGravityDirection = gravityDirection.dot(m_requestedDeltaWorldTranslation);

  //----------------------------
  // Record the vertical velocity of the character,
  //  but exclude any component that comes from the requested translation because this would artificially accelerate
  //  the character.
  //  If on the ground then leave the velocity at zero so that we run off cliff edges starting horizontally
  //  (even when we are stepping down).
  m_velocityInGravityDirection.setToZero();
  if (!m_onGround && m_gravityEnabled && (actualTranslationInGravityDirection > requestedTranslationInGravityDirection))
  {
    m_velocityInGravityDirection = m_actualDeltaWorldTranslation - m_requestedDeltaWorldTranslation;
    float deltaTranslationInGravityDirection = gravityDirection.dot(m_velocityInGravityDirection);
    m_velocityInGravityDirection = gravityDirection * (deltaTranslationInGravityDirection / deltaTime);
  }
}

//---------------------------------------------------------------------------------------------------------------------
bool CharacterControllerPhysX2::didControllerAchieveRequestedMovement()
{
  bool achievedRequestedMovement = true;
  m_actualDeltaWorldTranslation = (m_positionWorld - m_positionWorldPrevFrame);
  if (m_onGround)
  {
    // Calculate actual horizontal movement
    NMP_ASSERT(getPhysicsScene());
    NMP::Vector3 worldUpDirection = getPhysicsScene()->getWorldUpDirection();
    float amountInUpDirection = worldUpDirection.dot(m_actualDeltaWorldTranslation);
    NMP::Vector3 vectorInUpDirection = worldUpDirection * amountInUpDirection;
    NMP::Vector3 actualHorizontalMovementVector = m_actualDeltaWorldTranslation - vectorInUpDirection;

    // Compare the horizontal movement
    NMP::Vector3 deltaMovementVector = actualHorizontalMovementVector - m_requestedHorizontalDeltaTranslation;
    if (deltaMovementVector.magnitude() > (1e-4f *  m_activeControllerDef->getHeight()))
      achievedRequestedMovement = false;
  }
  m_net->setCharacterPropertiesAchievedRequestedMovement(achievedRequestedMovement);
  return achievedRequestedMovement;
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::updateCachedTransformsFromPhysicsRigPosition(float deltaTime)
{
  if (m_net->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    // When the characters root position is being controlled through the physical simulation of its rigid bodies
    //  (Rag doll for example) we need to update the character controllers position from this value after the
    //  simulation step. i.e. Make the character root position follow the ragdoll.
    //  This delta transform is calculated after simulation, in updatePostPhysics, and stored on the Network.
    //  The networks suggested delta translation is in the characters local space 
    //  (relative to the characters current world orientation).
    //  So to get the suggested delta translation in to world space rotate it by the characters current world orientation.
    m_actualDeltaWorldTranslation = m_orientationWorld.rotateVector(m_net->getTranslationChange());
    m_actualDeltaWorldOrientation = m_net->getOrientationChange();

    // Apply delta orientation and translation change.
    m_orientationWorld *= m_actualDeltaWorldOrientation;
    m_orientationWorld.normalise();
    m_positionWorld += m_actualDeltaWorldTranslation;

    // Store the new transform information on the network.  
    m_net->updateCharacterPropertiesWorldRootTransform(
                NMP::Matrix34(m_orientationWorld, m_positionWorld), 
                true);

    // Find the component of delta translation in the gravity direction.
    NMP::Vector3 gravityDirection(getPhysicsScene()->getGravity());
    gravityDirection.fastNormalise();
    float deltaTranslationInGravityDirection = gravityDirection.dot(m_actualDeltaWorldTranslation);

    // Calculate velocity in gravity direction.
    NMP_ASSERT(deltaTime > 0.0f);
    m_velocityInGravityDirection = gravityDirection * (deltaTranslationInGravityDirection / deltaTime);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 CharacterControllerPhysX2::computeWorldSpaceCCOriginOffset()
{
  // Rotate the relative offset in to the characters world space alignment.
  NMP::Vector3 worldSpaceCCOriginOffset;
  m_orientationWorld.rotateVector(m_originOffset, worldSpaceCCOriginOffset);
 
  return worldSpaceCCOriginOffset;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::disablePhysXController()
{
  if (m_physXController && m_physXControllerEnabled)
  {
    //----------------------------
    // teleport CC physX object to graveyard
    m_physXController->setPosition(NxExtendedVec3(m_graveyardPosition.x, m_graveyardPosition.y, m_graveyardPosition.z));
    m_physXControllerEnabled = false;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::enablePhysXController()
{
  if (m_physXController && !m_physXControllerEnabled)
  {
    // Teleport CC physX object back from graveyard.
    NMP::Vector3 nxControllerPosition = m_positionWorld + computeWorldSpaceCCOriginOffset();
    m_physXController->setPosition(NxExtendedVec3(nxControllerPosition.x, nxControllerPosition.y, nxControllerPosition.z));
    m_physXControllerEnabled = true;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::setPosition(NMP::Vector3& position)
{
  setTransform(position, m_orientationWorld);
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::setOrientation(NMP::Quat& orientation)
{
  setTransform(m_positionWorld, orientation);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX2::setTransform(
  const NMP::Vector3& position,
  const NMP::Quat&    orientation)
{
  m_actualDeltaWorldTranslation.setToZero();
  m_actualDeltaWorldOrientation.identity();
  m_requestedDeltaWorldTranslation.setToZero();
  m_requestedDeltaWorldOrientation.identity();

  // We are teleporting the character to a new position, if we don't set the previous frames transform to be
  //  the same as this one we could end up with an artificially high velocity.
  m_positionWorldPrevFrame = position;
  m_orientationWorldPrevFrame = orientation;

  // Set the new transform.
  m_positionWorld = position;
  m_orientationWorld = orientation;
  m_net->updateCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_orientationWorld, m_positionWorld), true);

  // Set the position 
  if (m_physXControllerEnabled)
  {
    NMP::Vector3 nxControllerPosition = m_positionWorld + computeWorldSpaceCCOriginOffset();
    m_physXController->setPosition(NxExtendedVec3(nxControllerPosition.x, nxControllerPosition.y, nxControllerPosition.z));
  }
}

//----------------------------------------------------------------------------------------------------------------------
// ControllerHitReport functions.
//----------------------------------------------------------------------------------------------------------------------
ControllerHitReport::ControllerHitReport(float maxTouchTime, TouchTimes& touchTimes) :
  m_maxPushForce(0.0f),
  m_dt(1.0f),
  m_maxTouchTime(maxTouchTime),
  m_touchTimes(touchTimes)
{
}

//----------------------------------------------------------------------------------------------------------------------
void ControllerHitReport::updatePreMove(float dt, float maxPushForce)
{
  m_dt = dt;
  m_maxPushForce = maxPushForce;
  for(TouchTimes::iterator it = m_touchTimes.begin() ; it != m_touchTimes.end() ; ++it)
  {
    it->second -= dt;
    if(it->second < 0.0f)
    {
      it->second = 0.0f;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
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
      {
        touchTime = m_maxTouchTime;
      }

      NxF32 force = m_maxPushForce * touchTime / m_maxTouchTime;
      actor.addForceAtPos(
                        -hit.worldNormal * force,
                        NxVec3((float) hit.worldPos.x,
                               (float) hit.worldPos.y,
                               (float) hit.worldPos.z),
                        NX_FORCE);
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

} // namespace Game
//----------------------------------------------------------------------------------------------------------------------
