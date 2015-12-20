// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erContactFeedback.h"
#include "euphoria/erEuphoriaUserData.h"
#include "mrPhysX3.h"
#include "mrPhysicsRigPhysX3.h"
#include "mrPhysicsScenePhysX3.h"
#include "euphoria/erValueValidators.h"

bool g_debugDrawDetailedContacts = false;
bool g_debugDrawContacts = false;
static ER::ContactFeedback s_contactFeedback;
physx::PxClientID ER::ContactFeedback::m_ownerClientID = physx::PX_DEFAULT_CLIENT;

namespace ER
{
//----------------------------------------------------------------------------------------------------------------------
// Add the callback if it isn't already added
void ContactFeedback::initialise(MR::PhysicsScene* physicsScene, physx::PxClientID ownerClientID)
{
  m_ownerClientID = ownerClientID;
  // Setting an event callback is a global operation that only needs to be done once per scene
  // So we don't need to set it if it has already been set by a different character.
  MR::PhysicsScenePhysX3* scene = (MR::PhysicsScenePhysX3*)physicsScene;
  if (scene->getPhysXScene()->getSimulationEventCallback(ownerClientID) == NULL)
  {
    scene->getPhysXScene()->setSimulationEventCallback(&s_contactFeedback, ownerClientID);
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Remove the callback
void ContactFeedback::deinitialise(MR::PhysicsScene* physicsScene)
{
  MR::PhysicsScenePhysX3* scene = (MR::PhysicsScenePhysX3*)physicsScene;
  scene->getPhysXScene()->setSimulationEventCallback(NULL, m_ownerClientID);
}

//----------------------------------------------------------------------------------------------------------------------
void ContactFeedback::setUserContactHandler(UserContactHandler* handler)
{
  s_contactFeedback.m_userContactHandler = handler;
}

//----------------------------------------------------------------------------------------------------------------------
bool ContactFeedback::getDrawDetailedContactsFlag() 
{
  return g_debugDrawDetailedContacts;
}

//----------------------------------------------------------------------------------------------------------------------
void ContactFeedback::setDrawDetailedContactsFlag(bool drawEnabled) 
{
  g_debugDrawDetailedContacts = drawEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
bool ContactFeedback::getDrawContactsFlag() 
{
  return g_debugDrawContacts;
}

//----------------------------------------------------------------------------------------------------------------------
void ContactFeedback::setDrawContactsFlag(bool drawEnabled) 
{
  g_debugDrawContacts = drawEnabled;
}

//----------------------------------------------------------------------------------------------------------------------
void ER::EuphoriaRigPartUserData::processData(
  physx::PxActor* contactedActor, 
  physx::PxShape* contactedShape, 
  const NMP::Vector3& point, 
  const NMP::Vector3& normal, 
  float impulseMagnitude)
{
  NMP_ASSERT(contactedActor);
  if (!m_accumulating)
  {
    startNewContact();
  }

  const NMP::Vector3 impulse = normal * impulseMagnitude;

  m_accumulating = true;
  m_lastTotalImpulseMagnitude += impulseMagnitude;
  m_lastTotalImpulse += impulse;
  m_lastTotalPositionScaled += point * impulseMagnitude; 
  m_lastTotalNormalScaled += normal * impulseMagnitude;
  m_lastTotalVelocityScaled += MR::nmPxVec3ToVector3(MR::getLinearVelocity(*contactedActor)) * impulseMagnitude;
  m_lastCollisionID = (int64_t)(size_t)contactedActor;

  if (m_numContacts == m_maxNumContacts)
  {
    physx::PxShape** origShapes = m_contactedShapes;
    uint16_t origNum = m_maxNumContacts;
    m_maxNumContacts *= 2;
    m_contactedShapes = 
      (physx::PxShape**) NMP::Memory::memAlloc(sizeof(physx::PxShape*)*m_maxNumContacts NMP_MEMORY_TRACKING_ARGS);
    memcpy(m_contactedShapes, origShapes, sizeof(physx::PxShape*)*origNum);
    NMP::Memory::memFree(origShapes);
  }
  NMP_ASSERT(m_numContacts < m_maxNumContacts);

  m_contactedShapes[m_numContacts] = contactedShape;
  ++m_numContacts;
}

//----------------------------------------------------------------------------------------------------------------------
static void setActorsInContact(physx::PxActor* actor0, 
                               physx::PxActor* actor1, 
                               physx::PxShape* shape0,
                               physx::PxShape* shape1,
                               const NMP::Vector3& point, 
                               const NMP::Vector3& normal, 
                               const float impulseMagnitude)
{
  // Check that the actor has a dynamic body so we don't get confused with the kinematic shapes used
  // for HK.
  MR::PhysicsRigPhysX3ActorData* act0 = MR::PhysicsRigPhysX3ActorData::getFromActor(actor0);
  MR::PhysicsRigPhysX3ActorData* act1 = MR::PhysicsRigPhysX3ActorData::getFromActor(actor1);
  MR::PhysicsRig::Part* p0 = act0 ? act0->m_owningRigPart : 0;
  MR::PhysicsRig::Part* p1 = act1 ? act1->m_owningRigPart : 0;

  NMP_ASSERT(Validators::Vector3Valid(point));
  NMP_ASSERT(Validators::FloatValid(impulseMagnitude));
  NMP_ASSERT(Validators::Vector3Normalised(normal));

#ifdef RECOVER_FROM_ERRORS
  if (
    !Validators::Vector3Valid(point) || 
    !Validators::FloatValid(impulseMagnitude) || 
    !Validators::Vector3Normalised(normal))
  {
    return;
  }
#endif

#if defined(MR_OUTPUT_DEBUGGING)
  const float lengthMultiplier = PxGetPhysics().getTolerancesScale().length;
  NMP::Vector3 impulse = normal * impulseMagnitude;

  if (g_debugDrawContacts)
  {
    if (impulseMagnitude > 0.0f)
    {
      MR_DEBUG_DRAW_POINT_GLOBAL(point, 0.01f * lengthMultiplier, NMP::Colour::WHITE);
    }
    else
    {
      MR_DEBUG_DRAW_POINT_GLOBAL(point, 0.01f * lengthMultiplier, NMP::Colour::RED);
    }
  }

  if (g_debugDrawDetailedContacts && impulseMagnitude > 0.0f)
  {
    MR_DEBUG_DRAW_VECTOR_GLOBAL(MR::VT_Normal, point, normal * 0.03f * lengthMultiplier, NMP::Colour::WHITE);
    MR_DEBUG_DRAW_VECTOR_GLOBAL(MR::VT_Normal, point, 
      MR::nmPxVec3ToVector3(actor0->isRigidActor()->getGlobalPose().p) - point, NMP::Colour::DARK_RED);
    MR_DEBUG_DRAW_VECTOR_GLOBAL(MR::VT_Normal, point, 
      MR::nmPxVec3ToVector3(actor1->isRigidActor()->getGlobalPose().p) - point, NMP::Colour::DARK_RED);
    MR_DEBUG_DRAW_VECTOR_GLOBAL(MR::VT_Impulse, point, impulse, NMP::Colour::BLUE);
  }

  // Only interested in active contacts, and zero force contacts were only passed if debug draw was
  // enabled at compile time
  if (impulseMagnitude <= 0.0f)
    return;

#endif // defined(MR_OUTPUT_DEBUGGING)

  // Only interested in characters
  NMP_ASSERT(p0 || p1);

  // don't register self collision
  if (p0 && p1 && p0->getOwningPhysicsRig() == p1->getOwningPhysicsRig())
  {
    return;
  }

  if (p0)
  {
    ER::EuphoriaRigPartUserData* data = ER::EuphoriaRigPartUserData::getFromPart(p0);
    if (data)
    {
      data->processData(actor1, shape1, point, normal, impulseMagnitude);
    }
  }
  if (p1)
  {
    ER::EuphoriaRigPartUserData* data = ER::EuphoriaRigPartUserData::getFromPart(p1);
    if (data)
    {
      data->processData(actor0, shape0, point, -normal, impulseMagnitude);
    }
  }
}

static const physx::PxU32 MAX_CONTACT_PAIR_POINTS = 16;
static physx::PxContactPairPoint contactPairPoints[MAX_CONTACT_PAIR_POINTS];

//----------------------------------------------------------------------------------------------------------------------
void ContactFeedback::onContact(
  const physx::PxContactPairHeader& pairHeader, 
  const physx::PxContactPair* pairs, 
  physx::PxU32 nbPairs)
{
  if (m_userContactHandler)
  {
    m_userContactHandler->onContact(pairHeader, pairs, nbPairs);
  }

  if (
    pairHeader.flags & physx::PxContactPairHeaderFlag::eDELETED_ACTOR_0 || 
    pairHeader.flags & physx::PxContactPairHeaderFlag::eDELETED_ACTOR_1
    )
  {
    return;
  }

  physx::PxActor* actor0 = pairHeader.actors[0];
  physx::PxActor* actor1 = pairHeader.actors[1];

  bool callSetActorsInContact = 
    MR::PhysicsRigPhysX3ActorData::getFromActor(actor0) || MR::PhysicsRigPhysX3ActorData::getFromActor(actor1);

  // Don't do the loop unless we would do something
  if (!callSetActorsInContact && !m_userContactHandler)
    return;

  for (physx::PxU32 iPair = 0 ; iPair != nbPairs ; ++iPair)
  {
    const physx::PxContactPair& pair = pairs[iPair];
    // Skip deleted shapes
    if (pair.flags & (physx::PxContactPairFlag::eDELETED_SHAPE_0 | physx::PxContactPairFlag::eDELETED_SHAPE_1))
    {
      continue;
    }

    physx::PxU32 numPairPoints = pair.extractContacts(contactPairPoints, MAX_CONTACT_PAIR_POINTS);


    for (physx::PxU32 iContact = 0 ; iContact != numPairPoints ; ++iContact)
    {
      const physx::PxContactPairPoint& contactPairPoint = contactPairPoints[iContact];
      const NMP::Vector3 normal = MR::nmPxVec3ToVector3(contactPairPoint.normal);
      const float impulseMagnitude = contactPairPoint.impulse.magnitude();
#if !defined(MR_OUTPUT_DEBUGGING)
      // Zero-impulse contacts are only used for drawing, so avoid the function call overhead if
      // debug draw is disabled.
      if (impulseMagnitude == 0.0f)
      {
        continue;
      }
#endif
      NMP_ASSERT(impulseMagnitude >= 0.0f); // PhysX used to report -ve sometimes
      const NMP::Vector3 point = MR::nmPxVec3ToVector3(contactPairPoint.position);
      physx::PxShape* shape0 = pair.shapes[0];
      physx::PxShape* shape1 = pair.shapes[1];
      // Don't just check if these are articulation links, as then we won't get hits on HK
      // parts. We can get the per-actor data even from the HK parts, so use that instead.
      // This is slightly more expensive, but shouldn't be too significant.
      if (
        MR::PhysicsRigPhysX3ActorData::getFromActor(actor0) || 
        MR::PhysicsRigPhysX3ActorData::getFromActor(actor1)
        )
      {
        setActorsInContact(actor0, actor1, shape0, shape1, point, normal, impulseMagnitude);
      }
    }
  }
}

} // namespace
