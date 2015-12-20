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
#include "defaultControllerMgr.h"
#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMSocket.h"
#include "NMPlatform/NMVector3.h"

#include "comms/mcomms.h"
#include "comms/packet.h"
#include "comms/sceneObject.h"

#include "../../iPhysicsMgr.h"
#include "../../iControllerMgr.h"

#include "mrPhysX3.h"
#include "physics/mrPhysicsRigDef.h"
#include "mrPhysicsRigPhysX3.h"
#include "defaultPhysicsMgr.h"
#include "../../defaultAssetMgr.h"
#include "../../defaultDataManager.h"
#include "../../defaultSceneObjectMgr.h"
#include "../../networkDefRecordManager.h"
#include "../../networkInstanceRecordManager.h"
#include "../../sceneObjectRecordManager.h"
#include "mrPhysX3Includes.h"
#include "Physics/mrPhysics.h"
#if defined(NM_HOST_CELL_PPU)
#include "NMPlatform/ps3/NMSPUManager.h"
#include "morpheme/mrDispatcherPS3.h"
#endif // NM_HOST_CELL_PPU

#include <stdlib.h>
//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (push)
  #pragma warning (disable : 4127)
#endif
//----------------------------------------------------------------------------------------------------------------------

// If a dynamic actor should be used to push the CC out of penetration
#define USE_CC_DYNAMIC_ACTOR

// If the character movement should "hug" the ground at the top of slopes etc.
#define STICK_TO_GROUND

// Altitude of the graveyard position
#define PHYSICS_GRAVEYARD_ALTITUDE -1000.0f

// Time take for the controller force to reach maximum
static const float MAX_FORCE_TIME = 0.3f;

// Make the dynamic shape attached to the character controller slightly larger than the kinematic
//  one so if the KCC does have to be respawned then there is a safety margin.
static const float DYNAMIC_CONTROLLER_RADIUS_SCALE = 1.1f;

