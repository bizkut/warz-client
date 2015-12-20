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
#include "physics/PhysX2/mrPhysicsScenePhysX2.h"
#include "physics/PhysX2/mrPhysX2.h"
#include "physics/PhysX2/mrPhysicsRigPhysX2.h"
#include "physics/PhysX2/mrCharacterControllerInterfacePhysX2.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::Report
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class Report : public NxUserRaycastReport
{
public:
  //------------------------------------------
  Report(const NxActor* skipCharControllerActor, const PhysicsRig* skipChar)
  {
    if (skipChar)
    {
      m_skipChar = skipChar;
    }
    else
      m_skipChar = NULL;

    if (skipCharControllerActor)
      m_characterControllerActor = const_cast<NxActor*>(skipCharControllerActor);
    else
      m_characterControllerActor = 0;

    m_bestHit.distance = NX_MAX_F32;
  }

  //------------------------------------------
  virtual bool onHit(const NxRaycastHit& hits)
  {
    if (m_characterControllerActor)
    {
      NxU32 nShapes = m_characterControllerActor->getNbShapes();
      for (NxU32 iShape = 0; iShape != nShapes; ++iShape)
      {
        const NxShape* shape = m_characterControllerActor->getShapes()[iShape];
        if (shape == hits.shape)
          return true;
      }
    }

    if (m_skipChar)
    {
      for (uint32_t i = m_skipChar->getNumParts(); i-- != 0; )
      {
        const NxActor* actor = ((PhysicsRigPhysX2::PartPhysX*)m_skipChar->getPart(i))->getActor();
        NxU32 nShapes = actor->getNbShapes();
        for (NxU32 iShape = 0; iShape != nShapes; ++iShape)
        {
          const NxShape* shape = actor->getShapes()[iShape];
          if (shape == hits.shape)
            return true;
        }
      }
    }
    if (hits.distance < m_bestHit.distance)
      m_bestHit = hits;
    return true;
  }

  //------------------------------------------
  const PhysicsRig* m_skipChar;
  NxActor*          m_characterControllerActor;
  NxRaycastHit      m_bestHit;
};

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsScenePhysX2::getFloorPositionBelow(
  const NMP::Vector3& pos,
  const PhysicsRig*   skipChar,
  float               distToCheck) const
{
  NxRay worldRay;
  worldRay.orig = nmVector3ToNxVec3(pos);
  NMP::Vector3 rayDirection = m_worldUpDirection * -1.0f;
  worldRay.dir = nmVector3ToNxVec3(rayDirection);

  Report report(((PhysicsRigPhysX2*)skipChar)->getCharacterControllerActor(), skipChar);
  report.m_bestHit.distance = distToCheck;
  NxShapesType shapesType = NX_ALL_SHAPES;
  NxU32 result = m_physXScene->raycastAllShapes(worldRay, report, shapesType, 0xffffffff, distToCheck);
  if (result && report.m_bestHit.distance < distToCheck)
    return nmNxVec3ToVector3(report.m_bestHit.worldImpact);
  else
    return pos + (rayDirection * distToCheck);
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsScenePhysX2::castRay(
  const NMP::Vector3&        start,
  const NMP::Vector3&        delta,
  const PhysicsRig*          skipChar,
  const CharacterControllerInterface* skipCharController,
  float&                     hitDist,
  NMP::Vector3&              hitPosition,
  NMP::Vector3&              hitNormal,
  NMP::Vector3&              hitVelocity) const
{
  NMP::Vector3 dir = delta;
  float len = dir.normaliseGetLength();

  NxRay worldRay;
  worldRay.orig = nmVector3ToNxVec3(start);
  worldRay.dir = nmVector3ToNxVec3(dir);
  Report report(((CharacterControllerInterfacePhysX2*)skipCharController)->getActor(), skipChar);
  report.m_bestHit.distance = len;
  NxShapesType shapesType = NX_ALL_SHAPES;
  NxU32 result =  m_physXScene->raycastAllShapes(worldRay, report, shapesType, 0xffffffff, len);
  if (result && report.m_bestHit.distance < len)
  {
    hitDist = report.m_bestHit.distance;
    hitPosition = nmNxVec3ToVector3(report.m_bestHit.worldImpact);
    hitNormal = nmNxVec3ToVector3(report.m_bestHit.worldNormal);
    const NxActor& actor = report.m_bestHit.shape->getActor();
    if (actor.isDynamic())
    {
      hitVelocity = nmNxVec3ToVector3(actor.getLinearVelocity() + 
        actor.getAngularVelocity().cross(report.m_bestHit.worldImpact - actor.getCMassGlobalPosition()));
    }
    else
    {
      hitVelocity.setToZero();
    }
    return true;
  }
  else
  {
    return false;
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsScenePhysX2::getGravity()
{
  NxVec3 grav;
  getPhysXScene()->getGravity(grav);
  return nmNxVec3ToVector3(grav);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsScenePhysX2::setGravity(const NMP::Vector3& gravity)
{
  NxVec3 nxGravity(gravity.x, gravity.y, gravity.z);
  getPhysXScene()->setGravity(nxGravity);
}

//----------------------------------------------------------------------------------------------------------------------
void addVelocityChangeToActor(NxActor& actor, const NMP::Vector3& velChange, const NMP::Vector3& worldPos, float torqueMultiplier)
{
  if (velChange.magnitude() == 0.0f)
    return;

  // Calculate inv inertia
  NMP::Matrix34 invInertiaWorld = nmNxMat33ToNmMatrix34(actor.getGlobalInertiaTensorInverse());;
  NMP::Vector3 COMPositionWorld = MR::getActorCOMPos(&actor);

  NMP::Vector3 localImpulsePos = worldPos - COMPositionWorld;
  NMP::Vector3 directionWorld = velChange.getNormalised();

  // Calculate impulse using the same equations as found in collision response code - i.e. relating
  // a required change in velocity in a certain direction to an impulse.
  float velChangePerUnitImpulse =  
    (1.0f / actor.getMass()) + 
    NMP::vDot(directionWorld, 
    NMP::vCross(invInertiaWorld.getRotatedVector(NMP::vCross(localImpulsePos, 
    directionWorld)), 
    localImpulsePos));

  NMP::Vector3 impulse = velChange / velChangePerUnitImpulse;

  addImpulseToActor(actor, impulse, worldPos, 1.0f);

  if (torqueMultiplier != 1.0f)
  {
    // Apply a rotational boost by applying the scaled velocity change either side of the CoM
    NMP::Vector3 actorCOMW = MR::getActorCOMPos(&actor);
    NMP::Vector3 offset = worldPos - actorCOMW;

    MR::addVelocityChangeToActor(actor, velChange * (torqueMultiplier - 1.0f) * 0.5f, worldPos + offset);
    MR::addVelocityChangeToActor(actor, velChange * -(torqueMultiplier - 1.0f) * 0.5f, worldPos - offset);
  }
}


} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
