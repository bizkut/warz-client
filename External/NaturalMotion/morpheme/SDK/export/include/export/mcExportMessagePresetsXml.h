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
#ifndef MCEXPORTMESSAGEPRESETSXML_H
#define MCEXPORTMESSAGEPRESETSXML_H
//----------------------------------------------------------------------------------------------------------------------
#include "export/mcExportMessagePresets.h"
#include "export/mcExportXml.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ME
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class MessagePresetExportXML : public MessagePresetExport
{
  friend class MessagePresetsListExportXML;

public:

  virtual ~MessagePresetExportXML();

  virtual const char* getName() const NM_OVERRIDE;

  virtual DataBlockExport* getDataBlock() NM_OVERRIDE;
  virtual const DataBlockExport* getDataBlock() const NM_OVERRIDE;

protected:

  MessagePresetExportXML(XMLElement* element);

  MessagePresetExportXML(const wchar_t* name);

  XMLElement*                         m_xmlElement;
  DataBlockExportXML*                 m_dataBlock;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class MessagePresetsListExportXML : public MessagePresetsListExport
{
  friend class MessagePresetLibraryExportXML;

public:

  virtual ~MessagePresetsListExportXML();

  virtual MessagePresetExport* addMessagePreset(const wchar_t* name) NM_OVERRIDE;

  virtual MR::MessageType getMessageType() const NM_OVERRIDE;
  virtual const char* getMessageName() const NM_OVERRIDE;

  virtual uint32_t getNumMessagePresets() const NM_OVERRIDE;
  virtual const MessagePresetExport* getMessagePreset(uint32_t index) const NM_OVERRIDE;

protected:

  MessagePresetsListExportXML(XMLElement* element);
  MessagePresetsListExportXML(MR::MessageType messageType, const wchar_t* messageName);

  XMLElement*                           m_xmlElement;
  std::vector<MessagePresetExportXML*>  m_messagePresets;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class MessagePresetLibraryExportXML : public MessagePresetLibraryExport
{
  friend class ExportFactoryXML;
  friend class NetworkDefExportXML;

public:

  virtual ~MessagePresetLibraryExportXML();

  virtual const char* getName() const NM_OVERRIDE;
  virtual GUID getGUID() const NM_OVERRIDE;
  virtual const char* getDestFilename() const NM_OVERRIDE;
  virtual bool write() NM_OVERRIDE;

  virtual MessagePresetsListExport* createMessagePresetsList(MR::MessageType messageType, const wchar_t* messageName) NM_OVERRIDE;

  virtual uint32_t getNumMessages() const NM_OVERRIDE;
  virtual MessagePresetsListExport* getMessagePresets(uint32_t index) NM_OVERRIDE;
  virtual const MessagePresetsListExport* getMessagePresets(uint32_t index) const NM_OVERRIDE;

protected:
  MessagePresetLibraryExportXML();

  std::string                           m_destinationFilename;
  XMLElement*                           m_xmlElement;
  std::vector<MessagePresetsListExportXML*> m_messagePresets;

  MessagePresetLibraryExportXML(XMLElement* element, const std::string& destinationFilename);
  MessagePresetLibraryExportXML(GUID guid, const wchar_t* destinationFilename);
};

} // namespace ME

//----------------------------------------------------------------------------------------------------------------------
#endif // MCPHYSICSEXPORT_H
//----------------------------------------------------------------------------------------------------------------------
