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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_PHYSX_H
#define MR_PHYSX_H

// Define this to enable attempted recovery from PhysX making the character explode. Simply resets
// the parts to the origin! Just do it on PC so consoles remain uncluttered for profiling.
#if defined(NM_HOST_WIN32) || defined(NM_HOST_WIN64)
  #define RECOVER_FROM_ERRORS
#endif

#define USE_PHYSX_SWEEPS_FOR_CHARACTERx // See MORPH-9039

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMQuat.h"
#include "mrPhysX3Includes.h"

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxVec3 nmVector3ToPxVec3(const NMP::Vector3& v)
{
  return physx::PxVec3(v.x, v.y, v.z);
}

//----------------------------------------------------------------------------------------------------------------------
static inline NMP::Vector3 nmPxVec3ToVector3(const physx::PxVec3& v)
{
  return NMP::Vector3(v.x, v.y, v.z);
}

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxExtendedVec3 nmVector3ToPxExtendedVec3(const NMP::Vector3& v)
{
  return physx::PxExtendedVec3(v.x, v.y, v.z);
}

//----------------------------------------------------------------------------------------------------------------------
static inline NMP::Vector3 nmPxExtendedVec3ToVector3(const physx::PxExtendedVec3& v)
{
  return NMP::Vector3((float) v.x, (float) v.y, (float) v.z);
}

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxQuat nmQuatToPxQuat(const NMP::Quat& q)
{
  return physx::PxQuat(q.x, q.y, q.z, q.w);
}

