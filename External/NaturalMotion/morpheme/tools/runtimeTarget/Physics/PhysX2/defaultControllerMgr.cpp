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

#include "mrPhysX2.h"
#include "physics/mrPhysicsRigDef.h"
#include "mrPhysicsRigPhysX2.h"
#include "defaultPhysicsMgr.h"
#include "../../defaultAssetMgr.h"
#include "../../defaultDataManager.h"
#include "../../defaultSceneObjectMgr.h"
#include "../../networkDefRecordManager.h"
#include "../../networkInstanceRecordManager.h"
#include "../../sceneObjectRecordManager.h"
#include "physics/mrPhysics.h"
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

// If the character movement should "hug" the ground at the top of slopes etc. This works by
// applying a downwards component of motion which then gets undone after the character controller
// update (with a second character controller update).
#define STICK_TO_GROUND

// Set this to a value N > 0 to enable swapping in/out of physics every N updates in the first network record.
#define TEST_REMOVAL_AND_ADDITION_OF_PHYSICS 0

// Maximum distance from the origin that the graveyard position can have
#define PHYSICS_GRAVEYARD_FIELD_SIZE 1000

// Altitude of the graveyard position
#define PHYSICS_GRAVEYARD_ALTITUDE -1000.0f

// Time take for the controller force to reach maximum
float g_maxForceTime = 0.3f;

//----------------------------------------------------------------------------------------------------------------------
/// \class ControllerHitReport
/// \brief Controller hit report for PhysX2. This class is used to apply forces to bodies that the character controller
///  comes into contact with.  The force is ramped up depending on how long the body has been in contact with the
///  character controller.
//----------------------------------------------------------------------------------------------------------------------
class ControllerHitReport : public NxUserControllerHitReport
{
public:
  ControllerHitReport(float maxTouchTime, TouchTimes& touchTimes);

  void updatePreMove(float dt, float maxPushForce);

  virtual NxControllerAction onShapeHit(const NxControllerShapeHit& hit);

  virtual NxControllerAction onControllerHit(const NxControllersHit& NMP_UNUSED(hit));

