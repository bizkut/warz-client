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
#ifndef MCPHYSICSEXPORT_H
#define MCPHYSICSEXPORT_H

//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMatrix34.h"
#include "NMPlatform/NMVector3.h"
#include "mcExport.h"

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4481)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
class AttributeBlockExport;

//----------------------------------------------------------------------------------------------------------------------
/// \brief
/// Base class for all exported primitive physics shapes.
//----------------------------------------------------------------------------------------------------------------------
class PhysicsShapeExport
{
public:
  enum ShapeType
  {
    SHAPE_TYPE_BOX,
    SHAPE_TYPE_CAPSULE,
    SHAPE_TYPE_CYLINDER,
    SHAPE_TYPE_PLANE,
    SHAPE_TYPE_SPHERE,
    SHAPE_TYPE_MESH
  };

  virtual ~PhysicsShapeExport() { }

  virtual ShapeType getShapeType() const = 0;

  NMP::Matrix34 getTransform() const;

  virtual const AttributeBlockExport* getAttributeBlock() const = 0;
  virtual AttributeBlockExport* getAttributeBlock() = 0;

  void setFriction(float friction);
  float getFriction() const;

  void setRestitution(float restitution);
  float getRestitution() const;

  void setDensity(float density);
  float getDensity() const;

  void setParentIndex(int index); ///< Set the (array) index of the shape as it appears in it's parent the Physics Volume
  int getParentIndex() const;     ///< Get the (array) index of the shape as it appears in it's parent the Physics Volume
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsBoxExport : public PhysicsShapeExport
{
public:
  virtual ~PhysicsBoxExport() { }

  ShapeType getShapeType() const NM_OVERRIDE { return SHAPE_TYPE_BOX; }

  NMP::Vector3 getDimensions() const;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsCapsuleExport : public PhysicsShapeExport
{
public:
  virtual ~PhysicsCapsuleExport() { }

  ShapeType getShapeType() const NM_OVERRIDE { return SHAPE_TYPE_CAPSULE; }

  float getRadius() const;
  float getLength() const;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsCylinderExport : public PhysicsShapeExport
{
public:
  virtual ~PhysicsCylinderExport() { }

  ShapeType getShapeType() const NM_OVERRIDE { return SHAPE_TYPE_CYLINDER; }

  float getRadius() const;
  float getLength() const;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsPlaneExport : public PhysicsShapeExport
{
public:
  virtual ~PhysicsPlaneExport() { }

  ShapeType getShapeType() const NM_OVERRIDE { return SHAPE_TYPE_PLANE; }
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsSphereExport : public PhysicsShapeExport
{
public:
  virtual ~PhysicsSphereExport() { }

  ShapeType getShapeType() const NM_OVERRIDE { return SHAPE_TYPE_SPHERE; }

  float getRadius() const;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsMeshExport : public PhysicsShapeExport
{
public:
  virtual ~PhysicsMeshExport() { }

  ShapeType getShapeType() const NM_OVERRIDE { return SHAPE_TYPE_MESH; }

  uint32_t getNumVertices() const;
  void getVertices(NMP::Vector3 vertices[], uint32_t size) const;

  uint32_t getNumIndices() const;
  void getIndices(uint32_t indicies[], uint32_t size) const;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsVolumeExport
{
public:
  virtual ~PhysicsVolumeExport() { }

  virtual const char* getName() const = 0;

  virtual PhysicsBoxExport* createBox(const NMP::Matrix34& transform, const NMP::Vector3& dimensions) = 0;
  virtual PhysicsCapsuleExport* createCapsule(const NMP::Matrix34& transform, float radius, float height) = 0;
  virtual PhysicsCylinderExport* createCylinder(const NMP::Matrix34& transform, float radius, float height) = 0;
  virtual PhysicsPlaneExport* createPlane(const NMP::Matrix34& transform) = 0;
  virtual PhysicsSphereExport* createSphere(const NMP::Matrix34& transform, float radius) = 0;
  virtual PhysicsMeshExport* createMesh(
    const NMP::Matrix34& transform,
    uint32_t             numVertices,
    const NMP::Vector3*  vertices,
    uint32_t             numIndices,
    const uint32_t*      indices) = 0;

  virtual uint32_t getNumShapes() const = 0;
  virtual const PhysicsShapeExport* getShape(uint32_t index) const = 0;

  virtual uint32_t getNumShapes(PhysicsShapeExport::ShapeType type) const = 0;
  virtual const PhysicsShapeExport* getShape(PhysicsShapeExport::ShapeType type, uint32_t index) const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsPartExport
{
public:
  virtual ~PhysicsPartExport() { }

  virtual const char* getName() const = 0;

  void setTransform(const NMP::Matrix34& transform);
  void setHasCollision(bool collision);
  void setLinearDamping(float linearDamping);
  void setAngularDamping(float angularDamping);
  void setMaxContactOffsetIncrease(float maxContactOffsetIncrease);

  NMP::Matrix34 getTransform() const;
  bool hasCollision() const;
  NMP::Vector3 getCenterOfMass() const;
  
  float getLinearDamping() const;
  float getAngularDamping() const;
  float getMaxContactOffsetIncrease() const;
  float MaxContactOffsetIncrease() const;
  const char* getBodyPlanTag() const;

  virtual const PhysicsVolumeExport* getVolume() const = 0;
  virtual PhysicsVolumeExport* getVolume() = 0;

  virtual const AttributeBlockExport*  getAttributeBlock() const = 0;
  virtual AttributeBlockExport*  getAttributeBlock() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsJointExport
{
public:
  virtual ~PhysicsJointExport() { }

  virtual const char* getName() const = 0;

  virtual const char* getJointType() const = 0;

  virtual uint32_t getParentIndex() const = 0;
  virtual uint32_t getChildIndex() const = 0;

  virtual const AttributeBlockExport*  getAttributeBlock() const = 0;
  virtual AttributeBlockExport*  getAttributeBlock() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsDisabledCollisionExport
{
public:
  enum DisabledCollisionType
  {
    DISABLED_TYPE_PAIR,
    DISABLED_TYPE_SET
  };

  virtual ~PhysicsDisabledCollisionExport() { }

  virtual DisabledCollisionType getDisabledType() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsDisabledCollisionPairExport : public PhysicsDisabledCollisionExport
{
public:
  virtual ~PhysicsDisabledCollisionPairExport() { }

  DisabledCollisionType getDisabledType() const NM_OVERRIDE { return DISABLED_TYPE_PAIR; }

  virtual uint32_t getPartAIndex() const = 0;
  virtual uint32_t getPartBIndex() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsDisabledCollisionSetExport : public PhysicsDisabledCollisionExport
{
public:
  virtual ~PhysicsDisabledCollisionSetExport() { }

  DisabledCollisionType getDisabledType() const NM_OVERRIDE { return DISABLED_TYPE_SET; }

  virtual void addPart(uint32_t partIndex) = 0;
  virtual void setEnabled(bool enabled) = 0;

  virtual uint32_t getNumParts() const = 0;
  virtual uint32_t getPartIndex(uint32_t index) const = 0;
  virtual bool     getEnabled() const = 0;
};

//----------------------------------------------------------------------------------------------------------------------
class PhysicsRigDefExport : public AssetExport
{
public:
  virtual void setDestFilename(const char* filename) = 0;

  virtual PhysicsPartExport* createPart(const char* name) = 0;

  virtual PhysicsJointExport* createJoint(
    const char* name,
    const char* type,
    uint32_t    parentIndex,
    uint32_t    childIndex) = 0;

  virtual PhysicsDisabledCollisionPairExport* createDisabledCollisionPair(
    uint32_t partAIndex,
    uint32_t partBIndex) = 0;
  virtual PhysicsDisabledCollisionSetExport* createDisabledCollisionSet() = 0;

  virtual uint32_t getNumParts() const = 0;
  virtual const PhysicsPartExport* getPart(uint32_t index) const = 0;

  virtual uint32_t getNumJoints() const = 0;
  virtual const PhysicsJointExport* getJoint(uint32_t index) const = 0;

  virtual uint32_t getNumDisabledCollisions() const = 0;
  virtual const PhysicsDisabledCollisionExport* getDisabledCollision(uint32_t index) const = 0;

  /// \brief Sets the physics root index part and offset, used when calculating the
  /// trajectory of the physics rig.
  virtual bool setPhysicsRootPart(
    uint32_t            index,
    const NMP::Vector3& translation,
    const NMP::Quat&    rotation) = 0;

  virtual bool getPhysicsRootPart(
    uint32_t&     index,
    NMP::Vector3& translation,
    NMP::Quat&    rotation) const = 0;

  /// \brief Marks an additional part as well as the root part to be used as a helper
  /// when calculating the trajectory of the physics rig.
  virtual bool addTrajectoryCalcMarkupPart(uint32_t index) = 0;

  virtual uint32_t getNumTrajectoryCalcMarkupParts() const = 0;
  virtual bool getTrajectoryCalcMarkupPartArray(uint32_t values[], uint32_t size) const = 0;

  virtual const AttributeBlockExport*  getAttributeBlock() const = 0;
  virtual AttributeBlockExport*  getAttributeBlock() = 0;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // MCPHYSICSEXPORT_H
