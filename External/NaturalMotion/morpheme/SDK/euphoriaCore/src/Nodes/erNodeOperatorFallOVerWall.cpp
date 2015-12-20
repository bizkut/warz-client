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
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorFallOverWall.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRig.h"
#include "physics/mrPhysicsScene.h"
#include "physics/mrPhysicsRig.h"
#include "euphoria/erBody.h"
#include "euphoria/erLimb.h"
#include "euphoria/erCharacter.h"
#include "morpheme/mrInstanceDebugInterface.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "euphoria/erPinInterface.h"
#include "NMGeomUtils/NMGeomUtils.h"

//----------------------------------------------------------------------------------------------------------------------

bool g_calcWallConfigFromCharacterPos = true;

namespace ER
{

// Data used in the performance update refreshed each frame but not persistent between frames.
//
struct AttribDataOperatorFallOverWallState::PerFrameState
{
  // Set everything to zero
  PerFrameState() {memset(this, 0, sizeof(*this));}

  // Returns true if the head part is past the wall edge
  bool isHeadPastEdge()const;

  // Returns true if chest part is past the wall edge
  bool isChestPastEdge()const;

  // Returns true if the pelvis is past the wall edge.
  bool isPelvisPastEdge()const;

  // Returns true if the COM is past the wall edge.
  bool isCOMPastEdge()const;

  // Returns true if the COM is predicted to be the specified distance past the wall edge after the given timestep.
  bool isComPastEdgePredicted(float timestep, float distance) const;

  // Returns true if the pelvis is predicted to be the specified distance past the wall edge after the given timestep.
  bool isPelvisPastEdgePredicted(float timestep, float distance) const;

  // Returns true if the hand is past the wall edge.
  bool isHandPastEdge(int i)const;

  // Returns true if the foot is past the wall edge.
  bool isFootPastEdge(int i)const;

  // Returns true if the hand, elbow and shoulder joints are the specified distance past the wall edge.
  bool isArmPastEdge(int i, float threshold = 0.0f) const;

  // Returns true if the above are true and at least one foot is also past the wall edge.
  bool isBodyPastEdge()const;

  // Returns true if character is facing the wall.
  bool isFacingWall()const;

  // Returns true if the character's head is lower than the feet.
  bool isInDive()const;

  // Returns true if any of chest, pelvis, avg of feet, hands if
  // horizontally within the specified distance from the wall edge.
  bool isWithinDistanceToWall(float maxDistance)const;

  // Returns true if any part from pelvis down is horizontally within the specified distance of 
  // the wall edge.
  bool isLowerBodyWithinDistanceToWall(float maxDistance)const;

  // Returns true if any parts of the specified leg is horizontally within the specified distance from
  // the wall edge.
  bool isLegWithinDistanceToWall(int i, float maxDistance)const;

  // Returns true if the pelvis is horizontally within the specified distance from the wall edge.
  bool isPelvisWithinDistanceToWall(float maxDistance)const;

  // Returns true if the chest is horizontally within the specified distance from the wall edge.
  bool isChestWithinDistanceToWall(float maxDistance)const;

  // Returns true if the COM is approaching the wall with velocity greater than or equal to that specified
  bool isMovingTowardsWall(float minVel)const;

  float calcCOMToWallNormalDistance() const { return m_comToWall.dot(m_wallNormal); }

  // Per frame physics and euphoria data
  //
  NMP::Vector3 m_headPos;
  NMP::Vector3 m_chestPos;
  NMP::Vector3 m_pelvisForward;
  NMP::Vector3 m_pelvisPos;
  NMP::Vector3 m_pelvisVel;
  NMP::Vector3 m_avgfootPos;
  NMP::Vector3 m_footPos[s_numLegs];
  NMP::Vector3 m_kneePos[s_numLegs];
  NMP::Vector3 m_hipPos[s_numLegs];
  NMP::Vector3 m_handPos[s_numArms];
  NMP::Vector3 m_elbowPos[s_numArms];
  NMP::Vector3 m_shoulderPos[s_numArms];
  NMP::Vector3 m_comPos;
  NMP::Vector3 m_spineUp;
  NMP::Vector3 m_comToWall;
  NMP::Vector3 m_comVel;
  NMP::Vector3 m_angVelAboutClosestEdgePoint;
  NMP::Matrix34 m_inertia;
  NMP::Vector3 m_closestEdgePoint;
  NMP::Vector3 m_wallNormal;
  NMP::Vector3 m_worldUp;
  NMP::Vector3 m_gravity;
  float        m_mass;
  float        m_timeStep;
  float        m_balanceAmount;
  float        m_comAngleToWallNormal;
  float        m_comHeight;
  float        m_wallEdgeHeight;
  float        m_edgeProjectionCoordinate;

