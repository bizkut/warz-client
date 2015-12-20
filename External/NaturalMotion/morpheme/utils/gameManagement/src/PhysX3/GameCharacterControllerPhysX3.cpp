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
#include "GameManagement/PhysX3/GameCharacterControllerPhysX3.h"
#include "mrPhysicsRigPhysX3Articulation.h"
#include "mrPhysicsRigPhysX3Jointed.h"
//----------------------------------------------------------------------------------------------------------------------

namespace Game
{

/// When the character controller is in contact with another body
///  this is the time taken for the controller force to reach its maximum.
///  (This is force the controller applies to the body it is contacting)
const float g_maxForceTime = 0.3f;

/// If enabled, a dynamic actor will be used to push the character controller out of penetration.
#define USE_CC_DYNAMIC_ACTOR

// If the character movement should "hug" the ground at the top of slopes etc.
#define STICK_TO_GROUND

/// Make the dynamic shape attached to the character controller slightly larger than the kinematic
///  one so if the KCC does have to be re-spawned then there is a safety margin.
const float g_dynamicControllerRadiusScale = 1.1f;

/// A couple of defaults used when creating a default material for a new controller (only if we haven't been provided with a material).
const physx::PxCombineMode::Enum g_defaultCCMaterialFrictionCombineMode    = physx::PxCombineMode::eMULTIPLY; ///< How to combine the friction of two contacting objects.
const physx::PxCombineMode::Enum g_defaultCCMaterialRestitutionCombineMode = physx::PxCombineMode::eMULTIPLY; ///< How to combine the restitution of two impacting objects.

//-----------
/// Character controller collision filtering structures.
///  These structures define collision filtering logic during the movement of the PhysX character controllers.
const physx::PxFilterData g_filterData(
                                       0,
                                       1 << MR::GROUP_CHARACTER_CONTROLLER | 
                                       1 << MR::GROUP_CHARACTER_PART | 
                                       1 << MR::GROUP_NON_COLLIDABLE | 
                                       1 << MR::GROUP_INTERACTION_PROXY,
                                       0,
                                       0);

/// Custom filtering logic for scene query intersection candidates.
MR::MorphemePhysX3QueryFilterCallback g_morphemePhysX3QueryFilterCallback(g_filterData);

/// Filtering data for PhysX character controller move call.
physx::PxControllerFilters g_physXControllerCollisionFilter(0, 0, &g_morphemePhysX3QueryFilterCallback);
//-----------

  
//----------------------------------------------------------------------------------------------------------------------
/// \class ControllerHitReport
/// \brief Controller hit report for PhysX3.
///
/// This class is used to apply forces to bodies that the character controller
/// comes into contact with. The force is ramped up depending on how long the body has been in contact with the
/// character controller.
//----------------------------------------------------------------------------------------------------------------------
class ControllerHitReport : public physx::PxUserControllerHitReport
{
public:
  ControllerHitReport(float maxTouchTime, TouchTimes& touchTimes);

  void updatePreMove(float dt, float maxPushForce);

  void onShapeHit(const physx::PxControllerShapeHit& hit) NM_OVERRIDE;
  void onControllerHit(const physx::PxControllersHit& hPit) NM_OVERRIDE;
  void onObstacleHit(const physx::PxControllerObstacleHit& hit) NM_OVERRIDE;

