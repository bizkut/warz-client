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
#include "PhysicsRigDefBuilder.h"
#include "export/mcExportPhysics.h"
#include "acPhysicsCore.h"
#include "physics/mrPhysicsRigDef.h"

#define MAX_NUM_LINKS 64

//----------------------------------------------------------------------------------------------------------------------

namespace AP
{
//----------------------------------------------------------------------------------------------------------------------
bool sixDOFHasSoftLimit(const ME::PhysicsJointExport* physicsJointExport)
{
  const ME::AttributeBlockExport* attributeBlock = physicsJointExport->getAttributeBlock();
  bool useSoftSwing1 = false;
  attributeBlock->getBoolAttribute("useSoftSwing1", useSoftSwing1);
  if (useSoftSwing1)
  {
    return true;
  }

  bool useSoftSwing2 = false;
  attributeBlock->getBoolAttribute("useSoftSwing2", useSoftSwing2);
  if (useSoftSwing2)
  {
    return true;
  }

  bool useSoftTwist = false;
  attributeBlock->getBoolAttribute("useSoftTwist", useSoftTwist);
  return useSoftTwist;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t getTotalNumMaterials(const ME::PhysicsRigDefExport* physicsRigDef)
{
  uint32_t numShapes = 0;
  for (uint32_t partIndex = 0, numParts = physicsRigDef->getNumParts(); partIndex < numParts; ++partIndex)
  {
    const ME::PhysicsPartExport* physicsPartExport = physicsRigDef->getPart(partIndex);
    const ME::PhysicsVolumeExport* physicsVolumeExport = physicsPartExport->getVolume();
    numShapes += physicsVolumeExport->getNumShapes();
  }
  return numShapes;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format PhysicsRigDefBuilder::getMemoryRequirements(
  AP::AssetProcessor*             NMP_UNUSED(processor),
  AP::PhysicsDriverDataBuilder*       physicsDriverBuilder,
  const ME::PhysicsRigDefExport*  physicsRigDefExport)
{
  NMP_VERIFY_MSG(physicsRigDefExport->getNumParts() > 0, "Physics rig has no parts");

  NMP::Memory::Format format(NMP::Memory::align(sizeof(MR::PhysicsRigDef), NMP_NATURAL_TYPE_ALIGNMENT), NMP_VECTOR_ALIGNMENT);

  // space for materials
  uint32_t totalNumMaterials = getTotalNumMaterials(physicsRigDefExport);
  NMP_VERIFY(totalNumMaterials > 0);

  format += NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part::Material) * totalNumMaterials, NMP_NATURAL_TYPE_ALIGNMENT);
  format += physicsDriverBuilder->getMaterialMemoryRequirements(totalNumMaterials);

  //-----------------------
  // space for the collision set objects
  uint32_t numDisabledCollisions = physicsRigDefExport->getNumDisabledCollisions();
  format += NMP::Memory::Format(sizeof(MR::PhysicsRigDef::CollisionGroup) * numDisabledCollisions, NMP_NATURAL_TYPE_ALIGNMENT);

  for (uint32_t i = 0; i < physicsRigDefExport->getNumDisabledCollisions(); ++i)
  {
    const ME::PhysicsDisabledCollisionExport* disabledCollision = physicsRigDefExport->getDisabledCollision(i);
    switch (disabledCollision->getDisabledType())
    {
    case ME::PhysicsDisabledCollisionExport::DISABLED_TYPE_PAIR:
      NMP_VERIFY_FAIL("PhysicsDisabledCollisionExport::DISABLED_TYPE_PAIR is not currently supported");
      break;

    case ME::PhysicsDisabledCollisionExport::DISABLED_TYPE_SET:
      const ME::PhysicsDisabledCollisionSetExport* set = (ME::PhysicsDisabledCollisionSetExport*)disabledCollision;
      // space for each index in this collision set
      format += NMP::Memory::Format(sizeof(int32_t) * set->getNumParts(), NMP_NATURAL_TYPE_ALIGNMENT);
      break;
    }
  }

  //-----------------------
  // trajectory parts
  int32_t numTrajectoryParts = physicsRigDefExport->getNumTrajectoryCalcMarkupParts();
  format += NMP::Memory::Format(sizeof(uint32_t) * numTrajectoryParts, NMP_NATURAL_TYPE_ALIGNMENT);

  //-----------------------
  // physics joints
  uint32_t numJoints = physicsRigDefExport->getNumJoints();
  format += NMP::Memory::Format(sizeof(MR::PhysicsJointDef*) * numJoints, NMP_NATURAL_TYPE_ALIGNMENT);
  
  for (uint32_t i = 0; i < numJoints; ++i)
  {
    const ME::PhysicsJointExport* jointExport = physicsRigDefExport->getJoint(i);

    const char* jointTypeStr = jointExport->getJointType();
    if (NMP_STRCMP(jointTypeStr, "6DOF") == 0)
    {
      format += NMP::Memory::Format(sizeof(MR::PhysicsSixDOFJointDef), NMP_VECTOR_ALIGNMENT);
      if (sixDOFHasSoftLimit(jointExport))
      {
        format += NMP::Memory::Format(sizeof(MR::PhysicsSixDOFJointDef::SoftLimit), NMP_VECTOR_ALIGNMENT);
      }
    }
    else if (NMP_STRCMP(jointTypeStr, "Ragdoll") == 0)
    {
      format += NMP::Memory::Format(sizeof(MR::PhysicsRagdollJointDef), NMP_VECTOR_ALIGNMENT);
    }

    format += physicsDriverBuilder->getJointMemoryRequirements(jointExport);

    format += NMP::Memory::Format(1 + strlen(jointExport->getName()), NMP_NATURAL_TYPE_ALIGNMENT);
  }

  //-----------------------
  // physics parts
  uint32_t numParts = physicsRigDefExport->getNumParts();
  format += NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part) * numParts, NMP_VECTOR_ALIGNMENT);

  for (uint32_t i = 0; i < numParts; ++i)
  {
    const ME::PhysicsPartExport* part = physicsRigDefExport->getPart(i);

    format += physicsDriverBuilder->getPartMemoryRequirements(part);

    //-----------------------
    // volume and shapes
    const ME::PhysicsVolumeExport* volume = part->getVolume();
    uint32_t numSpheres = volume->getNumShapes(ME::PhysicsShapeExport::SHAPE_TYPE_SPHERE);
    uint32_t numBoxes = volume->getNumShapes(ME::PhysicsShapeExport::SHAPE_TYPE_BOX);
    uint32_t numCapsules = volume->getNumShapes(ME::PhysicsShapeExport::SHAPE_TYPE_CAPSULE);

    format += NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part::Volume::Sphere) * numSpheres, NMP_VECTOR_ALIGNMENT);
    format += physicsDriverBuilder->getShapeMemoryRequirements(ME::PhysicsShapeExport::SHAPE_TYPE_SPHERE, numSpheres);

    format += NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part::Volume::Box) * numBoxes, NMP_VECTOR_ALIGNMENT);
    format += physicsDriverBuilder->getShapeMemoryRequirements(ME::PhysicsShapeExport::SHAPE_TYPE_BOX, numBoxes);

    format += NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part::Volume::Capsule) * numCapsules, NMP_VECTOR_ALIGNMENT);
    format += physicsDriverBuilder->getShapeMemoryRequirements(ME::PhysicsShapeExport::SHAPE_TYPE_CAPSULE, numCapsules);

    //-----------------------
    // volume name
    format += NMP::Memory::Format(1 + strlen(volume->getName()), NMP_NATURAL_TYPE_ALIGNMENT);

    //-----------------------
    // part name
    format += NMP::Memory::Format(1 + strlen(part->getName()), NMP_NATURAL_TYPE_ALIGNMENT);
  }

  return format;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Resource PhysicsRigDefBuilder::init(
  AP::AssetProcessor*             processor,
  AP::PhysicsDriverDataBuilder*       physicsDriverBuilder,
  const ME::PhysicsRigDefExport*  physicsRigDefExport)
{
  NMP::Memory::Format memFmt = getMemoryRequirements(processor, physicsDriverBuilder, physicsRigDefExport);
  NMP::Memory::Resource rigDefResource = NMPMemoryAllocateFromFormat(memFmt);
  NMP::Memory::Resource result = rigDefResource;

  NMP::Memory::Format format = NMP::Memory::Format(sizeof(MR::PhysicsRigDef), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::PhysicsRigDef* physicsRigDef = (MR::PhysicsRigDef*)result.alignAndIncrement(format);

  NMP::Vector3 p(NMP::Vector3::InitZero);
  NMP::Quat q(NMP::Quat::kIdentity);
  physicsRigDef->m_rootPart.index = 0;
  physicsRigDefExport->getPhysicsRootPart(physicsRigDef->m_rootPart.index, p, q);
  physicsRigDef->m_rootPart.transform.initialise(q, p);

  physicsRigDef->m_numParts = (int32_t)physicsRigDefExport->getNumParts();
  physicsRigDef->m_numJoints = (int32_t)physicsRigDefExport->getNumJoints();

  physicsRigDef->m_numCollisionGroups = (int32_t)physicsRigDefExport->getNumDisabledCollisions();
  physicsRigDef->m_numTrajectoryParts = (int32_t)physicsRigDefExport->getNumTrajectoryCalcMarkupParts();

  // note that each part can have multiple shapes, and therefore multiple materials
  physicsRigDef->m_numMaterials = getTotalNumMaterials(physicsRigDefExport);

  //-----------------------
  // materials: create the list of unique materials
  format = NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part::Material) * 
    physicsRigDef->m_numMaterials, NMP_NATURAL_TYPE_ALIGNMENT);
  physicsRigDef->m_materials = (MR::PhysicsRigDef::Part::Material*)result.alignAndIncrement(format);

  //-----------------------
  // Loop through the number of parts and shapes in order to initialise the materials
  uint32_t materialID = 0;
  for (uint32_t i = 0; i < physicsRigDefExport->getNumParts(); ++i)
  {
    const ME::PhysicsPartExport* part = physicsRigDefExport->getPart(i);
    const ME::PhysicsVolumeExport* volume = part->getVolume();

    // Loop through the number of shapes in each part
    for (uint32_t shapeIndex = 0, numShapes = volume->getNumShapes(); shapeIndex < numShapes; ++shapeIndex)
    {
      const ME::PhysicsShapeExport* shape = volume->getShape(shapeIndex);
      physicsRigDef->m_materials[materialID].friction = shape->getFriction();
      physicsRigDef->m_materials[materialID].restitution = shape->getRestitution();
      physicsDriverBuilder->initMaterialDriverData(shape, physicsRigDef->m_materials[materialID++], result);
    }
  }

  //-----------------------
  // collision groups
  format = NMP::Memory::Format(sizeof(MR::PhysicsRigDef::CollisionGroup) * physicsRigDef->m_numCollisionGroups, NMP_NATURAL_TYPE_ALIGNMENT);
  physicsRigDef->m_collisionGroups = (MR::PhysicsRigDef::CollisionGroup*) result.alignAndIncrement(format);

  for (int32_t i = 0; i != physicsRigDef->m_numCollisionGroups; ++i)
  {
    const ME::PhysicsDisabledCollisionSetExport* set = (ME::PhysicsDisabledCollisionSetExport*)physicsRigDefExport->getDisabledCollision(i);
    physicsRigDef->m_collisionGroups[i].enabled = set->getEnabled();
    physicsRigDef->m_collisionGroups[i].numIndices = set->getNumParts();

    format = NMP::Memory::Format(sizeof(int32_t) * physicsRigDef->m_collisionGroups[i].numIndices, NMP_NATURAL_TYPE_ALIGNMENT);
    physicsRigDef->m_collisionGroups[i].indices = (int32_t*) result.alignAndIncrement(format);

    for (int32_t j = 0; j < physicsRigDef->m_collisionGroups[i].numIndices; j++)
    {
      physicsRigDef->m_collisionGroups[i].indices[j] = set->getPartIndex(j);
    }
  }

  //-----------------------
  // trajectory parts
  format = NMP::Memory::Format(sizeof(uint32_t) * physicsRigDef->m_numTrajectoryParts, NMP_NATURAL_TYPE_ALIGNMENT);
  physicsRigDef->m_trajectoryParts = (uint32_t*) result.alignAndIncrement(format);
  physicsRigDefExport->getTrajectoryCalcMarkupPartArray(physicsRigDef->m_trajectoryParts, physicsRigDef->m_numTrajectoryParts);

  //-----------------------
  // joints
  format = NMP::Memory::Format(sizeof(MR::PhysicsJointDef*) * physicsRigDef->m_numJoints, NMP_NATURAL_TYPE_ALIGNMENT);
  physicsRigDef->m_joints = (MR::PhysicsJointDef**)result.alignAndIncrement(format);
 
  for (int32_t i = 0 ; i < physicsRigDef->m_numJoints; ++i)
  {
    const ME::PhysicsJointExport* jointExport = physicsRigDefExport->getJoint((uint32_t)i);

    const char* jointTypeStr = jointExport->getJointType();
    if (NMP_STRCMP(jointTypeStr, "6DOF") == 0)
    {
      physicsRigDef->m_joints[i] = initSixDOFJoint(result, jointExport);
    }
    else if (NMP_STRCMP(jointTypeStr, "Ragdoll") == 0)
    {
      physicsRigDef->m_joints[i] = initRagdollJoint(result, jointExport);
    }

    const ME::AttributeBlockExport* attributeBlock = jointExport->getAttributeBlock();

    physicsRigDef->m_joints[i]->m_parentPartFrame.identity();
    bool found = attributeBlock->getMatrix34Attribute("localMatrix", physicsRigDef->m_joints[i]->m_parentPartFrame);
    NMP_VERIFY(found);
    physicsRigDef->m_joints[i]->m_parentPartFrame.orthonormalise();
    physicsRigDef->m_joints[i]->m_parentFrameQuat = physicsRigDef->m_joints[i]->m_parentPartFrame.toQuat();
    physicsRigDef->m_joints[i]->m_parentPartIndex = jointExport->getParentIndex();

    physicsRigDef->m_joints[i]->m_childPartFrame.identity();
    found = attributeBlock->getMatrix34Attribute("offsetMatrix", physicsRigDef->m_joints[i]->m_childPartFrame);
    NMP_VERIFY(found);
    physicsRigDef->m_joints[i]->m_childPartFrame.orthonormalise();
    physicsRigDef->m_joints[i]->m_childFrameQuat = physicsRigDef->m_joints[i]->m_childPartFrame.toQuat();
    physicsRigDef->m_joints[i]->m_childPartIndex = jointExport->getChildIndex();

    //-----------------------
    // joint name
    const char* jointName = jointExport->getName();
    size_t jointNameLength = strlen(jointName);

    format = NMP::Memory::Format(sizeof(char) * (jointNameLength + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    physicsRigDef->m_joints[i]->m_name = (char*) result.alignAndIncrement(format);

    if (jointNameLength > 0)
    {
      NMP::Memory::memcpy(physicsRigDef->m_joints[i]->m_name, jointName, sizeof(char) * jointNameLength);
    }
    physicsRigDef->m_joints[i]->m_name[jointNameLength] = '\0';

    physicsDriverBuilder->initJointDriverData(jointExport, physicsRigDef->m_joints[i], result);
  }

  //-----------------------
  // parts
  format = NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part) * physicsRigDef->m_numParts, NMP_VECTOR_ALIGNMENT);
  physicsRigDef->m_parts = (MR::PhysicsRigDef::Part*) result.alignAndIncrement(format);
  materialID = 0;
  for (int32_t i = 0 ; i < physicsRigDef->m_numParts; ++i)
  {
    const ME::PhysicsPartExport* physicsPartExport = physicsRigDefExport->getPart(i);
    const ME::AttributeBlockExport* attributeBlock = physicsPartExport->getAttributeBlock();

    physicsRigDef->m_parts[i].actor.globalPose = physicsPartExport->getTransform();

    bool hasCollision = false;
    if (!attributeBlock->getBoolAttribute("hasCollision", hasCollision))
    {
      NMP_VERIFY_FAIL("could not find attribute 'hasCollision' in exported physics data");
    }
    physicsRigDef->m_parts[i].actor.hasCollision = (hasCollision ? 1 : 0);

    physicsRigDef->m_parts[i].actor.isFixed = false;
    physicsRigDef->m_parts[i].body.angularDamping = physicsPartExport->getAngularDamping();
    physicsRigDef->m_parts[i].body.linearDamping = physicsPartExport->getLinearDamping();

    physicsDriverBuilder->initPartDriverData(physicsPartExport, physicsRigDef->m_parts[i], result);

    //-----------------------
    // volume
    const ME::PhysicsVolumeExport* physicsVolumeExport = physicsPartExport->getVolume();

    physicsRigDef->m_parts[i].volume.numSpheres = (int32_t) physicsVolumeExport->getNumShapes(ME::PhysicsShapeExport::SHAPE_TYPE_SPHERE);
    physicsRigDef->m_parts[i].volume.numBoxes = (int32_t) physicsVolumeExport->getNumShapes(ME::PhysicsShapeExport::SHAPE_TYPE_BOX);
    physicsRigDef->m_parts[i].volume.numCapsules = (int32_t) physicsVolumeExport->getNumShapes(ME::PhysicsShapeExport::SHAPE_TYPE_CAPSULE);

    //-----------------------
    // volume spheres
    format = NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part::Volume::Sphere) * physicsRigDef->m_parts[i].volume.numSpheres, NMP_VECTOR_ALIGNMENT);
    physicsRigDef->m_parts[i].volume.spheres = (MR::PhysicsRigDef::Part::Volume::Sphere*) result.alignAndIncrement(format);

    for (int32_t j = 0; j < physicsRigDef->m_parts[i].volume.numSpheres; ++j)
    {
      const ME::PhysicsSphereExport* physicsSphereExport =
        (const ME::PhysicsSphereExport*) physicsVolumeExport->getShape(ME::PhysicsShapeExport::SHAPE_TYPE_SPHERE, j);

      physicsRigDef->m_parts[i].volume.spheres[j].localPose = physicsSphereExport->getTransform();
      physicsRigDef->m_parts[i].volume.spheres[j].parentIndex = physicsSphereExport->getParentIndex();
      physicsRigDef->m_parts[i].volume.spheres[j].radius = physicsSphereExport->getRadius();
      physicsRigDef->m_parts[i].volume.spheres[j].density = physicsSphereExport->getDensity();
      physicsRigDef->m_parts[i].volume.spheres[j].materialID = materialID++;

      physicsDriverBuilder->initShapeDriverData(physicsSphereExport, physicsRigDef->m_parts[i].volume.spheres[j], result);
    }

    //-----------------------
    // volume boxes
    format = NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part::Volume::Box) * physicsRigDef->m_parts[i].volume.numBoxes, NMP_VECTOR_ALIGNMENT);
    physicsRigDef->m_parts[i].volume.boxes = (MR::PhysicsRigDef::Part::Volume::Box*) result.alignAndIncrement(format);    

    for (int32_t j = 0; j < physicsRigDef->m_parts[i].volume.numBoxes; ++j)
    {
      const ME::PhysicsBoxExport* physicsBoxExport =
        (const ME::PhysicsBoxExport*) physicsVolumeExport->getShape(ME::PhysicsShapeExport::SHAPE_TYPE_BOX, j);

      physicsRigDef->m_parts[i].volume.boxes[j].localPose = physicsBoxExport->getTransform();
      physicsRigDef->m_parts[i].volume.boxes[j].parentIndex = physicsBoxExport->getParentIndex();
      physicsRigDef->m_parts[i].volume.boxes[j].dimensions = physicsBoxExport->getDimensions() * 0.5f;
      physicsRigDef->m_parts[i].volume.boxes[j].density = physicsBoxExport->getDensity();
      physicsRigDef->m_parts[i].volume.boxes[j].materialID = materialID++;

      physicsDriverBuilder->initShapeDriverData(physicsBoxExport, physicsRigDef->m_parts[i].volume.boxes[j], result);
    }

    //-----------------------
    // volume capsules
    format = NMP::Memory::Format(sizeof(MR::PhysicsRigDef::Part::Volume::Capsule) * physicsRigDef->m_parts[i].volume.numCapsules, NMP_VECTOR_ALIGNMENT);
    physicsRigDef->m_parts[i].volume.capsules = (MR::PhysicsRigDef::Part::Volume::Capsule*) result.alignAndIncrement(format);

    for (int32_t j = 0; j < physicsRigDef->m_parts[i].volume.numCapsules; ++j)
    {
      const ME::PhysicsCapsuleExport* physicsCapsuleExport =
        (const ME::PhysicsCapsuleExport*) physicsVolumeExport->getShape(ME::PhysicsShapeExport::SHAPE_TYPE_CAPSULE, j);

      physicsRigDef->m_parts[i].volume.capsules[j].localPose = physicsCapsuleExport->getTransform();
      physicsRigDef->m_parts[i].volume.capsules[j].parentIndex = physicsCapsuleExport->getParentIndex();
      physicsRigDef->m_parts[i].volume.capsules[j].radius = physicsCapsuleExport->getRadius();
      physicsRigDef->m_parts[i].volume.capsules[j].height = physicsCapsuleExport->getLength();
      physicsRigDef->m_parts[i].volume.capsules[j].density = physicsCapsuleExport->getDensity();
      physicsRigDef->m_parts[i].volume.capsules[j].materialID = materialID++;

      physicsDriverBuilder->initShapeDriverData(physicsCapsuleExport, physicsRigDef->m_parts[i].volume.capsules[j], result);
    }

    //-----------------------
    // volume name
    const char* volumeName = physicsVolumeExport->getName();
    size_t volumeNameLength = strlen(volumeName);

    format = NMP::Memory::Format(sizeof(char) * (volumeNameLength + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    physicsRigDef->m_parts[i].volume.name = (char*) result.alignAndIncrement(format);

    if (volumeNameLength > 0)
    {
      NMP::Memory::memcpy(physicsRigDef->m_parts[i].volume.name, volumeName, sizeof(char) * volumeNameLength);
    }
    physicsRigDef->m_parts[i].volume.name[volumeNameLength] = '\0';

    //-----------------------
    // part name
    const char* partName = physicsPartExport->getName();
    size_t partNameLength = strlen(partName);

    format = NMP::Memory::Format(sizeof(char) * (partNameLength + 1), NMP_NATURAL_TYPE_ALIGNMENT);
    physicsRigDef->m_parts[i].name = (char*) result.alignAndIncrement(format);

    if (partNameLength > 0)
    {
      NMP::Memory::memcpy(physicsRigDef->m_parts[i].name, partName, sizeof(char) * partNameLength);
    }
    physicsRigDef->m_parts[i].name[partNameLength] = '\0';
  }

  uint32_t index = 0;
  NMP::Vector3 translation(NMP::Vector3::InitZero);
  NMP::Quat rotation(NMP::Quat::kIdentity);
  physicsRigDefExport->getPhysicsRootPart(index, translation, rotation);

  NMP_VERIFY(result.format.size == 0);

  return rigDefResource;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsSixDOFJointDef::Motion getSixDOFMotion(
  const ME::AttributeBlockExport* attributeBlock,
  const char* attributeName)
{
  const char* value = 0;
  if (attributeBlock->getStringAttribute(attributeName, value))
  {
    if (strcmp(value, "MOTION_LIMITED") == 0)
    {
      return MR::PhysicsSixDOFJointDef::MOTION_LIMITED;
    }
    else if (strcmp(value, "MOTION_FREE") == 0)
    {
      return MR::PhysicsSixDOFJointDef::MOTION_FREE;
    }
    else if (strcmp(value, "MOTION_LOCKED") == 0)
    {
      return MR::PhysicsSixDOFJointDef::MOTION_LOCKED;
    }

    NMP_VERIFY_FAIL("Unknown '%s' motion type: %s", attributeName, value);
  }
  else
  {
    NMP_VERIFY_FAIL("Unable to find '%s' motion type attribute", attributeName);
  }

  return MR::PhysicsSixDOFJointDef::MOTION_FREE;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsJointDef* PhysicsRigDefBuilder::initSixDOFJoint(
  NMP::Memory::Resource&        resource,
  const ME::PhysicsJointExport* jointExport)
{
  NMP::Memory::Format format = NMP::Memory::Format(sizeof(MR::PhysicsSixDOFJointDef), NMP_VECTOR_ALIGNMENT);
  MR::PhysicsSixDOFJointDef* jointDef = (MR::PhysicsSixDOFJointDef*)resource.alignAndIncrement(format);
  jointDef->m_jointType = MR::PhysicsJointDef::JOINT_TYPE_SIX_DOF;

  const ME::AttributeBlockExport* attributeBlock = jointExport->getAttributeBlock();

  jointDef->m_swing1Motion = getSixDOFMotion(attributeBlock, "swing1Motion");
  jointDef->m_swing2Motion = getSixDOFMotion(attributeBlock, "swing2Motion");
  jointDef->m_twistMotion = getSixDOFMotion(attributeBlock, "twistMotion");

  double swing1Limit = 0.0;
  bool result = attributeBlock->getDoubleAttribute("swing1Limit", swing1Limit);
  NMP_VERIFY_MSG(result, "could not find attribute 'swing1Limit' in exported physics data");

  double swing2Limit = 0.0;
  result = attributeBlock->getDoubleAttribute("swing2Limit", swing2Limit);
  NMP_VERIFY_MSG(result, "could not find attribute 'swing2Limit' in exported physics data");

  double swingContactDistance = 0.05;
  result = attributeBlock->getDoubleAttribute("swingContactDistance", swingContactDistance);
  NMP_VERIFY_MSG(result, "could not find attribute 'swingContactDistance' in exported physics data");

  jointDef->m_hardLimits.setSwingLimit(
    static_cast< float >(swing1Limit), 
    static_cast< float >(swing2Limit), 
    static_cast< float >(swingContactDistance));

  double twistLimitLow = 0.0;
  result = attributeBlock->getDoubleAttribute("twistLimitLow", twistLimitLow);
  NMP_VERIFY_MSG(result, "could not find attribute 'twistLimitLow' in exported physics data");

  double twistLimitHigh = 0.0;
  result = attributeBlock->getDoubleAttribute("twistLimitHigh", twistLimitHigh);
  NMP_VERIFY_MSG(result, "could not find attribute 'twistLimitHigh' in exported physics data");

  double twistContactDistance = 0.05;
  result = attributeBlock->getDoubleAttribute("twistContactDistance", twistContactDistance);
  NMP_VERIFY_MSG(result, "could not find attribute 'twistContactDistance' in exported physics data");

  jointDef->m_hardLimits.setTwistLimit(
    static_cast< float >(twistLimitLow), 
    static_cast< float >(twistLimitHigh),
    static_cast< float >(twistContactDistance));

  if (sixDOFHasSoftLimit(jointExport))
  {
    format = NMP::Memory::Format(sizeof(MR::PhysicsSixDOFJointDef::SoftLimit), NMP_VECTOR_ALIGNMENT);
    MR::PhysicsSixDOFJointDef::SoftLimit* softLimitDef = (MR::PhysicsSixDOFJointDef::SoftLimit*)resource.alignAndIncrement(format);

    result = attributeBlock->getMatrix34Attribute("softLimitLocalMatrix", softLimitDef->m_parentPartFrame);
    NMP_VERIFY_MSG(result, "could not find attribute 'softLimitLocalMatrix' in exported physics data");

    result = attributeBlock->getMatrix34Attribute("softLimitOffsetMatrix", softLimitDef->m_childPartFrame);
    NMP_VERIFY_MSG(result, "could not find attribute 'softLimitOffsetMatrix' in exported physics data");

    bool useSoftSwing1 = false;
    result = attributeBlock->getBoolAttribute("useSoftSwing1", useSoftSwing1);
    NMP_VERIFY_MSG(result, "could not find attribute 'useSoftSwing1' in exported physics data");
    softLimitDef->m_swing1Motion = (useSoftSwing1 ? MR::PhysicsSixDOFJointDef::MOTION_LIMITED : MR::PhysicsSixDOFJointDef::MOTION_FREE);

    bool useSoftSwing2 = false;
    result = attributeBlock->getBoolAttribute("useSoftSwing2", useSoftSwing2);
    NMP_VERIFY_MSG(result, "could not find attribute 'useSoftSwing2' in exported physics data");
    softLimitDef->m_swing2Motion = (useSoftSwing2 ? MR::PhysicsSixDOFJointDef::MOTION_LIMITED : MR::PhysicsSixDOFJointDef::MOTION_FREE);

    bool useSoftTwist = false;
    result = attributeBlock->getBoolAttribute("useSoftTwist", useSoftTwist);
    NMP_VERIFY_MSG(result, "could not find attribute 'useSoftTwist' in exported physics data");
    softLimitDef->m_twistMotion = (useSoftTwist ? MR::PhysicsSixDOFJointDef::MOTION_LIMITED : MR::PhysicsSixDOFJointDef::MOTION_FREE);

    double swing1SoftLimit = 0.0;
    bool result = attributeBlock->getDoubleAttribute("swing1SoftLimit", swing1SoftLimit);
    NMP_VERIFY_MSG(result, "could not find attribute 'swing1SoftLimit' in exported physics data");
    softLimitDef->m_swing1Limit = (float)swing1SoftLimit;

    double swing2SoftLimit = 0.0;
    result = attributeBlock->getDoubleAttribute("swing2SoftLimit", swing2SoftLimit);
    NMP_VERIFY_MSG(result, "could not find attribute 'swing2SoftLimit' in exported physics data");
    softLimitDef->m_swing2Limit = (float)swing2SoftLimit;

    double twistSoftLimitLow = 0.0;
    result = attributeBlock->getDoubleAttribute("twistSoftLimitLow", twistSoftLimitLow);
    NMP_VERIFY_MSG(result, "could not find attribute 'twistSoftLimitLow' in exported physics data");
    softLimitDef->m_twistLimitLow = (float)twistSoftLimitLow;

    double twistSoftLimitHigh = 0.0;
    result = attributeBlock->getDoubleAttribute("twistSoftLimitHigh", twistSoftLimitHigh);
    NMP_VERIFY_MSG(result, "could not find attribute 'twistSoftLimitHigh' in exported physics data");
    softLimitDef->m_twistLimitHigh = (float)twistSoftLimitHigh;

    double softLimitStrength = 0.0;
    result = attributeBlock->getDoubleAttribute("softLimitStrength", softLimitStrength);
    NMP_VERIFY_MSG(result, "could not find attribute 'softLimitStrength' in exported physics data");
    softLimitDef->m_strengthScale = (float)softLimitStrength;

    jointDef->m_softLimit = softLimitDef;
  }
  else
  {
    jointDef->m_softLimit = NULL;
  }

  return jointDef;
}

//----------------------------------------------------------------------------------------------------------------------
MR::PhysicsJointDef* PhysicsRigDefBuilder::initRagdollJoint(
  NMP::Memory::Resource&        resource,
  const ME::PhysicsJointExport* jointExport)
{
  NMP::Memory::Format format = NMP::Memory::Format(sizeof(MR::PhysicsRagdollJointDef), NMP_VECTOR_ALIGNMENT);
  MR::PhysicsRagdollJointDef* jointDef = (MR::PhysicsRagdollJointDef*)resource.alignAndIncrement(format);
  jointDef->m_jointType = MR::PhysicsJointDef::JOINT_TYPE_RAGDOLL;

  const ME::AttributeBlockExport* attributeBlock = jointExport->getAttributeBlock();

  double coneAngle = 0.0;
  bool result = attributeBlock->getDoubleAttribute("coneAngle", coneAngle);
  NMP_VERIFY_MSG(result, "could not find attribute 'coneAngle' in exported physics data");
  jointDef->m_limits.setConeAngle(static_cast< float >(coneAngle));

  double planeMinAngle = 0.0;
  result = attributeBlock->getDoubleAttribute("planeMinAngle", planeMinAngle);
  NMP_VERIFY_MSG(result, "could not find attribute 'planeMinAngle' in exported physics data");
  jointDef->m_limits.setPlaneMinAngle(static_cast< float >(planeMinAngle));

  double planeMaxAngle = 0.0;
  result = attributeBlock->getDoubleAttribute("planeMaxAngle", planeMaxAngle);
  NMP_VERIFY_MSG(result, "could not find attribute 'planeMaxAngle' in exported physics data");
  jointDef->m_limits.setPlaneMaxAngle(static_cast< float >(planeMaxAngle));

  double twistMinAngle = 0.0;
  result = attributeBlock->getDoubleAttribute("twistMinAngle", twistMinAngle);
  NMP_VERIFY_MSG(result, "could not find attribute 'twistMinAngle' in exported physics data");
  jointDef->m_limits.setTwistMinAngle(static_cast< float >(twistMinAngle));

  double twistMaxAngle = 0.0;
  result = attributeBlock->getDoubleAttribute("twistMaxAngle", twistMaxAngle);
  NMP_VERIFY_MSG(result, "could not find attribute 'twistMaxAngle' in exported physics data");
  jointDef->m_limits.setTwistMaxAngle(static_cast< float >(twistMaxAngle));

  return jointDef;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