  MR::Network* m_network;
  MR::PhysicsRig* m_physicsRig;
  const ER::DimensionalScaling* m_scaling;
  ER::Character* m_character;
  MR::PhysicsScene* m_physicsScene;
};

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorFallOverWallDef functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorFallOverWallDef::endianSwap()
{
  NMP::endianSwap(m_reachTriggerDistance);
  NMP::endianSwap(m_reachTriggerVelocity);
  NMP::endianSwap(m_targetRollSpeed);
  NMP::endianSwap(m_sideRollAmount);
  NMP::endianSwap(m_overWallTimeout);
  NMP::endianSwap(m_fallenNotTriggeredTimeout);
  NMP::endianSwap(m_stalledTimeout);
  NMP::endianSwap(m_settleTimeout);
  NMP::endianSwap(m_legKickAmountDefault);
  NMP::endianSwap(m_targetApproachVelDefault);
  NMP::endianSwap(m_targetRollOverVelDefault);
  NMP::endianSwap(m_maxWallHeightOffset);
  NMP::endianSwap(m_fallTriggerDistance);
  NMP::endianSwap(m_attractorTriggerDistance);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorFallOverWallDef::locate(MR::AttribData* target)
{
  AttribDataOperatorFallOverWallDef* result = (AttribDataOperatorFallOverWallDef*)target;
  MR::AttribData::locate(target);
  result->endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorFallOverWallDef::dislocate(MR::AttribData* target)
{
  AttribDataOperatorFallOverWallDef* result = (AttribDataOperatorFallOverWallDef*)target;
  MR::AttribData::dislocate(target);
  result->endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorFallOverWallDef* AttribDataOperatorFallOverWallDef::create(
  NMP::MemoryAllocator* allocator,
  uint16_t              refCount)
{
  AttribDataOperatorFallOverWallDef* result;

  NMP::Memory::Format memReqs = AttribDataOperatorFallOverWallDef::getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
  NMP_ASSERT(resource.ptr);
  result = AttribDataOperatorFallOverWallDef::init(resource, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorFallOverWallDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataOperatorFallOverWallDef), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorFallOverWallDef* AttribDataOperatorFallOverWallDef::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorFallOverWallDef* result = (AttribDataOperatorFallOverWallDef*)resource.ptr;
  resource.increment(sizeof(AttribDataOperatorFallOverWallDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_FALLOVERWALL_DEF);
  result->setRefCount(refCount);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// Distributes an impulse over the whole physics rig optionally excluding arm and/or leg limbs as specified.
//
static void applyImpulseToWholeCharacterExceptArmsOrLegs( 
  ER::Character* character,
  MR::Network* network,
  const NMP::Vector3& impulse,        // linear force applied to character's CoM.
  const NMP::Vector3& torqueImpulse,  // torque applied to character's CoM.
  bool skipArms,                      // true excludes arm limbs
  bool skipLegs                       // true excludes leg limbs
  )
{
  NMP_ASSERT(character);
  if (!character)
  {
    return;
  }

  // Build a "use parts mask" to exclude arms/legs as required
  //
  // Allocate
  bool* usePartFlags = NULL;
  const int32_t numRigParts = character->getBody().getPhysicsRig()->getNumParts();
  NMP::TempMemoryAllocator* allocator = network->getTempMemoryAllocator();
  if (allocator)
  {
    NMP::Memory::Resource resource 
      = allocator->allocateFromFormat(NMP::Memory::Format(sizeof(bool) * numRigParts));
    NMP_ASSERT(resource.ptr);
    if (resource.ptr)
    {
      usePartFlags = (bool*)resource.ptr;
    }
  }
  NMP_ASSERT(usePartFlags);
  if (!usePartFlags)
  {
    return;
  }
  // Init
  memset((void*)usePartFlags, false, numRigParts * sizeof(bool));
  const int numLimbs = character->getBody().getNumLimbs();
  for (int i = 0; i < numLimbs; ++i)
  {
    // Skip arm and leg limbs
    LimbTypeEnum::Type limbType = character->getBody().getLimb(i).getType();
    if ( (skipArms && limbType == LimbTypeEnum::L_arm) || (skipLegs && limbType == LimbTypeEnum::L_leg))
    {
      continue;
    }

    // Set flag entries for parts in included limbs
    ER::Limb& limb = character->getBody().getLimb(i);
    for (uint32_t partIndex = limb.getIsRootLimb() ? 0 : 1; partIndex < limb.getTotalNumParts(); ++partIndex)
    {
#ifdef NMP_ENABLE_ASSERTS
      const int32_t rigPartIndex = limb.getPhysicsRigPartIndex(partIndex);
      NMP_ASSERT( (0 <= rigPartIndex) && (rigPartIndex < numRigParts) );
#endif
      usePartFlags[limb.getPhysicsRigPartIndex(partIndex)] = true;
    }
  }

  // Apply the impulse
  character->getBody().getPhysicsRig()->receiveWrenchImpulse(impulse, torqueImpulse, usePartFlags);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorFallOverWallState functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorFallOverWallState::locate(MR::AttribData* target)
{
  AttribDataOperatorFallOverWallState* result = (AttribDataOperatorFallOverWallState*)target;

  MR::AttribData::locate(target);
  NMP::endianSwap(result);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorFallOverWallState::dislocate(MR::AttribData* target)
{
  AttribDataOperatorFallOverWallState* result = (AttribDataOperatorFallOverWallState*)target;

  MR::AttribData::dislocate(target);
  endianSwap(result);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorFallOverWallState::endianSwap(AttribDataOperatorFallOverWallState* state)
{
  NMP::endianSwap(state->m_wallEdgePoint0);
  NMP::endianSwap(state->m_wallEdgePoint1);
  NMP::endianSwap(state->m_wallNormal);
  NMP::endianSwap(state->m_wallEdgeDir);
  for (int32_t i = 0; i < s_numArms; ++i)
  {
    NMP::endianSwap(state->m_reachTargetAnchors[i]);
  }
  NMP::endianSwap(state->m_closestHandIndex);
  NMP::endianSwap(state->m_signOfSideRoll);
  NMP::endianSwap(state->m_fallenNotTriggeredTime);
  NMP::endianSwap(state->m_stallingTime);
  NMP::endianSwap(state->m_overWallEdgeTime);
  NMP::endianSwap(state->m_settlingTime);
  NMP::endianSwap(state->m_resetMessageID);
  NMP::endianSwap(state->m_lastUpdateFrame);
  NMP::endianSwap(state->m_active);
  NMP::endianSwap(state->m_fallTriggered);
  NMP::endianSwap(state->m_sideRollTriggered);
  NMP::endianSwap(state->m_overWallEdge);
  NMP::endianSwap(state->m_stalled);
  NMP::endianSwap(state->m_reset);
  NMP::endianSwap(state->m_reachTriggered);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorFallOverWallState::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataOperatorFallOverWallState), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorFallOverWallState* AttribDataOperatorFallOverWallState::init(
  NMP::Memory::Resource& resource,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorFallOverWallState* result = (AttribDataOperatorFallOverWallState*)resource.ptr;
  resource.increment(sizeof(AttribDataOperatorFallOverWallState));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_FALLOVERWALL_STATE);
  result->setRefCount(refCount);
  result->m_lastUpdateFrame = 0;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataHandle AttribDataOperatorFallOverWallState::create(NMP::MemoryAllocator* allocator)
{
  MR::AttribDataHandle result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, 0);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}


//----------------------------------------------------------------------------------------------------------------------
// Performance specific
//----------------------------------------------------------------------------------------------------------------------

void AttribDataOperatorFallOverWallState::reset()
{
  // Some config state
  m_wallEdgePoint0.setToZero();
  m_wallEdgePoint1.setToZero();

  // State flags
  m_active            = false;  // set to true when performance is deemed to have started, false on completion
  m_fallTriggered     = false;  // set to true to allow cheat forces to run at all
  m_overWallEdge      = false;  // true if all parts are over the wall
  m_reset             = false;  // set to true if a reset is required on the next update
  m_stalled           = false;
  m_reachTriggered    = false;
  m_sideRollTriggered = false;

  // Timers
  m_fallenNotTriggeredTime      = 0.0f;
  m_stallingTime                = 0.0f;
  m_overWallEdgeTime            = 0.0f;
  m_settlingTime                = 0.0f;
  m_maxCOMAngleToWallNormal     = - NM_PI_OVER_TWO;
  m_maxCOMHeight                = -FLT_MAX;
  m_resetMessageID              = 0;

}


//----------------------------------------------------------------------------------------------------------------------
// "Pre behaviour update"
// Reads character dynamic state and calcs relative position of character wrt to wall
// returns true if network euphoria + physics data is available, false otherwise 
bool ER::AttribDataOperatorFallOverWallState::preUpdate(
                        MR::Network* net,
                        const NMP::Vector3& wallEdgePoint0,
                        const NMP::Vector3& wallEdgePoint1, 
                        float balanceAmount,
                        ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  // First determine that we have all the associated morpheme, and euphoria bits, return false if not.
  //
  perFrameState.m_network = net;
  NMP_ASSERT(perFrameState.m_network);
  if (!perFrameState.m_network)
  {
    perFrameState.m_physicsRig = 0;
    perFrameState.m_character = 0;
    perFrameState.m_physicsScene = 0;
    m_active = false;
    return false;
  }
  perFrameState.m_physicsRig = getPhysicsRig(net);
  perFrameState.m_character = networkGetCharacter(net);
  if (perFrameState.m_physicsRig && perFrameState.m_character)
  {
    perFrameState.m_physicsScene = perFrameState.m_physicsRig->getPhysicsScene();
  }  
  if (!perFrameState.m_physicsScene)
  {
    m_active = false;
    return false;
  }
  m_active = true;

  // Then acquire all the behaviour/dynamic state and positions of significant character parts
  //

  perFrameState.m_balanceAmount = balanceAmount;
  perFrameState.m_gravity = perFrameState.m_physicsScene->getGravity();
  perFrameState.m_worldUp = perFrameState.m_physicsScene->getWorldUpDirection();
  perFrameState.m_timeStep = perFrameState.m_physicsScene->getNextPhysicsTimeStep();
  perFrameState.m_comPos = perFrameState.m_physicsRig->calculateCentreOfMass();
  perFrameState.m_comVel = perFrameState.m_physicsRig->calculateCentreOfMassVelocity();
  perFrameState.m_inertia = perFrameState.m_physicsRig->calculateGlobalInertiaTensor();
  perFrameState.m_mass = perFrameState.m_physicsRig->calculateMass();
  ER::Body& body = perFrameState.m_character->getBody();
  perFrameState.m_scaling = &(body.getDimensionalScaling());

  // Wall info is calc'd only once when wall data is new this is determined by a difference of squares between the
  // current and the stored edge point inputs
  float diffSqtest 
    = (wallEdgePoint0 - m_wallEdgePoint0).magnitudeSquared() + (wallEdgePoint1 - m_wallEdgePoint1).magnitudeSquared();
  bool newWallData = (diffSqtest > 0.00001f);
    
  // Recompute the wall basis frame if we have new wall edge data
  if (newWallData)
  {
    // Calculate the basis directions without regard to sense
    //
    m_wallEdgePoint0 = wallEdgePoint0;
    m_wallEdgePoint1 = wallEdgePoint1;
    m_wallEdgeDir = m_wallEdgePoint1 - m_wallEdgePoint0;
    m_wallEdgeDir.fastNormalise();
    m_wallNormal = NMP::vCross(perFrameState.m_worldUp, m_wallEdgeDir );

    // Calculate the point on the wall edge closest to the character
    perFrameState.m_closestEdgePoint =
      NMRU::GeomUtils::getPointProjectedOntoLine(
        m_wallEdgePoint0, m_wallEdgePoint1, perFrameState.m_comPos, &perFrameState.m_edgeProjectionCoordinate);

    // Optional auto detection of character/wall config
    // Adjusts wall direction vectors (basis) according to what side the character is currently on
    // so wall normal points towards the character at the start of the FoW
    //
    if (g_calcWallConfigFromCharacterPos)
    {
      // Adjust signs of wall edge and normal depending on what side the com is on
      //
      NMP::Vector3 toWall = perFrameState.m_closestEdgePoint - perFrameState.m_comPos;
      // Flip edge and normal calc'd above, if wall normal is not pointing at the character.
      if ( NMP::vDot(m_wallNormal, toWall) > 0)
      {
        m_wallEdgeDir *= -1;
        m_wallNormal *= -1;
      }
    }
  }
  // Refresh or calculate wall relative info.
  //
  perFrameState.m_closestEdgePoint =
    NMRU::GeomUtils::getPointProjectedOntoLine(
    m_wallEdgePoint0, m_wallEdgePoint1, perFrameState.m_comPos, &perFrameState.m_edgeProjectionCoordinate);
  perFrameState.m_wallNormal = m_wallNormal;
  perFrameState.m_comToWall = perFrameState.m_closestEdgePoint - perFrameState.m_comPos;
  perFrameState.m_angVelAboutClosestEdgePoint = NMP::vCross(perFrameState.m_comPos - perFrameState.m_closestEdgePoint, perFrameState.m_comVel);

  // Compute angle between com pos and wall-normal, as measured from a reference point well below the edge
  // (reference point is dropped to give a smoother angle in the fall-over operating range).
  NMP::Vector3 wallCOMRef = perFrameState.m_closestEdgePoint - perFrameState.m_worldUp * perFrameState.m_scaling->scaleDist(2.0f);
  NMP::Vector3 wallToCOMRef = perFrameState.m_comPos - wallCOMRef;
  float x = wallToCOMRef.dot(perFrameState.m_wallNormal);
  float y = wallToCOMRef.dot(perFrameState.m_worldUp);
  perFrameState.m_comAngleToWallNormal = atan2(y, x);
  // Transform result so that it lies in [0, 270] or [-90, 0] (so angles increase with progress over wall)
  if (perFrameState.m_comAngleToWallNormal < - NM_PI_OVER_TWO)
  {
    perFrameState.m_comAngleToWallNormal += NM_PI * 2.0f;
  }

  // Register current com and wall heights
  perFrameState.m_comHeight = perFrameState.m_comPos.dot(perFrameState.m_worldUp);
  perFrameState.m_wallEdgeHeight = perFrameState.m_closestEdgePoint.dot(perFrameState.m_worldUp);
  // Update max com height as required
  if (m_maxCOMHeight < perFrameState.m_comHeight)
  {
    m_maxCOMHeight = perFrameState.m_comHeight;
  }

  // Read individual part data
  //
  perFrameState.m_headPos  = body.getLimb(body.m_definition->getFirstHeadLimbIndex()).getEndTransform().translation();
  int32_t firstSpineIndex = body.m_definition->getFirstSpineLimbIndex();
  perFrameState.m_chestPos  = body.getLimb(firstSpineIndex).getEndTransform().translation();
  perFrameState.m_pelvisPos  = body.getLimb(firstSpineIndex).getRootTransform().translation();
  perFrameState.m_pelvisForward = body.getLimb(firstSpineIndex).getRootTransform().xAxis();
  perFrameState.m_pelvisVel = perFrameState.m_physicsRig->getPart(body.getLimb(firstSpineIndex).getRootIndex())->getVel();
  perFrameState.m_spineUp = perFrameState.m_chestPos - perFrameState.m_pelvisPos;
  NMP_ASSERT(perFrameState.m_spineUp.magnitudeSquared() > 0.001f); // Sanity check.
  perFrameState.m_spineUp.fastNormalise();

  const int32_t firstArmIndex = body.m_definition->getFirstArmLimbIndex();
  for (int32_t i = 0; i < s_numArms; ++i)
  {
    perFrameState.m_handPos[i] = body.getLimb( firstArmIndex + i).getEndTransform().translation();
    perFrameState.m_shoulderPos[i] = body.getLimb( firstArmIndex + i).getBaseTransform().translation();
    perFrameState.m_elbowPos[i] = perFrameState.m_physicsRig->getPart(
      body.getLimb( firstArmIndex + i).getMidIndex())->getTransform().translation();
  }

  const int32_t firstLegIndex = body.m_definition->getFirstLegLimbIndex();
  for (int32_t i = 0; i < s_numLegs; ++i)
  {
    perFrameState.m_footPos[i] = body.getLimb( firstLegIndex + i).getEndTransform().translation();
    perFrameState.m_hipPos[i] = body.getLimb( firstLegIndex + i).getBaseTransform().translation();
    perFrameState.m_kneePos[i] = perFrameState.m_physicsRig->getPart(
      body.getLimb( firstLegIndex + i).getMidIndex())->getTransform().translation();
    perFrameState.m_avgfootPos  += perFrameState.m_footPos[i];
  }
  perFrameState.m_avgfootPos /= (float)s_numLegs;

#if defined MR_OUTPUT_DEBUGGING
  debugDrawCharacterToWallRelationship(perFrameState);
#endif

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void ER::AttribDataOperatorFallOverWallState::updateTimers(
  float stalledTimeout,
  const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  if (m_fallTriggered)
  {
    // Stalling condition: check progress on angle of com to wall edge. Fall is deemed to have stalled if this is 
    // not increasing.
    if (m_maxCOMAngleToWallNormal < perFrameState.m_comAngleToWallNormal)
    {
      m_stallingTime = 0.0f;
      if (m_maxCOMAngleToWallNormal < perFrameState.m_comAngleToWallNormal)
      {
          m_maxCOMAngleToWallNormal = perFrameState.m_comAngleToWallNormal;
      }
    }
    else
    {
      m_stallingTime += perFrameState.m_timeStep;
    }

    // Over wall means: passed point of no return i.e. successful completion
    if (perFrameState.isBodyPastEdge())
    {
      m_overWallEdge = true;
      m_overWallEdgeTime += perFrameState.m_timeStep;
    }
    else
    {
      m_overWallEdge = false;
      m_overWallEdgeTime = 0.0f;
    }

    // Falling means: FoW triggered and the character is falling but not over wall yet
    // triggered && !balancing && !overwall
    //
    if (m_stallingTime > stalledTimeout)
    {
      m_stalled = true;
      m_settlingTime += perFrameState.m_timeStep;
    }
    else
    {
      m_stalled = false;
      m_settlingTime = 0.0f;
    }
  }

  // Determine fallen not triggered
  if (m_active)
  {
    if (!m_fallTriggered && perFrameState.m_balanceAmount < 0.01)
    {
      m_fallenNotTriggeredTime += perFrameState.m_timeStep;
    }
    else
    {
      m_fallenNotTriggeredTime = 0.0f;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorFallOverWallState::updateEmittedMessages(
  float overWallTimeout,
  float settleTimeout,
  float fallenNotTriggeredTimeout
  )
{
  m_resetMessageID = AttribDataOperatorFallOverWallState::MSG_NONE;

  // Success: FoW triggered and then completed successfully and timed-out
  if (m_overWallEdge && m_overWallEdgeTime > overWallTimeout)
  {
    m_reset = true;
    m_resetMessageID = AttribDataOperatorFallOverWallState::MSG_COMPLETED;
    return;
  }

  // Fail: FoW triggered but stalled and timed-out
  if (m_stalled && m_settlingTime > settleTimeout) 
  {
    m_reset = true;
    m_resetMessageID = AttribDataOperatorFallOverWallState::MSG_STALLED;
    return;
  }

  // Fail: FoW never triggered but lost balance and timed-out
  // active && !triggered && !balancing 
  if (m_fallenNotTriggeredTime > fallenNotTriggeredTimeout)
  {
    m_reset = true;
    m_resetMessageID = AttribDataOperatorFallOverWallState::MSG_FALLEN_NOT_TRIGGERED;
    return;
  }
}

//----------------------------------------------------------------------------------------------------------------------
void ER::AttribDataOperatorFallOverWallState::updateLegAndSpineStrengths(
  float fallTriggerDistance,
  float overWallTimeout,
  float& spineStrength,
  float* legStrength,
  float* legWritheStrength,
  const AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  // Initialise leg and spine strengths to a sensible default (full == 1.0f)
  for (int32_t i = 0; i < s_numLegs; ++i)
  {
    legStrength[i] = 1.0f;
  }
  spineStrength = 1.0f;

  if (perFrameState.isLowerBodyWithinDistanceToWall(fallTriggerDistance) || perFrameState.isChestPastEdge())
  {
    // Default to no strength
    float strength = 0.0f;
    // Ramp strength back to normal as we approach the reset following a successful FoW
    if (m_overWallEdge && m_overWallEdgeTime > 0.0f)
    {
      if (m_overWallEdgeTime <= overWallTimeout)
      {
        strength = (m_overWallEdgeTime / overWallTimeout);
      }
      else
      {
        strength = 1.0f;
      }
    }

    if (perFrameState.isChestPastEdge())
    {
      for (int32_t i = 0; i < s_numLegs; ++i)
      {
        legStrength[i] = strength;
      }
      spineStrength = strength;
    }
    else
    {
      for (int32_t i = 0; i < s_numLegs; ++i)
      {
        if (perFrameState.isLegWithinDistanceToWall(i, fallTriggerDistance))
        {
          legStrength[i] = strength;
        }
      }
      if (perFrameState.isChestWithinDistanceToWall(fallTriggerDistance * 0.5f))
      {
        spineStrength = strength;
      }
    }
  }
  for (int32_t i = 0; i < s_numLegs; ++i)
  {
    legWritheStrength[i] = (1.0f - legStrength[i]);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 ER::AttribDataOperatorFallOverWallState::calcReachTargetAdjustmentForApproachFromAbove(
  const NMP::Vector3& tgtPos,
  const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState,
  int armIndex)
{
  NMP::Vector3 handPos(armIndex == 0 ? perFrameState.m_handPos[0] : perFrameState.m_handPos[1]);
  NMP::Vector3 handToTgt = tgtPos - handPos;
  
  // Only adjust at all if the hand is the "from" side of the wall
  if (handToTgt.dot(perFrameState.m_wallNormal) < 0.0f)
  {
    const float maxLift = perFrameState.m_scaling->scaleDist(0.25f);
    float targetLift = handToTgt.magnitude();
    if (handToTgt.dot(perFrameState.m_worldUp) < 0.0f) // If reaching from above need less lift
    {
      targetLift *= 0.5f;
    }
    return tgtPos + perFrameState.m_worldUp * NMP::minimum(targetLift, maxLift);
  }
  return tgtPos;
}
//----------------------------------------------------------------------------------------------------------------------
void ER::AttribDataOperatorFallOverWallState::updateReachTargets(
  const AttribDataOperatorFallOverWallState::PerFrameState& perFrameState,
  float* reachImportances,
  NMP::Vector3* reachTargets,
  NMP::Vector3* reachNormals)
{
  // Do not reach if pelvis and feet are all past the wall edge
  if (perFrameState.isPelvisPastEdge() && perFrameState.isFootPastEdge(0) && perFrameState.isFootPastEdge(1))
  {
    reachImportances[0] = reachImportances[1] = 0.0f;
    return;
  }

  // If the reach is not yet triggered then update the reach targets (projection of current shoulder positions
  // onto the wall edge).
  // For two armed characters:
  //   in fixing the targets the hand closest to its target at the point of triggering becomes the "dominant" hand, this 
  //   is the one that in practice the character is most likely to pivot about as the fall evolves the other 
  //   hand target is offset out along the wall edge to encourage this and to generally give some asymmetry to the whole 
  //   arrangement.
  //  If at least one hand is close enough then we targets are fixed for the rest of the fall.
  //
  if (!m_reachTriggered)
  {
    float threshold = perFrameState.m_scaling->scaleDist(0.1f);
    float wallToHandDistance[s_numArms];
    m_closestHandIndex = 0;
    for (int32_t i = 0; i < s_numArms; ++i)
    {
      // Reach targets are the projections of the shoulders onto the wall
      m_reachTargetAnchors[i] =
        NMRU::GeomUtils::getPointProjectedOntoLine(m_wallEdgePoint0, m_wallEdgePoint1, perFrameState.m_shoulderPos[i], NULL);
      
      // Record which hand is closest to the wall edge
      wallToHandDistance[i] = (perFrameState.m_handPos[i] - perFrameState.m_closestEdgePoint).magnitude();
      if (i > 0)
      {
        if (wallToHandDistance[i] < wallToHandDistance[i - 1])
        {
          m_closestHandIndex = i;
        }
      }
    }

    // Offseting one of the targets introduces a bit of asymmetry this implementation only really makes sense 
    // for a 2 armed character.
    bool isTwoArmedCharacter = (s_numArms == 2);
    if (isTwoArmedCharacter)
    {
      float shoulderWidth = (perFrameState.m_shoulderPos[1] - perFrameState.m_shoulderPos[0]).magnitude();
      if (m_closestHandIndex == 1)
      {
        m_reachTargetAnchors[0] +=
          shoulderWidth * (m_reachTargetAnchors[0]- m_reachTargetAnchors[1]).getNormalised();
      }
      else
      {
        m_reachTargetAnchors[1] +=
          shoulderWidth * (m_reachTargetAnchors[1] - m_reachTargetAnchors[0]).getNormalised();
      }
    }

    // Trigger reaching (anchor the reach targets) if at least one hand is close enough
    if (wallToHandDistance[m_closestHandIndex] < threshold)
    {
      m_reachTriggered = true;
    }
  }

  // After the reach has been triggered the targets remain anchored and only minor adjustments are made
  // to help minimise hands colliding with the face of the wall.
  // Each arm reaches only until it passes the wall edge which happens naturally as the character goes over.
  //
  float armOverThreshold = perFrameState.m_scaling->scaleDist(0.1f);
  for (int32_t i = 0; i < s_numArms; ++i)
  {
    reachImportances[i] = perFrameState.isArmPastEdge(0, armOverThreshold) ? 0.0f : 1.0f;
    if (reachImportances[i])
    {
      // Target adjustment to have the hands approach from above
      //
      reachTargets[i] = calcReachTargetAdjustmentForApproachFromAbove(m_reachTargetAnchors[i], perFrameState, i);
      NMP::Vector3 targetToHand = (perFrameState.m_handPos[i] - m_reachTargetAnchors[i]);
      float fwd = targetToHand.dot(perFrameState.m_wallNormal);
      float up = targetToHand.dot(perFrameState.m_worldUp);
      NMP::Vector3 n = ( perFrameState.m_wallNormal * fwd + perFrameState.m_worldUp * up).getNormalised();
      reachNormals[i] = n;
    }
  }
#if defined MR_OUTPUT_DEBUGGING
  debugDrawReachTargets(perFrameState, reachImportances, reachTargets, reachNormals );
#endif
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 ER::AttribDataOperatorFallOverWallState::updateStepVelocity(
  float fallTriggerDistance,
  float targetApproachVel,
  const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  // If character is still outside the fall trigger distance and is balanced then calculate
  // a step velocity demand, otherwise just return zero.
  //
  if ( !perFrameState.isPelvisWithinDistanceToWall(fallTriggerDistance) && perFrameState.m_balanceAmount > 0.5f)
  {
    NMP::Vector3 pelvisToWall(perFrameState.m_closestEdgePoint - perFrameState.m_pelvisPos);
    NMP::Vector3 refDirHorizontal = pelvisToWall.getComponentOrthogonalToDir(perFrameState.m_worldUp);
    refDirHorizontal.fastNormalise();
    return targetApproachVel * refDirHorizontal;
  }
  return NMP::Vector3(NMP::Vector3::InitTypeZero);
}

//----------------------------------------------------------------------------------------------------------------------
void ER::AttribDataOperatorFallOverWallState::calculateAndApplyCheatImpulses(
  float fallTriggerDistance,
  float torquesTriggerDistance,
  float targetRollOverVel,
  float targetRollSpeed,
  float sideRollAmount,
  const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  // 1. Attractor impulse (pulls character into the wall)
  //
  NMP::Vector3 attractorImpulse(NMP::Vector3::InitTypeZero);
  float timeFromNow = perFrameState.m_scaling->scaleTime(0.25f);
  float safeDistance = perFrameState.m_scaling->scaleDist(0.1f);
  if (!perFrameState.isPelvisPastEdgePredicted(timeFromNow, safeDistance) 
    || !perFrameState.isComPastEdgePredicted(timeFromNow, safeDistance ))
  {
    attractorImpulse = perFrameState.m_timeStep * calculateAttractorForce(
        fallTriggerDistance, targetRollOverVel, perFrameState);
  }
  // Linear impulse is applied to the whole body except the arms and legs
  applyImpulseToWholeCharacterExceptArmsOrLegs(
    perFrameState.m_character, perFrameState.m_network, attractorImpulse, NMP::Vector3::InitTypeZero, true, true);

  // 2. Roll torques (rotate the character over the wall)
  // 
  if (perFrameState.isLowerBodyWithinDistanceToWall(torquesTriggerDistance))
  {
    NMP::Vector3 edgeOverTorqueImpulse(NMP::Vector3::InitTypeZero);
    NMP::Vector3 edgeAlignTorqueImpulse(NMP::Vector3::InitTypeZero);

    // "Edge over" torque impulse tries to rotate the character over the wall about the edge.
    //
    bool pelvisAndComOver = (perFrameState.isPelvisPastEdge() && perFrameState.isCOMPastEdge());
    bool bothFeetOver = (perFrameState.isFootPastEdge(0) && perFrameState.isFootPastEdge(1));
    if (!(bothFeetOver && pelvisAndComOver) )
    {
      static float edgeVelDamper = perFrameState.m_scaling->scaleDamping(3.125f); //50
      static float edgeVelClamper = perFrameState.m_scaling->scaleAngAccel(5.0f);

      edgeOverTorqueImpulse = perFrameState.m_timeStep *
        calculateEdgeAngAccelerationTorque(
        edgeVelDamper,
        targetRollSpeed,
        edgeVelClamper,
        perFrameState);
    }

    // "Edge align" torque impulse tries to rotate the body into alignment with the wall edge.
    //
    bool feetOver = (perFrameState.isFootPastEdge(0) && perFrameState.isFootPastEdge(1));
    if (perFrameState.isChestPastEdge() && !feetOver)
    {
      edgeAlignTorqueImpulse = perFrameState.m_timeStep * calculateSpineAlignmentTorque(sideRollAmount,perFrameState);
    }

    // The sum of the two torque impulses is applied to the whole body except the arms.
    applyImpulseToWholeCharacterExceptArmsOrLegs(
      perFrameState.m_character, perFrameState.m_network, NMP::Vector3::InitTypeZero, edgeOverTorqueImpulse + edgeAlignTorqueImpulse, true, false);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 ER::AttribDataOperatorFallOverWallState::calculateEdgeAngAccelerationTorque(
  float damping,
  float targetAngVel,
  float maxAngAccel,
  const PerFrameState& perFrameState)
{
  // Compute component of current ang vel about the wall edge for comparison with target
  float edgeAngVel = perFrameState.m_angVelAboutClosestEdgePoint.dot(m_wallEdgeDir);

  // We only care if the ang vel is less than target
  if (edgeAngVel >= targetAngVel)
  {
    return NMP::Vector3::InitTypeZero;
  }

  // Calc magnitude of required correction
  float angAccel = NMP::minimum(damping * targetAngVel - edgeAngVel, maxAngAccel);

  // Convert the angular accel to a torque
  NMP::Vector3 angAccelTorque = (angAccel * m_wallEdgeDir);
  perFrameState.m_inertia.rotateVector(angAccelTorque);


#if defined MR_OUTPUT_DEBUGGING
  debugDrawAngAccelTorqueAsImpulse(
    perFrameState.m_comPos + perFrameState.m_scaling->scaleDist(perFrameState.m_worldUp),
    edgeAngVel * m_wallEdgeDir,
    targetAngVel * m_wallEdgeDir,
    angAccelTorque,
    1.0f,
    perFrameState);
#endif

  return angAccelTorque;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 ER::AttribDataOperatorFallOverWallState::calculateSpineAlignmentTorque(
  const float sideRollAmount,
  const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  // Compute the angular accel to align the (spine up direction with the steps)
  //

  // Early out for anything but a positive non-zero sideRollAmount
  if (sideRollAmount <= 0.0f)
  {
    return NMP::Vector3::InitTypeZero;
  }

  if (!m_sideRollTriggered)
  {
    float descr = perFrameState.m_spineUp.dot(m_wallEdgeDir);
    m_signOfSideRoll = descr > 0.0f ? 1.0f : -1.0f;
    m_sideRollTriggered = true;
  }

  // Correction axis of rotation
  NMP::Vector3 errorToLevelDirection(NMP::vCross(perFrameState.m_spineUp, m_signOfSideRoll * m_wallEdgeDir));

  // The acceleration (using the error calc'd above together with a dimensionally scaled reference gain 
  // scaled by the user specified sideRollAmount)
  const float gain = perFrameState.m_scaling->scaleStrength(15.0f) * sideRollAmount;
  const NMP::Vector3 accel( gain * errorToLevelDirection);
  
  // Torque from inertia and accel just calculated.
  NMP::Vector3 torque(perFrameState.m_inertia.getRotatedVector(accel));
  
  // Some debug output
  //
#if defined MR_OUTPUT_DEBUGGING
  MR::InstanceDebugInterface* debugInterface = perFrameState.m_network->getDispatcher()->getDebugInterface();
  if (debugInterface)
  {
    MR_DEBUG_DRAW_VECTOR(debugInterface,
      MR::VT_TorqueImpulse, 
      perFrameState.m_comPos, perFrameState.m_scaling->scaleDist(1.0f) * torque / perFrameState.m_scaling->scaleTorque(1.0f), NMP::Colour::PURPLE);
    MR_DEBUG_DRAW_VECTOR(debugInterface,
      MR::VT_Normal, 
      perFrameState.m_comPos, perFrameState.m_spineUp * perFrameState.m_scaling->scaleDist(2.0f), NMP::Colour::LIGHT_RED);
    MR_DEBUG_DRAW_VECTOR(debugInterface,
      MR::VT_Normal, 
      perFrameState.m_comPos, m_wallEdgeDir * perFrameState.m_scaling->scaleDist(2.0f) * m_signOfSideRoll, NMP::Colour::LIGHT_GREEN);
  }
#endif

  return torque;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 ER::AttribDataOperatorFallOverWallState::calculateAttractorForce(
  float fallTriggerDistance,
  float targetRollOverVel,
  const ER::AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  // The attractor force is calculated differently according to whether the character is approaching the wall
  // or has actually got there.
  NMP::Vector3 attractorForce(NMP::Vector3::InitTypeZero);

  // On approach to wall (not yet arrived). ///////////////////////////////////////////////////////////////////////////
  // Small attractor force is computed if the COM is vertically stalling.
  if ( !perFrameState.isPelvisWithinDistanceToWall(fallTriggerDistance) )
  {
    if (perFrameState.m_comHeight < m_maxCOMHeight && perFrameState.m_balanceAmount > 0.5f)
    {
      const float liftStrength = perFrameState.m_scaling->scaleStrength(10000.0f);
      const float liftMaxAccel = 1.5f * perFrameState.m_scaling->scaleAccel(9.8f);
      float liftAccel = 
        NMP::clampValue(liftStrength * (m_maxCOMHeight - 0.2f - perFrameState.m_comHeight), 0.0f, liftMaxAccel);
      attractorForce += perFrameState.m_mass * liftAccel * perFrameState.m_worldUp;
    }

#if defined MR_OUTPUT_DEBUGGING
    MR::InstanceDebugInterface* debugInterface = perFrameState.m_network->getDispatcher()->getDebugInterface();
    MR_DEBUG_DRAW_VECTOR( debugInterface, MR::VT_Force, perFrameState.m_comPos, 
      attractorForce * perFrameState.m_scaling->scaleForce(0.01f),
      NMP::Colour::WHITE);
#endif
  }
  // At the wall. //////////////////////////////////////////////////////////////////////////////////////////////////////
  else
  {
    // Make a reference direction for assistance force calcs.
    // Go with pure horizontal opposing wall normal if the character is basically over, otherwise take the direction
    // of shortest path from pelvis to edge.
    //
    NMP::Vector3 referenceDir;
    float lookAheadTime = perFrameState.m_scaling->scaleTime(0.1f);
    float lookAheadDistance = perFrameState.m_scaling->scaleDist(0.1f);
    bool isDefinitelyGoingOver =
      perFrameState.isPelvisPastEdgePredicted(lookAheadTime, lookAheadDistance)
      || perFrameState.isComPastEdgePredicted(lookAheadTime, lookAheadDistance);

    if (isDefinitelyGoingOver)
    {
      referenceDir = -perFrameState.m_wallNormal;
    }    
    else
    {
      NMP::Vector3 closesetPointToPelvis = 
        NMRU::GeomUtils::getPointProjectedOntoLine(m_wallEdgePoint0, m_wallEdgePoint1, perFrameState.m_pelvisPos, NULL);
      float offset = perFrameState.m_scaling->scaleDist(0.3f);
      NMP::Vector3 referencePoint = closesetPointToPelvis 
        + perFrameState.m_worldUp * offset - perFrameState.m_wallNormal * offset;
      referenceDir = referencePoint - perFrameState.m_pelvisPos;
      NMP_ASSERT(referenceDir.magnitudeSquared() > perFrameState.m_scaling->scaleDist(0.01f));
      referenceDir.fastNormalise();
    }

    // Calculate velocity top-up in direction of referenceDir
    float currVel = perFrameState.m_comVel.dot(referenceDir);
    float damping = perFrameState.m_scaling->scaleDamping(40.0f + m_stallingTime * 1000.0f);
    float maxAccel = 1.5f * perFrameState.m_scaling->scaleAccel(9.8f);
    float overWallAccel = NMP::clampValue( damping * (targetRollOverVel - currVel), 0.0f, maxAccel);
    attractorForce = referenceDir * perFrameState.m_mass * overWallAccel;

    // Calculate some lift assistance if the COM is stalled and the character is not past the point of no return.
    if ( (perFrameState.m_comHeight < m_maxCOMHeight) && !isDefinitelyGoingOver)
    {
      const float liftStrength = perFrameState.m_scaling->scaleStrength(m_stallingTime * m_stallingTime * 10000.0f);
      const float liftMaxAccel = 1.5f * perFrameState.m_scaling->scaleAccel(9.8f);
      float liftAccel = 
        NMP::clampValue(liftStrength * (m_maxCOMHeight - 0.02f - perFrameState.m_comHeight), 0.0f, liftMaxAccel);
      attractorForce += perFrameState.m_mass * liftAccel * perFrameState.m_worldUp;
    }

#if defined MR_OUTPUT_DEBUGGING
    MR::InstanceDebugInterface* debugInterface = perFrameState.m_network->getDispatcher()->getDebugInterface();
    const float pointSize = perFrameState.m_scaling->scaleDist(0.5f);
    MR_DEBUG_DRAW_VECTOR( debugInterface, MR::VT_Force, perFrameState.m_comPos + perFrameState.m_worldUp * pointSize, 
      perFrameState.m_scaling->scaleForce(0.01f) * attractorForce,
      NMP::Colour::LIGHT_BLUE);
#endif
  }

  return attractorForce;
}

#if defined MR_OUTPUT_DEBUGGING
//----------------------------------------------------------------------------------------------------------------------
// Debug draw

//----------------------------------------------------------------------------------------------------------------------
// Draw the setup/configuration of character wrt wall
void ER::AttribDataOperatorFallOverWallState::debugDrawCharacterToWallRelationship(
  const AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  float drawPointSize = perFrameState.m_scaling->scaleDist(0.25f);
  MR::InstanceDebugInterface* debugInterface = perFrameState.m_network->getDispatcher()->getDebugInterface();

  NMP::Colour white = NMP::Colour::WHITE;
  NMP::Colour lightRed = NMP::Colour::LIGHT_RED;
  NMP::Colour darkRed = NMP::Colour::DARK_RED;
  NMP::Colour lightGreen = NMP::Colour::LIGHT_GREEN;
  NMP::Colour darkGreen = NMP::Colour::DARK_GREEN;
  NMP::Colour lightBlue = NMP::Colour::LIGHT_BLUE;
  NMP::Colour darkBlue = NMP::Colour::DARK_BLUE;
  NMP::Colour darkOrange = NMP::Colour::DARK_ORANGE;

  // Reserve full alpha for after the fall has been triggered
  if (!m_fallTriggered) 
  {
    white.setAf(0.5f);
    lightRed.setAf(0.5f);
    darkRed.setAf(0.5f);
    lightGreen.setAf(0.5f);
    darkGreen.setAf(0.5f);
    lightBlue.setAf(0.5f);
    darkBlue.setAf(0.5f);
    darkOrange.setAf(0.5f);
  }

  // Draw current position and closest edge relationship
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_closestEdgePoint, drawPointSize, white);
  MR_DEBUG_DRAW_LINE( debugInterface, perFrameState.m_comPos, perFrameState.m_closestEdgePoint, white);

  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_comPos, drawPointSize, lightBlue);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_avgfootPos, drawPointSize, white);

  // Draw the character part positions on which the performance is predicated
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_chestPos, drawPointSize * 0.25f, white);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_pelvisPos, drawPointSize * 0.25f, white);

  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_hipPos[0], drawPointSize * 0.25f, lightBlue);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_hipPos[1], drawPointSize * 0.25f, lightRed);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_kneePos[0], drawPointSize * 0.25f, lightBlue);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_kneePos[1], drawPointSize * 0.25f, lightRed);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_footPos[0], drawPointSize * 0.25f, lightBlue);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_footPos[1], drawPointSize * 0.25f, lightRed);

  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_handPos[0], drawPointSize * 0.25f, lightBlue);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_handPos[1], drawPointSize * 0.25f, lightRed);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_elbowPos[0], drawPointSize * 0.25f, lightBlue);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_elbowPos[1], drawPointSize * 0.25f, lightRed);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_shoulderPos[0], drawPointSize * 0.25f, lightBlue);
  MR_DEBUG_DRAW_POINT( debugInterface, perFrameState.m_shoulderPos[1], drawPointSize * 0.25f, lightRed);


  // Draw wall edge
  MR_DEBUG_DRAW_POINT(debugInterface, m_wallEdgePoint0, drawPointSize, darkOrange);
  MR_DEBUG_DRAW_POINT(debugInterface, m_wallEdgePoint1, drawPointSize, darkOrange);
  MR_DEBUG_DRAW_LINE(debugInterface, m_wallEdgePoint0, m_wallEdgePoint1, darkOrange);

  NMP::Vector3 offset = perFrameState.m_worldUp * drawPointSize * 2.0f;
  const float scaledTwo = perFrameState.m_scaling->scaleDist(2.0f);

  // Draw the projection of the com on the wall edge
  MR_DEBUG_DRAW_VECTOR(debugInterface,MR::VT_Normal, perFrameState.m_closestEdgePoint, perFrameState.m_wallNormal * scaledTwo, darkRed);

  // Draw the wall basis vectors  
  MR_DEBUG_DRAW_VECTOR(debugInterface,MR::VT_Normal, perFrameState.m_closestEdgePoint + offset, perFrameState.m_wallNormal * scaledTwo, darkRed);
  MR_DEBUG_DRAW_VECTOR(debugInterface,MR::VT_Normal, perFrameState.m_closestEdgePoint + offset, m_wallEdgeDir * scaledTwo, darkGreen);
  MR_DEBUG_DRAW_VECTOR(debugInterface,MR::VT_Normal, perFrameState.m_closestEdgePoint + offset, perFrameState.m_worldUp * scaledTwo, darkBlue);

  // Draw the comvel
  MR_DEBUG_DRAW_VECTOR(debugInterface,MR::VT_Velocity, 
    perFrameState.m_comPos, perFrameState.m_scaling->scaleDist(1.0f) * perFrameState.m_comVel / perFrameState.m_scaling->scaleVel(1.0f) , lightGreen);
}

//----------------------------------------------------------------------------------------------------------------------
// Draw the reach targets
void ER::AttribDataOperatorFallOverWallState::debugDrawReachTargets(
  const AttribDataOperatorFallOverWallState::PerFrameState& perFrameState,
  float* reachImportances,
  NMP::Vector3* reachTargets,
  NMP::Vector3* reachNormals)
{
  MR::InstanceDebugInterface* debugInterface = perFrameState.m_network->getDispatcher()->getDebugInterface();
  for (int32_t i = 0; i < ER::AttribDataOperatorFallOverWallState::s_numArms; ++i)
  {
    NMP::Colour colour = i % 2 ? NMP::Colour::RED : NMP::Colour::BLUE;
    NMP::Colour lightColour = i % 2 ? NMP::Colour::LIGHT_RED : NMP::Colour::LIGHT_BLUE;
    NMP::Colour darkColour = i % 2 ? NMP::Colour::DARK_RED : NMP::Colour::DARK_BLUE;

    if (reachImportances[i])
    {
      MR_DEBUG_DRAW_POINT(
        debugInterface,
        reachTargets[i], perFrameState.m_scaling->scaleDist(0.5f), colour);
      MR_DEBUG_DRAW_VECTOR(
        debugInterface,MR::VT_Normal,
        reachTargets[i], perFrameState.m_scaling->scaleDist(reachNormals[i]), lightColour);

      MR_DEBUG_DRAW_POINT(
        debugInterface,
        m_reachTargetAnchors[i], perFrameState.m_scaling->scaleDist(0.5f), darkColour);
    }
  } 
}

//----------------------------------------------------------------------------------------------------------------------
void ER::AttribDataOperatorFallOverWallState::debugDrawAngAccelTorqueAsImpulse(
                                      const NMP::Vector3& debugDrawPos,
                                      const NMP::Vector3& currVel, 
                                      const NMP::Vector3& targetVel, 
                                      const NMP::Vector3& correctionTorque,
                                      float lightness,
                                      const AttribDataOperatorFallOverWallState::PerFrameState& perFrameState)
{
  NMP_ASSERT(0.0f <= lightness && lightness <= 1.0f);

  NMP::Colour redColour(NMP::Vector3(lightness, 0.0f, 0.0f));
  NMP::Colour greenColour(NMP::Vector3(0.0f, lightness, 0.0f));
  NMP::Colour purpleColour(NMP::Vector3(lightness, 0.0f, lightness));

  MR::InstanceDebugInterface* debugInterface = perFrameState.m_network->getDispatcher()->getDebugInterface();
  MR_DEBUG_DRAW_VECTOR(
    debugInterface,MR::VT_AngleDelta,
    debugDrawPos, perFrameState.m_scaling->scaleDist(1.0f) * currVel / perFrameState.m_scaling->scaleVel(1.0f) , redColour);
  MR_DEBUG_DRAW_VECTOR(
    debugInterface,MR::VT_AngleDelta,
    debugDrawPos, perFrameState.m_scaling->scaleDist(1.0f) * targetVel / perFrameState.m_scaling->scaleVel(1.0f), greenColour);
  MR_DEBUG_DRAW_VECTOR(
    debugInterface,MR::VT_TorqueImpulse,
    debugDrawPos, perFrameState.m_scaling->scaleDist(1.0f) * correctionTorque / perFrameState.m_scaling->scaleTorque(1.0f), purpleColour);
}
#endif //#if defined MR_OUTPUT_DEBUGGING

//----------------------------------------------------------------------------------------------------------------------
// PerFrameInput implementation
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isChestPastEdge() const
{
  float chestToWall = NMP::vDot(m_wallNormal, m_closestEdgePoint - m_chestPos);

  return chestToWall > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isPelvisPastEdge() const
{
  float pelvisToWall = NMP::vDot(m_wallNormal, m_closestEdgePoint - m_pelvisPos);

  return pelvisToWall > 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isCOMPastEdge() const
{
  float comToWall = NMP::vDot(m_wallNormal, m_closestEdgePoint - m_comPos);

  return comToWall > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isComPastEdgePredicted(float timestep, float distance) const
{
  float comToWall = NMP::vDot(m_wallNormal, m_closestEdgePoint - (m_comPos + m_comVel * timestep));

  return comToWall > distance;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isPelvisPastEdgePredicted(float timestep, float distance) const
{
  float pelvisToWall = NMP::vDot(m_wallNormal, m_closestEdgePoint - (m_pelvisPos + m_pelvisVel * timestep));

  return pelvisToWall > distance;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isHandPastEdge(int i) const
{
  NMP_ASSERT(NMP::valueInRange(i, 0, ER::AttribDataOperatorFallOverWallState::s_numArms));
  return NMP::vDot(m_wallNormal, m_closestEdgePoint - m_handPos[i]) > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isHeadPastEdge() const
{
  float headToWall = NMP::vDot(m_wallNormal, m_closestEdgePoint - m_headPos);

  return headToWall > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isFootPastEdge(int i) const
{
  NMP_ASSERT(NMP::valueInRange(i, 0, ER::AttribDataOperatorFallOverWallState::s_numLegs));
  return vDot(m_wallNormal, m_closestEdgePoint - m_footPos[i]) > 0;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isArmPastEdge(int i, float threshold) const
{
  NMP_ASSERT(NMP::valueInRange(i, 0, ER::AttribDataOperatorFallOverWallState::s_numArms));
  bool handIsOver = NMP::vDot(m_wallNormal, m_closestEdgePoint - m_handPos[i]) > threshold;
  bool elbowIsOver = NMP::vDot(m_wallNormal, m_closestEdgePoint - m_elbowPos[i]) > threshold;
  bool shoulderIsOver = NMP::vDot(m_wallNormal, m_closestEdgePoint - m_shoulderPos[i]) > threshold;

  return handIsOver && elbowIsOver && shoulderIsOver;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isBodyPastEdge() const
{
  return isHeadPastEdge() && isPelvisPastEdge() && isCOMPastEdge() && (isFootPastEdge(0) || isFootPastEdge(1));
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isFacingWall() const
{
  return NMP::vDot(m_wallNormal, m_pelvisForward) < 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isInDive() const
{
  return (m_avgfootPos - m_chestPos).dot(m_worldUp) > 0.0f;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isWithinDistanceToWall(float maxDistance) const
{
  float comDist = -m_comToWall.dot(m_wallNormal);
  float chestDist = (m_chestPos - m_closestEdgePoint).dot(m_wallNormal);
  float pelvisDist = (m_pelvisPos - m_closestEdgePoint).dot(m_wallNormal);
  float avgFootDist = (m_avgfootPos - m_closestEdgePoint).dot(m_wallNormal);
  float hand0Dist = (m_handPos[0] - m_closestEdgePoint).dot(m_wallNormal);
  float hand1Dist = (m_handPos[1] - m_closestEdgePoint).dot(m_wallNormal);

  return
    comDist < maxDistance
    ||  chestDist < maxDistance
    ||  pelvisDist < maxDistance
    ||  avgFootDist < maxDistance
    ||  hand0Dist < maxDistance
    ||  hand1Dist < maxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isLowerBodyWithinDistanceToWall(float maxDistance) const
{
  return 
    (m_footPos[0] - m_closestEdgePoint).dot(m_wallNormal) < maxDistance
    || (m_footPos[1] - m_closestEdgePoint).dot(m_wallNormal) < maxDistance
    || (m_kneePos[0] - m_closestEdgePoint).dot(m_wallNormal) < maxDistance
    || (m_kneePos[1] - m_closestEdgePoint).dot(m_wallNormal) < maxDistance
    || (m_pelvisPos - m_closestEdgePoint).dot(m_wallNormal) < maxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isLegWithinDistanceToWall(int i, float maxDistance) const
{
  NMP_ASSERT(NMP::valueInRange(i, 0, ER::AttribDataOperatorFallOverWallState::s_numLegs));

  return (m_footPos[i] - m_closestEdgePoint).dot(m_wallNormal) < maxDistance
    || (m_kneePos[i] - m_closestEdgePoint).dot(m_wallNormal) < maxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isPelvisWithinDistanceToWall(float maxDistance) const
{
  return (m_pelvisPos - m_closestEdgePoint).dot(m_wallNormal) < maxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isChestWithinDistanceToWall(float maxDistance) const
{
  return (m_chestPos - m_closestEdgePoint).dot(m_wallNormal) < maxDistance;
}

//----------------------------------------------------------------------------------------------------------------------
bool ER::AttribDataOperatorFallOverWallState::PerFrameState::isMovingTowardsWall(float minVel) const
{
  return -m_comVel.dot(m_wallNormal) > minVel;
}

//----------------------------------------------------------------------------------------------------------------------
static void createOutputs(MR::NodeDef* nodeDef, MR::Network* net)
{
  MR::NodeBin* nodeBin = net->getNodeBin(nodeDef->getNodeID());
  PinInterface pinInteface(nodeBin);

  pinInteface.createPin(net, FOW_CP_OUT_ARM0REACHIMPORTANCE, 0.0f);
  pinInteface.createPin(net, FOW_CP_OUT_ARM0REACHTARGET, NMP::Vector3Zero());
  pinInteface.createPin(net, FOW_CP_OUT_ARM0REACHNORMAL, NMP::Vector3Zero());

  pinInteface.createPin(net, FOW_CP_OUT_ARM1REACHIMPORTANCE, 0.0f);
  pinInteface.createPin(net, FOW_CP_OUT_ARM1REACHTARGET, NMP::Vector3Zero());
  pinInteface.createPin(net, FOW_CP_OUT_ARM1REACHNORMAL, NMP::Vector3Zero());

  pinInteface.createPin(net, FOW_CP_OUT_LEG0WRITHESTRENGTH, 0.0f);
  pinInteface.createPin(net, FOW_CP_OUT_LEG1WRITHESTRENGTH, 0.0f);
  pinInteface.createPin(net, FOW_CP_OUT_LEG0STRENGTH, 1.0f);
  pinInteface.createPin(net, FOW_CP_OUT_LEG1STRENGTH, 1.0f);
  pinInteface.createPin(net, FOW_CP_OUT_SPINESTRENGTH, 1.0f);

  pinInteface.createPin(net, FOW_CP_OUT_TARGET_VELOCITY, NMP::Vector3Zero());

  pinInteface.createPin(net, FOW_CP_OUT_FALLENNOTTRIGGEREDTIME, 0.0f);
  pinInteface.createPin(net, FOW_CP_OUT_STALLEDTIME, 0.0f);
  pinInteface.createPin(net, FOW_CP_OUT_OVERWALLTIME, 0.0f);
  pinInteface.createPin(net, FOW_CP_OUT_COMANGLETOWALLNORMAL, -NM_PI_OVER_TWO);

}

//----------------------------------------------------------------------------------------------------------------------
static AttribDataOperatorFallOverWallState* createStateData(MR::NodeDef* nodeDef, MR::Network* net)
{
  // Create storage for the state data.
  //
  MR::NodeID nodeID = nodeDef->getNodeID();
  MR::AttribDataHandle handle = AttribDataOperatorFallOverWallState::create(net->getPersistentMemoryAllocator());
  AttribDataOperatorFallOverWallState* stateData = (AttribDataOperatorFallOverWallState*)(handle.m_attribData);
  NMP_ASSERT(stateData); // failed to create state  
  MR::AttribAddress stateAddress(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeID, MR::INVALID_NODE_ID, MR::VALID_FOREVER);
  net->addAttribData(stateAddress, handle, MR::LIFESPAN_FOREVER);

  // Get the node's def (attribute settings) data.
  //
#if defined(NMP_ENABLE_ASSERTS)
  AttribDataOperatorFallOverWallDef* defData;
  defData = nodeDef->getAttribData<AttribDataOperatorFallOverWallDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
#endif // NMP_ENABLE_ASSERTS
  NMP_ASSERT(defData); // verify we have some

  return stateData;
}

//----------------------------------------------------------------------------------------------------------------------
void nodeOperatorFallOverWallInitInstance(MR::NodeDef* nodeDef, MR::Network* net)
{
  createOutputs(nodeDef, net);

  ER::AttribDataOperatorFallOverWallState* stateData = createStateData(nodeDef, net);
  if (stateData)
  {
    stateData->reset();
  }

  if (nodeDef->getNumReflexiveCPPins() > 0)
  {
    MR::nodeInitPinAttribDataInstance(nodeDef, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------
static AttribDataOperatorFallOverWallState* getStateData(MR::NodeDef* nodeDef, MR::Network* net)
{
  MR::NodeID nodeID = nodeDef->getNodeID();
  MR::NodeBinEntry* stateEntry = net->getAttribDataNodeBinEntry(
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    nodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME);
  NMP_ASSERT(stateEntry);

  return stateEntry ? stateEntry->getAttribData<AttribDataOperatorFallOverWallState>():0;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* nodeOperatorFallOverWallEmitMessageUpdate(
  MR::NodeDef* nodeDef,
  MR::PinIndex NMP_UNUSED(outputCPPinIndex),
  MR::Network* net)
{
  // If node is active check if there is a message to be sent
  if ( net->operatorCPNodeIsActive(nodeDef->getNodeID()) )
  {
    // Get the current state data
    AttribDataOperatorFallOverWallState* stateData = getStateData(nodeDef, net);

    // If we have a message, send it
    if (stateData->m_resetMessageID)
    {
      // Retrieve existing emitted messages attribute object or create one
      MR::AttribDataEmittedMessages* emittedMessagesInputAttrib =
        net->getOptionalAttribData<MR::AttribDataEmittedMessages>(
          MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES,
          nodeDef->getNodeID(),
          nodeDef->getNodeID(),
          MR::VALID_FRAME_ANY_FRAME);
      if (!emittedMessagesInputAttrib)
      {
        MR::AttribDataHandle handle = MR::AttribDataEmittedMessages::create(net->getPersistentMemoryAllocator());
        emittedMessagesInputAttrib = (MR::AttribDataEmittedMessages*)(handle.m_attribData);
        MR::AttribAddress address(
          MR::ATTRIB_SEMANTIC_EMITTED_MESSAGES,
          nodeDef->getNodeID(),
          nodeDef->getNodeID(),
          net->getCurrentFrameNo());
        net->addAttribData(address, handle, MR::LIFESPAN_FOREVER);
      }
      emittedMessagesInputAttrib->m_value = stateData->m_resetMessageID;
      stateData->m_resetMessageID = 0;
    }
  }
  return (MR::AttribData*)0;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* nodeOperatorFallOverWallOutputCPUpdate(
  MR::NodeDef* nodeDef,
  MR::PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  MR::Network* net)
{
  NMP_ASSERT(outputCPPinIndex < FOW_CP_OUT_COUNT);

  MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(nodeDef->getNodeID());

  // Get the definition data.
  AttribDataOperatorFallOverWallDef* defData
    = nodeDef->getAttribData<AttribDataOperatorFallOverWallDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF, animSet);
  NMP_ASSERT(defData);

  // Get the current state data.
  AttribDataOperatorFallOverWallState* stateData = getStateData(nodeDef, net);
  NMP_ASSERT(stateData);

  // Reset the fall over wall performance because 1 or 2 is true
  // 1 re-entering the FoW node's parent state (predicated on last frame number logged in 
  // the fall manager as compared with the current one)
  // 2 because stateData says so
  //
  const MR::FrameCount currentFrameNo = net->getCurrentFrameNo();
  bool reEnteringParentState = (stateData->m_lastUpdateFrame < (currentFrameNo - 1));
  if (stateData->m_reset || reEnteringParentState)
  {
    stateData->reset();
  }

  // Update the input parameters.
  // CP.

  MR::AttribDataBool* inputEnabled = 
    net->updateOptionalInputCPConnection<MR::AttribDataBool>(nodeDef->getInputCPConnection(ER::FOW_CP_IN_ENABLE), animSet);
  bool enabled(true);
  if (inputEnabled)
  {
    enabled = inputEnabled->m_value;

    // If the performance was disabled after it was started then reset
    if (!enabled && stateData->m_fallTriggered)
    {
      stateData->reset();
    }
  }

  MR::AttribDataVector3* inputRightEndEdgePoint = 
    net->updateOptionalInputCPConnection<MR::AttribDataVector3>(
      nodeDef->getInputCPConnection(FOW_CP_IN_RIGHT_EDGE_POINT), animSet);
  NMP::Vector3 rightEndEdgePoint(NMP::Vector3::InitTypeZero);
  if (inputRightEndEdgePoint)
  {
    rightEndEdgePoint = inputRightEndEdgePoint->m_value;
  }
  MR::AttribDataVector3* inputLeftEndEdgePoint = 
    net->updateOptionalInputCPConnection<MR::AttribDataVector3>(nodeDef->getInputCPConnection(
      FOW_CP_IN_LEFT_EDGE_POINT), animSet);
  NMP::Vector3 leftEndEdgePoint(NMP::Vector3::InitTypeZero);
  if (inputLeftEndEdgePoint)
  {
    leftEndEdgePoint = inputLeftEndEdgePoint->m_value;
  }

  MR::AttribDataFloat* inputBalanceAmount = 
    net->updateOptionalInputCPConnection<MR::AttribDataFloat>(nodeDef->getInputCPConnection(
      FOW_CP_IN_BALANCE_AMOUNT), animSet);
  float balanceAmount(0);
  if (inputBalanceAmount)
  {
    balanceAmount = inputBalanceAmount->m_value;
  }

  MR::AttribDataFloat* inputLegKickAmount = 
    net->updateOptionalInputCPConnection<MR::AttribDataFloat>(nodeDef->getInputCPConnection(
      FOW_CP_IN_LEG_KICK_AMOUNT), animSet);
  float legKickAmount(0);
  if (inputLegKickAmount)
  {
    legKickAmount = inputLegKickAmount->m_value;
  }
  else
  {
    legKickAmount = defData->m_legKickAmountDefault;
  }
  
  //------------------------------------
  // Obtain access to output pins
  //
  MR::NodeBin* nodeBin = net->getNodeBin(nodeDef->getNodeID());
  ER::PinInterface pinInterface(nodeBin);
  
  // Set output defaults
  //
  pinInterface.writeValue(FOW_CP_OUT_ARM0REACHIMPORTANCE, 0.0f);
  pinInterface.writeValue(FOW_CP_OUT_ARM0REACHTARGET, NMP::Vector3Zero());
  pinInterface.writeValue(FOW_CP_OUT_ARM0REACHNORMAL, NMP::Vector3Zero());

  pinInterface.writeValue(FOW_CP_OUT_ARM1REACHIMPORTANCE, 0.0f);
  pinInterface.writeValue(FOW_CP_OUT_ARM1REACHTARGET, NMP::Vector3Zero());
  pinInterface.writeValue(FOW_CP_OUT_ARM1REACHNORMAL, NMP::Vector3Zero());

  pinInterface.writeValue(FOW_CP_OUT_LEG0WRITHESTRENGTH, 0.0f);
  pinInterface.writeValue(FOW_CP_OUT_LEG0STRENGTH, 1.0f);
  pinInterface.writeValue(FOW_CP_OUT_LEG1WRITHESTRENGTH, 0.0f);
  pinInterface.writeValue(FOW_CP_OUT_LEG1STRENGTH, 1.0f);
  pinInterface.writeValue(FOW_CP_OUT_SPINESTRENGTH, 1.0f);

  pinInterface.writeValue(FOW_CP_OUT_TARGET_VELOCITY, NMP::Vector3Zero());

  pinInterface.writeValue(FOW_CP_OUT_FALLENNOTTRIGGEREDTIME, 0.0f);
  pinInterface.writeValue(FOW_CP_OUT_STALLEDTIME, 0.0f);
  pinInterface.writeValue(FOW_CP_OUT_OVERWALLTIME, 0.0f);
  pinInterface.writeValue(FOW_CP_OUT_COMANGLETOWALLNORMAL, -90.0f);
  
  if (enabled)
  {
    // Perform the actual task
    //
    ER::AttribDataOperatorFallOverWallState::PerFrameState perFrameState;
    if (!reEnteringParentState && stateData->preUpdate( net, rightEndEdgePoint, leftEndEdgePoint, balanceAmount, perFrameState))
    {
      // Update the performance
      float attractorTriggerDistance = perFrameState.m_scaling->scaleDist(defData->m_attractorTriggerDistance);
      float fallTriggerDistance = perFrameState.m_scaling->scaleDist(defData->m_fallTriggerDistance);
      if (!stateData->m_fallTriggered)
      {
        // Performance can be triggered if 
        // character has some balance && is within the trigger distance from the wall
        stateData->m_fallTriggered = 
          perFrameState.m_balanceAmount > 0.0f
          && (0.0f <= perFrameState.m_edgeProjectionCoordinate && perFrameState.m_edgeProjectionCoordinate <= 1.0f)
          && (perFrameState.isWithinDistanceToWall(attractorTriggerDistance));
      }

      if (stateData->m_fallTriggered && !stateData->m_stalled)
      {
        // Cheat force and reaching criteria
        //
        bool chestOverWall = perFrameState.isChestPastEdge();
        bool wallIsNotTooHigh = 
          (perFrameState.m_comToWall.dot(perFrameState.m_worldUp) < perFrameState.m_scaling->scaleDist(defData->m_maxWallHeightOffset));
        bool wallIsCloseEnoughForCheatForces = perFrameState.isWithinDistanceToWall(attractorTriggerDistance);
        bool canApplyCheatForces = ((chestOverWall) || (wallIsCloseEnoughForCheatForces && wallIsNotTooHigh));
        bool closeEnoughToReach = perFrameState.isWithinDistanceToWall(perFrameState.m_scaling->scaleDist(defData->m_reachTriggerDistance));
        bool movingTowardsEnoughToReach = perFrameState.isMovingTowardsWall(perFrameState.m_scaling->scaleVel(defData->m_reachTriggerVelocity));
        bool canReach = wallIsNotTooHigh && (canApplyCheatForces || (closeEnoughToReach && movingTowardsEnoughToReach));

        float comAngleDegrees = perFrameState.m_comAngleToWallNormal * 180.0f / NM_PI;
        pinInterface.writeValue(FOW_CP_OUT_COMANGLETOWALLNORMAL, comAngleDegrees);

        // Update reach if criteria satisfied
        //
        if (canReach)
        {
          float reachImportances[2];
          NMP::Vector3 reachTargets[2];
          NMP::Vector3 reachNormals[2];
          stateData->updateReachTargets(perFrameState, reachImportances, reachTargets, reachNormals );

          pinInterface.writeValue(FOW_CP_OUT_ARM0REACHIMPORTANCE, reachImportances[0]);
          pinInterface.writeValue(FOW_CP_OUT_ARM0REACHTARGET, reachTargets[0]);
          pinInterface.writeValue(FOW_CP_OUT_ARM0REACHNORMAL, reachNormals[0]);
          pinInterface.writeValue(FOW_CP_OUT_ARM1REACHIMPORTANCE, reachImportances[1]);
          pinInterface.writeValue(FOW_CP_OUT_ARM1REACHTARGET, reachTargets[1]);
          pinInterface.writeValue(FOW_CP_OUT_ARM1REACHNORMAL, reachNormals[1]);
        }

        // Apply "auxilliary" forces and torques if cheat force criteria satisfied
        //
        if (canApplyCheatForces)
        {
          // Legs and spine weakening 
          // Leg relax helps avoid having the balancer fight the roll over wall when knees or feet make contact
          // Spine relax encourages the character to slump and generally roll more naturually
          //
          float legStrengths[AttribDataOperatorFallOverWallState::s_numLegs];
          float legWritheStrengths[AttribDataOperatorFallOverWallState::s_numLegs];
          float spineStrength;
          stateData->updateLegAndSpineStrengths(
            fallTriggerDistance,
            defData->m_overWallTimeout,
            spineStrength,
            legStrengths,
            legWritheStrengths,
            perFrameState);

          // Feedback the values
          //
          pinInterface.writeValue(FOW_CP_OUT_LEG0STRENGTH, legStrengths[0]);
          pinInterface.writeValue(FOW_CP_OUT_LEG1STRENGTH, legStrengths[1]);
          pinInterface.writeValue(FOW_CP_OUT_LEG0WRITHESTRENGTH, legWritheStrengths[0] * legKickAmount);
          pinInterface.writeValue(FOW_CP_OUT_LEG1WRITHESTRENGTH, legWritheStrengths[1] * legKickAmount);
          pinInterface.writeValue(FOW_CP_OUT_SPINESTRENGTH, spineStrength);

          // Modulate the attribute set side roll amount according to how closely the pelvis forward direction matches
          // the com to wall direction, so that the more forwards the character is facing the more power it has to align.
          //
          NMP::Vector3 wallToComDir = -NMP::vNormalise(perFrameState.m_comToWall);
          float x = perFrameState.m_pelvisForward.dot(wallToComDir);
          float y = perFrameState.m_pelvisForward.dot(stateData->m_wallEdgeDir);
          float sideRollScale = NMP::nmfabs(atan2f(y, x)) / NM_PI;
          float sideRollAmount = defData->m_sideRollAmount * sideRollScale;
          float targetRollSpeed = perFrameState.m_scaling->scaleAngVel(NMP::degreesToRadians(defData->m_targetRollSpeed));
          //
          MR::AttribDataFloat* inputTargetApproachVel = 
            net->updateOptionalInputCPConnection<MR::AttribDataFloat>(nodeDef->getInputCPConnection(
            FOW_CP_IN_TARGET_APPROACH_VEL), animSet);
          float targetApproachVel(0);
          if (inputTargetApproachVel)
          {
            targetApproachVel = perFrameState.m_scaling->scaleVel(inputTargetApproachVel->m_value);
          }
          else
          {
            targetApproachVel = perFrameState.m_scaling->scaleVel(defData->m_targetApproachVelDefault);
          }
          MR::AttribDataFloat* inputTargetRollOverVel = 
            net->updateOptionalInputCPConnection<MR::AttribDataFloat>(nodeDef->getInputCPConnection(
            FOW_CP_IN_TARGET_ROLLOVER_VEL), animSet);
          float targetRollOverVel(0);
          if (inputTargetRollOverVel)
          {
            targetRollOverVel = perFrameState.m_scaling->scaleVel(inputTargetRollOverVel->m_value);
          }
          else
          {
            targetRollOverVel = perFrameState.m_scaling->scaleVel(defData->m_targetRollOverVelDefault);
          }

          // Calculate step target velocity and apply cheat forces as applicable.
          //
          NMP::Vector3 stepVelocityDemand(
            stateData->updateStepVelocity(fallTriggerDistance, targetApproachVel, perFrameState));
          pinInterface.writeValue(FOW_CP_OUT_TARGET_VELOCITY, stepVelocityDemand);

          stateData->calculateAndApplyCheatImpulses( fallTriggerDistance, fallTriggerDistance * 0.5f,
            targetRollOverVel, targetRollSpeed, sideRollAmount, perFrameState);

          
        }
      }
    }

    // Post update tasks
    //

    // Update timers and associated status flags
    stateData->updateTimers(defData->m_stalledTimeout, perFrameState);

    // Output timer values (status feedback)
    pinInterface.writeValue(FOW_CP_OUT_OVERWALLTIME, stateData->m_overWallEdgeTime);
    pinInterface.writeValue(FOW_CP_OUT_FALLENNOTTRIGGEREDTIME, stateData->m_fallenNotTriggeredTime);
    pinInterface.writeValue(FOW_CP_OUT_STALLEDTIME, stateData->m_stallingTime);

    // Resets and emitted messages
    //
    stateData->updateEmittedMessages(
      defData->m_overWallTimeout, defData->m_settleTimeout, defData->m_fallenNotTriggeredTimeout);
  }

  // Flag all outputs as updated this frame
  //
  pinInterface.setLastUpdateFrame(currentFrameNo, 0, FOW_CP_OUT_COUNT);
  // Flag state data as updated for this frame
  stateData->m_lastUpdateFrame = currentFrameNo;

  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