  float      m_maxPushForce;  ///< The maximum push force which can be applied by the character controller.
  float      m_dt;            ///< The time step since the last character controller update.
  float      m_maxTouchTime;  ///< The amount of time the controller will touch something before it ramps up to full push force.
  TouchTimes m_touchTimes;    ///< A map of Shape pointers to touch time (in seconds). 
};

//----------------------------------------------------------------------------------------------------------------------
// CharacterControllerPhysX functions
//----------------------------------------------------------------------------------------------------------------------
CharacterControllerPhysX3::CharacterControllerPhysX3() :
  m_positionWorld(NMP::Vector3::InitZero),
  m_orientationWorld(NMP::Quat::kIdentity),
  m_orientationWorldPrevFrame(NMP::Quat::kIdentity),
  m_actualDeltaWorldTranslation(NMP::Vector3::InitZero),
  m_actualDeltaWorldOrientation(NMP::Quat::kIdentity),
  m_physXController(NULL),
  m_physXDefaultControllerMaterial(NULL),
  m_physXRigidDynamicActor(NULL),
  m_physXDynamicJoint(NULL),
  m_gravityEnabled(false),
  m_physXControllerEnabled(false),
  m_onGround(false),
  m_onGroundPrevFrame (false),
  m_verticalStepVector(NMP::Vector3::InitZero),
  m_velocityInGravityDirection(NMP::Vector3::InitZero),
  m_originOffset(NMP::Vector3::InitZero),
  m_hitReport(NULL)
{
  // Instances are built through use of create and init. 
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerPhysX3::~CharacterControllerPhysX3()
{
  // Make sure tidy has been called.
  NMP_ASSERT(!m_physXController && !m_physXRigidDynamicActor && !m_physXDynamicJoint && !m_physXDefaultControllerMaterial);
}

//----------------------------------------------------------------------------------------------------------------------
CharacterControllerPhysX3* CharacterControllerPhysX3::create(
  MR::Network*                net,
  const NMP::Vector3&         initialPosition,
  const NMP::Quat&            initialOrientation,
  MR::PhysicsScenePhysX3*     physicsScene,
  physx::PxControllerManager* controllerManager,
  physx::PxMaterial*          characterControllerMaterial,
  MR::PhysicsRigPhysX3::Type  physicsRigType)
{
  CharacterControllerPhysX3* const instance = static_cast<CharacterControllerPhysX3*>(NMPMemoryAlloc(sizeof(CharacterControllerPhysX3)));
  new(instance) CharacterControllerPhysX3();

  instance->init(
              net,
              initialPosition,
              initialOrientation,
              physicsScene,
              controllerManager,
              characterControllerMaterial,
              physicsRigType);

  return instance;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::destroy(CharacterControllerPhysX3* characterController)
{
  NMP_ASSERT(characterController);

  characterController->term();
  NMP::Memory::memFree(characterController);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::init(
  MR::Network*                net,
  const NMP::Vector3&         initialPosition,
  const NMP::Quat&            initialOrientation,
  MR::PhysicsScenePhysX3*     physicsScene,
  physx::PxControllerManager* controllerManager,
  physx::PxMaterial*          characterControllerMaterial,
  MR::PhysicsRigPhysX3::Type  physicsRigType)
{  
  NMP_ASSERT(net && controllerManager);
  m_net = net;
  m_physXControllerManager = controllerManager;
  
  m_gravityEnabled = true;  // We are making a character with a physics system character controller, so assume gravity is on.
    
  // Store a pointer to the current character controller values from the network def data.
  m_activeControllerDef = net->getActiveCharacterControllerDef();

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
  // Zero deltas and velocities.
  m_actualDeltaWorldTranslation.setToZero();
  m_actualDeltaWorldOrientation.identity();
  m_requestedDeltaWorldTranslation.setToZero();
  m_requestedDeltaWorldOrientation.identity();
  m_velocityInGravityDirection.setToZero();
    
  
  // Create the physics rig.
  createPhysicsRig(net, physicsScene, physicsRigType, m_positionWorld);
    
  // Recalculate the offset from the origin of the character controller.
  m_originOffset = calculateOriginOffset(worldUpDirection);
    
  // Now set up the physX character controller object from the contents of the controller definition.
  createPhysXControllerObjects(physicsScene, characterControllerMaterial);

  // Tell the Network instance that this is the character controller that it should be communicating with.
  m_net->setCharacterController(this);

  // Set the networks position for the character controller.
  m_net->setCharacterPropertiesWorldRootTransform(NMP::Matrix34(m_orientationWorld, m_positionWorld));

  //----------------------------
  // Tell the physics rig about the character controller capsule that it is working with.
  MR::PhysicsRigPhysX3* physicsRig = (MR::PhysicsRigPhysX3*) getPhysicsRig(net);
  if (physicsRig)
  {
    NMP_ASSERT(m_physXController);
    physicsRig->setCharacterControllerActor(m_physXController->getActor()->isRigidDynamic());
  }
  
  //----------------------------
  // This character controller code only currently supports using separate character controller and physics updates.
  // (This method gives the most reliable placement of physics bodies prior to the physics simulation.)
  m_net->setCharacterPropertiesPhysicsAndCharacterControllerUpdate(MR::PHYSICS_AND_CC_UPDATE_SEPARATE);
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::term()
{
  // Destroy any PhysX character controller that we have created.
  destroyPhysXControllerObjects();

  // Wipe our reference to the PhysX controller manager.
  m_physXControllerManager = NULL;
  
  // Remove and delete the physics rig.
  destroyPhysicsRig(m_net);
  
  // Tidy the hit report structure.
  if (m_hitReport)
  {
    NMP::Memory::memFree(m_hitReport);
    m_hitReport = NULL;
  }

  // Wipe our reference to the global PhysX3 scene.
  setPhysicsScene(NULL);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::createPhysicsRig(
  MR::Network*               net,
  MR::PhysicsScenePhysX3*    physicsScene,
  MR::PhysicsRigPhysX3::Type physicsRigType,
  const NMP::Vector3&        initialPosition)
{
  NMP_ASSERT(net);

  MR::AnimRigDef* animRigDef = net->getActiveRig();
  MR::PhysicsRigDef* physicsRigDef = getActivePhysicsRigDef(net);

  if (physicsRigDef != NULL)
  {
    if (physicsRigType == MR::PhysicsRigPhysX3::TYPE_ARTICULATED)
    {
      // Build an articulated physics rig.
      NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(
                          MR::PhysicsRigPhysX3Articulation::getMemoryRequirements(physicsRigDef));
      NMP_ASSERT(resource.ptr);
      MR::PhysicsRigPhysX3Articulation* physicsRig = MR::PhysicsRigPhysX3Articulation::init(
                          resource,
                          physicsRigDef,
                          physicsScene,
                          physx::PX_DEFAULT_CLIENT,
                          15, // all client behaviour bits.
                          animRigDef,
                          getAnimToPhysicsMap(net->getNetworkDef(), net->getActiveAnimSetIndex()),
                          1 << MR::GROUP_CHARACTER_PART,
                          (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE) | (1 << MR::GROUP_INTERACTION_PROXY));
      physicsRig->setKinematicPos(initialPosition);
      setPhysicsRig(net, physicsRig);
    }
    else
    {
      // Build a jointed physics rig.
      NMP::Memory::Resource resource = NMPMemoryAllocateFromFormat(
                          MR::PhysicsRigPhysX3Jointed::getMemoryRequirements(physicsRigDef)); 
      NMP_ASSERT(resource.ptr);
      MR::PhysicsRigPhysX3Jointed* physicsRig = MR::PhysicsRigPhysX3Jointed::init(
                          resource,
                          physicsRigDef,
                          physicsScene,
                          physx::PX_DEFAULT_CLIENT,
                          15, // all client behaviour bits.
                          animRigDef,
                          getAnimToPhysicsMap(net->getNetworkDef(), net->getActiveAnimSetIndex()),
                          1 << MR::GROUP_CHARACTER_PART,
                          (1 << MR::GROUP_CHARACTER_CONTROLLER) | (1 << MR::GROUP_NON_COLLIDABLE) | (1 << MR::GROUP_INTERACTION_PROXY));
      physicsRig->setKinematicPos(initialPosition);
      setPhysicsRig(net, physicsRig);
    }
  }
  else
  { 
    // No physics in the network but we will still run a physical character controller.
    setPhysicsRig(net, NULL);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::destroyPhysicsRig(MR::Network *net)
{
  // Destroy any created physics rig.
  NMP_ASSERT(net);
  MR::PhysicsRig* physicsRig = getPhysicsRig(net);
  if (physicsRig)
  {
    physicsRig->term();
    NMP::Memory::memFree(physicsRig);
    setPhysicsRig(m_net, NULL);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::createPhysXControllerObjects(
  MR::PhysicsScenePhysX3* physicsScene,
  physx::PxMaterial*      controllerMaterial)
{
  m_physXControllerEnabled = true;

  //--------------------------------
  // Create the kinematic PxController object.
  //--------------------------------
  NMP_ASSERT(physicsScene && physicsScene->getPhysXScene());

  // Check a PxController hasn't already been created for this Physics Character Controller.
  NMP_ASSERT(!m_physXController);
    
  // Initialise a PhysX controller description structure.
  physx::PxControllerDesc* desc = NULL;

  // Collision shape specific initialisation of the character controller.
  physx::PxCapsuleControllerDesc capsuleDesc;
  physx::PxBoxControllerDesc boxDesc;
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
      boxDesc.halfSideExtent    = m_activeControllerDef->getWidth() * 0.5f;
      boxDesc.halfHeight        = m_activeControllerDef->getHeight() * 0.5f;
      boxDesc.halfForwardExtent = m_activeControllerDef->getDepth() * 0.5f;
      desc = &boxDesc;
      break;
    }
    default:
      NMP_ASSERT_FAIL_MSG("An unsupported CharacterController type has been selected for PhysX!");
  }
  NMP_ASSERT(desc);


  // Now set up the physX character controller object from the contents of the ccmRecord.
  desc->interactionMode = physx::PxCCTInteractionMode::eUSE_FILTER;

  // Set the up direction specified in the physics scene.
  NMP_ASSERT(physicsScene);
  NMP::Vector3 worldUpDirection = physicsScene->getWorldUpDirection();
  desc->upDirection = MR::nmVector3ToPxVec3(worldUpDirection);

  // Set the material to use for the controller object.
  if (controllerMaterial)
  {
    // Use the material provided.
    desc->material = controllerMaterial;
    m_physXDefaultControllerMaterial = NULL;
  }
  else
  {
    // We haven't been provided with a material to use so lets create a default one of our own.
    m_physXDefaultControllerMaterial = physicsScene->getPhysXScene()->getPhysics().createMaterial(0.0f, 0.0f, 0.0f);
    NMP_ASSERT(m_physXDefaultControllerMaterial);
    m_physXDefaultControllerMaterial->setFrictionCombineMode(g_defaultCCMaterialFrictionCombineMode);
    m_physXDefaultControllerMaterial->setRestitutionCombineMode(g_defaultCCMaterialRestitutionCombineMode);
    desc->material = m_physXDefaultControllerMaterial;
    controllerMaterial = m_physXDefaultControllerMaterial;
  }

  // Set slope limits.
  float maxSlopeAngleDegrees = m_activeControllerDef->getMaxSlopeAngle();
  float maxSlopeAngleRadians = NMP::degreesToRadians(maxSlopeAngleDegrees);
  float cosMaxSlopeAngle = cosf(maxSlopeAngleRadians);
  desc->slopeLimit = NMP::clampValue(cosMaxSlopeAngle, 0.0f, NM_PI / 2);
#ifdef NMP_ENABLE_ASSERTS
  if (desc->slopeLimit != cosMaxSlopeAngle)
  {
    NMP_DEBUG_MSG("WARNING: The max slope angle value of the character controller has been clamped.");
  }
#endif

  desc->contactOffset = m_activeControllerDef->getSkinWidth();
  desc->stepOffset = m_activeControllerDef->getStepHeight();
    
  // Create and initialise the hit report structure.
  m_hitReport = static_cast<ControllerHitReport*>(NMPMemoryAlloc(sizeof(ControllerHitReport)));
  new(m_hitReport) ControllerHitReport(g_maxForceTime, m_touchTimes);
  m_hitReport->m_maxPushForce = m_activeControllerDef->getMaxPushForce();
  desc->callback = m_hitReport;
    
  // Note currently using controller behaviour callback.
  //  (Behaviour callbacks are used to allow customization of the the controller's behaviour w.r.t. touched shapes.)
  desc->behaviorCallback = NULL;
  
  // Set the starting position for the physical controller object.
  NMP::Vector3 controllerPos = m_positionWorld + computeWorldSpaceCCOriginOffset();
  desc->position = MR::nmVector3ToPxExtendedVec3(controllerPos);
  
  // Create the physical controller object using the description.
  NMP_ASSERT(m_physXControllerManager && physicsScene->getPhysXScene());
  m_physXController = m_physXControllerManager->createController(
                                                physicsScene->getPhysXScene()->getPhysics(),
                                                physicsScene->getPhysXScene(),
                                                *desc);
  NMP_ASSERT(m_physXController);

  //--------------------------------
  // Set the filter data on the collision shape so that it collides with the environment,
  //  but not with character rig bodies etc.
  //--------------------------------

  physx::PxShape *controllerShape = NULL;
  m_physXController->getActor()->getShapes(&controllerShape, 1);
  NMP_ASSERT(controllerShape);
  physx::PxFilterData data;
  data.word0 = 1 << MR::GROUP_CHARACTER_CONTROLLER; // What it is
  data.word1 = 1 << MR::GROUP_CHARACTER_PART | 1 << MR::GROUP_NON_COLLIDABLE; // What it doesn't collide with
  // PhysX specific filter data (see physx::PxCCTInteractionMode::eUSE_FILTER) Corresponds to
  // activeGroups = 2 (i.e. 1 << word3). activeGroups = 1 is reserved for non-colliding CCs
  data.word3 = 1; 
  controllerShape->setSimulationFilterData(data);
  controllerShape->setQueryFilterData(data);


  //--------------------------------
  // Create a dynamic PxRigidDynamic object and attach it to the kinematic one.
  //--------------------------------

  NMP::Matrix34 initialTransform(NMP::Matrix34(m_orientationWorld, controllerPos));

#ifdef USE_CC_DYNAMIC_ACTOR
  m_physXRigidDynamicActor = PxGetPhysics().createRigidDynamic(
                             MR::nmMatrix34ToPxTransform(initialTransform));
  m_physXRigidDynamicActor->setSolverIterationCounts(4, 4);
  m_physXRigidDynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
  m_physXRigidDynamicActor->setLinearDamping(1.0f);
  

  physx::PxShape* dynamicShape = NULL; 
  switch (m_activeControllerDef->getShape())
  {
    case MR::CharacterControllerDef::CapsuleShape:
    {
      physx::PxCapsuleGeometry capsuleGeom(
                                m_activeControllerDef->getRadius() * g_dynamicControllerRadiusScale, 
                                m_activeControllerDef->getHeight() * 0.5f);
      dynamicShape = m_physXRigidDynamicActor->createShape(
                                capsuleGeom,
                                *controllerMaterial);
      break;
    }
    case MR::CharacterControllerDef::BoxShape:
    {
      physx::PxBoxGeometry boxGeom(
                                m_activeControllerDef->getWidth() * 0.5f,
                                m_activeControllerDef->getHeight() * 0.5f,
                                m_activeControllerDef->getDepth() * 0.5f);
      dynamicShape = m_physXRigidDynamicActor->createShape(
                                boxGeom,
                                *controllerMaterial);
      break;
    }
    default:
      NMP_ASSERT_FAIL_MSG("An unsupported CharacterController type has been selected for PhysX!");
  }

  dynamicShape->setName("Character controller dynamic object");
  dynamicShape->setContactOffset(m_activeControllerDef->getSkinWidth());
  dynamicShape->setLocalPose(controllerShape->getLocalPose());
  // Set up filter data .
  physx::PxFilterData queryData;
  queryData.word0 =  1 << MR::GROUP_COLLIDABLE_PUSHABLE | 1 << MR::GROUP_CHARACTER_CONTROLLER;
  queryData.word1 =  1 << MR::GROUP_CHARACTER_CONTROLLER; 

  dynamicShape->setSimulationFilterData(queryData);
  dynamicShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);

  // Give the dynamic actor a small mass - most objects should be within a factor of 1000 or so of
  // the physx tolerance scale, so choose a value at the bottom of that range.
  const physx::PxTolerancesScale& tolerancesScale = PxGetPhysics().getTolerancesScale();
  float mass = tolerancesScale.mass * 0.001f;
  physx::PxRigidBodyExt::setMassAndUpdateInertia(*m_physXRigidDynamicActor, mass);

  physicsScene->getPhysXScene()->addActor(*m_physXRigidDynamicActor);

  m_physXRigidDynamicActor->setGlobalPose(m_physXController->getActor()->getGlobalPose());

  // Create the joint to constrain it to the kinematic shape.
  m_physXDynamicJoint = PxD6JointCreate(
                            PxGetPhysics(),
                            m_physXController->getActor(),
                            physx::PxTransform::createIdentity(),
                            m_physXRigidDynamicActor, 
                            physx::PxTransform::createIdentity());

  // Don't constrain the position, since it will just get reset to the KCC position (assuming no
  // penetration) every update anyway. An alternative to this would be to use a weak drive.
  m_physXDynamicJoint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eFREE);
  m_physXDynamicJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
  m_physXDynamicJoint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eFREE);
#else
  m_physXRigidDynamicActor = NULL;
  m_physXDynamicJoint = NULL;
#endif
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::destroyPhysXControllerObjects()
{
  if (m_physXDefaultControllerMaterial)
  {
    m_physXDefaultControllerMaterial->release();
    m_physXDefaultControllerMaterial = NULL;
  }

  if (m_physXController)
  {
    // Kinematic controller object.
    m_physXController->release();
    m_physXController = NULL;
  }

  if (m_physXRigidDynamicActor)
  {
    // Dynamic controller object.
    NMP_ASSERT(m_physXDynamicJoint);
    m_physXDynamicJoint->release();
    m_physXDynamicJoint = NULL;
    m_physXRigidDynamicActor->release();
    m_physXRigidDynamicActor = NULL;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 CharacterControllerPhysX3::calculateOriginOffset(
  const NMP::Vector3& worldUpDirection)
{
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
NMP::Vector3 CharacterControllerPhysX3::computeWorldSpaceCCOriginOffset()
{
  // Rotate the relative offset in to the characters world space alignment.
  NMP::Vector3 worldSpaceCCOriginOffset;
  m_orientationWorld.rotateVector(m_originOffset, worldSpaceCCOriginOffset);

  return worldSpaceCCOriginOffset;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::resolveKinematicActorPenetration(MR::PhysicsRigPhysX3* physicsRig)
{
  NMP_ASSERT(m_physXControllerEnabled);
  NMP_ASSERT(m_physXController->getType() == physx::PxControllerShapeType::eCAPSULE); // We only support capsules at the moment.
  physx::PxCapsuleController* capsuleController = (physx::PxCapsuleController*) m_physXController;
  physx::PxTransform globalPose = capsuleController->getActor()->getGlobalPose(); // Get actors world space transform; 
                                                                                  //  assumes the local pose is identity.

  // Try to resolve any penetration of the kinematic Actor with moving dynamic objects (e.g. standing on moving platforms):
  //  Query PhysX to check if the kinematic character controllers Actor interpenetrates the environment at all,
  //  if it does: move the kinematic Actor to the dynamic Actor position in order to attempt to resolve the collision,
  //  if it does not: move the dynamic Actor to the kinematic Actor position so that it tracks the character movement.
  if (m_physXRigidDynamicActor)
  {
    // If the kinematic shape penetrates the environment then extract it (if possible)
    physx::PxScene* physXScene = getPhysicsScene()->getPhysXScene();

    // We check for overlap with dynamic objects even if the controller shape eSIMULATION_SHAPE is
    // cleared - this is because we can still stand on dynamic objects. It does mean that dynamic
    // objects will collide with the dynamic actor and get pushed slightly, but we can't prevent
    // that.
    physx::PxSceneQueryFilterFlags queryFilterFlags = 
                      physx::PxSceneQueryFilterFlag::eSTATIC |
                      physx::PxSceneQueryFilterFlag::eDYNAMIC |
                      physx::PxSceneQueryFilterFlag::ePREFILTER;

    // Use default PxFilterData here to bypass the internal filtering, and we pass the real flags
    // into the callback.
    physx::PxSceneQueryFilterData sceneQueryFilterData(physx::PxFilterData(), queryFilterFlags);
    physx::PxShape* hit;
    
    
    physx::PxCapsuleGeometry geom;
    geom.radius = capsuleController->getRadius();
    geom.halfHeight = capsuleController->getHeight() * 0.5f;
    physx::PxClientID clientID = physicsRig ? physicsRig->getClientID() : physx::PX_DEFAULT_CLIENT;
    if (physXScene->overlapAny(
                      geom,
                      globalPose,
                      hit,
                      sceneQueryFilterData, 
                      &g_morphemePhysX3QueryFilterCallback,
                      clientID))
    {
      // If the kinematic actor is penetrating, then move it to the dynamic actor position
      setControllerPosition(
                      m_physXController, 
                      0, 
                      MR::nmPxVec3ToVector3(m_physXRigidDynamicActor->getGlobalPose().p));
    }
    else
    {
      // If it's not penetrating then move the dynamic actor there (in case it got left behind somehow)
      m_physXRigidDynamicActor->setGlobalPose(m_physXController->getActor()->getGlobalPose());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::initRequestedWorldDeltaTransform(
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
void CharacterControllerPhysX3::addMovementOfStoodOnObject(float deltaTime)
{
  // Ray cast down to find the ground (if we are in contact).
  NMP_ASSERT(getPhysicsScene());
  NMP::Vector3 worldUpDirection = getPhysicsScene()->getWorldUpDirection();
  NMP::Vector3 rayStart = MR::nmPxExtendedVec3ToVector3(m_physXController->getPosition());
  // Go a little bit beyond the bottom of the controller in case we're on a slope, 
  // or if the ground is jittering a bit. The exact amount doesn't matter too much.
  NMP::Vector3 rayDelta = -m_originOffset * 1.2f;
  NMP::Vector3 hitVelocity; // The velocity of any point on an object that we intersect:
                            //  For example, if we ray cast on to a moving platform the velocity of the intersected 
                            //  point on the platform will be returned.
  NMP::Vector3 hitPosition, hitNormal; // not actually used.
  float hitDist;
  if (m_net->getCharacterController()->castRayIntoCollisionWorld(
      rayStart,
      rayDelta,
      hitDist,
      hitPosition,
      hitNormal,
      hitVelocity,
      m_net))
  {
    // If the ground surface is going down (away from us), don't apply an additional downward motion to the CC.
    if (hitVelocity.dot(worldUpDirection) < 0.0f)
      hitVelocity = hitVelocity.getComponentOrthogonalToDir(worldUpDirection);
    m_requestedDeltaWorldTranslation += hitVelocity * deltaTime;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::calculateRequestedHorizontalDeltaTranslation()
{
  NMP_ASSERT(getPhysicsScene());
  NMP::Vector3 worldUpDirection = getPhysicsScene()->getWorldUpDirection();
  float amountInUpDirection = worldUpDirection.dot(m_requestedDeltaWorldTranslation);
  NMP::Vector3 vectorInUpDirection = worldUpDirection * amountInUpDirection;
  m_requestedHorizontalDeltaTranslation = m_requestedDeltaWorldTranslation - vectorInUpDirection;
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::addAccelerationDueToGravity(float deltaTime)
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
void CharacterControllerPhysX3::addStickToGroundMovement()
{
#ifdef STICK_TO_GROUND
  if (m_onGround)
  {
    // Move character down by the maximum slope amount over the distance moved horizontally.
    float requestedDeltaHorizontalDistance = m_requestedHorizontalDeltaTranslation.magnitude();
    NMP_ASSERT(getPhysicsScene());
    NMP::Vector3 worldUpDirection = getPhysicsScene()->getWorldUpDirection();
    m_verticalStepVector =
        worldUpDirection * (-requestedDeltaHorizontalDistance * tanf(NMP::degreesToRadians(m_activeControllerDef->getMaxSlopeAngle())));
    m_requestedDeltaWorldTranslation += m_verticalStepVector;
  }
#endif // STICK_TO_GROUND
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::updateGroundContactState(
  float        deltaTime,
  physx::PxU32 controllerCollisionFlags)
{
  // Determine whether character is on the ground or in the air, and whether it has
  // just transitioned between the two.
  m_onGroundPrevFrame = m_onGround;
  m_onGround = (controllerCollisionFlags & physx::PxControllerFlag::eCOLLISION_DOWN) != 0 ;

  // Update how long the character has been in ground contact.
  if (m_onGround && m_onGroundPrevFrame)
    m_net->setCharacterPropertiesGroundContactTime(m_net->getCharacterPropertiesGroundContactTime() + deltaTime); // on ground
  else if (!m_onGround && !m_onGroundPrevFrame)
    m_net->setCharacterPropertiesGroundContactTime(m_net->getCharacterPropertiesGroundContactTime() - deltaTime); // in air
  else
    m_net->setCharacterPropertiesGroundContactTime(0.0f); // transition from/to ground to/from air
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::updateControllerMove(
  float               deltaTime,
  const NMP::Vector3* requestedDeltaWorldTranslation,
  const NMP::Quat*    requestedDeltaWorldOrientation)
{
  NMP_ASSERT(m_net && m_physXController && m_activeControllerDef);
  
  //----------------------------
  // Update the last frames transform.
  m_orientationWorldPrevFrame = m_orientationWorld;
  m_positionWorldPrevFrame = m_positionWorld;
  
  //-------------------
  MR::PhysicsRigPhysX3* physicsRig = (MR::PhysicsRigPhysX3*) getPhysicsRig(m_net);
  if (physicsRig && physicsRig->isReferenced() && (m_net->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS))
  {
    //----------------------------
    // The character root position is being controlled by the position of the physics rig.

    // Disable the physics controller object.
    enablePhysXController(false);

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
    enablePhysXController(true);

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

    // Try to resolve any penetration of the kinematic Actor with moving dynamic objects (e.g. standing on moving platforms).
    resolveKinematicActorPenetration(physicsRig);
   
    // Add the movement of any object that we are stood on to this frames requested delta movement of our character.
    // This is so we stay in contact with things like moving platforms.
    addMovementOfStoodOnObject(deltaTime);

    // Calculate what component of the requested delta translation is horizontal.
    calculateRequestedHorizontalDeltaTranslation();
    
    // If the controller was in ground contact last frame try and keep it in contact this frame by adding
    //  a vertical step down to this frames requested delta movement of our character.
    addStickToGroundMovement();

    // Actually move the controller object by executing physX's 'collide and slide' controller movement function.
    m_hitReport->updatePreMove(deltaTime, m_activeControllerDef->getMaxPushForce());
    physx::PxU32 controllerCollisionFlags = m_physXController->move(
                                                MR::nmVector3ToPxVec3(m_requestedDeltaWorldTranslation),
                                                0.00000001f,
                                                deltaTime,
                                                g_physXControllerCollisionFilter);

    // Determine whether character is on the ground or in the air, and whether it has just transitioned between the two.
    //  Also update how long the character has been in ground contact.
    updateGroundContactState(deltaTime, controllerCollisionFlags);
  }
}
  
//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::updateControllerMoveAdjustingStepping(float deltaTime)
{
#ifdef STICK_TO_GROUND
  if (m_physXControllerEnabled && m_onGroundPrevFrame && !m_onGround)
  {
    // We were on the ground, but are no longer on it,
    //  undo the extra vertical movement that we added to keep our character in ground contact (when going down hill).
    m_physXController->move(
                        -MR::nmVector3ToPxVec3(m_verticalStepVector),
                        0.00000001f,  // The minimum traveled distance to consider; smaller distances create no movement.
                        deltaTime,
                        g_physXControllerCollisionFilter);
  }
#endif // STICK_TO_GROUND
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::updateCachedTransformsFromCharacterControllerPosition()
{
  // The PxController::move(desired) function above has changed the position of the PxController according to the
  //  desired change from animation filtered through the NxController's semi-physical collision detection.
  // So we now need to update our cached world transform to reflect this new value,
  //  and also set the root position for the network's animation data.
  if (m_physXControllerEnabled)
  {
    // Get and store the updated position of the PhysX Controller.
    physx::PxExtendedVec3 newWorldPos = m_physXController->getPosition();
    m_positionWorld.set((float) newWorldPos.x, (float) newWorldPos.y, (float) newWorldPos.z);

    // Take off the offset value to translate the character position down from the center of
    //  the controller body to where the feet should be.
    m_positionWorld -= computeWorldSpaceCCOriginOffset();

    // Determine whether the character achieved the requested delta movement this frame.
    didControllerAchieveRequestedMovement();
           
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
bool CharacterControllerPhysX3::didControllerAchieveRequestedMovement()
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
void CharacterControllerPhysX3::updateCachedTransformsFromPhysicsRigPosition(float deltaTime)
{
  if (m_net->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
  {
    NET_LOG_MESSAGE(99, "NetworkInstanceRecord::updatePostPhysics: Updating trajectory");
    // When the characters root position is being controlled through the physical simulation of its rigid bodies
    //  (Rag doll for example) we need to update the character controllers position from this value after the
    //  simulation step. i.e. Make the character root position follow the ragdoll.
    //  This delta transform is calculated after simulation, in updatePostPhysics, and stored on the Network.
    //  The networks suggested delta translation is in the characters local space 
    //  (relative to the characters current world orientation).
    //  So to get the suggested delta translation in to world space rotate it by the characters current world orientation.
    m_actualDeltaWorldOrientation = m_net->getOrientationChange();
    m_actualDeltaWorldTranslation = m_orientationWorld.rotateVector(m_net->getTranslationChange());

    // Apply delta orientation and translation change.
    m_orientationWorldPrevFrame = m_orientationWorld;
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
// Note that all the different shapes may have different collision and query status.
void CharacterControllerPhysX3::enablePhysXController(bool enable)
{
  if (m_physXController && (m_physXControllerEnabled != enable))
  {
    if (enable)
    {
      // We are turning on the controller so teleport the character controller physX objects to the
      // correct position.
      NMP::Vector3 physXControllerPosition = m_positionWorld + computeWorldSpaceCCOriginOffset();
      setControllerPosition(m_physXController, m_physXRigidDynamicActor, physXControllerPosition);
    }

    // Set flags on the controller actor shapes.
    physx::PxRigidDynamic *controllerActor = m_physXController->getActor();
    NMP_ASSERT(controllerActor);
    physx::PxShape *controllerShape = 0;
    controllerActor->getShapes(&controllerShape, 1);

    physx::PxShapeFlags flags = controllerShape->getFlags(); 
    bool simEnabled = flags & physx::PxShapeFlag::eSIMULATION_SHAPE;
    if (simEnabled != enable)
    {
      controllerShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, enable);
    }
    bool queryEnabled = flags & physx::PxShapeFlag::eSCENE_QUERY_SHAPE;
    if (queryEnabled != enable)
    {
      controllerShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, enable);
    }
    // CC movement uses its own filter system for collision with other CCs that is based on a bitmask
    //  between word3<<1 and the activeGroups. The filter function passed into move only applies to
    //  collision with other objects.
    physx::PxFilterData filterData = controllerShape->getSimulationFilterData();
    filterData.word3 = enable ? 1 : 0;
    controllerShape->setSimulationFilterData(filterData);

    // Set flags on the dynamic actor shapes (which doesn't engage in queries).
    physx::PxShape *dynamicActorShape = 0;
    m_physXRigidDynamicActor->getShapes(&dynamicActorShape, 1);

    flags = dynamicActorShape->getFlags(); 
    bool dynamicActorEnabled = flags & physx::PxShapeFlag::eSIMULATION_SHAPE;
    if (dynamicActorEnabled != enable)
    {
      dynamicActorShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, enable);
    }

    m_physXControllerEnabled = enable;
  }
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::setPosition(NMP::Vector3& position)
{
  setTransform(position, m_orientationWorld);
}

//---------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::setOrientation(NMP::Quat& orientation)
{
  setTransform(m_positionWorld, orientation);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::setTransform(
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

  // Set the position of the kinematic object.
  NMP::Vector3 physXControllerPosition = m_positionWorld + computeWorldSpaceCCOriginOffset();
  setControllerPosition(m_physXController, m_physXRigidDynamicActor, physXControllerPosition);
}

//----------------------------------------------------------------------------------------------------------------------
void CharacterControllerPhysX3::setControllerPosition(
  physx::PxController*   controller, 
  physx::PxRigidDynamic* dynamicActor,
  const NMP::Vector3&    position) // The true position of the controller object (usually at its center)
{
  controller->setPosition(MR::nmVector3ToPxExtendedVec3(position));

  // Note that setPosition doesn't actually set the actor position. Set it explicitly, because it's
  // going to get read and used. It is also possible that if this isn't done then it is subsequently
  // swept to the new position (should check this).
  physx::PxRigidDynamic* controllerActor = controller->getActor();
  physx::PxTransform t = controllerActor->getGlobalPose();
  t.p = MR::nmVector3ToPxVec3(position);
  controllerActor->setGlobalPose(t);

  if (dynamicActor)
  {
    dynamicActor->setGlobalPose(t);
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
  for (TouchTimes::iterator it = m_touchTimes.begin(); it != m_touchTimes.end(); ++it)
  {
    it->second -= dt;
    if (it->second < 0.0f)
      it->second = 0.0f;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ControllerHitReport::onShapeHit(const physx::PxControllerShapeHit& hit)
{
  if (hit.shape)
  {
    physx::PxActor& actor = hit.shape->getActor();
    if (actor.isRigidDynamic())
    {
      float& touchTime = m_touchTimes[&actor];
      touchTime += 2.0f * m_dt; // twice since it will have been decremented if it's persistant
      if (touchTime > m_maxTouchTime)
        touchTime = m_maxTouchTime;
      physx::PxF32 coeff = m_maxPushForce * touchTime / m_maxTouchTime;
      physx::PxRigidBodyExt::addForceAtPos(
                          *actor.isRigidDynamic(),
                          hit.dir * coeff,
                          physx::PxVec3((float) hit.worldPos.x, (float) hit.worldPos.y, (float) hit.worldPos.z),
                          physx::PxForceMode::eFORCE);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ControllerHitReport::onControllerHit(const physx::PxControllersHit& NMP_UNUSED(hit))
{
}

//----------------------------------------------------------------------------------------------------------------------
void ControllerHitReport::onObstacleHit(const physx::PxControllerObstacleHit& NMP_UNUSED(hit))
{
}

} // namespace Game
//----------------------------------------------------------------------------------------------------------------------
