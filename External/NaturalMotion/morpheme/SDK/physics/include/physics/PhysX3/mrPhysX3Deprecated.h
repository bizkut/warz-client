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
#ifndef MR_PHYSX3_DEPRECATED_H
#define MR_PHYSX3_DEPRECATED_H
//----------------------------------------------------------------------------------------------------------------------
#include "mrPhysX3Includes.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Note that the entire contents of this file is just there as a temporary workaround for PhysX
// largely losing its descriptor API. Everything here should be removed - see MORPH-11344
//----------------------------------------------------------------------------------------------------------------------


#ifdef NM_COMPILER_SNC
#pragma diag_push
#pragma diag_suppress=1700 // class "PxArticulationLinkDesc" has virtual functions but non-virtual destructor
#endif

//----------------------------------------------------------------------------------------------------------------------
class PxShapeDesc
{
public:
  physx::PxGeometry*        geometry;
  physx::PxTransform        localPose;
  physx::PxFilterData      simulationFilterData;
  physx::PxFilterData      queryFilterData;
  physx::PxShapeFlags      flags;
  physx::PxPtrArray<physx::PxMaterial> materials;
  physx::PxReal          contactOffset;
  physx::PxReal          restOffset;
  void*          userData;
  const char*        name;
  PX_INLINE      PxShapeDesc(const physx::PxTolerancesScale& scale);
  PX_INLINE       void setToDefault(const physx::PxTolerancesScale& scale);
  PX_INLINE void setMaterials(physx::PxMaterial*const* materials_, physx::PxU32 materialCount_)
  {
    materials.set( materials_, materialCount_ );
  }
  PX_INLINE void setSingleMaterial(physx::PxMaterial* material)
  {
    materials.setSingle( material );
  }

private:    
  void operator=(const PxShapeDesc &) {}
};

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE PxShapeDesc::PxShapeDesc(const physx::PxTolerancesScale& scale)
{
  geometry          = NULL;
  localPose          = physx::PxTransform::createIdentity();
  simulationFilterData.setToDefault();
  queryFilterData.setToDefault();
  flags            = physx::PxShapeFlag::eVISUALIZATION | physx::PxShapeFlag::eSIMULATION_SHAPE | physx::PxShapeFlag::eSCENE_QUERY_SHAPE;
  contactOffset        = 0.02f * scale.length;
  restOffset          = 0.0f;
  userData          = NULL;
  name            = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE void PxShapeDesc::setToDefault(const physx::PxTolerancesScale& scale)
{
  *this = PxShapeDesc(scale);
}

//----------------------------------------------------------------------------------------------------------------------
class PxActorDesc
{
protected:
  physx::PxActorType::Enum    type;
public:
  physx::PxDominanceGroup    dominanceGroup;
  physx::PxActorFlags      actorFlags;
  physx::PxClientID        ownerClient;
  physx::PxU32          clientBehaviorBits;
  void*          userData; 
  const char*        name;
  PX_INLINE physx::PxActorType::Enum getType() const { return type; }
  PX_INLINE virtual bool isValid() const;
protected:
  PX_INLINE PxActorDesc(physx::PxActorType::Enum t);
  virtual ~PxActorDesc() {};
private:
  PX_INLINE PxActorDesc() {}
};

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE PxActorDesc::PxActorDesc(physx::PxActorType::Enum t) : type(t)
{
  dominanceGroup      = 0;
  actorFlags      = physx::PxActorFlag::eVISUALIZATION;
  ownerClient      = physx::PX_DEFAULT_CLIENT;
  clientBehaviorBits  = 0;
  userData      = NULL;
  name        = NULL;
}

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE bool PxActorDesc::isValid() const
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
class PxRigidActorDesc : public PxActorDesc
{
public:
  physx::PxPtrArray<const PxShapeDesc> shapes;
  PX_INLINE void setShapes(const PxShapeDesc*const* shapes_, physx::PxU32 shapeCount_)
  {
    shapes.set( shapes_, shapeCount_ );
  }
  PX_INLINE void setSingleShape(const PxShapeDesc& shape)
  {
    shapes.setSingle( &shape );
  }
protected:
  PX_INLINE PxRigidActorDesc(physx::PxActorType::Enum t);
};

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE PxRigidActorDesc::PxRigidActorDesc(physx::PxActorType::Enum t) : PxActorDesc(t)
{
}

//----------------------------------------------------------------------------------------------------------------------
class PxRigidBodyDesc : public PxRigidActorDesc
{
public:
  physx::PxTransform        massLocalPose;
  physx::PxVec3          massSpaceInertia;
  physx::PxReal          mass;

protected:
  PX_INLINE PxRigidBodyDesc(physx::PxActorType::Enum t);
};

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE PxRigidBodyDesc::PxRigidBodyDesc(physx::PxActorType::Enum t) : PxRigidActorDesc(t)
{
  massLocalPose      = physx::PxTransform::createIdentity();
  mass               = 1.0f;
  massSpaceInertia   = physx::PxVec3(1.0f, 1.0f, 1.0f);
}

//----------------------------------------------------------------------------------------------------------------------
class PxRigidDynamicDesc : public PxRigidBodyDesc
{
public:
  physx::PxTransform        globalPose;
  physx::PxReal          linearDamping;
  physx::PxReal          angularDamping;
  physx::PxVec3          linearVelocity;
  physx::PxVec3          angularVelocity;
  physx::PxReal          maxAngularVelocity;
  physx::PxReal          sleepEnergyThreshold;
  physx::PxReal          sleepDamping;
  physx::PxReal          wakeUpCounter;
  physx::PxU32          minPositionIterations;
  physx::PxU32          minVelocityIterations;
  physx::PxReal          contactReportThreshold;
  physx::PxRigidDynamicFlags    rigidDynamicFlags;
  PX_INLINE void setToDefault(const physx::PxTolerancesScale& scale);
  PX_INLINE PxRigidDynamicDesc(const physx::PxTolerancesScale& scale);
};

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE PxRigidDynamicDesc::PxRigidDynamicDesc(const physx::PxTolerancesScale& scale) 
: PxRigidBodyDesc(physx::PxActorType::eRIGID_DYNAMIC)
{
  globalPose        = physx::PxTransform::createIdentity();
  linearDamping      = 0.0f;
  angularDamping      = 0.05f;
  linearVelocity      = physx::PxVec3(0);
  angularVelocity      = physx::PxVec3(0);
  maxAngularVelocity    = 7.0f;
  sleepEnergyThreshold  = 5e-5f * scale.speed * scale.speed;
  sleepDamping      = 0.0f;
  wakeUpCounter      = 20.0f*0.02f;
  minPositionIterations   = 4;
  minVelocityIterations  = 2;
  contactReportThreshold  = PX_MAX_REAL;
  rigidDynamicFlags    = physx::PxRigidDynamicFlags();
}

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE void PxRigidDynamicDesc::setToDefault(const physx::PxTolerancesScale& scale)
{
  *this = PxRigidDynamicDesc(scale);
}

//----------------------------------------------------------------------------------------------------------------------
class PxRigidStaticDesc : public PxRigidActorDesc
{
public:
  physx::PxTransform          globalPose; 
  PX_INLINE void setToDefault(const physx::PxTolerancesScale& scale);
  PX_INLINE PxRigidStaticDesc(const physx::PxTolerancesScale& scale);
};

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE PxRigidStaticDesc::PxRigidStaticDesc(const physx::PxTolerancesScale& ) : PxRigidActorDesc(physx::PxActorType::eRIGID_STATIC)
{
  globalPose = physx::PxTransform::createIdentity();
}

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE void PxRigidStaticDesc::setToDefault(const physx::PxTolerancesScale& scale)
{
  *this = PxRigidStaticDesc(scale);
}

//----------------------------------------------------------------------------------------------------------------------
class PxArticulationJointDesc
{
public:
  physx::PxTransform       parentPose;
  physx::PxTransform       childPose;
  physx::PxQuat            targetPosition;
  physx::PxVec3            targetVelocity;
  physx::PxReal            spring;
  physx::PxReal            damping;
  physx::PxReal            internalCompliance;
  physx::PxReal            externalCompliance;
  physx::PxReal            swingLimitY;
  physx::PxReal            swingLimitZ;
  bool                              swingLimitEnabled;
  physx::PxReal            twistLimitLow;
  physx::PxReal            twistLimitHigh;
  bool                              twistLimitEnabled;
  void setToDefault(const physx::PxTolerancesScale& scale);
  PxArticulationJointDesc(const physx::PxTolerancesScale& scale);
};

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE PxArticulationJointDesc::PxArticulationJointDesc(const physx::PxTolerancesScale& NMP_UNUSED(scale))
{
  parentPose      = physx::PxTransform::createIdentity();
  childPose      = physx::PxTransform::createIdentity();

  targetPosition    = physx::PxQuat::createIdentity();
  targetVelocity    = physx::PxVec3(0);

  spring        = 0.0f;
  damping        = 0.0f;

  internalCompliance  = 1.0f;
  externalCompliance  = 1.0f;

  swingLimitY      = 0.0f;
  swingLimitZ      = 0.0f;
  swingLimitEnabled  = false;

  twistLimitLow    = 0.0f;
  twistLimitHigh    = 0.0f;
  twistLimitEnabled  = false;
}

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE void PxArticulationJointDesc::setToDefault(const physx::PxTolerancesScale& scale)
{
  *this = PxArticulationJointDesc(scale);
}

//----------------------------------------------------------------------------------------------------------------------
class PxArticulationLinkDesc : public PxRigidBodyDesc
{
public:
  physx::PxTransform          globalPose;
  physx::PxVec3            linearVelocity;
  physx::PxVec3            angularVelocity;
  physx::PxReal            inertiaSphericalisation;
  PxArticulationJointDesc*  joint;
  PX_INLINE PxArticulationLinkDesc(const physx::PxTolerancesScale& scale);
};

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE PxArticulationLinkDesc::PxArticulationLinkDesc(const physx::PxTolerancesScale& NMP_UNUSED(scale)) : 
PxRigidBodyDesc(physx::PxActorType::eARTICULATION_LINK),
globalPose(physx::PxTransform::createIdentity()),
linearVelocity(physx::PxVec3(0)),
angularVelocity(physx::PxVec3(0))
{
  joint        = 0;
}

//----------------------------------------------------------------------------------------------------------------------
template<typename DescType, typename ActorType>
void createShapes(const DescType& desc, ActorType& actor)
{
  for (physx::PxU32 iShape = 0 ; iShape != desc.shapes.getCount(); ++iShape)
  {
    const PxShapeDesc& shapeDesc = *desc.shapes[iShape];
    physx::PxShape *shape = actor.createShape(
      *shapeDesc.geometry, 
      *shapeDesc.materials[0],
      shapeDesc.localPose);
    NMP_ASSERT(shape);
    if (shape)
    {
      shape->setSimulationFilterData(shapeDesc.simulationFilterData);
      shape->setQueryFilterData(shapeDesc.queryFilterData);
      shape->setFlags(shapeDesc.flags);
      shape->setContactOffset(shapeDesc.contactOffset);
      shape->setRestOffset(shapeDesc.restOffset);
      shape->setName(shapeDesc.name);
      shape->userData = shapeDesc.userData;
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE physx::PxRigidDynamic* PxCreateRigidDynamic(const PxRigidDynamicDesc& desc)
{
  physx::PxRigidDynamic* result = PxGetPhysics().createRigidDynamic(desc.globalPose);

  // PxRigidDynamicDesc
  result->setLinearDamping(desc.linearDamping);
  result->setAngularDamping(desc.angularDamping);
  result->setLinearVelocity(desc.linearVelocity);
  result->setAngularVelocity(desc.angularVelocity);
  result->setMaxAngularVelocity(desc.maxAngularVelocity);
  result->setSleepThreshold(desc.sleepEnergyThreshold);
  result->setSolverIterationCounts(desc.minPositionIterations, desc.minVelocityIterations);
  result->setContactReportThreshold(desc.contactReportThreshold);
  result->setRigidDynamicFlags(desc.rigidDynamicFlags);

  // PxRigidBodyDesc
  result->setCMassLocalPose(desc.massLocalPose);
  result->setMassSpaceInertiaTensor(desc.massSpaceInertia);
  if (desc.mass > 0.0f)
    result->setMass(desc.mass);

  // PxRigidActorDesc
  createShapes(desc, *result);

  // PxActorDesc
  result->setDominanceGroup(desc.dominanceGroup);
  result->setActorFlags(desc.actorFlags);
  result->setOwnerClient(desc.ownerClient);
  result->setClientBehaviorBits(desc.clientBehaviorBits);
  result->setName(desc.name);
  result->userData = desc.userData; 

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PX_INLINE physx::PxRigidStatic* PxCreateRigidStatic(const PxRigidStaticDesc& desc)
{
  physx::PxRigidStatic* result = PxGetPhysics().createRigidStatic(desc.globalPose);

  // PxRigidActorDesc
  createShapes(desc, *result);

  // PxActorDesc
  result->setDominanceGroup(desc.dominanceGroup);
  result->setActorFlags(desc.actorFlags);
  result->setOwnerClient(desc.ownerClient);
  result->setClientBehaviorBits(desc.clientBehaviorBits);
  result->setName(desc.name);
  result->userData = desc.userData; 

  return result;
}

#ifdef NM_COMPILER_SNC
#pragma diag_pop
#endif

#endif // MR_PHYSX3_DEPRECATED_H
