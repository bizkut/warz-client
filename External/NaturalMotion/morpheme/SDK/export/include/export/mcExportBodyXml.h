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
#ifndef MCBODYEXPORTXML_H
#define MCBODYEXPORTXML_H

//----------------------------------------------------------------------------------------------------------------------
#include "mcExportBody.h"
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
class LimbDefExportXML;

//----------------------------------------------------------------------------------------------------------------------
/// HamstringExportXML
//----------------------------------------------------------------------------------------------------------------------
class HamstringExportXML : public HamstringExport
{
  friend class LimbDefExportXML;

public:
  virtual ~HamstringExportXML();

  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  HamstringExportXML(XMLElement* element);
  HamstringExportXML();

  XMLElement*              m_xmlElement;
  AttributeBlockXML*       m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// LimbDefExportXML
//----------------------------------------------------------------------------------------------------------------------
class LimbDefExportXML : public LimbDefExport
{
  friend class BodyDefExportXML;

public:
  virtual ~LimbDefExportXML();

  const char* getName() const NM_OVERRIDE;

  const HamstringExport* getHamstring() const NM_OVERRIDE;
  HamstringExport* getHamstring() NM_OVERRIDE;

  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  LimbDefExportXML(XMLElement* element);
  LimbDefExportXML(const char* name);

  XMLElement*              m_xmlElement;
  AttributeBlockXML*       m_xmlAttributeBlock;
  HamstringExportXML*   m_hamstring;
};

///----------------------------------------------------------------------------------------------------------------------
/// SelfAvoidanceExportXML
//----------------------------------------------------------------------------------------------------------------------
class SelfAvoidanceExportXML : public SelfAvoidanceExport
{
  friend class BodyDefExportXML;

public:
  void setRadius(float r) NM_OVERRIDE;
  float getRadius() const NM_OVERRIDE;

protected:
  SelfAvoidanceExportXML(XMLElement* element);
  SelfAvoidanceExportXML();

  XMLElement*             m_xmlElement;
};

//----------------------------------------------------------------------------------------------------------------------
/// BodyDefExportXML
//----------------------------------------------------------------------------------------------------------------------
class BodyDefExportXML : public BodyDefExport
{
  friend class ExportFactoryXML;

public:
  virtual ~BodyDefExportXML();

  /// AssetExport overrides
  LimbDefExport* createLimb(const char* name) NM_OVERRIDE;

  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;

  uint32_t getNumLimbs() const NM_OVERRIDE;
  const LimbDefExport* getLimb(uint32_t index) const NM_OVERRIDE;

  void setDestFilename(const char* filename) NM_OVERRIDE;

  bool write() NM_OVERRIDE;

  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

  AnimationPoseDefExport* createAnimationPose(const char* name) NM_OVERRIDE;
  uint32_t getAnimationPoseCount() const NM_OVERRIDE;
  const AnimationPoseDefExport* getAnimationPose(uint32_t index) const NM_OVERRIDE;
  const AnimationPoseDefExport* getAnimationPose(const char* poseName) const NM_OVERRIDE;

  SelfAvoidanceExport* createSelfAvoidanceExport() NM_OVERRIDE;
  
  void getSelfAvoidance(float& radius) const NM_OVERRIDE;

protected:
  BodyDefExportXML(
    XMLElement*         element, 
    ExportFactoryXML*   factory,
    const char*         destFileName);
  BodyDefExportXML(
    const GUID           guid, 
    const wchar_t*       destFileName, 
    const char*          bodyName);

  ExportFactoryXML*           m_exportFactory;        ///< a local export factory, needed for loading the xml asset sources
  XMLElement*                 m_xmlElement;
  std::string                 m_destFileName;
  std::vector<LimbDefExport*> m_limbs;
  AttributeBlockXML*          m_xmlAttributeBlock;
  SelfAvoidanceExportXML*     m_selfAvoidance;
  std::vector<AnimationPoseDefExport*> m_animationPoses;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // MCBODYEXPORTXML_H
