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
#ifndef MCPHYSICSEXPORTXML_H
#define MCPHYSICSEXPORTXML_H
//----------------------------------------------------------------------------------------------------------------------
#include "mcExportPhysics.h"
#include "mcExportXml.h"

#ifdef _MSC_VER
  #pragma warning(push)
  #pragma warning(disable:4481)
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
class ExportFactoryXML;

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsBoxExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsBoxExportXML : public PhysicsBoxExport
{
  friend class PhysicsVolumeExportXML;

public:
  virtual ~PhysicsBoxExportXML();

  // PhysicsShapeExport overrides
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsBoxExportXML(XMLElement* element);
  PhysicsBoxExportXML(const NMP::Matrix34& transform, const NMP::Vector3& dimensions);

  XMLElement*        m_xmlElement;
  AttributeBlockXML* m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsCapsuleExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsCapsuleExportXML : public PhysicsCapsuleExport
{
  friend class PhysicsVolumeExportXML;

public:
  virtual ~PhysicsCapsuleExportXML();

  // PhysicsShapeExport overrides
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsCapsuleExportXML(XMLElement* element);
  PhysicsCapsuleExportXML(const NMP::Matrix34& transform, float radius, float height);

  XMLElement*        m_xmlElement;
  AttributeBlockXML* m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsCylinderExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsCylinderExportXML : public PhysicsCylinderExport
{
  friend class PhysicsVolumeExportXML;

public:
  virtual ~PhysicsCylinderExportXML();

  // PhysicsShapeExport overrides
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsCylinderExportXML(XMLElement* element);
  PhysicsCylinderExportXML(const NMP::Matrix34& transform, float radius, float height);

  XMLElement*        m_xmlElement;
  AttributeBlockXML* m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPlaneExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsPlaneExportXML : public PhysicsPlaneExport
{
  friend class PhysicsVolumeExportXML;

public:
  virtual ~PhysicsPlaneExportXML();

  // PhysicsShapeExport overrides
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsPlaneExportXML(XMLElement* element);
  PhysicsPlaneExportXML(const NMP::Matrix34& transform);

  XMLElement*        m_xmlElement;
  AttributeBlockXML* m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsSphereExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsSphereExportXML : public PhysicsSphereExport
{
  friend class PhysicsVolumeExportXML;

public:
  virtual ~PhysicsSphereExportXML();

  // PhysicsShapeExport overrides
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsSphereExportXML(XMLElement* element);
  PhysicsSphereExportXML(const NMP::Matrix34& transform, float radius);

  XMLElement*        m_xmlElement;
  AttributeBlockXML* m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsMeshExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsMeshExportXML : public PhysicsMeshExport
{
  friend class PhysicsVolumeExportXML;

public:
  virtual ~PhysicsMeshExportXML();

  // PhysicsShapeExport overrides
  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsMeshExportXML(XMLElement* element);
  PhysicsMeshExportXML(
    const NMP::Matrix34& transform,
    uint32_t             numVertices,
    const NMP::Vector3   vertices[],
    uint32_t             numIndices,
    const uint32_t       indices[]);

  XMLElement*        m_xmlElement;
  AttributeBlockXML* m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsVolumeExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsVolumeExportXML : public PhysicsVolumeExport
{
  friend class PhysicsPartExportXML;

public:
  virtual ~PhysicsVolumeExportXML();

  const char* getName() const NM_OVERRIDE;

  PhysicsBoxExport* createBox(const NMP::Matrix34& transform, const NMP::Vector3& dimensions) NM_OVERRIDE;
  PhysicsCapsuleExport* createCapsule(const NMP::Matrix34& transform, float radius, float height) NM_OVERRIDE;
  PhysicsCylinderExport* createCylinder(const NMP::Matrix34& transform, float radius, float height) NM_OVERRIDE;
  PhysicsPlaneExport* createPlane(const NMP::Matrix34& transform) NM_OVERRIDE;
  PhysicsSphereExport* createSphere(const NMP::Matrix34& transform, float radius) NM_OVERRIDE;
  PhysicsMeshExport* createMesh(
    const NMP::Matrix34& transform,
    uint32_t             numVertices,
    const NMP::Vector3*  vertices,
    uint32_t             numIndices,
    const uint32_t*      indices) NM_OVERRIDE;

  uint32_t getNumShapes() const NM_OVERRIDE;
  const PhysicsShapeExport* getShape(uint32_t index) const NM_OVERRIDE;

  uint32_t getNumShapes(PhysicsShapeExport::ShapeType type) const NM_OVERRIDE;
  const PhysicsShapeExport* getShape(PhysicsShapeExport::ShapeType type, uint32_t index) const NM_OVERRIDE;

protected:
  PhysicsVolumeExportXML(XMLElement* element);
  PhysicsVolumeExportXML(const char* name);

  XMLElement*                      m_xmlElement;
  std::vector<PhysicsShapeExport*> m_shapes;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsPartExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsPartExportXML : public PhysicsPartExport
{
  friend class PhysicsRigDefExportXML;

public:
  virtual ~PhysicsPartExportXML();

  const char* getName() const NM_OVERRIDE;

  const PhysicsVolumeExport* getVolume() const NM_OVERRIDE;
  PhysicsVolumeExport* getVolume() NM_OVERRIDE;

  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsPartExportXML(XMLElement* element);
  PhysicsPartExportXML(const char* name);

  XMLElement*              m_xmlElement;
  PhysicsVolumeExportXML*  m_volume;
  AttributeBlockXML*       m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsHingeExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsJointExportXML : public PhysicsJointExport
{
  friend class PhysicsRigDefExportXML;

public:
  // PhysicsJointExport overrides
  virtual ~PhysicsJointExportXML();

  const char* getName() const NM_OVERRIDE;

  const char* getJointType() const NM_OVERRIDE;

  uint32_t getParentIndex() const NM_OVERRIDE;
  uint32_t getChildIndex() const NM_OVERRIDE;

  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsJointExportXML(XMLElement* element, PhysicsRigDefExportXML* rig);
  PhysicsJointExportXML(
    const char*             name,
    const char*             type,
    uint32_t                parentIndex,
    uint32_t                childIndex,
    PhysicsRigDefExportXML* rig);

  XMLElement*             m_xmlElement;
  PhysicsRigDefExportXML* m_rig;
  AttributeBlockXML*      m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsDisabledCollisionPairExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsDisabledCollisionPairExportXML : public PhysicsDisabledCollisionPairExport
{
  friend class PhysicsRigDefExportXML;

public:
  uint32_t getPartAIndex() const NM_OVERRIDE;
  uint32_t getPartBIndex() const NM_OVERRIDE;

protected:
  PhysicsDisabledCollisionPairExportXML(XMLElement* element, PhysicsRigDefExportXML* rig);
  PhysicsDisabledCollisionPairExportXML(
    uint32_t                partAIndex,
    uint32_t                partBIndex,
    PhysicsRigDefExportXML* rig);

  PhysicsRigDefExportXML* m_rig;
  XMLElement*             m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsDisabledCollisionSetExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsDisabledCollisionSetExportXML : public PhysicsDisabledCollisionSetExport
{
  friend class PhysicsRigDefExportXML;

public:
  void addPart(uint32_t partIndex) NM_OVERRIDE;
  void setEnabled(bool enabled) NM_OVERRIDE;

  uint32_t getNumParts() const NM_OVERRIDE;
  uint32_t getPartIndex(uint32_t index) const NM_OVERRIDE;
  bool     getEnabled() const NM_OVERRIDE;

protected:
  PhysicsDisabledCollisionSetExportXML(XMLElement* element, PhysicsRigDefExportXML* rig);
  PhysicsDisabledCollisionSetExportXML(PhysicsRigDefExportXML* rig);

  PhysicsRigDefExportXML* m_rig;
  XMLElement*             m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// PhysicsRigDefExportXML
//----------------------------------------------------------------------------------------------------------------------
class PhysicsRigDefExportXML : public PhysicsRigDefExport
{
  friend class ExportFactoryXML;

public:
  virtual ~PhysicsRigDefExportXML();

  // AssetExport overrides
  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;

  bool write() NM_OVERRIDE;

  // PhysicsRigDefExport overrides
  void setDestFilename(const char* filename) NM_OVERRIDE;

  PhysicsPartExport* createPart(const char* name) NM_OVERRIDE;

  PhysicsJointExport* createJoint(
    const char* name,
    const char* type,
    uint32_t    parent,
    uint32_t    child) NM_OVERRIDE;

  PhysicsDisabledCollisionPairExport* createDisabledCollisionPair(
    uint32_t partA,
    uint32_t partB) NM_OVERRIDE;
  PhysicsDisabledCollisionSetExport* createDisabledCollisionSet() NM_OVERRIDE;

  uint32_t getNumParts() const NM_OVERRIDE;
  const PhysicsPartExport* getPart(uint32_t index) const NM_OVERRIDE;
  const uint32_t getPartIndex(const PhysicsPartExport* part) const;

  uint32_t getNumJoints() const NM_OVERRIDE;
  const PhysicsJointExport* getJoint(uint32_t index) const NM_OVERRIDE;

  uint32_t getNumDisabledCollisions() const NM_OVERRIDE;
  const PhysicsDisabledCollisionExport* getDisabledCollision(uint32_t index) const NM_OVERRIDE;

  bool setPhysicsRootPart(
    uint32_t            index,
    const NMP::Vector3& translation,
    const NMP::Quat&    rotation) NM_OVERRIDE;
  bool getPhysicsRootPart(
    uint32_t&     index,
    NMP::Vector3& translation,
    NMP::Quat&    rotation) const NM_OVERRIDE;

  bool addTrajectoryCalcMarkupPart(uint32_t index) NM_OVERRIDE;

  uint32_t getNumTrajectoryCalcMarkupParts() const NM_OVERRIDE;
  bool getTrajectoryCalcMarkupPartArray(uint32_t values[], uint32_t size) const NM_OVERRIDE;

  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  PhysicsRigDefExportXML(XMLElement* element, const char* destFileName);
  PhysicsRigDefExportXML(const GUID guid, const wchar_t* destFileName, const char* rigName);

  XMLElement*                                   m_xmlElement;
  std::string                                   m_destFileName;
  std::vector<PhysicsPartExport*>               m_parts;
  std::vector<PhysicsJointExport*>              m_joints;
  std::vector<PhysicsDisabledCollisionExport*>  m_disabledCollisions;
  AttributeBlockXML*                            m_xmlAttributeBlock;
  AttributeExport*                              m_trajectoryCalcMarkupArray;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // MCPHYSICSEXPORTXML_H
