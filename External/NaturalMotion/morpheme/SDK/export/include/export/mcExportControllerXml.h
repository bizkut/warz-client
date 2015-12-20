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
#ifndef MCCONTROLLEREXPORTXML_H
#define MCCONTROLLEREXPORTXML_H

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
/// CharacterControllerExportXML
//----------------------------------------------------------------------------------------------------------------------
class CharacterControllerExportXML : public CharacterControllerExport
{
  friend class ExportFactoryXML;

public:
  virtual ~CharacterControllerExportXML();

  /// AssetExport overrides
  const char* getName() const NM_OVERRIDE;
  GUID getGUID() const NM_OVERRIDE;
  const char* getDestFilename() const NM_OVERRIDE;

  bool write() NM_OVERRIDE;

  /// CharacterControllerExport overrides
  void setDestFilename(const char* filename) NM_OVERRIDE;

  const AttributeBlockExport* getAttributeBlock() const NM_OVERRIDE;
  AttributeBlockExport* getAttributeBlock() NM_OVERRIDE;

protected:
  CharacterControllerExportXML(XMLElement* element, const std::string& destFileName);
  CharacterControllerExportXML(const GUID guid, const wchar_t* destFileName, const char* rigName);

  XMLElement*                                   m_xmlElement;
  std::string                                   m_destFileName;
  AttributeBlockXML*                            m_xmlAttributeBlock;
};

//----------------------------------------------------------------------------------------------------------------------
} // namespace ME

#ifdef _MSC_VER
  #pragma warning(pop)
#endif

#endif // MCCONTROLLEREXPORTXML_H