//----------------------------------------------------------------------------------------------------------------------
static inline NMP::Quat nmPxQuatToQuat(const physx::PxQuat& q)
{
  return NMP::Quat(q.x, q.y, q.z, q.w);
}
//----------------------------------------------------------------------------------------------------------------------
static inline NMP::Matrix34 nmPxTransformToNmMatrix34(const physx::PxTransform& nxT)
{
  NMP::Quat quat;
  quat.setXYZW(nxT.q.x, nxT.q.y, nxT.q.z, nxT.q.w);
  NMP::Matrix34 m;
  m.fromQuat(quat);
  m.translation().set(nxT.p.x, nxT.p.y, nxT.p.z);
  return m;
}
//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxTransform nmMatrix34ToPxTransform(const NMP::Matrix34& m)
{
  NMP::Quat quat = m.toQuat();
  physx::PxTransform t;
  t.p = nmVector3ToPxVec3(m.translation());
  t.q = physx::PxQuat(quat.x, quat.y, quat.z, quat.w);
  return t;
}

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxTransform getActorTransform(const physx::PxActor& actor)
{
  if (actor.isRigidActor())
    return actor.isRigidActor()->getGlobalPose();
  return physx::PxTransform();
}

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxTransform getCOMGlobalPose(const physx::PxActor& actor)
{
  const physx::PxRigidBody* body = actor.isRigidBody();
  if (body)
    return body->getGlobalPose() * body->getCMassLocalPose();
  else
    return actor.isRigidActor()->getGlobalPose();
}

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxVec3 getLinearVelocity(const physx::PxActor& actor)
{
  const physx::PxRigidBody* body = actor.isRigidBody();
  if (body)
    return body->getLinearVelocity();
  else
    return physx::PxVec3(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxVec3 getAngularVelocity(const physx::PxActor& actor)
{
  const physx::PxRigidBody* body = actor.isRigidBody();
  if (body)
    return body->getAngularVelocity();
  else
    return physx::PxVec3(0,0,0);
}

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxVec3 getPointVelocity(const physx::PxActor &actor, const physx::PxVec3 &point)
{
  physx::PxVec3 rpoint = point - getCOMGlobalPose(actor).p;

  physx::PxVec3 velocity;
  velocity = getLinearVelocity(actor);
  velocity += getAngularVelocity(actor).cross(rpoint);

  return velocity;
}

//----------------------------------------------------------------------------------------------------------------------
static inline physx::PxTransform getShapeGlobalPose(const physx::PxShape& shape)
{
  return getActorTransform((physx::PxActor&)shape.getActor()) * shape.getLocalPose();
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// getFilterInfo()
//
static inline void setFilterObjectAttributeType(physx::PxFilterObjectAttributes& attr, physx::PxFilterObjectType::Enum type)
{
  NMP_ASSERT((attr & (physx::PxFilterObjectType::eMAX_TYPE_COUNT-1)) == 0);
  attr |= type;
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// getFilterInfo()
//
static inline bool hasTriggerFlags(physx::PxShapeFlags flags)  { 
  return flags & physx::PxShapeFlag::eTRIGGER_SHAPE ? true : false;
}

//----------------------------------------------------------------------------------------------------------------------
// Extracts filter data for the given shape in the format required for the physx filter shader callback.
static inline void getFilterInfo(
  const physx::PxShape* const shape,
  physx::PxFilterObjectAttributes& filterAttr,
  physx::PxFilterData& filterData)
{
  filterAttr = 0;
  physx::PxShapeFlags flags = shape->getFlags();

  if (hasTriggerFlags(flags))
  {
    filterAttr |= physx::PxFilterObjectFlags::eTRIGGER;
  }

  physx::PxActor* actor = &(shape->getActor());
  NMP_ASSERT(actor);

  physx::PxRigidDynamic* rigidDynamicActor = actor->isRigidDynamic();
  if (rigidDynamicActor)
  {
    if (!rigidDynamicActor->isArticulationLink())
    {
      if (rigidDynamicActor->getRigidDynamicFlags() & physx::PxRigidDynamicFlag::eKINEMATIC)
      {
        filterAttr |= physx::PxFilterObjectFlags::eKINEMATIC;
      }

      setFilterObjectAttributeType(filterAttr, physx::PxFilterObjectType::eRIGID_DYNAMIC);
    }
    else
    {
      setFilterObjectAttributeType(filterAttr, physx::PxFilterObjectType::eARTICULATION);
    }
  }
  else
  {
    setFilterObjectAttributeType(filterAttr, physx::PxFilterObjectType::eRIGID_STATIC);
  }

  filterData = shape->getSimulationFilterData();
}

//----------------------------------------------------------------------------------------------------------------------
// Function gets the custom collision filter shader in use for scene obtained from shape pointer.
static inline physx::PxSimulationFilterShader getSimulationFilterShaderFromShape(const physx::PxShape* const shape)
{
  const physx::PxActor* const actor = &(shape->getActor());
  NMP_ASSERT(actor);
  const physx::PxScene* const pxScene = actor->getScene();
  NMP_ASSERT(pxScene);
  return (pxScene->getFilterShader());
}

//----------------------------------------------------------------------------------------------------------------------
// Filter shape by invoking filter shader function with client mask i.e. compare shape test group against client ignore group.
static inline bool applyFilterShader(const physx::PxShape* const shape, const physx::PxFilterData* const userFilterData)
{
  NMP_ASSERT(shape);
  NMP_ASSERT(userFilterData);

  physx::PxFilterObjectAttributes userFilterAttribs = physx::PxFilterObjectType::eUNDEFINED;
  physx::PxFilterData shapeFilterData;
  physx::PxFilterObjectAttributes shapeFilterAttribs = 0;

  MR::getFilterInfo(shape, shapeFilterAttribs, shapeFilterData);

  // Other args required by the api.
  physx::PxPairFlags pairFlags;
  const void* constantBlock = 0;
  const physx::PxU32 constantBlockSize = 32;

  physx::PxFilterFlags filterFlags = MR::getSimulationFilterShaderFromShape(shape)(
    userFilterAttribs,
    *userFilterData,

    shapeFilterAttribs,
    shapeFilterData, // Obtained from shape->getSimulationFilterData();

    // Other args required by the api.
    pairFlags,
    constantBlock,
    constantBlockSize);

  return (filterFlags == physx::PxFilterFlag::eDEFAULT);
}

}

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_PHYSX_H
//----------------------------------------------------------------------------------------------------------------------