//----------------------------------------------------------------------------------------------------------------------
// Note that all the different shapes may have different collision and query status.
static void enableControllerCollision(
  physx::PxController*   controller, 
  physx::PxRigidDynamic* dynamicActor,
  bool                   enable)
{
  physx::PxRigidDynamic *controllerActor = controller->getActor();

  // Set flags on the controller actor shapes
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
  // between word3<<1 and the activeGroups. The filter function passed into move only applies to
  // collision with other objects.
  physx::PxFilterData filterData = controllerShape->getSimulationFilterData();
  filterData.word3 = enable ? 1 : 0;
  controllerShape->setSimulationFilterData(filterData);

  if (dynamicActor)
  {
    // Set flags on the dynamic actor shapes (which doesn't engage in queries)
    physx::PxShape *dynamicActorShape = 0;
    dynamicActor->getShapes(&dynamicActorShape, 1);

    flags = dynamicActorShape->getFlags(); 
    bool dynamicActorEnabled = flags & physx::PxShapeFlag::eSIMULATION_SHAPE;
    if (dynamicActorEnabled != enable)
    {
      dynamicActorShape->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, enable);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
static void setControllerPosition(
  physx::PxController*   controller, 
  physx::PxRigidDynamic* dynamicActor,
  const NMP::Vector3&    p)
{
  controller->setPosition(MR::nmVector3ToPxExtendedVec3(p));

  // Note that setPosition doesn't actually set the actor position. Set it explicitly, because it's
  // going to get read and used. It is also possible that if this isn't done then it is subsequently
  // swept to the new position (should check this).
  physx::PxRigidDynamic* controllerActor = controller->getActor();
  physx::PxTransform t = controllerActor->getGlobalPose();
  t.p = MR::nmVector3ToPxVec3(p);
  controllerActor->setGlobalPose(t);

  if (dynamicActor)
  {
    dynamicActor->setGlobalPose(t);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// CC Behaviour callback for physX
//----------------------------------------------------------------------------------------------------------------------
class ControllerBehaviorCallback : public physx::PxControllerBehaviorCallback
{
  physx::PxU32 getBehaviorFlags(const physx::PxShape& NMP_UNUSED(shape)) NM_OVERRIDE {return 0;}
  physx::PxU32 getBehaviorFlags(const physx::PxController& NMP_UNUSED(controller)) NM_OVERRIDE {return 0;}
  physx::PxU32 getBehaviorFlags(const physx::PxObstacle& NMP_UNUSED(obstacle)) NM_OVERRIDE {return 0;}
};
// Just need one of these for now, since we don't want any of the features
static ControllerBehaviorCallback s_defaultControllerBehaviorCallback;

//----------------------------------------------------------------------------------------------------------------------
// Controller hit report for physX
//----------------------------------------------------------------------------------------------------------------------
class ControllerHitReport : public physx::PxUserControllerHitReport
{
public:
  ControllerHitReport(float maxTouchTime, TouchTimes& touchTimes);

  void updatePreMove(float dt, float maxPushForce);

  void onShapeHit(const physx::PxControllerShapeHit& hit) NM_OVERRIDE;
  void onControllerHit(const physx::PxControllersHit& hPit) NM_OVERRIDE;
  void onObstacleHit(const physx::PxControllerObstacleHit& hit) NM_OVERRIDE;

  float      m_maxPushForce;
  float      m_dt;
  float      m_maxTouchTime;
  TouchTimes m_touchTimes;
};

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

//----------------------------------------------------------------------------------------------------------------------
ControllerRecord::ControllerRecord() :
  m_pxController(NULL),
  m_pxRigidDynamicActor(NULL),
  m_pxDynamicJoint(NULL),
  m_physicsRig(NULL),
  m_requestedMovement(NMP::Vector3(NMP::Vector3::InitZero)),
  m_characterPosition(NMP::Vector3(NMP::Vector3::InitZero)),
  m_characterOrientation(NMP::Quat::kIdentity),
  m_controllerEnabled(false),
  m_gravityEnabled(false),
  m_onGround(false),
  m_onGroundOld(false),
  m_extraVerticalMove(0.0f, 0.0f, 0.0f),
  m_originOffset(0.0f, 0.0f, 0.0f),
  m_connectRepresentation(NULL)
{
  m_hitReport = new ControllerHitReport(MAX_FORCE_TIME, m_touchTimes);
}

//----------------------------------------------------------------------------------------------------------------------
ControllerRecord::~ControllerRecord()
{
  delete m_hitReport;
}

//----------------------------------------------------------------------------------------------------------------------
// This Default character controller manager
//----------------------------------------------------------------------------------------------------------------------
DefaultControllerMgr::DefaultControllerMgr(DefaultPhysicsMgr* physicsManager, RuntimeTargetContext* context) :
  m_physicsManager(physicsManager),
  m_context(context),
  m_controllerMaterial(NULL)
{
  m_manager = PxCreateControllerManager(PxGetPhysics().getFoundation());

  // Create a default material
  physx::PxPhysics& physics = PxGetPhysics();
  m_controllerMaterial = physics.createMaterial(0.0f, 0.0f, 0.0f);
  m_controllerMaterial->setFrictionCombineMode(physx::PxCombineMode::eMULTIPLY);
  m_controllerMaterial->setRestitutionCombineMode(physx::PxCombineMode::eMULTIPLY);
  NMP_ASSERT(m_controllerMaterial);

  // initialize the graveyard
  m_graveSeed = 0;
}

//----------------------------------------------------------------------------------------------------------------------
DefaultControllerMgr::~DefaultControllerMgr()
{
  m_manager->release();
  m_controllerMaterial->release();
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::spaceGraveyard(NMP::Vector3& graveyardPos, float factor)
{
  float u = 0;
  float v = 0;

  uint32_t graveSeed = m_graveSeed;

  while(graveSeed)
  {
    u += factor*(graveSeed&1);
    graveSeed >>= 1;
    v += factor*(graveSeed&1);
    graveSeed >>= 1;
    factor *= 2;
  }

  graveyardPos.x += u;

  // Y-Up
  if (graveyardPos.y!=0)
  {    
    graveyardPos.z += v;
  }
  // Z-Up
  else
  {    
    graveyardPos.y += v;
  }

  m_graveSeed++;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DefaultControllerMgr::calculateOriginOffset(
  ControllerRecord*   ccRecord,
  const NMP::Vector3& worldUpDirection)
{
  NMP::Vector3 offset(NMP::Vector3::InitTypeOne);
  switch (ccRecord->m_controllerParams.getShape())
  {
    case MR::CharacterControllerDef::CapsuleShape:
    {
      offset = 
        worldUpDirection *
          ((ccRecord->m_controllerParams.getHeight() * 0.5f) + // Half hight of the cylinder that forms the central body of the capsule.
          ccRecord->m_controllerParams.getRadius() +           // Sphere on bottom of character capsule.
          ccRecord->m_controllerParams.getSkinWidth());        // Skin width around the whole character controller.
      break;
    }
    default:
      NMP_ASSERT_FAIL(); // Not a valid shape type
  }

  return offset;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DefaultControllerMgr::computeWorldSpaceCCOriginOffset(ControllerRecord* ccRecord)
{
  NMP::Vector3 worldSpaceCCOriginOffset;

  if (ccRecord->m_overrideBasics.m_overrideAbsPosition)
  {
    //--------------------------------
    // Offset to an absolute position:
    //  The position of the character controller is being overridden with an absolute value so
    //  calculate the required offset from the character in order to maintain this position.
    ccRecord->m_characterOrientation.rotateVector(ccRecord->m_originOffset, worldSpaceCCOriginOffset);
    worldSpaceCCOriginOffset += (ccRecord->m_overrideBasics.m_currentPositionAbs - ccRecord->m_characterPosition);
  }
  else
  {
    //--------------------------------
    // Offset to a relative position: the origin offset + specified local space offset.
    NMP::Vector3 localSpaceCCOriginOffset = ccRecord->m_originOffset + ccRecord->m_overrideBasics.m_currentPositionOffset;

    // Rotate this offset in to the characters world space alignment.
    ccRecord->m_characterOrientation.rotateVector(localSpaceCCOriginOffset, worldSpaceCCOriginOffset);
  }

  return worldSpaceCCOriginOffset;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::createControllerRecord(
  MCOMMS::InstanceID  instanceID,
  MR::Network*        const network,
  const NMP::Vector3& characterStartPosition,
  const NMP::Quat&    characterStartRotation)
{
  void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ControllerRecord), NMP_VECTOR_ALIGNMENT);
  ControllerRecord* ccRecord = new(alignedMemory)ControllerRecord();
  m_ccRecords[instanceID] = ccRecord;

  //--------------------------
  // Initialise character controller properties.
  ccRecord->m_overrideBasics.init();

  //--------------------------
  ccRecord->m_characterPosition = characterStartPosition;
  ccRecord->m_characterPositionOld = characterStartPosition;
  ccRecord->m_characterOrientation = characterStartRotation;
  ccRecord->m_characterOrientationOld = characterStartRotation;
  ccRecord->m_controllerEnabled = true;

  DefaultPhysicsMgr* physicsMgr = (DefaultPhysicsMgr*) m_physicsManager;
  NMP_ASSERT(physicsMgr && physicsMgr->getPhysicsScene());
  PhysXScene* scene = physicsMgr->getPhysicsScene()->getPhysXScene();

  // Get the world up direction.
  NMP::Vector3 worldUpDirection = physicsMgr->getPhysicsScene()->getWorldUpDirection();

  // Get the CharacterControllerDef from the Network
  MR::CharacterControllerDef* ccDef = network->getActiveCharacterControllerDef();

  // Nudge the start position up very slightly so that, in the typical case with the ground plane at
  // zero height etc, we don't start borderline intersecting the ground.
  ccRecord->m_characterPosition += worldUpDirection * 1e-5f;
  ccRecord->setPhysicsScene(physicsMgr->getPhysicsScene());
  ccRecord->m_network = network;
  ccRecord->m_velocityInGravityDirectionDt = 0.0f;
  ccRecord->m_deltaTranslation = NMP::Vector3::InitZero;
  ccRecord->m_deltaOrientation = NMP::Quat::kIdentity;
  ccRecord->m_velocityInGravityDirection.setToZero();

  // set the position of the physical body to the 'graveyard', 
  // PHYSICS_GRAVEYARD_ALTITUDE units times the size of the character below the origin.  
  NMP::Vector3 graveyardPos = worldUpDirection * PHYSICS_GRAVEYARD_ALTITUDE * ccDef->getHeight();
  spaceGraveyard(graveyardPos, 2.0f);
  ccRecord->m_graveyardPos = graveyardPos;
    
  // Create a physics rig for the network if it's current anim set needs one
  m_physicsManager->createPhysicsRig(ccRecord->m_network, &graveyardPos);

  // Store the physics rig locally as it may get added to/removed from the network
  ccRecord->m_physicsRig = getPhysicsRig(ccRecord->m_network);
  
  // Set the current character controller values from the def data (i.e. no CC scaling is applied at the moment)
  ccRecord->m_controllerParams = *ccDef;
  
  // Recalculate the offset from the origin of the character controller.
  ccRecord->m_originOffset = calculateOriginOffset(ccRecord, worldUpDirection);
 
  // Now set up the physX character controller object from the contents of the ccmRecord.
  physx::PxCapsuleControllerDesc desc;
  desc.interactionMode = physx::PxCCTInteractionMode::eUSE_FILTER;
  desc.radius = ccRecord->m_controllerParams.getRadius();
  desc.height = ccRecord->m_controllerParams.getHeight();

  // Apply the up direction specified in connect.
  desc.upDirection = MR::nmVector3ToPxVec3(worldUpDirection);
  desc.material = m_controllerMaterial;

  float maxSlopeAngleDegrees = ccRecord->m_controllerParams.getMaxSlopeAngle();
  float maxSlopeAngleRadians = NMP::degreesToRadians(maxSlopeAngleDegrees);
  float cosMaxSlopeAngle = cosf(maxSlopeAngleRadians);
  desc.slopeLimit = NMP::clampValue(cosMaxSlopeAngle, 0.0f, NM_PI_OVER_TWO);
#ifdef NMP_ENABLE_ASSERTS
  if (desc.slopeLimit != cosMaxSlopeAngle)
  {
    NMP_DEBUG_MSG("WARNING: The max slope angle value of the character controller has been clamped.");
  }
#endif
  desc.contactOffset = ccRecord->m_controllerParams.getSkinWidth();
  desc.stepOffset = ccRecord->m_controllerParams.getStepHeight();
  ccRecord->m_hitReport->m_maxPushForce = ccRecord->m_controllerParams.getMaxPushForce();
  desc.callback = ccRecord->m_hitReport;
  desc.behaviorCallback = &s_defaultControllerBehaviorCallback;

  NMP::Vector3 controllerPos = ccRecord->m_characterPosition + computeWorldSpaceCCOriginOffset(ccRecord);
  desc.position = MR::nmVector3ToPxExtendedVec3(controllerPos);
  ccRecord->m_pxController = m_manager->createController(PxGetPhysics(), scene, desc);

  NMP_ASSERT(ccRecord->m_pxController);

  // Set the filter data on the collision shape so it collides with the environment etc
  physx::PxShape *shape = NULL;
  ccRecord->m_pxController->getActor()->getShapes(&shape, 1);
  NMP_ASSERT(shape);
  physx::PxFilterData data;
  data.word0 = 1 << MR::GROUP_CHARACTER_CONTROLLER; // What it is
  data.word1 = 1 << MR::GROUP_CHARACTER_PART | 1 << MR::GROUP_NON_COLLIDABLE; // What it doesn't collide with
  // PhysX specific filter data (see physx::PxCCTInteractionMode::eUSE_FILTER) Corresponds to
  // activeGroups = 2 (i.e. 1 << word3). activeGroups = 1 is reserved for non-colliding CCs
  data.word3 = 1; 
  shape->setSimulationFilterData(data);
  shape->setQueryFilterData(data);

  NMP::Matrix34 initialTransform(
    NMP::Matrix34(ccRecord->m_characterOrientation, ccRecord->m_characterPosition));

#ifdef USE_CC_DYNAMIC_ACTOR
  // Create a dynamic object and attach it to the kinematic one
  ccRecord->m_pxRigidDynamicActor = PxGetPhysics().createRigidDynamic(
    MR::nmMatrix34ToPxTransform(initialTransform));
  ccRecord->m_pxRigidDynamicActor->setSolverIterationCounts(4, 4);
  ccRecord->m_pxRigidDynamicActor->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
  ccRecord->m_pxRigidDynamicActor->setLinearDamping(1.0f);

  physx::PxCapsuleGeometry geom(
    ccRecord->m_controllerParams.getRadius() * DYNAMIC_CONTROLLER_RADIUS_SCALE, 
    ccRecord->m_controllerParams.getHeight() * 0.5f);
  physx::PxShape* dynamicShape = ccRecord->m_pxRigidDynamicActor->createShape(
    geom, *m_controllerMaterial);
  dynamicShape->setName("Character controller dynamic object");
  dynamicShape->setContactOffset(ccRecord->m_controllerParams.getSkinWidth());
  dynamicShape->setLocalPose(shape->getLocalPose());
  // set up filter data 
  physx::PxFilterData queryData;
  queryData.word0 =  
    1 << MR::GROUP_COLLIDABLE_PUSHABLE |
    1 << MR::GROUP_CHARACTER_CONTROLLER;

  queryData.word1 =  
    1 << MR::GROUP_CHARACTER_CONTROLLER; 

  dynamicShape->setSimulationFilterData(queryData);
  dynamicShape->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, false);

  // Give the dynamic actor a small mass - most objects should be within a factor of 1000 or so of
  // the physx tolerance scale, so choose a value at the bottom of that range.
  const physx::PxTolerancesScale& tolerancesScale = PxGetPhysics().getTolerancesScale();
  float mass = tolerancesScale.mass * 0.001f;
  physx::PxRigidBodyExt::setMassAndUpdateInertia(
    *ccRecord->m_pxRigidDynamicActor,
    mass);

  scene->addActor(*ccRecord->m_pxRigidDynamicActor);

  ccRecord->m_pxRigidDynamicActor->setGlobalPose(
    ccRecord->m_pxController->getActor()->getGlobalPose());

  // Create the joint to constrain it to the kinematic shape
  ccRecord->m_pxDynamicJoint = PxD6JointCreate(
    PxGetPhysics(),
    ccRecord->m_pxController->getActor(),
    physx::PxTransform::createIdentity(),
    ccRecord->m_pxRigidDynamicActor, 
    physx::PxTransform::createIdentity());

  // Don't constrain the position, since it will just get reset to the KCC position (assuming no
  // penetration) every update anyway. An alternative to this would be to use a weak drive.
  ccRecord->m_pxDynamicJoint->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eFREE);
  ccRecord->m_pxDynamicJoint->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eFREE);
  ccRecord->m_pxDynamicJoint->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eFREE);
#else
  charControllerRecord->m_dynamicActor = 0;
  charControllerRecord->m_dynamicJoint = 0;
#endif
  // Finalise things
  ccRecord->m_network->setCharacterController(ccRecord);
  ccRecord->m_network->updateCharacterPropertiesWorldRootTransform(
    initialTransform,
    false);

  makeLinkBetweenPhysicsRigAndController(instanceID, getPhysicsRig(ccRecord->m_network));


  // Set the network to use the same physics/CC update method as in the physics manager. In
  // principle, it doesn't need to be exactly the same method, and it can change at any time, but it
  // does need to be such that if the physics manager is using a separate/combined update method,
  // then so should the network (however the details of the prediction method can differ, since the
  // physics manager ignores this).
  ccRecord->m_network->setCharacterPropertiesPhysicsAndCharacterControllerUpdate(
    m_physicsManager->getPhysicsAndCharacterControllerUpdate());
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateControllerRecord(MCOMMS::InstanceID instanceID)
{
  ControllerRecord* ccRecord = m_ccRecords[instanceID];

  // Get the CharacterControllerDef from the Network
  MR::CharacterControllerDef* ccDef = ccRecord->m_network->getActiveCharacterControllerDef();

  // Set the current character controller values from the def data (i.e. no CC scaling is applied at the moment)
  ccRecord->m_controllerParams = *ccDef;   
  ccRecord->m_network->setCharacterController(ccRecord);

  // Make the controller recompute offsets etc, as this function is generally only called after an anim set (char controller) change.
  scaleControllerProperties(
    instanceID,
    ccRecord->m_overrideBasics.m_currentHorizontalScale,
    ccRecord->m_overrideBasics.m_currentVerticalScale);

  ccRecord->m_network->updateCharacterPropertiesWorldRootTransform(
    NMP::Matrix34(ccRecord->m_characterOrientation, ccRecord->m_characterPosition),
    true);
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultControllerMgr::getCollisionEnabled(MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_controllerEnabled)
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultControllerMgr::getOnGround(MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_controllerEnabled && ccRecord->m_onGround)
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
bool DefaultControllerMgr::getAchievedRequestedMovement(MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_controllerEnabled && ccRecord->m_network->getCharacterPropertiesAchievedRequestedMovement())
      return true;
  }
  return false;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::scaleControllerProperties(
  MCOMMS::InstanceID instanceID,
  float              horizontalFraction,
  float              verticalFraction)
{
  Records::iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    ControllerRecord* ccRecord = it->second;
    ccRecord->m_controllerParams = *ccRecord->m_network->getActiveCharacterControllerDef();

    const MR::CharacterControllerDef* controllerDef = ccRecord->m_network->getActiveCharacterControllerDef();
    // Note: we currently scale the width and depth (for box controllers) using the horizontalFraction fraction.
    ccRecord->m_controllerParams.setRadius(controllerDef->getRadius() * horizontalFraction);
    ccRecord->m_controllerParams.setHeight(controllerDef->getHeight() * verticalFraction);
    ccRecord->m_controllerParams.setWidth(controllerDef->getWidth() * horizontalFraction);
    ccRecord->m_controllerParams.setDepth(controllerDef->getDepth() * horizontalFraction);

    // Recalculate the offset from the origin of the character controller.
    NMP_ASSERT(m_physicsManager && m_physicsManager->getPhysicsScene());
    NMP::Vector3 worldUpDirection = m_physicsManager->getPhysicsScene()->getWorldUpDirection();
    ccRecord->m_originOffset = calculateOriginOffset(ccRecord, worldUpDirection);
    

    physx::PxCapsuleController* cc = (physx::PxCapsuleController*) ccRecord->m_pxController;
    const float originalRadius = cc->getRadius();
    const float originalHeight = cc->getHeight();

    cc->setRadius(ccRecord->m_controllerParams.getRadius());
    cc->setHeight(ccRecord->m_controllerParams.getHeight());

    // Note that physx adjusts the position for changes in height, but not for changes in radius.
    // However, we go into the block below in either case, because we also need to change the
    // dynamic actor properties when they change
    if (originalRadius != cc->getRadius() || originalHeight != cc->getHeight())
    {
      // When changing the height of the capsule PhysX3 offsets the controller position by half of
      // the change in height. Subsequent penetration due to changes in radius on a slope, or
      // changes in height under an overhang, will be resolved using the dynamic actor elsewhere.
      float heightChange = ccRecord->m_controllerParams.getRadius() - originalRadius;
      heightChange += (ccRecord->m_controllerParams.getHeight() - originalHeight) * 0.5f;
      NMP::Vector3 offsetChange = worldUpDirection * heightChange;

      physx::PxExtendedVec3 p = cc->getPosition() + MR::nmVector3ToPxExtendedVec3(offsetChange);
      setControllerPosition(ccRecord->m_pxController, ccRecord->m_pxRigidDynamicActor, MR::nmPxExtendedVec3ToVector3(p));

      // Adjust the dynamic actor shape too
      if (ccRecord->m_pxRigidDynamicActor)
      {
        physx::PxShape *dynamicActorShape = 0;
        ccRecord->m_pxRigidDynamicActor->getShapes(&dynamicActorShape, 1);

        physx::PxCapsuleGeometry geom(
          cc->getRadius() * DYNAMIC_CONTROLLER_RADIUS_SCALE, 
          cc->getHeight() * 0.5f);
        dynamicActorShape->setGeometry(geom);
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::makeLinkBetweenPhysicsRigAndController(
  MCOMMS::InstanceID instanceID,
  MR::PhysicsRig*    physicsRig)
{
  if (physicsRig)
  {
    MR::PhysicsRigPhysX3* physicsRigPhysX3 = (MR::PhysicsRigPhysX3*)physicsRig;
    physicsRigPhysX3->setCharacterControllerActor(getCharacterControllerActor(instanceID)->isRigidDynamic());
  }
}

//----------------------------------------------------------------------------------------------------------------------
physx::PxActor* DefaultControllerMgr::getCharacterControllerActor(MCOMMS::InstanceID instanceID)
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_pxController)
      return ccRecord->m_pxController->getActor();
  }
  return 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::disableCollision(MCOMMS::InstanceID instanceID)
{
  Records::iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_pxController && ccRecord->m_controllerEnabled)
    {
      enableControllerCollision(ccRecord->m_pxController, ccRecord->m_pxRigidDynamicActor, false);
      ccRecord->m_controllerEnabled = false;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::enableCollision(MCOMMS::InstanceID instanceID)
{
  Records::iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_pxController && !ccRecord->m_controllerEnabled)
    {
      // teleport CCM physX object back from graveyard
      NMP::Vector3 p;
      p = ccRecord->m_characterPosition + computeWorldSpaceCCOriginOffset(ccRecord);

      setControllerPosition(ccRecord->m_pxController, ccRecord->m_pxRigidDynamicActor, p);
      ccRecord->m_controllerEnabled = true;

      enableControllerCollision(ccRecord->m_pxController, ccRecord->m_pxRigidDynamicActor, true);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::destroyControllerRecord(MCOMMS::InstanceID instanceID)
{
  Records::iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_pxController)
    {
      ccRecord->m_pxController->release();
      ccRecord->m_pxController = NULL;
    }
    if (ccRecord->m_pxRigidDynamicActor)
    {
      ccRecord->m_pxDynamicJoint->release();
      ccRecord->m_pxRigidDynamicActor->release();
      ccRecord->m_pxRigidDynamicActor = NULL;
      ccRecord->m_pxDynamicJoint = NULL;
    }

    // Set the physics rig on the network since it might have been removed
    setPhysicsRig(ccRecord->m_network, ccRecord->m_physicsRig);

    m_physicsManager->destroyPhysicsRig(ccRecord->m_network);

    if (ccRecord->m_connectRepresentation)
    {
      const MCOMMS::SceneObjectID sceneObjectID = ccRecord->m_connectRepresentation->getSceneObjectID();

      // The connect representation of the controller is owned by the connection and may therefore
      // have been destroyed as part of a connection close already.
      if (m_context->getSceneObjectManager()->findSceneObject(sceneObjectID))
      {
        SceneObjectRecord* const sceneObject = 
          m_context->getSceneObjectManager()->removeSceneObjectRecord(sceneObjectID);
        sceneObject->releaseAndDestroy();
      }
    }
    NMP::Memory::memFree(ccRecord);
    m_ccRecords.erase(it);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::setRequestedMovement(
  MCOMMS::InstanceID  instanceID,
  const NMP::Vector3& delta)
{
  Records::iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    ControllerRecord* ccRecord = it->second;
    ccRecord->m_requestedMovement = delta;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::setPosition(
  MCOMMS::InstanceID  instanceID,
  const NMP::Vector3& pos)
{
  // set CC position
  Records::iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    ControllerRecord* ccRecord = it->second;
    ccRecord->m_characterPosition = pos;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::setOrientation(
  MCOMMS::InstanceID  instanceID,
  const NMP::Quat&    ori)
{
  // set CC position
  Records::iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    ControllerRecord* ccRecord = it->second;
    ccRecord->m_characterOrientation = ori;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::teleport(MCOMMS::InstanceID instanceID, const NMP::PosQuat& rootTransform)
{
  Records::iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_pxController && ccRecord->m_controllerEnabled)
    {
      ccRecord->m_characterOrientation = rootTransform.m_quat;
      ccRecord->m_characterPosition = rootTransform.m_pos;

      NMP::Vector3 p = ccRecord->m_characterPosition + computeWorldSpaceCCOriginOffset(ccRecord);
      setControllerPosition(ccRecord->m_pxController, ccRecord->m_pxRigidDynamicActor, p);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DefaultControllerMgr::getPosition(MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    return ccRecord->m_characterPosition;
  }
  return NMP::Vector3::InitZero;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat DefaultControllerMgr::getOrientation(MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    return ccRecord->m_characterOrientation;
  }
  return NMP::Quat(0.0f, 0.0f, 0.0f, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 DefaultControllerMgr::getPositionDelta(MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    return ccRecord->m_deltaTranslation;
  }
  return NMP::Vector3::InitZero;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat DefaultControllerMgr::getOrientationDelta(MCOMMS::InstanceID instanceID) const
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    return ccRecord->m_deltaOrientation;
  }
  return NMP::Quat(0.0f, 0.0f, 0.0f, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::applyOverriddenProperties(
  MCOMMS::InstanceID instanceID,
  ControllerRecord*  ccRecord)
{
  //------------------------
  // Horizontal scaling override.
  MR::FrameCount frameNumber = ccRecord->m_network->getCurrentFrameNo();
  bool applyNewScaling = false;
  float radiusScaleValue = ccRecord->m_overrideBasics.m_currentHorizontalScale;
  const MR::AttribDataFloat* radiusScaleOverride = static_cast<const MR::AttribDataFloat*> (ccRecord->getRequestedPropertyOverride(MR::CC_PROPERTY_TYPE_HORIZONTAL_SCALE, frameNumber));
  if (radiusScaleOverride)
    radiusScaleValue = radiusScaleOverride->m_value;
  else
    radiusScaleValue = ccRecord->m_overrideBasics.m_defaultHorizontalScale;
  if (radiusScaleValue != ccRecord->m_overrideBasics.m_currentHorizontalScale)
  {
    ccRecord->m_overrideBasics.m_currentHorizontalScale = radiusScaleValue;
    applyNewScaling = true;
  }

  //------------------------
  // Vertical scaling override.
  float heightScaleValue = ccRecord->m_overrideBasics.m_currentVerticalScale;
  const MR::AttribDataFloat* heightScaleOverride = static_cast<const MR::AttribDataFloat*> (ccRecord->getRequestedPropertyOverride(MR::CC_PROPERTY_TYPE_VERTICAL_SCALE, frameNumber));
  if (heightScaleOverride)
    heightScaleValue = heightScaleOverride->m_value;
  else
    heightScaleValue = ccRecord->m_overrideBasics.m_defaultVerticalScale;
  if (heightScaleValue != ccRecord->m_overrideBasics.m_currentVerticalScale)
  {
    ccRecord->m_overrideBasics.m_currentVerticalScale = heightScaleValue;
    applyNewScaling = true;
  }
  
  if (applyNewScaling)
  {
    // Only rescale the character if anything has actually changed.
    scaleControllerProperties(instanceID, radiusScaleValue, heightScaleValue);
  }

  //------------------------
  // Position offset override.
  NMP::Vector3 positionOffsetValue = ccRecord->m_overrideBasics.m_currentPositionOffset;
  const MR::AttribDataVector3* positionOffsetOverride = static_cast<const MR::AttribDataVector3*> (ccRecord->getRequestedPropertyOverride(MR::CC_PROPERTY_TYPE_POSITION_OFFSET, frameNumber));
  if (positionOffsetOverride)
    positionOffsetValue = positionOffsetOverride->m_value;
  else
    positionOffsetValue = ccRecord->m_overrideBasics.m_defaultPositionOffset;
  if (positionOffsetValue != ccRecord->m_overrideBasics.m_currentPositionOffset)
  {
    ccRecord->m_overrideBasics.m_currentPositionOffset = positionOffsetValue;
   
    NMP::PosQuat characterTransform;
    characterTransform.m_quat = ccRecord->m_characterOrientation;
    characterTransform.m_pos = ccRecord->m_characterPosition;
    teleport(instanceID, characterTransform);
  }

  //------------------------
  // Abs position override.
  const MR::AttribDataVector3* absPositionOverride = static_cast<const MR::AttribDataVector3*> (ccRecord->getRequestedPropertyOverride(MR::CC_PROPERTY_TYPE_POSITION_ABSOLUTE, frameNumber));
  if (absPositionOverride)
  {
    // Override the absolute position of the character controller.
    ccRecord->m_overrideBasics.m_currentPositionAbs = absPositionOverride->m_value;
    ccRecord->m_overrideBasics.m_overrideAbsPosition = true;

    NMP::PosQuat characterTransform;
    characterTransform.m_quat = ccRecord->m_characterOrientation;
    characterTransform.m_pos = ccRecord->m_characterPosition;
    teleport(instanceID, characterTransform);
  }
  else
  {
    ccRecord->m_overrideBasics.m_currentPositionAbs = ccRecord->m_overrideBasics.m_defaultPositionAbs;
    ccRecord->m_overrideBasics.m_overrideAbsPosition = false;
  }
  
  //------------------------
  // Vertical movement control state change request.
  uint32_t verticalMoveStateRequestValue = ccRecord->m_overrideBasics.m_currentVerticalMoveState;
  const MR::AttribDataUInt* stateRequestOverride = static_cast<const MR::AttribDataUInt*> (ccRecord->getRequestedPropertyOverride(MR::CC_PROPERTY_TYPE_VERTICAL_MOVEMENT_STATE, frameNumber));
  if (stateRequestOverride)
    verticalMoveStateRequestValue = stateRequestOverride->m_value;
  else
    verticalMoveStateRequestValue = ccRecord->m_overrideBasics.m_defaultVerticalMoveState;
  if (verticalMoveStateRequestValue != ccRecord->m_overrideBasics.m_currentVerticalMoveState)
  {
    ccRecord->m_overrideBasics.m_currentVerticalMoveState = verticalMoveStateRequestValue;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateControllers(float dt)
{
  physx::PxFilterData filterData(
    0,
    1 << MR::GROUP_CHARACTER_CONTROLLER | 
    1 << MR::GROUP_CHARACTER_PART | 
    1 << MR::GROUP_NON_COLLIDABLE | 
    1 << MR::GROUP_INTERACTION_PROXY,
    0,
    0);
  MR::MorphemePhysX3QueryFilterCallback morphemePhysX3QueryFilterCallback(filterData);
  physx::PxControllerFilters controllerFilter(0, 0, &morphemePhysX3QueryFilterCallback);

  // go through all controller records, updating them
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    ControllerRecord* ccRecord = it->second;
    MCOMMS::InstanceID instanceID = it->first;
    ccRecord->m_characterPositionOld = ccRecord->m_characterPosition;

    //-------------------  
    // Applying any override properties that have been set this frame.
    applyOverriddenProperties(instanceID, ccRecord);

    //-------------------
    MR::PhysicsRig* physicsRig = getPhysicsRig(ccRecord->m_network);
    MR::PhysicsRigPhysX3* physicsRigPhysX3 = (MR::PhysicsRigPhysX3*) physicsRig;

    if (physicsRig &&
        physicsRig->isReferenced() &&
        ccRecord->m_network->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
    {
      disableCollision(instanceID);
    }
    else
    {
      enableCollision(instanceID);
    }

    if (ccRecord->m_controllerEnabled)
    { 
      physx::PxCapsuleController* capsuleController = (physx::PxCapsuleController*) ccRecord->m_pxController;
      physx::PxTransform globalPose = capsuleController->getActor()->getGlobalPose(); // assumes the local pose is identity

      // ... if the controller is switched on, update it according to Px-controller semi-physics
      if (ccRecord->m_pxRigidDynamicActor)
      {
        // If the kinematic shape penetrates the environment then extract it (if possible)
        physx::PxScene* physXScene = ((MR::PhysicsScenePhysX3*)m_physicsManager->getPhysicsScene())->getPhysXScene();

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
        physx::PxClientID clientID = physicsRigPhysX3 ? physicsRigPhysX3->getClientID() : physx::PX_DEFAULT_CLIENT;
        if (physXScene->overlapAny(
          geom,
          globalPose,
          hit,
          sceneQueryFilterData, 
          &morphemePhysX3QueryFilterCallback,
          clientID))
        {
          // If the kinematic actor is penetrating, then move it to the dynamic actor position
          setControllerPosition(
            ccRecord->m_pxController, 
            0, 
            MR::nmPxVec3ToVector3(ccRecord->m_pxRigidDynamicActor->getGlobalPose().p));
        }
        else
        {
          // If it's not penetrating then move the dynamic actor there (in case it got left behind somehow)
          ccRecord->m_pxRigidDynamicActor->setGlobalPose(ccRecord->m_pxController->getActor()->getGlobalPose());
        }
      }

      // requestedMovement = the desired movement, as calculated from the animation system, that we
      // are trying to achieve
     
      // Calculate the requested movement of the character controller:
      //  The supplied requested movement is for the character. To find the requested movement of the character controller
      //  we need to find out where it should be if the requested character movement was achieved.
      NMP::Vector3 currentCCPos((float) globalPose.p.x, (float) globalPose.p.y, (float) globalPose.p.z);
      NMP::Vector3 requestedCCPos = ccRecord->m_characterPosition + ccRecord->m_requestedMovement + computeWorldSpaceCCOriginOffset(ccRecord);
      NMP::Vector3 requestedMovement = requestedCCPos - currentCCPos;


      // Move the controller according to the ground velocity by raycasting down
      NMP::Vector3 worldUpDirection = m_physicsManager->getPhysicsScene()->getWorldUpDirection();
      NMP::Vector3 rayStart = MR::nmPxExtendedVec3ToVector3(ccRecord->m_pxController->getPosition());
      // Raycast down to find the ground. Go a little bit beyond the bottom of the controller in
      // case we're on a slope, or if the ground is jittering a bit. The exact amount doesn't matter
      // too much.
      NMP::Vector3 rayDelta = -ccRecord->m_originOffset * 1.2f;
      NMP::Vector3 hitVelocity;
      NMP::Vector3 hitPosition, hitNormal; // not actually used
      float hitDist;
      if (ccRecord->m_network->getCharacterController()->castRayIntoCollisionWorld(
          rayStart, rayDelta, hitDist, hitPosition, hitNormal, hitVelocity, ccRecord->m_network))
      {
        // If the ground surface is going down, don't apply an additional downward motion to the CC
        if (hitVelocity.dot(worldUpDirection) < 0.0f)
          hitVelocity = hitVelocity.getComponentOrthogonalToDir(worldUpDirection);
        requestedMovement += hitVelocity * dt;
      }

      // Calculate horizontal movement
      NMP_ASSERT(m_physicsManager && m_physicsManager->getPhysicsScene());
      float amountInUpDirection = worldUpDirection.dot(requestedMovement);
      NMP::Vector3 vectorInUpDirection = worldUpDirection * amountInUpDirection;
      ccRecord->m_requestedHorizontalMovement = requestedMovement - vectorInUpDirection;

#ifdef STICK_TO_GROUND
      if (ccRecord->m_onGround && (ccRecord->m_overrideBasics.m_currentVerticalMoveState != MR::CC_STATE_VERTICAL_MOVEMENT_ANIMATION))
      {
        // Move character down by the maximum slope amount over the distance moved horizontally.
        float horDist = ccRecord->m_requestedHorizontalMovement.magnitude();
        ccRecord->m_extraVerticalMove =
          worldUpDirection * (-horDist * tanf(NMP::degreesToRadians(ccRecord->m_controllerParams.getMaxSlopeAngle())));
        requestedMovement += (ccRecord->m_extraVerticalMove);
      }
#endif
      // execute physX's 'collide and slide' controller movement function.
      ccRecord->m_hitReport->updatePreMove(dt, ccRecord->m_controllerParams.getMaxPushForce());
      physx::PxU32 flags = ccRecord->m_pxController->move(
        MR::nmVector3ToPxVec3(requestedMovement),
        0.00000001f,
        dt,
        controllerFilter);

      // Determine whether character is on the ground or in the air, and whether it has
      // just transitioned between the two.
      ccRecord->m_onGroundOld = ccRecord->m_onGround;
      ccRecord->m_onGround = (flags & physx::PxControllerFlag::eCOLLISION_DOWN) != 0 ;

      if (ccRecord->m_onGround && ccRecord->m_onGroundOld)
        ccRecord->m_network->setCharacterPropertiesGroundContactTime(ccRecord->m_network->getCharacterPropertiesGroundContactTime() + dt); // on ground
      else if (!ccRecord->m_onGround && !ccRecord->m_onGroundOld)
        ccRecord->m_network->setCharacterPropertiesGroundContactTime(ccRecord->m_network->getCharacterPropertiesGroundContactTime() - dt); // in air
      else
        ccRecord->m_network->setCharacterPropertiesGroundContactTime(0.0f); // transition from/to ground to/from air
    }
    else
    { // ... else there is no controller enabled, so just keyframe the character wherever the
      // animation says it should go, irrespective of collisions or anything else.
      ccRecord->m_characterPosition += ccRecord->m_requestedMovement;
      ccRecord->m_onGroundOld = false;
      ccRecord->m_onGround = false;
      ccRecord->m_network->setCharacterPropertiesGroundContactTime(0.0f);
      ccRecord->m_network->setCharacterPropertiesAchievedRequestedMovement(true);
    }
  }

#ifdef STICK_TO_GROUND
  // Undo the extra vertical movement if we were on the ground, but we're not now.
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_controllerEnabled &&
        ccRecord->m_onGroundOld &&
        !ccRecord->m_onGround &&
        (ccRecord->m_overrideBasics.m_currentVerticalMoveState != MR::CC_STATE_VERTICAL_MOVEMENT_ANIMATION))
    {
      ccRecord->m_pxController->move(
        -MR::nmVector3ToPxVec3(ccRecord->m_extraVerticalMove),
        0.00000001f,
        dt,
        controllerFilter);
    }
  }
#endif

  // The PxController->Move(desired) function above has changed the position of the PxController
  // according to the desired change from animation filtered through the PxController's
  // semi-physical collision detection. So we now need to update the ccmRecord's m_characterPosition
  // to reflect this new value, and also set the root position for the network's animation data. We
  // do this for every controller in turn.
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    ControllerRecord* ccRecord = it->second;
    MCOMMS::InstanceID instanceID = it->first;

    // The updated position of the PxController
    physx::PxExtendedVec3 newPos = ccRecord->m_pxController->getPosition();

    if (ccRecord->m_controllerEnabled)
    {
      // Take off the offset value to translate the character position down from the center of
      // the controller body to where the feet should be.
      NMP::Vector3 offset = computeWorldSpaceCCOriginOffset(ccRecord);
      ccRecord->m_characterPosition = MR::nmPxExtendedVec3ToVector3(newPos) - offset;

      // determine whether the character achieved the requested movement
      bool achievedRequestedMovement = true;
      if (ccRecord->m_onGround)
      {
        // Calculate actual horizontal movement
        NMP::Vector3 actualTranslation = (ccRecord->m_characterPosition - ccRecord->m_characterPositionOld);
        NMP_ASSERT(m_physicsManager && m_physicsManager->getPhysicsScene());
        NMP::Vector3 worldUpDirection = m_physicsManager->getPhysicsScene()->getWorldUpDirection();
        float amountInUpDirection = worldUpDirection.dot(actualTranslation);
        NMP::Vector3 vectorInUpDirection = worldUpDirection * amountInUpDirection;
        NMP::Vector3 actualHorizontalMovementVector = actualTranslation - vectorInUpDirection;

        // Compare the horizontal movement
        NMP::Vector3 deltaMovementVector = actualHorizontalMovementVector - ccRecord->m_requestedHorizontalMovement;
        if (deltaMovementVector.magnitude() > (1e-4f * ccRecord->m_controllerParams.getHeight()))
          achievedRequestedMovement = false;
      }
      ccRecord->m_network->setCharacterPropertiesAchievedRequestedMovement(achievedRequestedMovement);

      // set the world root transform for the network (this also determines the deltaTranslation
      // due to gravity)
      m_physicsManager->setRoot(instanceID, ccRecord->m_characterPosition);
    }

    ccRecord->m_requestedMovement.setToZero();
  }
}

//----------------------------------------------------------------------------------------------------------------------
ControllerRecord* DefaultControllerMgr::getControllerRecord(MCOMMS::InstanceID instanceID)
{
  return m_ccRecords[instanceID];
}

//----------------------------------------------------------------------------------------------------------------------
// Functions for controlling the Connect scene-object representing the character controller capsule
//----------------------------------------------------------------------------------------------------------------------
MCOMMS::SceneObject* DefaultControllerMgr::addNewControllerRepresentation(
  MCOMMS::SceneObjectID objectID, 
  MCOMMS::Connection* owner)
{
  // CharacterControllerDef
  // IMPORTANT:
  // 1. There already exists at this point a scene object that describes the character itself; this
  //    is the scene object ID that was passed in.
  // 2. The job of this function is to add a SECOND scene object at the same position as the character
  //    with the parameters of the CCMRecord.
  // 3. The CCMRecord should already have been set up for the character before this function is called.
  MCOMMS::SceneObject* characterSceneObject = m_context->getSceneObjectManager()->findSceneObject(objectID);
  if (characterSceneObject)
  {
    MCOMMS::Attribute* instanceIDAttr = characterSceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_INSTANCEID);
    NMP_ASSERT(instanceIDAttr);
    MCOMMS::InstanceID* instanceID = static_cast<MCOMMS::InstanceID*>(instanceIDAttr->getData());
    MCOMMS::InstanceID networkInstanceID = *instanceID;

    ControllerRecord* ccRecord = m_ccRecords[networkInstanceID];
    if (ccRecord == NULL)
    {
      // A CCMRecord should have been set up for this controller, but it clearly
      // hasn't been yet.  Why not?
      NMP_ASSERT_FAIL();

      return NULL;
    }

    // if there is already a scene object for this character then don't create a new one.
    if (ccRecord->m_connectRepresentation)
    {
      return ccRecord->m_connectRepresentation;
    }

    NMP::Vector3 position = ccRecord->m_characterPosition;
    position += computeWorldSpaceCCOriginOffset(ccRecord);
    MR::CharacterControllerDef::ShapeType shape = ccRecord->m_network->getActiveCharacterControllerDef()->getShape();
    NMP::Colour colour = ccRecord->m_network->getActiveCharacterControllerDef()->getColour();
    float radius = ccRecord->m_network->getActiveCharacterControllerDef()->getRadius();
    float height = ccRecord->m_network->getActiveCharacterControllerDef()->getHeight();
    float width = ccRecord->m_network->getActiveCharacterControllerDef()->getWidth();
    float depth = ccRecord->m_network->getActiveCharacterControllerDef()->getDepth();
    float skinWidth = ccRecord->m_network->getActiveCharacterControllerDef()->getSkinWidth();

    // Initialise the attributes
    std::vector<MCOMMS::Attribute*> attributes;

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        "CharacterController",
        MCOMMS::Attribute::SEMANTIC_NAME));

    NMP::Matrix34 transform(NMP::Matrix34Identity());
    transform.translation() = position;
    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        transform,
        MCOMMS::Attribute::SEMANTIC_TRANSFORM,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        ccRecord->m_controllerEnabled ? 1 : 0,
        MCOMMS::Attribute::SEMANTIC_ACTIVE_PHYSICS_OBJECT,
        true));

    switch (shape)
    {
      case MR::CharacterControllerDef::CapsuleShape:
      default:
        attributes.push_back(
          MCOMMS::Attribute::createAttribute(
            MCOMMS::Attribute::CONTROLLER_SHAPE_CAPSULE,
            MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
        break;
      case MR::CharacterControllerDef::CylinderShape:
        attributes.push_back(
          MCOMMS::Attribute::createAttribute(
            MCOMMS::Attribute::CONTROLLER_SHAPE_CYLINDER,
            MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
        break;
      case MR::CharacterControllerDef::BoxShape:
        attributes.push_back(
          MCOMMS::Attribute::createAttribute(
            MCOMMS::Attribute::CONTROLLER_SHAPE_BOX,
            MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
        break;
      case MR::CharacterControllerDef::SphereShape:
        attributes.push_back(
          MCOMMS::Attribute::createAttribute(
            MCOMMS::Attribute::CONTROLLER_SHAPE_SPHERE,
            MCOMMS::Attribute::SEMANTIC_NMDEMO_CHARACTER_CONTROLLER_SHAPE));
        break;
    }

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        height,
        MCOMMS::Attribute::SEMANTIC_HEIGHT,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        width,
        MCOMMS::Attribute::SEMANTIC_LENGTH,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        depth,
        MCOMMS::Attribute::SEMANTIC_DEPTH,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        radius,
        MCOMMS::Attribute::SEMANTIC_RADIUS,
        true));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        skinWidth,
        MCOMMS::Attribute::SEMANTIC_SKIN_WIDTH));

    attributes.push_back(
      MCOMMS::Attribute::createAttribute(
        colour,
        MCOMMS::Attribute::SEMANTIC_COLOUR));

    // Add object to the list of registered objects.
    DefaultSceneObjectMgr* soMgr = 
      static_cast<DefaultSceneObjectMgr*>(MCOMMS::getRuntimeTarget()->getSceneObjectManager());
    NMP_ASSERT(soMgr);

    MCOMMS::SceneObjectID controllerID = soMgr->getUnusedSceneObjectID();
    SceneObjectRecord* controller = SceneObjectRecord::create(
      controllerID, (uint32_t)attributes.size(), &attributes[0], owner);

    for (std::vector<MCOMMS::Attribute*>::iterator it = attributes.begin(); it != attributes.end(); ++it)
    {
      NMP::Memory::memFree((void*)(*it)->getData());
      MCOMMS::Attribute::destroy((*it));
    }

    if (controller)
    {
      ccRecord->m_connectRepresentation = controller->getObject();
      m_context->getSceneObjectManager()->addSceneObjectRecord(controller);
      return controller->getObject();
    }
  }

  return NULL;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateControllerRepresentations()
{
  // CharacterControllerDef
  // Run through the list of objects used to visualize the character position and volume
  // Update their dynamic attributes from the associated character to reflect the current state

  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    ControllerRecord* ccRecord = it->second;
    MCOMMS::SceneObject* object = ccRecord->m_connectRepresentation;
    if (object == NULL)
      continue;

    NMP::Vector3 position = ccRecord->m_characterPosition;
    position += computeWorldSpaceCCOriginOffset(ccRecord);
    float radius = ccRecord->m_controllerParams.getRadius();
    float height = ccRecord->m_controllerParams.getHeight();
    float width = ccRecord->m_controllerParams.getWidth();
    float depth = ccRecord->m_controllerParams.getDepth();

    MCOMMS::Attribute* transformAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_TRANSFORM);
    if (transformAttr)
    {
      NMP::Matrix34* transform = (NMP::Matrix34*)(transformAttr->getData());
      if (transform)
      {
        NMP::Matrix34 matrix(NMP::Matrix34Identity());
        matrix.translation() = position;
        *transform = matrix;
      }
    }

    MCOMMS::Attribute* activeAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_ACTIVE_PHYSICS_OBJECT);
    if (activeAttr)
    {
      uint32_t* active = (uint32_t*)(activeAttr->getData());
      if (active)
      {
        *active = ccRecord->m_controllerEnabled ? 1 : 0;
      }
    }

    MCOMMS::Attribute* heightAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_HEIGHT);
    if (heightAttr)
    {
      float* heightValue = (float*)(heightAttr->getData());
      if (heightValue)
      {
        *heightValue = height;
      }
    }

    MCOMMS::Attribute* depthAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_DEPTH);
    if (depthAttr)
    {
      float* depthValue = (float*)(depthAttr->getData());
      if (depthValue)
      {
        *depthValue = depth;
      }
    }

    MCOMMS::Attribute* lengthAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_LENGTH);
    if (lengthAttr)
    {
      float* lengthValue = (float*)(lengthAttr->getData());
      if (lengthValue)
      {
        *lengthValue = width;
      }
    }

    MCOMMS::Attribute* radiusAttr = object->getAttribute(MCOMMS::Attribute::SEMANTIC_RADIUS);
    if (radiusAttr)
    {
      float* radiusValue = (float*)(radiusAttr->getData());
      if (radiusValue)
      {
        *radiusValue = radius;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstanceNetworksBegin(float dt)
{
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    ControllerRecord* cr = getControllerRecord(instanceID);
    MR::Network* network = cr->m_network;
    network->startUpdate(dt, false, true);
#if 0
    // Enable this to test how the network copes when the physics rig is removed from the network.
    static bool assignPhysicsRig = true;
    if (assignPhysicsRig)
    {
      setPhysicsRig(network, cr->m_physicsRig);
    }
    else
    {
      setPhysicsRig(network, 0);
      while (cr->m_physicsRig->isReferenced())
        cr->m_physicsRig->removeReference();
    }
#endif
  }
  m_numCompleted = 0;
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::continueNetworkUpdate(MR::Network* network, MR::TaskID NMP_USED_FOR_ASSERTS(taskID))
{
  MR::Task* task = NULL;
  network->update(task);

  if (network->m_exeResult == MR::EXECUTE_RESULT_COMPLETE)
  {
    ++m_numCompleted;

    // reset the network
    network->getTempMemoryAllocator()->reset();
  }
  else if (network->m_exeResult == MR::EXECUTE_RESULT_BLOCKED)
  {
    NMP_ASSERT(task->m_taskid == taskID);
    ++m_numBlocked;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstanceNetworksContinue(MR::TaskID taskID)
{
  // Continue updating networks until all are completed.
  uint32_t numNetworks = (uint32_t)m_ccRecords.size();
  m_numBlocked = 0;

  while (m_numCompleted < numNetworks - m_numBlocked)
  {
    for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
    {
      MR::Network* network = getControllerRecord(it->first)->m_network;

      switch (network->m_exeResult)
      {
      case MR::EXECUTE_RESULT_INVALID:
      case MR::EXECUTE_RESULT_BLOCKED:
      case MR::EXECUTE_RESULT_COMPLETE:
        break;

      case MR::EXECUTE_RESULT_STARTED:
        continueNetworkUpdate(network, taskID);
        break;

      case MR::EXECUTE_RESULT_IN_PROGRESS:
#if defined(NM_USE_SPU_DISPATCHER_ON_PS3)
        // We finish an update when the SPU has finished, otherwise spin waiting by moving on to another
        if (((MR::DispatcherPS3*)network->getDispatcher())->checkIfSPUHasFinished())
        {
          continueNetworkUpdate(network, taskID);
        }
#else
        continueNetworkUpdate(network, taskID);
#endif
        break;

      default:
        NMP_ASSERT(false);
      }
    }
  }

  // allow the blocked networks to continue at next update
  if (m_numBlocked)
  {
    for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
    {
      MR::Network* network = getControllerRecord(it->first)->m_network;

      if (network->m_exeResult == MR::EXECUTE_RESULT_BLOCKED)
      {
        network->m_exeResult = MR::EXECUTE_RESULT_IN_PROGRESS;
      }
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstanceNetworksEnd()
{
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    MR::Network* network = getControllerRecord(instanceID)->m_network;

    float delta = network->getLastUpdateTimeStep();
    m_physicsManager->updateInteractionProxy(instanceID, delta);

    // finalize post network update
    network->endUpdate();

    // clean up the allocator
    NMP::TempMemoryAllocator* tempAllocator = network->getTempMemoryAllocator();
    tempAllocator->reset();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesConnections(float dt)
{
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updateConnections(instanceID, dt);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesPreController(float delta)
{
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updatePreController(instanceID, delta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesPrePhysics(float delta)
{
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updatePrePhysics(instanceID, delta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesPostPhysicsInit(float delta)
{
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updatePostPhysicsInit(instanceID, delta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateInstancesPostPhysics(float delta)
{
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    MCOMMS::InstanceID instanceID = it->first;
    m_physicsManager->updatePostPhysics(instanceID, delta);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::sendInstanceProfileTimingDebugOutput()
{
#ifdef MR_OUTPUT_DEBUGGING
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    ControllerRecord* ccRecord = it->second;
    MR::Network* network = ccRecord->m_network;

    network->sendInstanceProfileTimingDebugOutput();
  }
#endif // MR_OUTPUT_DEBUGGING
}

//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_COMPILER_MSVC
  #pragma warning (pop)
#endif
//----------------------------------------------------------------------------------------------------------------------
