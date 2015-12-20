// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.  
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#define  _CRT_SECURE_NO_WARNINGS // for strncpy

#include "NMPlatform/NMPlatform.h"
#include "morpheme/mrRig.h"
#include "morpheme/mrBlendOps.h"
#include "mrPhysX3.h"
#include "mrPhysicsDriverDataPhysX3.h"
#include "mrPhysicsRigPhysX3Articulation.h"
#include "physics/mrPhysicsRigDef.h"
#include "physics/mrPhysicsAttribData.h"
#include "mrPhysicsScenePhysX3.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "morpheme/mrAttribData.h"

#include "NMPlatform/NMProfiler.h"

#include "NMPlatform/NMvpu.h"

//----------------------------------------------------------------------------------------------------------------------
#define MINIMUM_COMPLIANCE 0.001f

// Sanity checks on passing strength/damping to physx
#define MAX_STRENGTH 1e12f
#define MAX_DAMPING 1e25f

namespace MR 
{

// This limit isn't nice, but PhysX is very jittery with tiny ranges, and currently we don't
// have a hinge joint type in PhysX. See MORPH-11273
static const float s_minSwingLimit = NMP::degreesToRadians(3.0f);


ArticulationExplosionHandler* PhysicsRigPhysX3Articulation::s_explosionHandler = 0;

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysicsRigPhysX3Articulation::getMemoryRequirements(PhysicsRigDef *physicsRigDef)
{
  uint32_t numBones = physicsRigDef->getNumParts();
  uint32_t numJoints = physicsRigDef->getNumJoints();
  uint32_t numMaterials = physicsRigDef->getNumMaterials();

  NMP::Memory::Format result(sizeof(PhysicsRigPhysX3Articulation), NMP_VECTOR_ALIGNMENT);

  // space for the materials
  result += NMP::Memory::Format(sizeof(physx::PxMaterial *) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the part pointers
  result += NMP::Memory::Format(sizeof(PhysicsRigPhysX3Articulation::PartPhysX3Articulation*) * numBones, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the joint pointers
  result += NMP::Memory::Format(sizeof(PhysicsRigPhysX3Articulation::JointPhysX3Articulation*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the parts
  result += NMP::Memory::Format(
    NMP::Memory::align(sizeof(PhysicsRigPhysX3Articulation::PartPhysX3Articulation), NMP_NATURAL_TYPE_ALIGNMENT) * numBones,
    NMP_NATURAL_TYPE_ALIGNMENT);

  // Space for the joints
  result += NMP::Memory::Format(
    NMP::Memory::align(sizeof(PhysicsRigPhysX3Articulation::JointPhysX3Articulation), NMP_NATURAL_TYPE_ALIGNMENT) * numJoints,
    NMP_NATURAL_TYPE_ALIGNMENT);

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Articulation *PhysicsRigPhysX3Articulation::init(
  NMP::Memory::Resource &resource, 
  PhysicsRigDef         *physicsRigDef, 
  PhysicsScene          *physicsScene, 
  physx::PxClientID      ownerClientID,
  uint32_t               clientBehaviourBits,
  AnimRigDef            *animRigDef,
  AnimToPhysicsMap      *animToPhysicsMap,
  int32_t                collisionTypeMask,
  int32_t                collisionIgnoreMask)
{
  PhysicsRigPhysX3Articulation *result = (PhysicsRigPhysX3Articulation*)resource.ptr;
  resource.increment(sizeof(PhysicsRigPhysX3Articulation));

  new (result) PhysicsRigPhysX3Articulation((PhysicsScenePhysX3*) physicsScene);
  PhysicsRigPhysX3::init(result, PhysicsRigPhysX3::TYPE_ARTICULATED, ownerClientID, clientBehaviourBits);
  result->m_cachedSleepThreshold = 0.0f;

  uint32_t numParts = physicsRigDef->getNumParts();
  uint32_t numJoints = physicsRigDef->getNumJoints();
  uint32_t numMaterials = physicsRigDef->getNumMaterials();

  // Materials
  resource.align(NMP::Memory::Format(sizeof(physx::PxMaterial*) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_materials = (physx::PxMaterial**)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(physx::PxMaterial*) * numMaterials, NMP_NATURAL_TYPE_ALIGNMENT));

  // Part pointers
  resource.align(NMP::Memory::Format(sizeof(PhysicsRig::Part*) * numParts, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_parts = (PhysicsRig::Part **)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(PhysicsRig::Part*) * numParts, NMP_NATURAL_TYPE_ALIGNMENT));

  // Joint pointers
  resource.align(NMP::Memory::Format(sizeof(PhysicsRig::Joint*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT));
  result->m_joints = (PhysicsRig::Joint **)resource.ptr;
  resource.increment(NMP::Memory::Format(sizeof(PhysicsRig::Joint*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT));

  // Parts
  for (uint32_t i = 0; i < numParts; i++)
  {
    resource.align(NMP::Memory::Format(sizeof(PhysicsRigPhysX3Articulation::PartPhysX3Articulation), NMP_NATURAL_TYPE_ALIGNMENT));
    result->m_parts[i] = (PhysicsRigPhysX3Articulation::PartPhysX3Articulation *)resource.ptr;
    resource.increment(NMP::Memory::Format(sizeof(PhysicsRigPhysX3Articulation::PartPhysX3Articulation), NMP_NATURAL_TYPE_ALIGNMENT));
  }

  // Joints
  for (uint32_t i = 0; i < numJoints; i++)
  {
    resource.align(NMP::Memory::Format(sizeof(PhysicsRigPhysX3Articulation::JointPhysX3Articulation), NMP_NATURAL_TYPE_ALIGNMENT));
    result->m_joints[i] = (PhysicsRigPhysX3Articulation::JointPhysX3Articulation *)resource.ptr;
    resource.increment(NMP::Memory::Format(sizeof(PhysicsRigPhysX3Articulation::JointPhysX3Articulation), NMP_NATURAL_TYPE_ALIGNMENT));
  }

  result->m_animRigDef = animRigDef;
  result->m_animToPhysicsMap = animToPhysicsMap;
  result->m_physicsRigDef = physicsRigDef;

  result->m_collisionTypeMask = collisionTypeMask;
  result->m_collisionIgnoreMask = collisionIgnoreMask;
  result->m_numCollisionGroupIndices = 0;

  // Fill m_materials in with the different materials in the physicsScene
  for (uint32_t i = 0; i < physicsRigDef->m_numMaterials; ++i)
  {
    result->m_materials[i] = createMaterial(physicsRigDef->m_materials[i]);
  }

  uint32_t totalNumShapes = 0;
  for (uint32_t iPart = 0 ; iPart < numParts ; ++iPart)
  {
    PhysicsRigDef::Part &part = physicsRigDef->m_parts[iPart];
    PhysicsRigDef::Part::Volume &volume = part.volume;
    uint32_t numShapes = volume.numSpheres + volume.numBoxes + volume.numCapsules;
    totalNumShapes += numShapes;
  }

  // Allocate memory for link and shape descriptors - they all have to be filled
  // in before anything is created.
  PxArticulationLinkDesc **linkDescs = (PxArticulationLinkDesc **)NMPMemoryAlloc(
    sizeof(PxArticulationLinkDesc*) * numParts);
  NMP_ASSERT(linkDescs);

  PxShapeDesc **shapeDescs = (PxShapeDesc **)NMPMemoryAlloc(
    sizeof(PxShapeDesc*) * totalNumShapes);
  NMP_ASSERT(shapeDescs);
  for (uint32_t iShape = 0 ; iShape < totalNumShapes ; ++iShape)
  {
    shapeDescs[iShape] = (PxShapeDesc *)NMPMemoryAlloc(
      sizeof(PxShapeDesc));
    NMP_ASSERT(shapeDescs[iShape]);
    (void) new (shapeDescs[iShape]) PxShapeDesc(PxGetPhysics().getTolerancesScale());
    shapeDescs[iShape]->name = "Articulation rig part (kinematic or dynamic)";
  }

  // Convert collision sets into a mask per part
  int *partGroupMasks = (int *)alloca(numParts * sizeof(int));
  int *allowedPartGroupMasks = (int *)alloca(numParts * sizeof(int));
  for (uint32_t i = 0; i<numParts; i++)
  {
    partGroupMasks[i] = 0;
    allowedPartGroupMasks[i] = 0;
  }
  for (int32_t i = 0; i<physicsRigDef->m_numCollisionGroups; i++)
  {
    if (physicsRigDef->m_collisionGroups[i].enabled)
    {
      for (int j = 0; j<physicsRigDef->m_collisionGroups[i].numIndices; j++)
      {
        // Indices can't reference a part out of range
        NMP_ASSERT(physicsRigDef->m_collisionGroups[i].indices[j] < (int)numParts); 
        partGroupMasks[physicsRigDef->m_collisionGroups[i].indices[j]] |= 1<<i;
      }
    }

    for (int j = 0; j<physicsRigDef->m_collisionGroups[i].numIndices; j++)
    {
      // Indices can't reference a part out of range
      NMP_ASSERT(physicsRigDef->m_collisionGroups[i].indices[j] < (int)numParts); 
      allowedPartGroupMasks[physicsRigDef->m_collisionGroups[i].indices[j]] |= 1<<i;
    }
  }

  // Incremented as we work on each shape
  uint32_t iShape = 0;

  // There's a maximum limit on the number of parts in an articulation imposed by physx
  NMP_ASSERT(numParts <= 64);

  // Loop setting up each part
  for (uint32_t iPart = 0; iPart < numParts; ++iPart)
  {
    const PhysicsRigDef::Part &part = physicsRigDef->m_parts[iPart];
    const PhysicsRigDef::Part::Volume &volume = part.volume;

    const PhysicsBodyDriverDataPhysX3* bodyDriverData = (const PhysicsBodyDriverDataPhysX3*)part.body.driverData;

    // Actor
    PxRigidDynamicDesc rigidDynamicDesc(PxGetPhysics().getTolerancesScale()); 
    rigidDynamicDesc.ownerClient = result->m_ownerClientID;
    rigidDynamicDesc.globalPose = nmMatrix34ToPxTransform(part.actor.globalPose);
    rigidDynamicDesc.name = part.name;
    rigidDynamicDesc.mass = 1e10f; // gets overridden
    rigidDynamicDesc.massSpaceInertia = 
      physx::PxVec3(rigidDynamicDesc.mass,rigidDynamicDesc.mass,rigidDynamicDesc.mass);

    NMP_ASSERT(part.actor.hasCollision);
    NMP_ASSERT(!part.actor.isFixed);
    rigidDynamicDesc.angularDamping = part.body.angularDamping;
    rigidDynamicDesc.linearDamping = part.body.linearDamping;
    rigidDynamicDesc.maxAngularVelocity = bodyDriverData->m_maxAngularVelocity;
    if (rigidDynamicDesc.maxAngularVelocity < 0.0f)
      rigidDynamicDesc.maxAngularVelocity = 1e6;

    // Note that the following iterations don't affect the internal character physics/drives as
    // this part of the descriptor never makes it into the articulation part.
    // Articulation-specific iterations are set up later. See MORPH-11268

    // This is the number iterations for resolving collisions
    rigidDynamicDesc.minPositionIterations = bodyDriverData->m_positionSolverIterationCount;
    // This is the number of iterations for resolving penetration
    rigidDynamicDesc.minVelocityIterations = bodyDriverData->m_velocitySolverIterationCount;

    uint32_t iFirstShapeForPart = iShape;
    uint32_t numShapes = volume.numBoxes + volume.numCapsules + volume.numSpheres;

    physx::PxFilterData filterData(
      result->m_collisionTypeMask, 
      result->m_collisionIgnoreMask, 
      result->getRigID(), 
      partGroupMasks[iPart]);
    physx::PxShapeFlags flags = physx::PxShapeFlag::eSCENE_QUERY_SHAPE | physx::PxShapeFlag::eSIMULATION_SHAPE;
#ifdef USE_PHYSX_SWEEPS_FOR_CHARACTER
    flags |= physx::PxShapeFlag::eUSE_SWEPT_BOUNDS;
#endif

    // Volumes
    PhysicsRigDef::Part::Volume::Shape* inOrderShapes[MAX_SHAPES_IN_VOLUME];
    physx::PxGeometryType::Enum inOrderShapeTypes[MAX_SHAPES_IN_VOLUME];
    NMP_ASSERT(numShapes <= MAX_SHAPES_IN_VOLUME);

    for (int32_t j = 0 ; j < volume.numBoxes ; ++j)
    {
      int32_t parentIndex = volume.boxes[j].parentIndex;
      inOrderShapes[parentIndex] = &(volume.boxes[j]);
      inOrderShapeTypes[parentIndex] = physx::PxGeometryType::eBOX;
    }
    for (int32_t j = 0 ; j < volume.numCapsules ; ++j)
    {
      int32_t parentIndex = volume.capsules[j].parentIndex;
      inOrderShapes[parentIndex] = &(volume.capsules[j]);
      inOrderShapeTypes[parentIndex] = physx::PxGeometryType::eCAPSULE;
    }
    for (int32_t j = 0 ; j < volume.numSpheres ; ++j)
    {
      int32_t parentIndex = volume.spheres[j].parentIndex;
      inOrderShapes[parentIndex] = &(volume.spheres[j]);
      inOrderShapeTypes[parentIndex] = physx::PxGeometryType::eSPHERE;
    }

    // Volumes
    for (uint32_t j = 0 ; j < numShapes ; ++j)
    {
      PhysicsShapeDriverDataPhysX3* shapeDriverData = (PhysicsShapeDriverDataPhysX3*)inOrderShapes[j]->driverData;

      switch (inOrderShapeTypes[j])
      {
      case physx::PxGeometryType::eBOX:
        {
          PhysicsRigDef::Part::Volume::Box* shape = reinterpret_cast<PhysicsRigDef::Part::Volume::Box *>(inOrderShapes[j]);
          // Get material from the list of different materials (m_materials)
          NMP_ASSERT(shape->materialID <= numMaterials);
          physx::PxMaterial* material = result->m_materials[shape->materialID];

          PxShapeDesc *shapeDesc = shapeDescs[iShape++];
          void *shapeMem = NMPMemoryAlloc(sizeof(physx::PxBoxGeometry));
          NMP_ASSERT(shapeMem);
          physx::PxBoxGeometry *box = new(shapeMem) physx::PxBoxGeometry(nmVector3ToPxVec3(shape->dimensions));
          shapeDesc->geometry = box; 
          shapeDesc->localPose = nmMatrix34ToPxTransform(shape->localPose);
          shapeDesc->contactOffset = shapeDriverData->m_contactOffset;
          shapeDesc->restOffset = shapeDriverData->m_restOffset;
          shapeDesc->setSingleMaterial(material);
          shapeDesc->userData = &(shape->density);
        }
        break;

      case physx::PxGeometryType::eCAPSULE:
        {
          PhysicsRigDef::Part::Volume::Capsule* shape = reinterpret_cast<PhysicsRigDef::Part::Volume::Capsule *>(inOrderShapes[j]);
          // Get material from the list of different materials (m_materials)
          NMP_ASSERT(shape->materialID <= numMaterials);
          physx::PxMaterial* material = result->m_materials[shape->materialID];

          PxShapeDesc *shapeDesc = shapeDescs[iShape++];
          void *shapeMem = NMPMemoryAlloc(sizeof(physx::PxCapsuleGeometry));
          NMP_ASSERT(shapeMem);
          physx::PxCapsuleGeometry *capsule = new(shapeMem) physx::PxCapsuleGeometry(shape->radius, shape->height * 0.5f);
          shapeDesc->geometry = capsule; 
          // convert to capsule orientated along z, not y
          NMP::Matrix34 m(NMP::Matrix34::kIdentity);
          m.fromEulerXYZ(NMP::Vector3(0, NM_PI/2.f, 0));
          shapeDesc->localPose = nmMatrix34ToPxTransform(m * shape->localPose);
          shapeDesc->contactOffset = shapeDriverData->m_contactOffset;
          shapeDesc->restOffset = shapeDriverData->m_restOffset;
          shapeDesc->setSingleMaterial(material);
          shapeDesc->userData = &(shape->density);

          // Get material from the list of different materials (m_materials)
          NMP_ASSERT(shape->materialID <= numMaterials);
        }
        break;

      case physx::PxGeometryType::eSPHERE:
        {
          PhysicsRigDef::Part::Volume::Sphere* shape = reinterpret_cast<PhysicsRigDef::Part::Volume::Sphere *>(inOrderShapes[j]);
          // Get material from the list of different materials (m_materials)
          NMP_ASSERT(shape->materialID <= numMaterials);
          physx::PxMaterial* material = result->m_materials[shape->materialID];

          PxShapeDesc *shapeDesc = shapeDescs[iShape++];
          void *shapeMem = NMPMemoryAlloc(sizeof(physx::PxSphereGeometry));
          NMP_ASSERT(shapeMem);
          physx::PxSphereGeometry *sphere = new(shapeMem) physx::PxSphereGeometry(shape->radius);
          shapeDesc->geometry = sphere; 
          shapeDesc->localPose = nmMatrix34ToPxTransform(shape->localPose);
          shapeDesc->contactOffset = shapeDriverData->m_contactOffset;
          shapeDesc->restOffset = shapeDriverData->m_restOffset;
          shapeDesc->setSingleMaterial(material);

          shapeDesc->userData = &(shape->density);
        }
        break;

      default:
        NMP_ASSERT_FAIL();
      }
    }

    // Fix up common data
    for (uint32_t shapeIndex = 0 ; shapeIndex < numShapes ; ++shapeIndex)
    {
      PxShapeDesc *shapeDesc = shapeDescs[shapeIndex+iFirstShapeForPart];
      shapeDesc->simulationFilterData = filterData;
      shapeDesc->queryFilterData = filterData;
      shapeDesc->flags = flags;
    }

    rigidDynamicDesc.setShapes(&shapeDescs[iFirstShapeForPart], numShapes);

    // Freed at the end of the function
    void *linkMem = NMPMemoryAlloc(sizeof(PxArticulationLinkDesc));
    NMP_ASSERT(linkMem);
    PxArticulationLinkDesc *linkDesc = new(linkMem) PxArticulationLinkDesc(PxGetPhysics().getTolerancesScale());
    linkDescs[iPart] = linkDesc;
    linkDescs[iPart]->globalPose = nmMatrix34ToPxTransform(part.actor.globalPose);
    // Will make the following code calculate the mass using the densities. However, if +ve, it can
    // be made to use a single total mass.
    linkDescs[iPart]->mass = -1; 
    linkDescs[iPart]->massLocalPose = physx::PxTransform::createIdentity();

    linkDescs[iPart]->setShapes(&shapeDescs[iFirstShapeForPart], numShapes);
    linkDescs[iPart]->ownerClient = result->m_ownerClientID;
    linkDescs[iPart]->clientBehaviorBits = result->m_clientBehaviourBits;

    linkDescs[iPart]->inertiaSphericalisation = bodyDriverData->m_inertiaSphericalisation;

    new(result->m_parts[iPart]) PhysicsRigPhysX3Articulation::PartPhysX3Articulation(partGroupMasks[iPart], allowedPartGroupMasks[iPart]);
    PartPhysX3Articulation *partPhysX = (PartPhysX3Articulation*)(result->m_parts[iPart]);
    partPhysX->m_physicsRig = result;
#ifdef STORE_PART_AND_JOINT_NAMES
    size_t partNameLength = NMP_STRLEN(part.name);
    NMP_ASSERT(partNameLength < PhysicsRigPhysX3Articulation::MAX_NAME_SIZE);
    NMP_STRNCPY_S(partPhysX->m_name, PhysicsRigPhysX3Articulation::MAX_NAME_SIZE, part.name);
#endif

#if 1
    // Also create a kinematic "ghost" that is used for collision in HK
    partPhysX->m_kinematicActor = PxCreateRigidDynamic(rigidDynamicDesc);
    NMP_ASSERT(partPhysX->m_kinematicActor);
    partPhysX->m_kinematicActor->setName("Physics rig kinematic part");

    // The following line shouldn't be necessary, but it isn't kinematic without it.
    partPhysX->m_kinematicActor->setRigidDynamicFlag(physx::PxRigidDynamicFlag::eKINEMATIC, true);

    NMP_ASSERT(partPhysX->m_kinematicActor);
    PhysicsRigPhysX3ActorData::create(partPhysX->m_kinematicActor, partPhysX, result);
#endif
  }

  if (physicsRigDef->getNumJoints())
  {
    PhysicsRigPhysX3Articulation::createJoints((PhysicsScenePhysX3*) physicsScene, physicsRigDef, result, linkDescs);
  }

  // Set all the masks etc for the kinematic parts - might be better to do this at creation.
  for (uint32_t iPart = 0; iPart < numParts; ++iPart)
  {
    physx::PxShape *shapes[MAX_SHAPES_IN_VOLUME];
    physx::PxFilterData filterData(
      result->m_collisionTypeMask, 
      result->m_collisionIgnoreMask, result->getRigID(), 
      partGroupMasks[iPart]);

    physx::PxRigidDynamic *kinematicActor = ((PartPhysX3Articulation*)result->getPart(iPart))->getKinematicActor();
    if (kinematicActor)
    {
      filterData.word0 &= ~(1<<GROUP_COLLIDABLE_PUSHABLE);
      filterData.word0 |= 1<<GROUP_COLLIDABLE_NON_PUSHABLE;
      filterData.word1 |= 1<<GROUP_COLLIDABLE_NON_PUSHABLE;
      physx::PxU32 numShapes =  kinematicActor->getShapes(&shapes[0], MAX_SHAPES_IN_VOLUME);
      NMP_ASSERT(shapes[0]);
      for (physx::PxU32 j = 0 ; j < numShapes ; ++j)
      {
        shapes[j]->setQueryFilterData(filterData);
        shapes[j]->setSimulationFilterData(filterData);
        shapes[j]->userData = 0; // original value was just used to store the density
      }
    }
  }


  // Free the working memory.
  for (uint32_t shapeIndex = 0 ; shapeIndex < totalNumShapes ; ++shapeIndex)
  {
    if (shapeDescs[shapeIndex]->geometry)
    {
      NMP::Memory::memFree(shapeDescs[shapeIndex]->geometry);
    }
  }

  for (uint32_t i = 0; i < totalNumShapes; ++i)
  {
    NMP::Memory::memFree(shapeDescs[i]);
  }
  NMP::Memory::memFree(shapeDescs);

  for (uint32_t i = 0; i < numParts; ++i)
  {
    NMP::Memory::memFree(linkDescs[i]);
  }
  NMP::Memory::memFree(linkDescs);

  // Note that the parent joint index of a bone is guaranteed (in morpheme export) to always the
  // bone index - 1, since every part has a parent joint and parent part, except for the root part.
  Part* part = result->getPart(0);
  part->setParentPartIndex(-1);
  for (uint32_t i = 1; i < physicsRigDef->getNumParts(); i++)
  {
    part = result->getPart(i);
    part->setParentPartIndex(physicsRigDef->m_joints[i-1]->m_parentPartIndex);
  }

  result->restoreAllJointDrivesToDefault();

  result->m_isArticulationAddedToScene = false;

  // Need to generate the cached values.
  result->generateCachedValues(1.0f); // timestep is irrelevant at this point

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::createLink(
  physx::PxArticulation *articulation, 
  physx::PxArticulationLink* parent, 
  PhysicsRigPhysX3Articulation::PartPhysX3Articulation* part, 
  PhysicsRigPhysX3Articulation* physicsRig,
  PxArticulationLinkDesc& linkDesc)
{
  m_rigidBody = articulation->createLink(parent, linkDesc.globalPose);
  NMP_ASSERT(m_rigidBody);
  uint32_t numShapes = linkDesc.shapes.getCount();
  for (uint32_t iShape = 0; iShape<numShapes; iShape++)
  {
    const PxShapeDesc *shapeDesc = linkDesc.shapes[iShape];
    physx::PxShape *shape = m_rigidBody->createShape(*shapeDesc->geometry, *shapeDesc->materials[0], shapeDesc->localPose);
    NMP_ASSERT(shape);
#ifdef STORE_PART_AND_JOINT_NAMES
    m_rigidBody->setName(getName());
#endif
    shape->setContactOffset(shapeDesc->contactOffset);
    shape->setRestOffset(shapeDesc->restOffset);
    shape->setSimulationFilterData(shapeDesc->simulationFilterData);
    shape->setQueryFilterData(shapeDesc->queryFilterData);
    shape->setFlags(shapeDesc->flags);
    shape->userData = 0; // Original value was just used to store the density
  }

  if (linkDesc.mass < 0.0f)
  {
    float* densities = (float*)NMPMemoryAlloc(sizeof(float) * numShapes);
    NMP_ASSERT(densities);
    for (uint32_t iShape = 0; iShape < numShapes; iShape++)
    {
      densities[iShape] = *((float*)linkDesc.shapes[iShape]->userData);
    }

    physx::PxRigidBodyExt::updateMassAndInertia(*m_rigidBody, densities, numShapes); // uses density

    NMP::Memory::memFree(densities);

    if (linkDesc.inertiaSphericalisation > 0.0f)
    {
      // This piece of code makes the inertia tensors on articulations more even, it helps a little
      // with jitter, and artifacts are not likely to be big, since each link is constrained anyway.
      physx::PxVec3 massVec = m_rigidBody->getMassSpaceInertiaTensor();
      float averageMass = (massVec.x + massVec.y + massVec.z) / 3.0f;
      physx::PxVec3 evenMass(averageMass, averageMass, averageMass);
      massVec += (evenMass-massVec) * linkDesc.inertiaSphericalisation;
      m_rigidBody->setMassSpaceInertiaTensor(massVec);
    }
  }
  else
  {
    physx::PxRigidBodyExt::setMassAndUpdateInertia(*m_rigidBody, linkDesc.mass); // uses mass
  }

  m_mass = m_rigidBody->getMass();
  m_inertia = nmPxVec3ToVector3(m_rigidBody->getMassSpaceInertiaTensor());
  NMP_ASSERT(m_mass > 0.0f);

  PhysicsRigPhysX3ActorData::create(m_rigidBody, part, physicsRig);
  m_rigidBody->setClientBehaviorBits(physicsRig->m_clientBehaviourBits); 
  m_rigidBody->setOwnerClient(physicsRig->m_ownerClientID);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::createJoints(
  PhysicsScenePhysX3 *physicsScene, 
  PhysicsRigDef *physicsRigDef, 
  PhysicsRigPhysX3Articulation *physicsRig, 
  PxArticulationLinkDesc **linkDescs)
{
  physx::PxArticulation *articulation = physicsScene->getPhysXScene()->getPhysics().createArticulation();
  NMP_ASSERT(articulation != NULL);
  // Since we use warm starting the number of iterations can be very low. However, it should be
  // exposed too (as accuracy will be better with a higher count, if the user can afford it). See MORPH-11268
  articulation->setInternalDriveIterations(1 | 0x80000000); // enable warm starting
  articulation->setExternalDriveIterations(1 | 0x80000000); // enable warm starting
  articulation->setName("PhysicsRigPhysX3Articulation");

  // TODO expose these in connect - MORPH-11268
  articulation->setSolverIterationCounts(4, 2);

  physicsRig->setArticulation(articulation);

  // Add the root link
  const PhysicsJointDef* joint = physicsRigDef->m_joints[0];
  PartPhysX3Articulation *part = (PartPhysX3Articulation*)physicsRig->m_parts[joint->m_parentPartIndex];

  // Copy the first generated link into the morpheme rig part data... assuming it makes a link for this root.
  part->createLink(articulation, 0, part, physicsRig, *linkDescs[joint->m_parentPartIndex]);

  for (int32_t i = 0; i < physicsRigDef->m_numJoints; ++i)
  {
    NMP_ASSERT(physicsRigDef->m_joints[i]->m_jointType == PhysicsJointDef::JOINT_TYPE_SIX_DOF);
    const PhysicsSixDOFJointDef* jointDef = (const PhysicsSixDOFJointDef*)physicsRigDef->m_joints[i];
    const PhysicsJointDriverDataPhysX3* driverData = (const PhysicsJointDriverDataPhysX3*)jointDef->m_driverData;

    new (physicsRig->m_joints[i]) PhysicsRigPhysX3Articulation::JointPhysX3Articulation(jointDef);

    NMP_ASSERT(jointDef->m_parentPartIndex < (int32_t)physicsRig->getNumParts());
    NMP_ASSERT(jointDef->m_childPartIndex < (int32_t)physicsRig->getNumParts());
    part = (PartPhysX3Articulation*)physicsRig->m_parts[jointDef->m_childPartIndex];
    PartPhysX3Articulation* parentPart = ((PartPhysX3Articulation*)physicsRig->m_parts[jointDef->m_parentPartIndex]);

    part->createLink(
      articulation,
      parentPart->getArticulationLink(), 
      part, 
      physicsRig,
      *linkDescs[jointDef->m_childPartIndex]);

    JointPhysX3Articulation *jointPhysX = (JointPhysX3Articulation*)physicsRig->m_joints[i];
    physx::PxArticulationJoint *pxJoint = part->getArticulationLink()->getInboundJoint();
    jointPhysX->m_jointInternal = pxJoint;

    /// Internal and external compliances get set by behaviours, and also reset to 1 in
    /// restoreAllJointDrivesToDefault which is called when a reference is added/removed (i.e.
    /// whenever partial body physics configuration changes.
    pxJoint->setExternalCompliance(1.0f); // this gets overwritten in behaviours
    pxJoint->setInternalCompliance(1.0f); // this gets overwritten in behaviours 
    float s2Limit = NMP::maximum(jointDef->m_hardLimits.getSwing2Limit(), s_minSwingLimit);
    float s1Limit = NMP::maximum(jointDef->m_hardLimits.getSwing1Limit(), s_minSwingLimit);
    pxJoint->setSwingLimitContactDistance(jointDef->m_hardLimits.getSwingLimitContactDistance());
    pxJoint->setSwingLimitContactDistance(jointDef->m_hardLimits.getSwingLimitContactDistance());
    pxJoint->setSwingLimitEnabled(true);
    pxJoint->setTwistLimitContactDistance(jointDef->m_hardLimits.getTwistLimitContactDistance());
    pxJoint->setTwistLimitContactDistance(jointDef->m_hardLimits.getTwistLimitContactDistance());
    pxJoint->setTwistLimitEnabled(true);
    pxJoint->setSpring(0.0f);
    pxJoint->setDamping(driverData->m_articulationDamping);
    // Set the limits, clamping if necessary
    jointPhysX->writeLimits();

    const float amountOfTangentialDamping = 0.8f; // TODO read this in from file?
    float limitCurvature = 2.0f / (s1Limit + s2Limit); 
    // Do we scale by curvature or curvature squared? well, it is somewhere between curvature and
    // curvature squared depending on how fast the joint is moving. Choosing curvature squared here
    // as it gives a better drop off. Why scale by curvature squared then? maybe because big
    // relative motions happen most on small (high curvature) limits.
    pxJoint->setTangentialDamping(amountOfTangentialDamping*limitCurvature*limitCurvature); 
    
    physx::PxTransform jointToParentActor = nmMatrix34ToPxTransform(jointDef->m_parentPartFrame);   // joint -> actor
    pxJoint->setParentPose(jointToParentActor);
    
    physx::PxTransform jointToChildActor = nmMatrix34ToPxTransform(jointDef->m_childPartFrame);    // joint -> actor
    pxJoint->setChildPose(jointToChildActor);

    // Set pointer to joint def in joint.
//    jointPhysX->setDefinition(jointDef);

#ifdef STORE_PART_AND_JOINT_NAMES
    NMP_ASSERT(strlen(jointDef->m_name) < PhysicsRigPhysX3Articulation::MAX_NAME_SIZE - 1);
    strncpy(jointPhysX->m_name, jointDef->m_name, PhysicsRigPhysX3Articulation::MAX_NAME_SIZE);
#endif

    jointPhysX->m_strength = 0.0f;
    jointPhysX->m_damping = driverData->m_articulationDamping;

    jointPhysX->m_maxStrength = driverData->m_articulationSpring;
    jointPhysX->m_maxDamping = driverData->m_articulationDamping;

    jointPhysX->m_driveStrengthScale = driverData->m_driveStrengthScale;
    jointPhysX->m_driveDampingScale = driverData->m_driveDampingScale;
    jointPhysX->m_driveCompensationScale = driverData->m_driveCompensationScale;
    jointPhysX->m_driveMinDampingScale = driverData->m_driveMinDampingScale;

    jointPhysX->m_lastTargetOrientation.setXYZW(0,0,0,0); // So will always update the physX internal target orientation on the first frame.

    jointPhysX->m_rotationDirty = true;
  }
 
#ifdef USE_ARTICULATION_AGGREGATE
  physicsRig->m_aggregate = physicsScene->getPhysXScene()->getPhysics().createAggregate(physicsRig->getNumParts(), true); 
  physicsRig->m_aggregate->addArticulation(*articulation);
#endif
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::term()
{
  physx::PxScene *physXScene = getPhysicsScenePhysX()->getPhysXScene();
  if (physXScene)
  {
    for (uint32_t i = getNumParts(); i-- != 0; )
    {
      PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];
      PhysicsRigPhysX3ActorData::destroy(
        PhysicsRigPhysX3ActorData::getFromActor(part->getArticulationLink()), part->getArticulationLink());
      if (part->getKinematicActor())
      {
        PhysicsRigPhysX3ActorData::destroy(
          PhysicsRigPhysX3ActorData::getFromActor(part->getKinematicActor()), part->getKinematicActor());
        part->getKinematicActor()->release();
      }
    }

    // Releasing the aggregate alone results in the contents being re-added to the scene directly.
    // So - release the articulation first. 
    m_articulation->release();
#ifdef USE_ARTICULATION_AGGREGATE
    m_aggregate->release();
#endif
  }

  for (int32_t i = (int32_t) getNumMaterials(); i-- != 0; )
  {
    m_materials[i]->release();
  }

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Articulation::PhysicsRigPhysX3Articulation(PhysicsScenePhysX3 *physicsScene)
{
  m_physicsScene = physicsScene;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Articulation::PartPhysX3Articulation::PartPhysX3Articulation(
  int32_t defaultCollisionGroupMask, int32_t allowedCollisionGroupMask)
{
  m_userData = NULL;
  m_recalcVels = false;
  m_parentPartIndex = -1;
  m_isKinematic = false;
  m_defaultCollisionGroupMask = m_currentCollisionGroupMask = defaultCollisionGroupMask;
  m_allowedCollisionGroupMask = allowedCollisionGroupMask;
  m_mass = 0.0f;
  m_extraMass = 0.0f;
  m_extraMassCOMPosition.setToZero();
  m_modifiedFlags = 0;
  m_kinematicActor = 0;
  m_constraintToKinematic = 0;
  m_massMultiplier = 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Articulation::PartPhysX3Articulation::~PartPhysX3Articulation() 
{
  if (m_constraintToKinematic)
    m_constraintToKinematic->release();
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Articulation::PartPhysX3Articulation::PartPhysX3Articulation(const PhysicsRigPhysX3Articulation::PartPhysX3Articulation &other) 
{
  *this = other;
}

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Articulation::PartPhysX3Articulation &PhysicsRigPhysX3Articulation::PartPhysX3Articulation::operator=(const PhysicsRigPhysX3Articulation::PartPhysX3Articulation &other) 
{
  if (this == &other)
    return *this;

#ifdef STORE_PART_AND_JOINT_NAMES
  memcpy(m_name, other.m_name, MAX_NAME_SIZE);
#endif
  m_rigidBody = other.m_rigidBody;
  m_parentPartIndex = other.m_parentPartIndex;
  return *this;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::setCurrentCollisionGroupMask(physx::PxU32 mask)
{
  m_currentCollisionGroupMask = mask;

  physx::PxShape *shapeBuffer[MAX_SHAPES_IN_VOLUME];
  physx::PxU32 numShapes = m_rigidBody->getShapes(shapeBuffer, MAX_SHAPES_IN_VOLUME);
  for (physx::PxU32 iShape = 0 ; iShape < numShapes ; ++iShape)
  {
    physx::PxShape *shape = shapeBuffer[iShape];
    physx::PxFilterData filterData = shape->getSimulationFilterData();
    if (filterData.word3 != mask)
    {
      filterData.word3 = mask;
      shape->setSimulationFilterData(filterData);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::PartPhysX3Articulation::generateCachedValues(float timeStep)
{
  // Get the pose from the kinematic part if the articulation wasn't added,
  // otherwise it is delayed by a frame in HK (because in HK we set the position
  // to the last transform and expect physx to move it according to the
  // velocity).
  const physx::PxTransform currentGlobalPose = 
    ((PhysicsRigPhysX3Articulation*)m_physicsRig)->m_isArticulationAddedToScene 
    ? m_rigidBody->getGlobalPose() 
    : m_kinematicActor->getGlobalPose();

  if (!currentGlobalPose.isFinite())
  {
    return false;
  }

  const NMP::Matrix34 currentTM = nmPxTransformToNmMatrix34(currentGlobalPose);

  // Note that the local COM offset wouldn't normally change, but it might if the part shapes are modified.
  NMP_ASSERT(m_rigidBody->getCMassLocalPose().isFinite());
  m_cache.cachedCOMOffset = nmPxTransformToNmMatrix34(m_rigidBody->getCMassLocalPose());

  NMP::Vector3 currentComPos = currentTM.getTransformedVector(m_cache.cachedCOMOffset.translation());

  if (m_recalcVels && timeStep > 0.0f) 
  {
    // reset velocities after physics based on new transform relative to cached (previous) transform

    // calculate the motion at the CoM for the velocities
    NMP::Matrix34 invPrevTM(m_cache.cachedTransform); invPrevTM.invertFast();
    NMP::Matrix34 motionTM = invPrevTM * currentTM;
    m_cache.cachedVel = (motionTM.getTransformedVector(currentComPos) - currentComPos) / timeStep;
    m_cache.cachedAngVel = motionTM.toQuat().toRotationVector(false) / timeStep;
    m_recalcVels = false;
  }
  else
  {
    physx::PxVec3 angVel = m_rigidBody->getAngularVelocity();
    physx::PxVec3 linVel = m_rigidBody->getLinearVelocity();
    if (!angVel.isFinite() || !linVel.isFinite())
    {
      return false;
    }
    m_cache.cachedAngVel = nmPxVec3ToVector3(angVel);
    m_cache.cachedVel = nmPxVec3ToVector3(linVel);
  }

  m_cache.cachedTransform = currentTM;
  m_cache.cachedCOMPosition = currentComPos;

  // The transforms from PhysX can be pretty bad, so orthonormalise the result.
  m_cache.cachedTransform.orthonormalise();

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::handleExplosion(const NMP::Matrix34& worldRoot)
{
  if (m_isArticulationAddedToScene)
  {
    removeArticulationFromScene();
    addArticulationToScene();
  }

  for (uint32_t i = 0; i < getNumParts(); i++)
  {
    PartPhysX3Articulation* partPhysX3Articulation = (PartPhysX3Articulation*)m_parts[i];

    NMP::Matrix34 tm;
    calculateWorldSpacePartTM(
      tm, 
      i, 
      *m_animRigDef->getBindPose()->m_transformBuffer, 
      *m_animRigDef->getBindPose()->m_transformBuffer, 
      worldRoot, 
      false);

    partPhysX3Articulation->setTransform(tm);
    partPhysX3Articulation->setVel(NMP::Vector3::InitZero);
    partPhysX3Articulation->setAngVel(NMP::Vector3::InitZero);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::generateCachedValues(float timeStep)
{
  bool OK = true;
  for (uint32_t i = 0; i < getNumParts(); i++)
  {
    if (!((PartPhysX3Articulation*)m_parts[i])->generateCachedValues(timeStep))
    {
      OK = false;
    }
  }
  if (!OK)
  {
    NMP_MSG("PhysX has exploded - reinitialising at the last known position\n");

    PartPhysX3Articulation* rootPartPhysX3Articulation = (PartPhysX3Articulation*)m_parts[0];
    NMP::Matrix34 worldRoot = rootPartPhysX3Articulation->getTransform();
    if (!worldRoot.isValidTM(0.001f))
    {
      worldRoot.identity();
    }

    if (s_explosionHandler)
    {
      (*s_explosionHandler)(this, worldRoot);
    }
    else
    {
      handleExplosion(worldRoot);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::updatePrePhysics(float timeStep)
{
  const PhysicsRigDef* physicsRigDef = getPhysicsRigDef();

  // Activate requested collision sets
  for (uint32_t i = 0, numParts = getNumParts(); i < numParts; i++)
  {
    MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation *partPhysX = getPartPhysX3Articulation(i);
    physx::PxU32 desiredMask = partPhysX->getDefaultCollisionGroupMask();
    physx::PxU32 allowedMask = partPhysX->getAllowedCollisionGroupMask();
    NMP_ASSERT(m_numCollisionGroupIndices < m_maxCollisionGroupIndices);
    for (int iSet = 0 ; iSet < m_numCollisionGroupIndices ; ++iSet)
    {
      int collisionGroupIndexToActivate = m_collisionGroupIndicesToActivate[iSet];
      if (collisionGroupIndexToActivate >= 0 && (allowedMask & 1 << collisionGroupIndexToActivate))
      {
        NMP_ASSERT(collisionGroupIndexToActivate < m_maxCollisionGroupIndices);
        desiredMask |= 1 << collisionGroupIndexToActivate;
      }
    }
    // If the desired mask has changed then set it on all the shapes.
    if (desiredMask != partPhysX->getCurrentCollisionGroupMask())
    {
      partPhysX->setCurrentCollisionGroupMask(desiredMask);
    }

    // increase the skin width to provide pseudo-CCD
    const PhysicsActorDriverDataPhysX3* actorDriverData =
      (const PhysicsActorDriverDataPhysX3*)physicsRigDef->m_parts[i].actor.driverData;

    float maxSkinWidthIncrease = actorDriverData->m_maxContactOffsetIncrease;
    float speed = partPhysX->getVel().magnitude();
    float skinWidthIncrease = NMP::minimum(speed * timeStep, maxSkinWidthIncrease);
    if (skinWidthIncrease > 0.0f)
    {
      // don't call less than 0 as it resets what the behaviours have asked for (and calling with 0
      // does nothing).
      setSkinWidthIncrease(i, skinWidthIncrease);
    }

    // Reset properties that weren't modified since the last pre-physics call
    if (!(partPhysX->m_modifiedFlags & PartPhysX3Articulation::MODIFIED_EXTRA_MASS))
    {
      partPhysX->setExtraMass(0.0f, NMP::Vector3::InitZero);
    }
    if (!(partPhysX->m_modifiedFlags & PartPhysX3Articulation::MODIFIED_INERTIA))
    {
      partPhysX->setMassSpaceInertia(partPhysX->getOriginalMassSpaceInertia() * partPhysX->m_massMultiplier);
    }
    partPhysX->m_modifiedFlags = 0;
  }

  // Update physX joint limits.
  writeJointLimits();

  // Prevent large compliances from letting the ISF value in physx get < 1, which would lead to
  // instability.
  uint32_t numJoints = getNumJoints();
  for (uint32_t iJoint = 0 ; iJoint != numJoints ; ++iJoint)
  {
    JointPhysX3Articulation* joint = getJointPhysX3Articulation(iJoint);

    float s = joint->getStrength();
    float d = joint->getDamping();
    float c = joint->getExternalCompliance();

    float isf = (1 + d * timeStep + s * timeStep * timeStep) / c;
    if (isf < 1.0f)
    {
      c *= isf;
      joint->setExternalCompliance(c);
    }
  }

  // reset the list of collision sets to activate
  m_numCollisionGroupIndices = 0;

  // Set the separation properties - these will get reset in updatePostPhysics
  m_articulation->setSeparationTolerance(m_desiredJointProjectionLinearTolerance);
  // Note that there is no angular tolerance
  m_articulation->setMaxProjectionIterations(m_desiredJointProjectionIterations);

  updateRegisteredJoints();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::updatePostPhysics(float timeStep)
{
  if (!isReferenced())
  {
    return;
  }

  reenableSleeping();

  PhysicsRigPhysX3Articulation *physicsRigPhysX = (PhysicsRigPhysX3Articulation *)this;
  physicsRigPhysX->generateCachedValues(timeStep);

  // Reset all the joint drives to be relaxed - this is needed so that behaviours can interact
  // nicely with simple physics and we don't have to worry about overwriting physics nodes when
  // trying to stop their actions persisting... However, the multiple calls to the physics api are
  // not likely to be efficient - we should make it so the physx api calls are all done in a
  // pre/post-physics function only, and then this could just be done by setting some state
  // variables. See MORPH-11292
  for (uint32_t i = 0 ;i < physicsRigPhysX->getNumJoints(); ++i)
  {
    JointPhysX3Articulation *joint = getJointPhysX3Articulation(i);
    joint->m_rotationDirty = true;
    joint->setStrength(0.0f);
    joint->setDamping(joint->getMaxDamping());
    joint->setDriveCompensation(0.0f);
  }

#if defined(MR_OUTPUT_DEBUGGING)
  // Copy joint limits from joint into serialisation data structure.
  for (uint32_t i = 0, numJoints = getNumJoints(); i < numJoints; ++i)
  {
    static_cast< JointPhysX3* >(getJoint(i))->updateSerializeTxFrameData();
  }
#endif // MR_OUTPUT_DEBUGGING

  resetJointLimits();
  m_desiredJointProjectionIterations = 0;
  m_desiredJointProjectionLinearTolerance = FLT_MAX;

  for (uint32_t i = 0 ; i < physicsRigPhysX->getNumParts() ; ++i)
  {
    physicsRigPhysX->setSkinWidthIncrease(i, 0.0f);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::setExtraMass(float mass, const NMP::Vector3& massCOMPosition)
{
  m_extraMass = mass;
  m_extraMassCOMPosition = massCOMPosition;
  m_modifiedFlags |= MODIFIED_EXTRA_MASS;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getAugmentedCOMPosition() const
{
  return (m_cache.cachedCOMPosition * m_mass + m_extraMassCOMPosition * m_extraMass) / (m_mass + m_extraMass);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getCOMPosition() const 
{
  return m_cache.cachedCOMPosition; 
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getVelocityAtPoint(const NMP::Vector3 &point) const
{
  NMP::Vector3 rpoint = point - getCOMPosition();
  return getVel() + NMP::vCross(getAngVel(), rpoint);
}

//---------------------------------------------------------------------------------------------------------------------- 
NMP::Vector3 PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getAngularMomentum() const  
{
  NMP::Matrix34 inertia = getGlobalInertiaTensor();
  NMP::Vector3 angVel = getAngVel();
  NMP::Vector3 angMom;
  inertia.rotateVector(angVel, angMom);
  return angMom;
}  

//----------------------------------------------------------------------------------------------------------------------  
NMP::Vector3 PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getLinearMomentum() const  
{    
  return getVel() * getMass();
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Vector3 PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getMassSpaceInertiaTensor() const
{
  return nmPxVec3ToVector3(m_rigidBody->getMassSpaceInertiaTensor());
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Matrix34 PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getGlobalInertiaTensor() const
{
  NMP::Vector3 t = nmPxVec3ToVector3(m_rigidBody->getMassSpaceInertiaTensor());
  NMP::Matrix34 massSpaceInertiaTensor(NMP::Matrix34::kIdentity);
  massSpaceInertiaTensor.scale3x3(t);
  NMP::Matrix34 localOffset = getCOMOffsetLocal();
  NMP::Matrix34 result = massSpaceInertiaTensor * localOffset * getTransform();
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::setMassSpaceInertia(const NMP::Vector3& inertia)
{
  // Node that this does little more than copying the data on the PhysX side, so should be pretty
  // fast.
  m_rigidBody->setMassSpaceInertiaTensor(MR::nmVector3ToPxVec3(inertia));
  m_modifiedFlags |= MODIFIED_INERTIA;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getQuaternion() const
{
  return nmPxQuatToQuat(m_rigidBody->getGlobalPose().q);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::setVel(const NMP::Vector3 &v)
{
  m_rigidBody->setLinearVelocity(nmVector3ToPxVec3(v));
  m_cache.cachedVel = v;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::setAngVel(const NMP::Vector3 &v)
{
  m_rigidBody->setAngularVelocity(nmVector3ToPxVec3(v));
  m_cache.cachedAngVel = v;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::moveTo(const NMP::Matrix34 &tm, bool updateCache)
{
  // Note that with PhysX3 hardkeyframing the kinematic part uses move, but the dynamic part
  // position gets set.
  m_rigidBody->setGlobalPose(nmMatrix34ToPxTransform(tm));
  if (m_isKinematic && m_kinematicActor)
    m_kinematicActor->setKinematicTarget(nmMatrix34ToPxTransform(tm));
  if (updateCache)
  {
    m_cache.cachedTransform = tm;
    m_cache.cachedCOMPosition = nmPxVec3ToVector3(nmMatrix34ToPxTransform(tm).transform(m_rigidBody->getCMassLocalPose().p));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::setPosition(const NMP::Vector3 &NMP_UNUSED(p))
{
  // This function is not currently implemented for PhysX3
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::setQuaternion(const NMP::Quat &NMP_UNUSED(q))
{
  // This function is not currently implemented for PhysX3
  NMP_ASSERT_FAIL();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::setTransform(const NMP::Matrix34 &tm)
{
  NMP_ASSERT(tm.isValidTM(0.1f));
  m_rigidBody->setGlobalPose(nmMatrix34ToPxTransform(tm));
  if (m_isKinematic && m_kinematicActor)
    m_kinematicActor->setGlobalPose(nmMatrix34ToPxTransform(tm));
  m_cache.cachedTransform = tm;
  m_cache.cachedCOMPosition = nmPxVec3ToVector3(nmMatrix34ToPxTransform(tm).transform(m_rigidBody->getCMassLocalPose().p));
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::makeKinematic(
  bool kinematic, float massMultiplier, bool enableConstraint)
{
  if (!kinematic)
  {
    m_isBeingKeyframed = false;
    massMultiplier = 1.0f;
    enableConstraint = false;
  }

  if (
    kinematic == m_isKinematic && 
    massMultiplier == m_massMultiplier && 
    enableConstraint == (m_constraintToKinematic ? true : false)
    )
  {
    return;
  }

  physx::PxArticulationLink *link = getArticulationLink();
  NMP_ASSERT(link);

  if (!kinematic)
  {
    // Move the kinematic shape somewhere far.
    if (m_kinematicActor)
    {
      m_kinematicActor->setGlobalPose(nmMatrix34ToPxTransform(
        ((PhysicsRigPhysX3Articulation*)m_physicsRig)->m_kinematicPose));
    }
  }
  else
  {
    if (m_kinematicActor)
    {
      m_kinematicActor->setGlobalPose(link->getGlobalPose());
    }
  }
  m_isKinematic = kinematic;

  if (massMultiplier != m_massMultiplier)
  {
    link->setMass(m_mass * massMultiplier);
    link->setMassSpaceInertiaTensor(nmVector3ToPxVec3(m_inertia) * massMultiplier);
    m_massMultiplier = massMultiplier;
  }

  if (enableConstraint && !m_constraintToKinematic)
  {
    m_constraintToKinematic = PxD6JointCreate(
      PxGetPhysics(), 
      m_kinematicActor,
      physx::PxTransform::createIdentity(),
      link,
      physx::PxTransform::createIdentity());
    m_constraintToKinematic->setMotion(physx::PxD6Axis::eX, physx::PxD6Motion::eLOCKED);
    m_constraintToKinematic->setMotion(physx::PxD6Axis::eY, physx::PxD6Motion::eLOCKED);
    m_constraintToKinematic->setMotion(physx::PxD6Axis::eZ, physx::PxD6Motion::eLOCKED);
    m_constraintToKinematic->setMotion(physx::PxD6Axis::eSWING1, physx::PxD6Motion::eLOCKED);
    m_constraintToKinematic->setMotion(physx::PxD6Axis::eSWING2, physx::PxD6Motion::eLOCKED);
    m_constraintToKinematic->setMotion(physx::PxD6Axis::eTWIST, physx::PxD6Motion::eLOCKED);
  }
  else if (!enableConstraint && m_constraintToKinematic)
  {
    m_constraintToKinematic->release();
    m_constraintToKinematic = 0;
  }

}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::PartPhysX3Articulation::isKinematic() const
{
  return m_isKinematic;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::enableActorCollision(physx::PxActor *actor, bool enable)
{
  NMP_ASSERT(actor);
  physx::PxShape *shapes[MAX_SHAPES_IN_VOLUME];
  physx::PxRigidActor *rigidActor = actor->isRigidActor();
  NMP_ASSERT(rigidActor);
  NMP_ASSERT(rigidActor->getNbShapes() <= MAX_SHAPES_IN_VOLUME);
  physx::PxU32 numShapes = rigidActor->getShapes(&shapes[0], MAX_SHAPES_IN_VOLUME);
  NMP_ASSERT(numShapes && shapes[0]);

  // Assume all have the same collision enabled/disabled status. However, note that they may have
  // different collision and query status.
  physx::PxShapeFlags flags = shapes[0]->getFlags(); 

  bool enabledSimulation = flags & physx::PxShapeFlag::eSIMULATION_SHAPE;
  if (enabledSimulation != enable)
  {
    for (physx::PxU32 i = 0 ; i < numShapes ; ++i)
    {
      shapes[i]->setFlag(physx::PxShapeFlag::eSIMULATION_SHAPE, enable);
    }
  }

  bool enabledQuery = flags & physx::PxShapeFlag::eSCENE_QUERY_SHAPE;
  if (enabledQuery != enable)
  {
    for (physx::PxU32 i = 0 ; i < numShapes ; ++i)
    {
      shapes[i]->setFlag(physx::PxShapeFlag::eSCENE_QUERY_SHAPE, enable);
    }
  }
  // shouldn't need to call resetFiltering here, as the docs don't indicate that it's necessary
  // after setting the flags since the filter function uses eSUPPRESS (i.e. the filter shader will
  // get called again if the flags change).
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::PartPhysX3Articulation::enableCollision(bool enable)
{
  if (m_isKinematic && m_kinematicActor)
  {
    enableActorCollision(m_kinematicActor, enable);
  }
  else
  {
    enableActorCollision(m_rigidBody, enable);
  }
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::PartPhysX3Articulation::getCollisionEnabled() const
{
  static const physx::PxU32 maxShapes = 1;
  physx::PxShape *shapes[maxShapes];
  m_rigidBody->getShapes(&shapes[0], maxShapes);
  NMP_ASSERT(shapes[0]);
  physx::PxShapeFlags flags = shapes[0]->getFlags(); // assume all have the same collision enabled/disabled status
  bool enabledOnDynamic = flags & physx::PxShapeFlag::eSIMULATION_SHAPE;//getActorFlags() & NX_AF_DISABLE_COLLISION;

  return enabledOnDynamic;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::PartPhysX3Articulation::storeState(PhysicsSerialisationBuffer &savedState)
{
  savedState.addValue(getTransform());
  savedState.addValue(getVel());
  savedState.addValue(getAngVel());
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::PartPhysX3Articulation::restoreState(PhysicsSerialisationBuffer &savedState)
{
  NMP::Matrix34 m = savedState.getValue<NMP::Matrix34>();
  setTransform(m);
  NMP::Vector3 vel = savedState.getValue<NMP::Vector3>();
  setVel(vel);
  NMP::Vector3 angVel = savedState.getValue<NMP::Vector3>();
  setAngVel(angVel);
  return true;
}

#if defined(MR_OUTPUT_DEBUGGING)

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX3Articulation::PartPhysX3Articulation::serializeTxPersistentData(
  uint16_t nameToken, 
  uint32_t objectID, 
  void* outputBuffer, 
  uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsPartPersistentData);

  uint32_t numBoxes = 0;
  uint32_t numCapsules = 0;
  uint32_t numSpheres = 0;

  physx::PxU32 numShapes = m_kinematicActor->getNbShapes();
  NMP_ASSERT(numShapes < MAX_SHAPES_IN_VOLUME);

  physx::PxShape* shapes[MAX_SHAPES_IN_VOLUME];
  numShapes = m_kinematicActor->getShapes(shapes, MAX_SHAPES_IN_VOLUME);
  for (physx::PxU32 i = 0; i != numShapes; ++i)
  {
    const physx::PxShape *shape = shapes[i];

    physx::PxGeometryType::Enum type = shape->getGeometryType();
    switch (type)
    {
    case physx::PxGeometryType::eSPHERE:
      ++numSpheres;
      break;
    case physx::PxGeometryType::eBOX:
      ++numBoxes;
      break;
    case physx::PxGeometryType::eCAPSULE:
      ++numCapsules;
      break;
    default:
      break;
    }
  }

  dataSize += numBoxes * sizeof(PhysicsBoxPersistentData);
  dataSize += numCapsules * sizeof(PhysicsCapsulePersistentData);
  dataSize += numSpheres * sizeof(PhysicsSpherePersistentData);

  if (outputBuffer != 0)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsPartPersistentData *partPersistentData = (PhysicsPartPersistentData *)outputBuffer;

    partPersistentData->m_parentIndex = getParentPartIndex();
    partPersistentData->m_physicsObjectID = objectID;
    partPersistentData->m_numBoxes = numBoxes;
    partPersistentData->m_numCapsules = numCapsules;
    partPersistentData->m_numSpheres = numSpheres;
    partPersistentData->m_nameToken = nameToken;

    // convert to capsule orientated along y, not z, this code mirrors the creation code.
    NMP::Matrix34 yToZ(NMP::Matrix34::kIdentity);

    // convert to capsule orientated along z, not y
    NMP::Matrix34 capsuleConversionTx(NMP::Matrix34::kIdentity);
    capsuleConversionTx.fromEulerXYZ(NMP::Vector3(0, NM_PI_OVER_TWO, 0));

    uint32_t indexBox = 0;
    uint32_t indexCapsule = 0;
    uint32_t indexSphere = 0;
    for (physx::PxU32 i = 0; i != numShapes; ++i)
    {
      const physx::PxShape *pxShape = shapes[i];

      physx::PxGeometryType::Enum type = pxShape->getGeometryType();
      switch (type)
      {
      case physx::PxGeometryType::eSPHERE:
        {
          physx::PxSphereGeometry pxSphere;
          NMP_USED_FOR_ASSERTS(bool result =) pxShape->getSphereGeometry(pxSphere);
          NMP_ASSERT(result);

          PhysicsSpherePersistentData* persistentData = partPersistentData->getSphere(indexSphere);

          physx::PxTransform localPose = pxShape->getLocalPose();
          persistentData->m_localPose = nmPxTransformToNmMatrix34(localPose);

          persistentData->m_radius = pxSphere.radius;

          persistentData->m_parentIndex = i;
          NMP::netEndianSwap(persistentData->m_parentIndex);
          NMP::netEndianSwap(persistentData->m_localPose);
          NMP::netEndianSwap(persistentData->m_radius);

          ++indexSphere;
          break;
        }
      case physx::PxGeometryType::eBOX:
        {
          physx::PxBoxGeometry pxBox;
          NMP_USED_FOR_ASSERTS(bool result =) pxShape->getBoxGeometry(pxBox);
          NMP_ASSERT(result);

          PhysicsBoxPersistentData* persistentData = partPersistentData->getBox(indexBox);

          physx::PxTransform localPose = pxShape->getLocalPose();
          persistentData->m_localPose = nmPxTransformToNmMatrix34(localPose);

          persistentData->m_width = 2.0f * pxBox.halfExtents.x;
          persistentData->m_height = 2.0f * pxBox.halfExtents.y;
          persistentData->m_depth = 2.0f * pxBox.halfExtents.z;

          persistentData->m_parentIndex = i;
          NMP::netEndianSwap(persistentData->m_parentIndex);
          NMP::netEndianSwap(persistentData->m_localPose);
          NMP::netEndianSwap(persistentData->m_width);
          NMP::netEndianSwap(persistentData->m_height);
          NMP::netEndianSwap(persistentData->m_depth);

          ++indexBox;
          break;
        }
      case physx::PxGeometryType::eCAPSULE:
        {
          physx::PxCapsuleGeometry pxCapsule;
          NMP_USED_FOR_ASSERTS(bool result =) pxShape->getCapsuleGeometry(pxCapsule);
          NMP_ASSERT(result);

          PhysicsCapsulePersistentData* persistentData = partPersistentData->getCapsule(indexCapsule);

          physx::PxTransform localPose = pxShape->getLocalPose();
          persistentData->m_localPose = capsuleConversionTx * nmPxTransformToNmMatrix34(localPose);

          persistentData->m_radius = pxCapsule.radius;
          persistentData->m_height = 2.0f * pxCapsule.halfHeight;

          persistentData->m_parentIndex = i;
          NMP::netEndianSwap(persistentData->m_parentIndex);
          NMP::netEndianSwap(persistentData->m_localPose);
          NMP::netEndianSwap(persistentData->m_radius);
          NMP::netEndianSwap(persistentData->m_height);

          ++indexCapsule;
          break;
        }
      default:
        break;
      }
    }

    NMP_ASSERT(indexBox == numBoxes);
    NMP_ASSERT(indexCapsule == numCapsules);
    NMP_ASSERT(indexSphere == numSpheres);

    NMP::netEndianSwap(partPersistentData->m_numSpheres);
    NMP::netEndianSwap(partPersistentData->m_numCapsules);
    NMP::netEndianSwap(partPersistentData->m_numBoxes);
    NMP::netEndianSwap(partPersistentData->m_nameToken);
    NMP::netEndianSwap(partPersistentData->m_parentIndex);
    NMP::netEndianSwap(partPersistentData->m_physicsObjectID);
  }

  return dataSize;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX3Articulation::PartPhysX3Articulation::serializeTxFrameData(void* outputBuffer, uint32_t NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsPartFrameData);

  if (outputBuffer != 0)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);
    PhysicsPartFrameData *partFrameData = (PhysicsPartFrameData *)outputBuffer;

    physx::PxTransform globalPose;
    if (m_isKinematic)
    {
      globalPose = m_kinematicActor->getGlobalPose();
    }
    else
    {
      globalPose = m_rigidBody->getGlobalPose();
    }
    partFrameData->m_globalPose = nmPxTransformToNmMatrix34(globalPose);

    NMP::netEndianSwap(partFrameData->m_globalPose);
  }

  return dataSize;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
// PhysicsRigPhysX3Articulation::JointPhysX3Articulation
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
PhysicsRigPhysX3Articulation::JointPhysX3Articulation::JointPhysX3Articulation(const PhysicsSixDOFJointDef* const def)
: JointPhysX3(def)
{}

#if defined(MR_OUTPUT_DEBUGGING)
//----------------------------------------------------------------------------------------------------------------------
uint32_t PhysicsRigPhysX3Articulation::JointPhysX3Articulation::serializeTxPersistentData(
  const PhysicsJointDef* jointDef,
  uint16_t  stringToken,
  void*     outputBuffer,
  uint32_t  NMP_USED_FOR_ASSERTS(outputBufferSize)) const
{
  uint32_t dataSize = sizeof(PhysicsSixDOFJointPersistentData);
  if (outputBuffer)
  {
    NMP_ASSERT(outputBufferSize >= dataSize);

    PhysicsSixDOFJointPersistentData* persistentData = (PhysicsSixDOFJointPersistentData*)outputBuffer;

    persistentData->m_parentLocalFrame = jointDef->m_parentPartFrame;
    persistentData->m_childLocalFrame = jointDef->m_childPartFrame;
    persistentData->m_parentPartIndex = jointDef->m_parentPartIndex;
    persistentData->m_childPartIndex = jointDef->m_childPartIndex;

    persistentData->m_jointType = PhysicsJointPersistentData::JOINT_TYPE_SIX_DOF;

    physx::PxReal swingLimitY = 0.0f;
    physx::PxReal swingLimitZ = 0.0f;
    m_jointInternal->getSwingLimit(swingLimitY, swingLimitZ);

    // for some reason PxArticulationJoint returns the swing limit values
    // the wrong way round so swap the y and z values.
    persistentData->m_swing1Limit = swingLimitZ;
    persistentData->m_swing2Limit = swingLimitY;

    physx::PxReal twistLimitLow = 0.0f;
    physx::PxReal twistLimitHigh = 0.0f;
    m_jointInternal->getTwistLimit(twistLimitLow, twistLimitHigh);

    persistentData->m_twistLimitLow = twistLimitLow;
    persistentData->m_twistLimitHigh = twistLimitHigh;
    persistentData->m_nameToken = stringToken;

    PhysicsSixDOFJointPersistentData::endianSwap(persistentData);
  }

  return dataSize;
}
#endif

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::JointPhysX3Articulation::storeState(PhysicsSerialisationBuffer& savedState)
{
  (void) savedState;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::JointPhysX3Articulation::restoreState(PhysicsSerialisationBuffer& savedState)
{
  (void) savedState;
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::enableLimit(bool enable)
{
  m_jointInternal->setSwingLimitEnabled(enable);
  m_jointInternal->setTwistLimitEnabled(enable);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::writeLimits()
{
  // PhysX crashes with zero swing range, as well as it just causing jitter when very small (e.g.
  // with "hinge" limits).
  float swing1 = NMP::maximum(m_modifiableLimits.getSwing1Limit(), s_minSwingLimit);
  float swing2 = NMP::maximum(m_modifiableLimits.getSwing2Limit(), s_minSwingLimit);
  float twistLow = m_modifiableLimits.getTwistLimitLow();
  float twistHigh = m_modifiableLimits.getTwistLimitHigh();

  // Optimise this with MORPH-16668. Note that swing1/2 are reversed (compare with the jointed rig
  // where they're not reversed) - this is intentional - just how the joint is in PhysX.
  m_jointInternal->setSwingLimit(swing2, swing1);
  m_jointInternal->setTwistLimit(twistLow, twistHigh);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::setStrength(float strength)
{
  NMP_ASSERT(m_jointInternal);
  NMP_ASSERT(strength >= 0.0f && strength < MAX_STRENGTH);

  m_strength = strength;
  m_jointInternal->setSpring(strength);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::setDamping(float damping)
{
  NMP_ASSERT(m_jointInternal);
  NMP_ASSERT(damping >= 0.0f && damping < MAX_DAMPING);
  m_damping = damping;
  m_jointInternal->setDamping(damping);
}

//----------------------------------------------------------------------------------------------------------------------
bool PhysicsRigPhysX3Articulation::JointPhysX3Articulation::supportsDriveCompensation()
{
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::setDriveCompensation(float driveCompensation)
{
  NMP_ASSERT(m_jointInternal);
  NMP_ASSERT(driveCompensation >= 0.f); 
  setInternalCompliance(1.f/(1.f + driveCompensation));
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3Articulation::JointPhysX3Articulation::getStrength() const
{
  return m_strength;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3Articulation::JointPhysX3Articulation::getDamping() const
{
  return m_damping;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3Articulation::JointPhysX3Articulation::getDriveCompensation() const
{
  NMP_ASSERT(m_jointInternal);
  float internalCompliance = m_jointInternal->getInternalCompliance();
  if (internalCompliance < MINIMUM_COMPLIANCE)
    internalCompliance = MINIMUM_COMPLIANCE;
  return (1.0f / internalCompliance) - 1.0f;
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3Articulation::JointPhysX3Articulation::getInternalCompliance() const
{
  return m_jointInternal->getInternalCompliance();
}

//----------------------------------------------------------------------------------------------------------------------
float PhysicsRigPhysX3Articulation::JointPhysX3Articulation::getExternalCompliance() const
{
  return m_jointInternal->getExternalCompliance();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::setExternalCompliance(float compliance) 
{
  if (compliance < MINIMUM_COMPLIANCE)
    compliance = MINIMUM_COMPLIANCE;
  m_jointInternal->setExternalCompliance(compliance);
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::setInternalCompliance(float compliance) 
{
  if (compliance < MINIMUM_COMPLIANCE)
    compliance = MINIMUM_COMPLIANCE;
  m_jointInternal->setInternalCompliance(compliance);
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX3Articulation::JointPhysX3Articulation::getTargetOrientation()
{
  NMP_ASSERT(m_jointInternal);
  return nmPxQuatToQuat(m_jointInternal->getTargetOrientation());
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::setTargetOrientation(const NMP::Quat &orientation)
{
  NMP_ASSERT(orientation.isValid());
  // This check allows characters to go to sleep when a constant target is being passed in.
  // Since setTargetOrientation wakes up the character.
  if (orientation != m_lastTargetOrientation)
  {
    m_jointInternal->setTargetOrientation(nmQuatToPxQuat(orientation));
  }
  m_lastTargetOrientation = orientation;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::JointPhysX3Articulation::setVelocity(const NMP::Vector3 &velocity)
{
  NMP_ASSERT(velocity.isValid());
  m_jointInternal->setTargetVelocity(nmVector3ToPxVec3(velocity));
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::makeKinematic(bool moveToKinematicPos)
{
  NMP_ASSERT(m_refCount == 0);

  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];
    part->makeKinematic(true, 1.0f, false);
    if (part->m_kinematicActor)
      part->enableActorCollision(part->m_kinematicActor, true);
    part->enableActorCollision(part->m_rigidBody, false);
  }

  if (moveToKinematicPos)
  {
    moveAllToKinematicPos();
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::removeFromScene()
{
  NMP_ASSERT(m_refCount == 0);

  // Workaround for MORPH-15443 where physx sometimes crashes if the articulation is asleep when
  // added to the scene. Still there in PhysX 3.2.1
  m_articulation->wakeUp();

  removeArticulationFromScene();
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];
    if (part->m_kinematicActor)
    {
      getPhysicsScenePhysX()->getPhysXScene()->removeActor(*part->m_kinematicActor);
    }
  }
}
//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::addToScene()
{
  NMP_ASSERT(m_refCount == 0);
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];
    if (part->m_kinematicActor)
    {
      getPhysicsScenePhysX()->getPhysXScene()->addActor(*part->m_kinematicActor);
    }
  }
  addArticulationToScene();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::addArticulationToScene()
{
  if (m_isArticulationAddedToScene)
    return;
  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    // use the fact that the position of the PhysX bone is offset so that it is at the same
    // location as the joint morpheme joint
    PhysicsRig::Part* part = m_parts[i];
    part->setTransform(part->getTransform());
    part->setVel(part->getVel());
    part->setAngVel(part->getAngVel());
  }
#ifdef USE_ARTICULATION_AGGREGATE
  getPhysicsScenePhysX()->getPhysXScene()->addAggregate(*m_aggregate);
#else
  getPhysicsScenePhysX()->getPhysXScene()->addArticulation(*m_articulation);
#endif

  m_isArticulationAddedToScene = true;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::removeArticulationFromScene()
{
  if (!m_isArticulationAddedToScene)
    return;
#ifdef USE_ARTICULATION_AGGREGATE
  getPhysicsScenePhysX()->getPhysXScene()->removeAggregate(*m_aggregate);
#else
  getPhysicsScenePhysX()->getPhysXScene()->removeArticulation(*m_articulation);
#endif
  m_isArticulationAddedToScene = false;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::makeDynamic()
{
  if (!m_isArticulationAddedToScene)
    addArticulationToScene();

  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];
    part->makeKinematic(false, 1.0f, false);
    if (part->m_kinematicActor)
      part->enableActorCollision(part->m_kinematicActor, false);
    part->enableActorCollision(part->m_rigidBody, true);
  }

  for (uint32_t i=0; i<getNumJoints(); ++i)
  {
    PhysicsRigPhysX3Articulation::JointPhysX3Articulation *joint = (JointPhysX3Articulation*)getJoint(i);
    // enable the joint limit
    joint->enableLimit(true);
  }

  // Re-enable gravity.  This will have been disabled by moveAllToKinematicPos if it was previously called.
  if (m_refCount == 0)
  {
    for (uint32_t i = 0; i < getNumParts(); ++i)
    {
      PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];
      // re-enable gravity
      part->m_rigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, false);
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::moveAllToKinematicPos()
{
  physx::PxVec3 delta = nmVector3ToPxVec3(m_kinematicPose.translation()) - ((PartPhysX3Articulation*)getPart(0))->m_rigidBody->getGlobalPose().p;

  // Move the kinematic shape somewhere far.
  physx::PxTransform kinematicPose = nmMatrix34ToPxTransform(m_kinematicPose);

  for (uint32_t i = 0; i < getNumParts(); ++i)
  {
    // move the dynamic part
    PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];
    physx::PxTransform t = part->m_rigidBody->getGlobalPose();
    t.p += delta;
    part->m_rigidBody->setGlobalPose(t);
    part->m_rigidBody->setLinearVelocity(physx::PxVec3(0,0,0));
    part->m_rigidBody->setAngularVelocity(physx::PxVec3(0,0,0));
    // disable gravity
    part->m_rigidBody->setActorFlag(physx::PxActorFlag::eDISABLE_GRAVITY, true);
    if (part->m_kinematicActor)
      part->m_kinematicActor->setGlobalPose(kinematicPose);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::restoreAllJointDrivesToDefault()
{
  for (uint32_t i = 0 ;i < getNumJoints(); ++i)
  {
    PhysicsRigPhysX3Articulation::JointPhysX3Articulation *joint = (JointPhysX3Articulation*)m_joints[i];
    joint->setStrength(0.0f);
    joint->setDamping(joint->getMaxDamping());
    joint->setExternalCompliance(1.0f);
    joint->setInternalCompliance(1.0f);
  }

  m_desiredJointProjectionIterations = 0;
  m_desiredJointProjectionLinearTolerance = FLT_MAX;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::disableSleeping()
{
  float threshold = m_articulation->getSleepThreshold();
  if (threshold > 0.0f)
    m_cachedSleepThreshold = threshold;
  m_articulation->setSleepThreshold(0.0f);
  m_articulation->wakeUp();
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::reenableSleeping()
{
  if (m_articulation->getSleepThreshold() == 0.0f)
    m_articulation->setSleepThreshold(m_cachedSleepThreshold);
}

//----------------------------------------------------------------------------------------------------------------------
#if !defined(NMP_PLATFORM_SIMD) || defined(NM_HOST_IOS)
void PhysicsRigPhysX3Articulation::applyHardKeyframing(
  const NMP::DataBuffer &targetBuffer,
  const NMP::DataBuffer *previousTargetBuffer,
  const NMP::DataBuffer &fallbackBuffer,
  const NMP::Matrix34   &worldRoot,
  const NMP::Matrix34   *previousWorldRoot,
  bool                   enableCollision,
  float                  massMultiplier,
  bool                   enableConstraint,
  float                  dt,
  const PartChooser     &partChooser)
{
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef exists. Check AnimationSets to have physics rigs defined");

  bool hasPrevious = previousTargetBuffer && previousWorldRoot;

  int32_t numParts =  getNumParts();
  NMP::Matrix34* targetTMs = (NMP::Matrix34*)alloca(sizeof(NMP::Matrix34) * (numParts+1));
  NMP::Matrix34* targetTMsOld = 0;
  if (hasPrevious)
  {
    targetTMsOld = (NMP::Matrix34*)alloca(sizeof(NMP::Matrix34) * (numParts+1));
    calculateWorldSpacePartTMsCacheWithVelocity(
      targetTMs,
      targetTMsOld,
      targetBuffer,
      *previousTargetBuffer,
      fallbackBuffer,
      worldRoot,
      *previousWorldRoot);
  }
  else
  {
    calculateWorldSpacePartTMsCache(targetTMs, targetBuffer, fallbackBuffer, worldRoot);
  }

  bool wholeBodyHK = true;
  for (int32_t j = 0; j < numParts; ++j)
  {
    if (!partChooser.usePart(j))
    {
      wholeBodyHK = false;
      continue;
    }

    PhysicsRigPhysX3Articulation::PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[j];
    part->makeKinematic(true, massMultiplier, enableConstraint);
    part->m_isBeingKeyframed = true;

    // Disable the collision on the dynamic actor.
    part->enableActorCollision(part->m_rigidBody, false);
    // Enable collision on the kinematic actor if desired.
    if (part->m_kinematicActor)
      part->enableActorCollision(part->m_kinematicActor, enableCollision);

    // PhysX applies the velocity before the position update (I think) - so if we move to the target
    // position, we have to set the velocity to zero, otherwise the final position is actually
    // position+velocity*dt, which will be a whole frame's offset. This causes big problems with
    // split body physics, as the other parts are being constrained to a part that has zero
    // velocity, but it keeps moving! When we have the velocity data set the position to
    // targetPosition-velocity*dt, and set the velocity as well. This makes sure that the velocity
    // is passed correctly across the interface when there's split body physics.
    //
    // TODO Note that this relies on a fixed dt, since we assume that the time since the last
    // transforms is the same as the time of the upcoming step...
    if (dt > 0.0f && hasPrevious)
    {
      // Use the fact that the position of the PhysX part is offset so that it is at the same
      // location as the morpheme joint
      NMP::Vector3 offset = part->getCOMPosition();
      NMP::Matrix34 offsetTM(NMP::Matrix34::kIdentity), invOffsetTM(NMP::Matrix34::kIdentity);
      offsetTM.translation() = -offset;
      invOffsetTM.translation() = offset;

      // calculate the motion of the target
      NMP::Matrix34 invTargetTMOld(targetTMsOld[j+1]); invTargetTMOld.invertFast();
      // The following pre- and post-multiplication converts diffTM into the actual motion TM
      // centered at the COM.
      NMP::Matrix34 targetMotionTM = invOffsetTM * invTargetTMOld * targetTMs[j+1] * offsetTM;

      NMP::Vector3 targetVel = targetMotionTM.translation() / dt;
      NMP::Vector3 targetAngVel = targetMotionTM.toQuat().toRotationVector(false) / dt;

      // set the dynamic part TMs
      part->moveTo(targetTMsOld[j+1], false);

      // override the kinematic part so that it's where the dynamic parts will be
      if (part->isKinematic() && part->getKinematicActor())
        part->getKinematicActor()->setKinematicTarget(nmMatrix34ToPxTransform(targetTMs[j+1]));

      part->setVel(targetVel);
      part->setAngVel(targetAngVel);
    }
    else
    {
      part->moveTo(targetTMs[j+1], false);
      part->setVel(NMP::Vector3::InitZero);
      part->setAngVel(NMP::Vector3::InitZero);
    }

    part->recalcNextVel();
  }

  if (wholeBodyHK)
  {
    removeArticulationFromScene();
  }
  else
  {
    // Go through all the joints and disable joint limits when both parts are HK. This is undone by
    // the call to enable the joint limit in makeDynamic
    for (uint32_t i = 0 ; i < getNumJoints() ; ++i)
    {
      PhysicsRigPhysX3Articulation::JointPhysX3Articulation *joint = (JointPhysX3Articulation*)getJoint(i);
      const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[i];
      uint32_t i1 = jointDef->m_parentPartIndex;

      if (getPart(i1)->isKinematic())
      {
        uint32_t i2 = jointDef->m_childPartIndex;

        if (getPart(i2)->isKinematic())
        {
          // disable the joint limit
          joint->enableLimit(false);
          // Decrease the compliance so that split body interactions work much better. Better than
          // setting the damping to a large value because that tends to stop the joints reaching their
          // targets.
          // Note that setting this to a smaller value than about 0.1 results in the cm rig
          // exploding...
          joint->setExternalCompliance(0.1f);
          joint->setInternalCompliance(0.1f);
        }
      }
    }
  }
}

#else
//----------------------------------------------------------------------------------------------------------------------
// NMP_PLATFORM_SIMD
void PhysicsRigPhysX3Articulation::applyHardKeyframing(
  const NMP::DataBuffer &targetBuffer,
  const NMP::DataBuffer *previousTargetBuffer,
  const NMP::DataBuffer &fallbackBuffer,
  const NMP::Matrix34   &worldRoot,
  const NMP::Matrix34   *previousWorldRoot,
  bool                   enableCollision,
  float                  massMultiplier,
  bool                   enableConstraint,
  float                  dt,
  const PartChooser     &partChooser)
{
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef exists. Check AnimationSets to have physics rigs defined");

  bool hasPrevious = previousTargetBuffer && previousWorldRoot;

  int32_t numParts = getNumParts();
  NMP::vpu::Matrix* targetTMs = (NMP::vpu::Matrix*)alloca(sizeof(NMP::vpu::Matrix) * (numParts+1));
  NMP::vpu::Matrix* targetTMsOld = 0;
  if (hasPrevious)
  {
    targetTMsOld = (NMP::vpu::Matrix*)alloca(sizeof(NMP::vpu::Matrix) * (numParts+1));
    calculateWorldSpacePartTMsCacheWithVelocity(
      targetTMs,
      targetTMsOld,
      targetBuffer,
      *previousTargetBuffer,
      fallbackBuffer,
      worldRoot,
      *previousWorldRoot);
  }
  else
  {
    calculateWorldSpacePartTMsCache(targetTMs, targetBuffer, fallbackBuffer, worldRoot);
  }
 
  bool wholeBodyHK = true;
  for (int32_t j = 0; j < numParts; ++j)
  {   
    if (!partChooser.usePart(j))
    {
      wholeBodyHK = false;
      continue;
    }

    PhysicsRigPhysX3Articulation::PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[j];
    part->makeKinematic(true, massMultiplier, enableConstraint);
    part->m_isBeingKeyframed = true;

    // Disable the collision on the dynamic actor.
    part->enableActorCollision(part->m_rigidBody, false);
    // Enable collision on the kinematic actor if desired.
    if (part->m_kinematicActor)
      part->enableActorCollision(part->m_kinematicActor, enableCollision);

    // PhysX applies the velocity before the position update (I think) - so if we move to the target
    // position, we have to set the velocity to zero, otherwise the final position is actually
    // position+velocity*dt, which will be a whole frame's offset. This causes big problems with
    // split body physics, as the other parts are being constrained to a part that has zero
    // velocity, but it keeps moving! When we have the velocity data set the position to
    // targetPosition-velocity*dt, and set the velocity as well. This makes sure that the velocity
    // is passed correctly across the interface when there's split body physics.
    //
    // TODO Note that this relies on a fixed dt, since we assume that the time since the last
    // transforms is the same as the time of the upcoming step...
    if (dt > 0.0f && hasPrevious)
    {
      // Use the fact that the position of the PhysX part is offset so that it is at the same
      // location as the morpheme joint 
      NMP::Matrix34 currentTM = part->getTransform();
      NMP::Vector3 offset = part->getCOMPosition();
      NMP::Matrix34 offsetTM(NMP::Matrix34::kIdentity), invOffsetTM(NMP::Matrix34::kIdentity);
      offsetTM.translation() = -offset;
      invOffsetTM.translation() = offset;

      // calculate the motion of the target
      NMP::Matrix34 invTargetTMOld(M34vpu(targetTMsOld[j+1])); invTargetTMOld.invertFast();
      // The following pre- and post-multiplication converts diffTM into the actual motion TM
      // centered at the COM.
      NMP::Matrix34 targetMotionTM = invOffsetTM * invTargetTMOld * M34vpu(targetTMs[j+1]) * offsetTM;

      NMP::Vector3 targetVel = targetMotionTM.translation() / dt;
      NMP::Vector3 targetAngVel = targetMotionTM.toQuat().toRotationVector(false) / dt;

      // set the dynamic part TMs
      part->moveTo(M34vpu(targetTMsOld[j+1]), false);
      part->setVel(targetVel);
      part->setAngVel(targetAngVel);

      // override the kinematic part so that it's where the dynamic parts will be
      if (part->isKinematic() && part->getKinematicActor())
        part->getKinematicActor()->setKinematicTarget(nmMatrix34ToPxTransform(M34vpu(targetTMs[j+1])));

    }
    else
    {
      part->moveTo(M34vpu(targetTMs[j+1]), false);
      part->setVel(NMP::Vector3::InitZero);
      part->setAngVel(NMP::Vector3::InitZero);
    }

    part->recalcNextVel();
  }

  if (wholeBodyHK)
  {
    removeArticulationFromScene();
  }
  else
  {
    // Go through all the joints and disable joint limits when both parts are HK. This is undone by
    // the call to enable the joint limit in makeDynamic
    for (uint32_t i = 0 ; i < getNumJoints() ; ++i)
    {
      PhysicsRigPhysX3Articulation::JointPhysX3Articulation *joint = (JointPhysX3Articulation*)getJoint(i);
      const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[i];

      uint32_t i1 = jointDef->m_parentPartIndex;

      if (getPart(i1)->isKinematic())
      {
        uint32_t i2 = jointDef->m_childPartIndex;

        if (getPart(i2)->isKinematic())
        {
          // disable the joint limit
          joint->enableLimit(false);
          // Decrease the compliance so that split body interactions work much better. Better than
          // setting the damping to a large value because that tends to stop the joints reaching their
          // targets.
          // Note that setting this to a smaller value than about 0.1 results in the cm rig
          // exploding...
          joint->setExternalCompliance(0.1f);
          joint->setInternalCompliance(0.1f);
        }
      }
    }
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::applySoftKeyframing(
    const NMP::DataBuffer &targetBuffer,
    const NMP::DataBuffer &targetBufferOld,
    const NMP::DataBuffer &fallbackBuffer,
    const NMP::Matrix34   &worldRoot,
    const NMP::Matrix34   &worldRootOld,
    bool                   enableCollision,
    bool                   enableJointLimits,
    bool                   preserveMomentum,
    float                  externalJointCompliance,
    float                  gravityCompensationFrac,
    float                  dt,
    float                  weight,
    float                  maxAccel,
    float                  maxAngAccel,
    const PartChooser     &partChooser)
{
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef exists. Check AnimationSets to have physics rigs defined");

  // Do a first pass to set the properties that need to be set even if the weight is zero
  int32_t numParts = getNumParts();
  for (int32_t i = 0; i < numParts; ++i)
  {
    if (!partChooser.usePart(i))
    {
      continue;
    }

    PhysicsRigPhysX3Articulation::PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];

    part->makeKinematic(false, 1.0f, false);
    part->m_isBeingKeyframed = true;
    // Enable the collision on the dynamic actor if desired.
    part->enableActorCollision(part->m_rigidBody, enableCollision);
    // Disable collision on the kinematic actor.
    if (part->m_kinematicActor)
      part->enableActorCollision(part->m_kinematicActor, false);

    // Set the external compliance on the associated joint (joint index = part index - 1, so there's
    // no joint for the root part).
    if (i != 0)
    {
      PhysicsRigPhysX3Articulation::JointPhysX3Articulation *joint = (JointPhysX3Articulation*)getJoint(i-1);
      joint->setExternalCompliance(externalJointCompliance);
    }

    // Enable/disable joint limits on the parent joint, but only if the parent part is also soft
    // keyframed by this node.
    int32_t parentPartIndex = part->getParentPartIndex();
    if (parentPartIndex >= 0)
    {
      if (partChooser.usePart(parentPartIndex))
      {
        int32_t parentJointIndex = i - 1;
        if (parentJointIndex >= 0)
        {
          PhysicsRigPhysX3Articulation::JointPhysX3Articulation* joint = (JointPhysX3Articulation*)getJoint(parentJointIndex);
          joint->enableLimit(enableJointLimits);
        }
      }
    }
  }

#ifdef DEBUG_SK
  static NMP::Vector3 prevVelTargets[64];
  static NMP::Vector3 prevAngVelTargets[64];
#endif

#if !defined(NMP_PLATFORM_SIMD) || defined(NM_HOST_IOS)
  NMP::Matrix34* targetTMs = (NMP::Matrix34*)alloca(sizeof(NMP::Matrix34) * (numParts+1));
  NMP::Matrix34* targetTMsOld = (NMP::Matrix34*)alloca(sizeof(NMP::Matrix34) * (numParts+1));
#else
  NMP::vpu::Matrix* targetTMs = (NMP::vpu::Matrix*)alloca(sizeof(NMP::vpu::Matrix) * (numParts+1));
  NMP::vpu::Matrix* targetTMsOld = (NMP::vpu::Matrix*)alloca(sizeof(NMP::vpu::Matrix) * (numParts+1));
#endif

  calculateWorldSpacePartTMsCacheWithVelocity( targetTMs,
    targetTMsOld,
    targetBuffer,
    targetBufferOld,
    fallbackBuffer,
    worldRoot,
                                               worldRootOld );

  // If the weight is effectively zero then we are only concerned with deviation
  bool weightIsEffectivelyZero = (weight <= 0.0000001f);
 
  // Adjust these for weight
  maxAccel *= weight;
  maxAngAccel *= weight;

  // Calculate this once for use in the loop
  NMP::Vector3 gravityDeltaVel = getPhysicsScenePhysX()->getGravity() * (weight * dt * gravityCompensationFrac);

  // The variables required if we are preserving momentum.
  NMP::Vector3 originalCOMVel(NMP::Vector3::InitZero);
  NMP::Vector3 newCOMVel(NMP::Vector3::InitZero);
  float totalPreservedMass = 0.0f;

  // Iterate over the parts calculating deviation, velocity and angular velocity for each one.
  for (int32_t i = 0; i < numParts; ++i)
  {
    if (!partChooser.usePart(i))
    {
      continue;
    }

    PhysicsRigPhysX3Articulation::PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];

    if (preserveMomentum)
    {
      originalCOMVel += part->getVel() * part->getMass();
      totalPreservedMass += part->getMass();
    }

    // Use the fact that the position of the PhysX bone is offset so that it is at the same
    // location as the morpheme joint 
    if (dt > 0.0f)
    {
      NMP::Matrix34 currentTM = part->getTransform();

      NMP::Vector3 offset = part->getCOMPosition();
      NMP::Matrix34 offsetTM(NMP::Matrix34::kIdentity), invOffsetTM(NMP::Matrix34::kIdentity);
      offsetTM.translation() = -offset;
      invOffsetTM.translation() = offset;

      // Calculate the motion to go from current to new
      NMP::Matrix34 invCurrentTM(currentTM); invCurrentTM.invertFast();
      // The following pre- and post-multiplication converts diffTM into the actual motion TM
      // centered at the COM. 
#if !defined(NMP_PLATFORM_SIMD) || defined(NM_HOST_IOS)
      NMP::Matrix34 motionTM = invOffsetTM * invCurrentTM * targetTMs[i+1] * offsetTM;
#else
      NMP::Matrix34 motionTM = invOffsetTM * invCurrentTM * M34vpu(targetTMs[i+1]) * offsetTM;
#endif

      // Store the distance/angle error
      part->m_SKDeviation = motionTM.translation().magnitude();
      part->m_SKDeviationAngle = motionTM.toRotationVector().magnitude();

      // If the weight is effectively zero then the velocity and angular velocity
      // for each part will be unchanged. Only calculate deviation here.
      if (weightIsEffectivelyZero)
      {
        continue;
      }

      // Calculate the motion of the target itself
#if !defined(NMP_PLATFORM_SIMD) || defined(NM_HOST_IOS)
      NMP::Matrix34 invTargetTMOld(targetTMsOld[i+1]); invTargetTMOld.invertFast();
      NMP::Matrix34 targetMotionTM = invOffsetTM * invTargetTMOld * targetTMs[i+1] * offsetTM;
#else
      NMP::Matrix34 invTargetTMOld(M34vpu(targetTMsOld[i+1])); invTargetTMOld.invertFast();
      NMP::Matrix34 targetMotionTM = invOffsetTM * invTargetTMOld * M34vpu(targetTMs[i+1]) * offsetTM;
#endif

      // This is where the velocity multiplier can be applied
      NMP::Vector3 translation = motionTM.translation();
      NMP::Vector3 rotation = motionTM.toQuat().toRotationVector(false);

      NMP::Vector3 newVel = translation / dt;
      NMP::Vector3 curVel = part->getVel();
      if (maxAccel >= 0.0f)
      {
        // Prevent overshoot by calculating the max speed we can have in the direction towards
        // the target given that we cannot decelerate faster than maxAccel
        NMP::Vector3 deltaVel = newVel - curVel;
        NMP::Vector3 targetVel = targetMotionTM.translation() / dt;
        NMP::Vector3 translationDir = translation;
        float distToTarget = translationDir.normaliseGetLength();
        float curVelAlongTranslation = curVel.dot(translationDir);
        float targetVelAlongTranslation = targetVel.dot(translationDir);

        if (curVelAlongTranslation > targetVelAlongTranslation)
        {
          float timeToCatchUp = distToTarget / (curVelAlongTranslation - targetVelAlongTranslation);
          float maxCurVelAlongTranslation = targetVelAlongTranslation + timeToCatchUp * maxAccel;
          if (curVelAlongTranslation > maxCurVelAlongTranslation)
          {
            // Replace the old component along the translation with the new max value
            newVel += translationDir * (maxCurVelAlongTranslation - newVel.dot(translationDir));
            deltaVel = newVel - curVel;
          }
        }

        // Clamp the acceleration
        float deltaVelMag = deltaVel.magnitude();
        if (deltaVelMag > maxAccel * dt)
          deltaVel *= maxAccel * dt / deltaVelMag;

        // Apply gravity compensation
        deltaVel -= gravityDeltaVel;
        newVel = curVel + deltaVel;
      }
      part->setVel(newVel);

      if (preserveMomentum)
        newCOMVel += newVel * part->getMass();

      NMP::Vector3 newAngVel = rotation / dt;
      NMP::Vector3 curAngVel = part->getAngVel();
      if (maxAngAccel >= 0.0f)
      {
        // Limit the max angular velocity target - this is just a straight conversion of the linear velocity code, so I
        // _think_ it's "correct"!
        NMP::Vector3 deltaAngVel = newAngVel - curAngVel;
        NMP::Vector3 targetAngVel = targetMotionTM.toQuat().toRotationVector(false) / dt;
        NMP::Vector3 rotationDir = rotation;
        float distToTarget = rotationDir.normaliseGetLength();
        float curAngVelAlongRotation = curAngVel.dot(rotationDir);
        float targetAngVelAlongRotation = targetAngVel.dot(rotationDir);

        if (curAngVelAlongRotation > targetAngVelAlongRotation)
        {
          float timeToCatchUp = distToTarget / (curAngVelAlongRotation - targetAngVelAlongRotation);
          float maxCurAngVelAlongRotation = targetAngVelAlongRotation + timeToCatchUp * maxAngAccel;

          if (curAngVelAlongRotation > maxCurAngVelAlongRotation)
          {
            // Replace the old component along the translation with the new max value
            newAngVel += rotationDir * (maxCurAngVelAlongRotation - newAngVel.dot(rotationDir));
            deltaAngVel = newAngVel - curAngVel;
          }
        }

        // Clamp the acceleration
        float deltaAngVelMag = deltaAngVel.magnitude();
        if (deltaAngVelMag > maxAngAccel * dt)
          deltaAngVel *= maxAngAccel * dt / deltaAngVelMag;
        newAngVel = curAngVel + deltaAngVel;
      }
      part->setAngVel(newAngVel);

#ifdef DEBUG_SK
      if (i == 0)
      {
        printf("VelTarget = (%6.2f %6.2f %6.2f) CurVel = (%6.2f %6.2f %6.2f) next VelTarget = (%6.2f %6.2f %6.2f)     dt = %6.4f\n",
          prevVelTargets[i].x, prevVelTargets[i].y, prevVelTargets[i].z, 
          curVel.x, curVel.y, curVel.z,
          newVel.x, newVel.y, newVel.z,
          dt);
    }
      prevVelTargets[i] = newVel;
      prevAngVelTargets[i] = newAngVel;
#endif

  }
  }

  if (preserveMomentum && weightIsEffectivelyZero == false)
  {
    originalCOMVel /= totalPreservedMass;
    newCOMVel /= totalPreservedMass;
    NMP::Vector3 correctionVel = originalCOMVel - newCOMVel;
    for (uint32_t i = 0; i < getNumParts(); ++i)
    {
      if (!partChooser.usePart(i))
        continue;
      PhysicsRigPhysX3Articulation::PartPhysX3Articulation *part = (PartPhysX3Articulation*)m_parts[i];
      NMP::Vector3 partVel = part->getVel();
      part->setVel(partVel + correctionVel);
    }
  }
}

//---------------------------------------------------------------------------------------------------------------------- 
void PhysicsRigPhysX3Articulation::applyActiveAnimation(uint32_t jointIndex, const NMP::Quat &targetQuat, bool makeChildDynamic) 
{ 
  NMP_ASSERT(jointIndex < getNumJoints());  
  physx::PxArticulationJoint *joint = ((JointPhysX3Articulation*)m_joints[jointIndex])->m_jointInternal;  
  const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
  if (makeChildDynamic)  
  {   
    PartPhysX3Articulation *childPart = (PartPhysX3Articulation*)m_parts[jointDef->m_childPartIndex];     
    childPart->makeKinematic(false, 1.0f, false);
    childPart->m_isBeingKeyframed = false;
  }   
  // Don't force either of the parts to have collision - no way we could know which one _should_ have collision if it's
  // disabled elsewhere.
  joint->setTargetOrientation(nmQuatToPxQuat(targetQuat));  
  return; 
}

//----------------------------------------------------------------------------------------------------------------------
// Drives the joints to the targets given by the input animation buffer.
void PhysicsRigPhysX3Articulation::applyActiveAnimation(
    const NMP::DataBuffer& targetBuffer,
    const NMP::DataBuffer& fallbackBuffer,
    float                  strengthMultiplier,
    float                  dampingMultiplier,
    float                  internalCompliance,
    float                  externalCompliance,
    bool                   enableJointLimits,
    const JointChooser    &jointChooser,
    float                  limitClampFraction)
{
  NMP_ASSERT_MSG(m_physicsRigDef != NULL, "No RigDef exists. Check AnimationSets to have physics rigs defined");

  for (uint32_t i = 0; i < getNumJoints(); ++i)
  {
    if (!jointChooser.useJoint(i))
      continue;

    PhysicsRigPhysX3Articulation::JointPhysX3Articulation *joint = (JointPhysX3Articulation*)m_joints[i];
    const PhysicsSixDOFJointDef* jointDef = static_cast<const PhysicsSixDOFJointDef*>(m_physicsRigDef->m_joints[i]);
    PhysicsRigPhysX3Articulation::PartPhysX3Articulation *childPart = (PartPhysX3Articulation*)m_parts[jointDef->m_childPartIndex];
    childPart->makeKinematic(false, 1.0f, false);
    childPart->m_isBeingKeyframed = false;

    // Don't force either of the bones to have collision - no way we could know which one _should_
    // have collision if it's disabled elsewhere.

    float newStrength = joint->getMaxStrength() * strengthMultiplier;
    float newDamping  = joint->getMaxDamping() * dampingMultiplier;
    joint->setStrength(newStrength);
    joint->setDamping(newDamping);
    joint->setInternalCompliance(internalCompliance);
    joint->setExternalCompliance(externalCompliance);
    joint->enableLimit(enableJointLimits);

    if (strengthMultiplier < 0.0000001f)
      continue;

    NMP::Quat curQ;
    getQuatFromTransformBuffer(jointDef->m_childPartIndex, targetBuffer, fallbackBuffer, curQ);

    // q is the rotation of the child relative to the parent (in parent space).
    // We need to account for the offset axes in the joint.

    // Get the local joint axes in each frame as l0, l1
    NMP::Quat l0 = jointDef->m_parentFrameQuat;
    NMP::Quat l1 = jointDef->m_childFrameQuat;

    // Now "assuming" the parent is at the origin (since we already have the relative rotation q)
    // we want to calculate rot, the relative rotation of the child local frame from the parent local frame
    NMP::Quat l0Inv = ~l0;

    // Target orientations outside the limits cause oscillations when physical limits are enabled
    if (limitClampFraction >= 0.0f)
    {
      joint->clampToLimits(curQ, limitClampFraction, NULL);
    }
    NMP::Quat curFrameQ = l0Inv * curQ * l1;
    joint->setTargetOrientation(curFrameQ);
  }
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Quat PhysicsRigPhysX3Articulation::getJointQuat(uint32_t jointIndex) 
{
  NMP_ASSERT(jointIndex < getNumJoints());
  JointPhysX3Articulation *joint = (JointPhysX3Articulation*)m_joints[jointIndex];
  const PhysicsJointDef* jointDef = m_physicsRigDef->m_joints[jointIndex];
  if (joint->m_rotationDirty)
  {
    NMP::Quat part1Quat = (getPartPhysX3Articulation(jointDef->m_parentPartIndex))->getQuaternion();
    NMP::Quat part2Quat = (getPartPhysX3Articulation(jointDef->m_childPartIndex))->getQuaternion();
    joint->m_actualRotation = ~(part1Quat * jointDef->m_parentFrameQuat) * (part2Quat * jointDef->m_childFrameQuat); 
    joint->m_rotationDirty = false;
  }
  return joint->m_actualRotation;
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::setCollisionGroupsToActivate(const int *collisionGroupIndices, int numCollisionGroupIndices)
{
  NMP_ASSERT(numCollisionGroupIndices <= m_maxCollisionGroupIndices);
  m_numCollisionGroupIndices = NMP::minimum(m_maxCollisionGroupIndices, numCollisionGroupIndices);
  
  for (int i = 0 ; i < m_numCollisionGroupIndices ; ++i)
  {
    m_collisionGroupIndicesToActivate[i] = collisionGroupIndices[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::dumpToRepX(physx::repx::RepXCollection *collection, 
                                              physx::repx::RepXIdToRepXObjectMap *idMap) const
{
  for (uint32_t iMaterial = 0 ; iMaterial < m_physicsRigDef->m_numMaterials ; ++iMaterial)
  {
    physx::repx::RepXObject material = physx::repx::createRepXObject(m_materials[iMaterial]);
#ifdef NMP_ENABLE_ASSERTS
    physx::repx::RepXAddToCollectionResult result =
#endif
      collection->addRepXObjectToCollection(material, *idMap);
    NMP_ASSERT(result.isValid());
  }

  physx::repx::RepXObject articulation = physx::repx::createRepXObject(m_articulation);
#ifdef NMP_ENABLE_ASSERTS
  physx::repx::RepXAddToCollectionResult result =
#endif
    collection->addRepXObjectToCollection(articulation, *idMap);
  NMP_ASSERT(result.isValid());
}

//----------------------------------------------------------------------------------------------------------------------
void PhysicsRigPhysX3Articulation::setExplosionHandler(ArticulationExplosionHandler* handler)
{
  s_explosionHandler = handler;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
