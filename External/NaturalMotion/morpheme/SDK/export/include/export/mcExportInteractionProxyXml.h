// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef MC_INTERACTION_PROXY_EXPORT_XML_H
#define MC_INTERACTION_PROXY_EXPORT_XML_H

//----------------------------------------------------------------------------------------------------------------------
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
/// InteractionProxyExportXML
//----------------------------------------------------------------------------------------------------------------------
class InteractionProxyExportXML : public InteractionProxyExport
{
  friend class ExportFactoryXML;

public:
  virtual ~InteractionProxyExportXML();

  /// AssetExport overrides
  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;

  bool write() NM_OVERRIDE;

  /// InteractionProxyExport overrides
  void setDestFilename(const char* filename) NM_OVERRIDE;

  void setParentIndex(unsigned int) NM_OVERRIDE;
  void setRotation(float x, float y, float z, float w) NM_OVERRIDE;
  void setTranslation(float x, float y, float z) NM_OVERRIDE;
  void setVisibility(bool visible) NM_OVERRIDE;
  void setUserData(unsigned int) NM_OVERRIDE;
  void setShape(const char* name); // Capsule, Box or Sphere
  void setHeight(float x) NM_OVERRIDE;
  void setRadius(float x) NM_OVERRIDE;
  void setWidth(float x) NM_OVERRIDE;
  void setDepth(float x) NM_OVERRIDE;

  unsigned int getParentIndex() const NM_OVERRIDE;
  void getRotation(float& x, float& y, float& z, float& w) const NM_OVERRIDE;
  void getTranslation(float& x, float& y, float& z) const NM_OVERRIDE;
  bool getVisibility() const NM_OVERRIDE;
  unsigned int getUserData() const NM_OVERRIDE;
  const char* getShape() const NM_OVERRIDE; // Capsule, Box or Sphere
  float getHeight() const NM_OVERRIDE;
  float getRadius() const NM_OVERRIDE;
  float getWidth() const NM_OVERRIDE;
  float getDepth() const NM_OVERRIDE;

protected:
  InteractionProxyExportXML(XMLElement* element, const std::string& destFileName);
  InteractionProxyExportXML(const GUID guid, const wchar_t* destFileName, const char* proxyName);

  XMLElement* m_xmlElement;
  std::string m_destFileName;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // MC_INTERACTION_PROXY_EXPORT_XML_H