  /// \brief The maximum push force which can be applied by the character controller.
  float      m_maxPushForce;
  /// \brief the timestep since the last character controller update.
  float      m_dt;
  /// \brief The amount of time the controller will touch something before it ramps up to full push force.
  float      m_maxTouchTime;
  /// \brief A map of Shape pointers to touch time (in seconds). 
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

//----------------------------------------------------------------------------------------------------------------------
NxControllerAction ControllerHitReport::onControllerHit(const NxControllersHit& NMP_UNUSED(hit))
{
  return NX_ACTION_NONE;
}

//----------------------------------------------------------------------------------------------------------------------
ControllerRecord::ControllerRecord() :
  m_nxController(NULL),
  m_requestedMovement(NMP::Vector3(NMP::Vector3::InitZero)),
  m_characterPosition(NMP::Vector3(NMP::Vector3::InitZero)),
  m_characterOrientation(NMP::Quat::kIdentity),
  m_controllerEnabled(false),
  m_onGround(false),
  m_onGroundOld(false),
  m_extraVerticalMove(0.0f, 0.0f, 0.0f),
  m_originOffset(0.0f, 0.0f, 0.0f),
  m_connectRepresentation(NULL)
{
  m_hitReport = new ControllerHitReport(g_maxForceTime, m_touchTimes);
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
  m_context(context)
{
  m_manager = NxCreateControllerManager(NxGetPhysicsSDKAllocator());
  m_graveSeed = 0;
}

//----------------------------------------------------------------------------------------------------------------------
DefaultControllerMgr::~DefaultControllerMgr()
{
  NxReleaseControllerManager(m_manager);
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
          ((ccRecord->m_controllerParams.getHeight() * 0.5f) + // Half height of the cylinder that forms the central body of the capsule.
          ccRecord->m_controllerParams.getRadius() +           // Sphere on bottom of character capsule.
          ccRecord->m_controllerParams.getSkinWidth());        // Skin width around the whole character controller.
      break;
    }
    case MR::CharacterControllerDef::BoxShape:
    {
      offset = 
        worldUpDirection *
          ((ccRecord->m_controllerParams.getHeight() * 0.5f) + // Half height of the box.
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

  // Set the current character controller values from the def data (i.e. no CC scaling is applied at the moment)
  ccRecord->m_controllerParams = *ccDef;

  // Recalculate the offset from the origin of the character controller.
  ccRecord->m_originOffset = calculateOriginOffset(ccRecord, worldUpDirection);

  // Now set up the physX character controller object from the contents of the ccmRecord.
  NxControllerDesc* desc = NULL;

  // Shape specific initialisation of the character controller.
  switch (ccRecord->m_controllerParams.getShape())
  {
    case MR::CharacterControllerDef::CapsuleShape:
    {
      NxCapsuleControllerDesc* capsuleDesc = new NxCapsuleControllerDesc();
      capsuleDesc->radius = ccRecord->m_controllerParams.getRadius();
      capsuleDesc->height = ccRecord->m_controllerParams.getHeight();
      desc = capsuleDesc;
      break;
    }
    case MR::CharacterControllerDef::BoxShape:
    {
      NxBoxControllerDesc* boxDesc = new NxBoxControllerDesc();
      // The box extents are half-extents.
      boxDesc->extents.x = ccRecord->m_controllerParams.getWidth() * 0.5f;
      boxDesc->extents.y = ccRecord->m_controllerParams.getHeight() * 0.5f;
      boxDesc->extents.z = ccRecord->m_controllerParams.getDepth() * 0.5f;
      desc = boxDesc;
      break;
    }
    default:
      NMP_ASSERT_FAIL(); // Not a valid shape type
  }

  NMP_ASSERT_MSG(desc, "Invalid CharacterController type selected for PhysX2!");

  desc->interactionFlag = NXIF_INTERACTION_INCLUDE;

  // Apply the up direction specified in connect.
  // NOTE: PhysX character controller can only cope with the up direction being in one of the primary world axies.
  NMP_ASSERT(worldUpDirection.x > 0.99f || worldUpDirection.z > 0.99f || worldUpDirection.y > 0.99f);
  if (worldUpDirection.x > 0.99f)
    desc->upDirection = NX_X;
  else if (worldUpDirection.z > 0.99f)
    desc->upDirection = NX_Z;
  else
    desc->upDirection = NX_Y;

  float maxSlopeAngleDegrees = ccRecord->m_controllerParams.getMaxSlopeAngle();
  float maxSlopeAngleRadians = NMP::degreesToRadians(maxSlopeAngleDegrees);
  float cosMaxSlopeAngle = cosf(maxSlopeAngleRadians);
  desc->slopeLimit = NMP::clampValue(cosMaxSlopeAngle, 0.0f, NM_PI_OVER_TWO);
#ifdef NMP_ENABLE_ASSERTS
  if (desc->slopeLimit != cosMaxSlopeAngle)
  {
    NMP_DEBUG_MSG("WARNING: The max slope angle value of the character controller has been clamped.");
  }
#endif
  desc->skinWidth = ccRecord->m_controllerParams.getSkinWidth();
  desc->stepOffset = ccRecord->m_controllerParams.getStepHeight();
  ccRecord->m_hitReport->m_maxPushForce = ccRecord->m_controllerParams.getMaxPushForce();
  desc->callback = ccRecord->m_hitReport;

  NMP::Vector3 controllerPos = ccRecord->m_characterPosition + computeWorldSpaceCCOriginOffset(ccRecord);
  desc->position.set(controllerPos.x, controllerPos.y, controllerPos.z);

  ccRecord->m_nxController = m_manager->createController(scene, *desc);

  // We no longer need the desc
  if (ccRecord->m_controllerParams.getShape() == MR::CharacterControllerDef::CapsuleShape)
  {
    delete (NxCapsuleControllerDesc*)(desc);
  }
  else if (ccRecord->m_controllerParams.getShape() == MR::CharacterControllerDef::BoxShape)
  {
    delete (NxBoxControllerDesc*)(desc);
  }

  NMP_ASSERT(ccRecord->m_nxController);
  ccRecord->m_nxController->setCollision(false);
  ccRecord->setActor(ccRecord->m_nxController->getActor());
  ccRecord->m_network->setCharacterController(ccRecord);

  ccRecord->m_network->updateCharacterPropertiesWorldRootTransform(
                NMP::Matrix34(ccRecord->m_characterOrientation, ccRecord->m_characterPosition),
                false);

  makeLinkBetweenPhysicsRigAndController(instanceID, getPhysicsRig(ccRecord->m_network));

  // Turn off collision detection between CC capsule and all body parts
  const MR::PhysicsRig* physicsRig = getPhysicsRig(ccRecord->m_network);
  if (physicsRig)
  {
    size_t nBones = physicsRig->getNumParts();
    for (uint32_t i = 0; i < (uint32_t)nBones; ++i)
    {
      MR::PhysicsRigPhysX2::PartPhysX* part = (MR::PhysicsRigPhysX2::PartPhysX*) physicsRig->getPart(i);
      scene->setActorPairFlags(
                *part->getActor(),
                *ccRecord->m_nxController->getActor(),
                NX_IGNORE_PAIR);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::updateControllerRecord(MCOMMS::InstanceID instanceID)
{
  ControllerRecord* ccRecord = m_ccRecords[instanceID];

  // Get the world up direction.
  DefaultPhysicsMgr* physicsMgr = (DefaultPhysicsMgr*) m_physicsManager;
  NMP_ASSERT(physicsMgr && physicsMgr->getPhysicsScene());
  NMP::Vector3 worldUpDirection = physicsMgr->getPhysicsScene()->getWorldUpDirection();

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

    NMP::Vector3 origOffset = ccRecord->m_originOffset;

    // Note: we currently scale the width and depth (for box controllers) using the horizontalFraction fraction.
    ccRecord->m_controllerParams.setRadius(ccRecord->m_network->getActiveCharacterControllerDef()->getRadius() * horizontalFraction);
    ccRecord->m_controllerParams.setSkinWidth(ccRecord->m_network->getActiveCharacterControllerDef()->getSkinWidth() * horizontalFraction);
    ccRecord->m_controllerParams.setHeight(ccRecord->m_network->getActiveCharacterControllerDef()->getHeight() * verticalFraction);
    ccRecord->m_controllerParams.setWidth(ccRecord->m_network->getActiveCharacterControllerDef()->getWidth() * horizontalFraction);
    ccRecord->m_controllerParams.setDepth(ccRecord->m_network->getActiveCharacterControllerDef()->getDepth() * horizontalFraction);
    ccRecord->m_controllerParams.setStepHeight(ccRecord->m_network->getActiveCharacterControllerDef()->getStepHeight() * verticalFraction);

    // Recalculate the offset from the origin of the character controller.
    NMP_ASSERT(m_physicsManager && m_physicsManager->getPhysicsScene());
    NMP::Vector3 worldUpDirection = m_physicsManager->getPhysicsScene()->getWorldUpDirection();
    ccRecord->m_originOffset = calculateOriginOffset(ccRecord, worldUpDirection);
      
    // Set the new PhysX controller properties.
    switch (ccRecord->m_controllerParams.getShape())
    {
      case MR::CharacterControllerDef::CapsuleShape:
      {
        NxCapsuleController* cc = (NxCapsuleController*) ccRecord->m_nxController;
        cc->setRadius(ccRecord->m_controllerParams.getRadius());
        cc->setHeight(ccRecord->m_controllerParams.getHeight());
        break;
      }
      case MR::CharacterControllerDef::BoxShape:
      {
        NxBoxController* cc = (NxBoxController*) ccRecord->m_nxController;
        NxVec3 extents;
        // The box extents are half-extents.
        extents.x = ccRecord->m_controllerParams.getWidth() * 0.5f;
        extents.y = ccRecord->m_controllerParams.getHeight() * 0.5f;
        extents.z = ccRecord->m_controllerParams.getDepth() * 0.5f;
        cc->setExtents(extents);
        break;
      }
      default:
        NMP_ASSERT_FAIL(); // Not a valid shape type
    }

    // We have scaled the height of the character controller so we also need to correct its position.
    NMP::Vector3 offsetChange = ccRecord->m_originOffset - origOffset;
    if (offsetChange.magnitude() != 0.0f)
    {
      offsetChange += (worldUpDirection * 0.001f);
      ccRecord->m_nxController->setPosition(ccRecord->m_nxController->getPosition() + NxExtendedVec3(offsetChange.x, offsetChange.y, offsetChange.z));
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void DefaultControllerMgr::makeLinkBetweenPhysicsRigAndController(
  MCOMMS::InstanceID instanceID,
  MR::PhysicsRig*    physicalBody)
{
  if (physicalBody)
  {
    MR::PhysicsRigPhysX2* physXbody = (MR::PhysicsRigPhysX2*) physicalBody;
    physXbody->setCharacterControllerActor(getCharacterControllerActor(instanceID));
  }
}

//----------------------------------------------------------------------------------------------------------------------
PhysXActor* DefaultControllerMgr::getCharacterControllerActor(MCOMMS::InstanceID instanceID)
{
  Records::const_iterator it = m_ccRecords.find(instanceID);
  if (it != m_ccRecords.end())
  {
    const ControllerRecord* ccRecord = it->second;
    if (ccRecord->m_nxController)
      return ccRecord->m_nxController->getActor();
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
    if (ccRecord->m_nxController)
    {
      NMP::Vector3 gp = ccRecord->m_graveyardPos;
      // teleport CCM physX object to graveyard
      ccRecord->m_nxController->setPosition(NxExtendedVec3(gp.x, gp.y, gp.z));
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
    if (ccRecord->m_nxController && !ccRecord->m_controllerEnabled)
    {
      // Teleport CC physX object back from graveyard.
      NMP::Vector3 p;
      p = ccRecord->m_characterPosition + computeWorldSpaceCCOriginOffset(ccRecord);

      ccRecord->m_nxController->setPosition(NxExtendedVec3(p.x, p.y, p.z));
      ccRecord->m_controllerEnabled = true;
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
    if (ccRecord->m_nxController)
      m_manager->releaseController(*ccRecord->m_nxController);

    m_physicsManager->destroyPhysicsRig(ccRecord->m_network);

    if (ccRecord->m_connectRepresentation)
    {
      const MCOMMS::SceneObjectID sceneObjectID = ccRecord->m_connectRepresentation->getSceneObjectID();

      // The connect representation of the controller is owned by the connection and may therefore have been destroyed
      // as part of a connection close already.
      if (m_context->getSceneObjectManager()->findSceneObject(sceneObjectID))
      {
        SceneObjectRecord* const sceneObject = m_context->getSceneObjectManager()->removeSceneObjectRecord(sceneObjectID);
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
    if (ccRecord->m_nxController && ccRecord->m_controllerEnabled)
    {
      ccRecord->m_characterOrientation = rootTransform.m_quat;
      ccRecord->m_characterPosition = rootTransform.m_pos;

      NMP::Vector3 p;
      p = ccRecord->m_characterPosition + computeWorldSpaceCCOriginOffset(ccRecord);
      ccRecord->m_nxController->setPosition(NxExtendedVec3(p.x, p.y, p.z));
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
  if (!positionOffsetValue.compare(ccRecord->m_overrideBasics.m_currentPositionOffset, 0.000001f))
  {
    ccRecord->m_overrideBasics.m_currentPositionOffset = positionOffsetValue;
   
    NMP::PosQuat characterTransform;
    characterTransform.m_quat = ccRecord->m_characterOrientation;
    characterTransform.m_pos = ccRecord->m_characterPosition;
    teleport(instanceID, characterTransform);
  }

  //------------------------
  // Abs position override.
  NMP::Vector3 absPositionValue = ccRecord->m_overrideBasics.m_currentPositionAbs;
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
  NxU32 collisionGroups = (1 << MR::GROUP_COLLIDABLE_NON_PUSHABLE) | (1 << MR::GROUP_COLLIDABLE_PUSHABLE);
  NxU32 flags;
  NxF32 sharpness = 1.0f;

  // go through all controller records, updating them
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    ControllerRecord* ccRecord = it->second;
    MCOMMS::InstanceID instanceID = it->first;
    ccRecord->m_characterPositionOld = ccRecord->m_characterPosition;
    MR::PhysicsRig* physicsRig = getPhysicsRig(ccRecord->m_network);

    //-------------------  
    // Applying any override properties that have been set this frame.
    applyOverriddenProperties(instanceID, ccRecord);

    //-------------------  
    if (physicsRig && physicsRig->isReferenced() &&
        ccRecord->m_network->getRootControlMethod() == MR::Network::ROOT_CONTROL_PHYSICS)
      disableCollision(instanceID);
    else
      enableCollision(instanceID);


    if (ccRecord->m_controllerEnabled)
    { // ... if the controller is switched on, update it according to Nx-controller semi-physics
      
      // requestedMovement = the desired movement, as calculated from the animation system, that we are trying
      //  to achieve.
      
      // Calculate the requested movement of the character controller:
      //  The supplied requested movement is for the character. To find the requested movement of the character controller
      //  we need to find out where it should be if the requested character movement was achieved.
      NxExtendedVec3 currentCCPosPhysX = ccRecord->m_nxController->getPosition();
      NMP::Vector3 currentCCPos((float) currentCCPosPhysX.x, (float) currentCCPosPhysX.y, (float) currentCCPosPhysX.z);
      NMP::Vector3 requestedCCPos = ccRecord->m_characterPosition + ccRecord->m_requestedMovement + computeWorldSpaceCCOriginOffset(ccRecord);
      NMP::Vector3 requestedMovement = requestedCCPos - currentCCPos;

      // Calculate horizontal movement
      NMP_ASSERT(m_physicsManager && m_physicsManager->getPhysicsScene());
      NMP::Vector3 worldUpDirection = m_physicsManager->getPhysicsScene()->getWorldUpDirection();
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
      // Execute physX's 'collide and slide' controller movement function.
      ccRecord->m_hitReport->updatePreMove(dt, ccRecord->m_controllerParams.getMaxPushForce());
      ccRecord->m_nxController->move(
                                    MR::nmVector3ToNxVec3(requestedMovement),
                                    collisionGroups,
                                    0.00000001f,
                                    flags,
                                    sharpness);

      // Determine whether character is on the ground or in the air, and whether it has
      //  just transitioned between the two.
      ccRecord->m_onGroundOld = ccRecord->m_onGround;
      ccRecord->m_onGround = (flags & NXCC_COLLISION_DOWN) != 0 ;

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

  m_manager->updateControllers();

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
      ccRecord->m_nxController->move(
                                    NxVec3(-ccRecord->m_extraVerticalMove.x,
                                           -ccRecord->m_extraVerticalMove.y,
                                           -ccRecord->m_extraVerticalMove.z),
                                    collisionGroups,
                                    0.00000001f,
                                    flags,
                                    sharpness);
    }
  }
  m_manager->updateControllers();
#endif

  // The NxController->Move(desired) function above has changed the position of the NxController according to the
  // desired change from animation filtered through the NxController's semi-physical collision detection.
  // So we now need to update the ccmRecord's m_characterPosition to reflect this new value, and also set the
  // root position for the network's animation data.  We do this for every controller in turn.
  for (Records::iterator it = m_ccRecords.begin(); it != m_ccRecords.end(); ++it)
  {
    ControllerRecord* ccRecord = it->second;
    MCOMMS::InstanceID instanceID = it->first;

    // The updated position of the NxController
    NxExtendedVec3 newPos = ccRecord->m_nxController->getPosition();

    if (ccRecord->m_controllerEnabled)
    {
      NMP::Vector3 offset = computeWorldSpaceCCOriginOffset(ccRecord);
      ccRecord->m_characterPosition.set((float) newPos.x, (float) newPos.y, (float) newPos.z);
      // Take off the offset value to translate the character position down from the center of
      //  the controller body to where the feet should be.
      ccRecord->m_characterPosition -= offset;

      // Determine whether the character achieved the requested movement.
      bool achievedRequestedMovement = true;
      if (ccRecord->m_onGround)
      {
        // Calculate actual horizontal movement.
        NMP::Vector3 actualTranslation = (ccRecord->m_characterPosition - ccRecord->m_characterPositionOld);
        NMP_ASSERT(m_physicsManager && m_physicsManager->getPhysicsScene());
        NMP::Vector3 worldUpDirection = m_physicsManager->getPhysicsScene()->getWorldUpDirection();
        float amountInUpDirection = worldUpDirection.dot(actualTranslation);
        NMP::Vector3 vectorInUpDirection = worldUpDirection * amountInUpDirection;
        NMP::Vector3 actualHorizontalMovementVector = actualTranslation - vectorInUpDirection;

        // Compare the horizontal movement.
        NMP::Vector3 deltaMovementVector = actualHorizontalMovementVector - ccRecord->m_requestedHorizontalMovement;
        if (deltaMovementVector.magnitude() > (1e-4f * ccRecord->m_controllerParams.getHeight()))
          achievedRequestedMovement = false;
      }
      ccRecord->m_network->setCharacterPropertiesAchievedRequestedMovement(achievedRequestedMovement);

      // Set the world root transform for the network
      //  (this also determines the deltaTranslation due to gravity)
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
MCOMMS::SceneObject* DefaultControllerMgr::addNewControllerRepresentation(MCOMMS::SceneObjectID networkInstanceObjectID, MCOMMS::Connection* owner)
{
  // CharacterControllerDef
  // IMPORTANT:
  // 1. There already exists at this point a scene object that describes the character itself; this
  //    is the scene object ID that was passed in.
  // 2. The job of this function is to add a SECOND scene object at the same position as the character
  //    with the parameters of the CCMRecord.
  // 3. The CCMRecord should already have been set up for the character before this function is called.
  MCOMMS::SceneObject* characterSceneObject = m_context->getSceneObjectManager()->findSceneObject(networkInstanceObjectID);
  if (characterSceneObject)
  {
    MCOMMS::Attribute* instanceIDAttr = characterSceneObject->getAttribute(MCOMMS::Attribute::SEMANTIC_INSTANCEID);
    NMP_ASSERT(instanceIDAttr);
    MCOMMS::InstanceID instanceID = *static_cast<MCOMMS::InstanceID*>(instanceIDAttr->getData());

    ControllerRecord* ccRecord = m_ccRecords[instanceID];
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
    float height = ccRecord->m_network->getActiveCharacterControllerDef()->getHeight();
    float width = ccRecord->m_network->getActiveCharacterControllerDef()->getWidth();
    float depth = ccRecord->m_network->getActiveCharacterControllerDef()->getDepth();
    float radius = ccRecord->m_network->getActiveCharacterControllerDef()->getRadius();
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
    DefaultSceneObjectMgr* soMgr = static_cast<DefaultSceneObjectMgr*>(MCOMMS::getRuntimeTarget()->getSceneObjectManager());
    NMP_ASSERT(soMgr);

    MCOMMS::SceneObjectID controllerID = soMgr->getUnusedSceneObjectID();
    SceneObjectRecord* controller =
      SceneObjectRecord::create(
        controllerID,
        (uint32_t)attributes.size(),
        &attributes[0],
        owner);

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
    getControllerRecord(instanceID)->m_network->startUpdate(dt, false, true);
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
    MR::Network* network = getControllerRecord(it->first)->m_network;

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
