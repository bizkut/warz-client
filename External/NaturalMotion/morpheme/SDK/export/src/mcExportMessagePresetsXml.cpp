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
#include "export/mcExportMessagePresetsXml.h"
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
namespace ME
{
//----------------------------------------------------------------------------------------------------------------------
/// MessagePresetExportXML
//----------------------------------------------------------------------------------------------------------------------
MessagePresetExportXML::~MessagePresetExportXML()
{
}

//----------------------------------------------------------------------------------------------------------------------
const char* MessagePresetExportXML::getName() const
{
  const char* name = m_xmlElement->getAttribute("name");
  return name;
}

//----------------------------------------------------------------------------------------------------------------------
DataBlockExport* MessagePresetExportXML::getDataBlock()
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
const DataBlockExport* MessagePresetExportXML::getDataBlock() const
{
  return m_dataBlock;
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetExportXML::MessagePresetExportXML(XMLElement* element)
: m_xmlElement(element)
{
  m_dataBlock = new DataBlockExportXML(element->findChild("DataBlock"));
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetExportXML::MessagePresetExportXML(const wchar_t* name)
: m_xmlElement(new XMLElement("MessagePreset")),
  m_dataBlock(new DataBlockExportXML())
{
  m_xmlElement->LinkEndChild(m_dataBlock->m_xmlElement);

  m_xmlElement->setAttribute("name", name);
}

//----------------------------------------------------------------------------------------------------------------------
/// MessagePresetsExportXML
//----------------------------------------------------------------------------------------------------------------------
MessagePresetsListExportXML::~MessagePresetsListExportXML()
{
  // Destroy message presets
  uint32_t numMessagePresets = (uint32_t)m_messagePresets.size();
  for (uint32_t i = 0 ; i < numMessagePresets ; ++i)
  {
    delete m_messagePresets[i];
  }
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetExport* MessagePresetsListExportXML::addMessagePreset(const wchar_t* name)
{
  MessagePresetExportXML* messagePreset = new MessagePresetExportXML(name);

  m_xmlElement->LinkEndChild(messagePreset->m_xmlElement);
  m_messagePresets.push_back(messagePreset);

  return messagePreset;
}

//----------------------------------------------------------------------------------------------------------------------
MR::MessageType MessagePresetsListExportXML::getMessageType() const
{
  unsigned int val;
  m_xmlElement->getUIntAttribute("messageType", val);
  return static_cast<MR::MessageType>(val);
}

//----------------------------------------------------------------------------------------------------------------------
const char* MessagePresetsListExportXML::getMessageName() const
{
  return m_xmlElement->getAttribute("messageName");
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t MessagePresetsListExportXML::getNumMessagePresets() const
{
  uint32_t count = static_cast<uint32_t>(m_messagePresets.size());
  return count;
}

//----------------------------------------------------------------------------------------------------------------------
const MessagePresetExport* MessagePresetsListExportXML::getMessagePreset(uint32_t index) const
{
  const MessagePresetExport* messagePreset = m_messagePresets[index];
  return messagePreset;
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetsListExportXML::MessagePresetsListExportXML(XMLElement* element)
: m_xmlElement(element)
{
  BuildNodeList(MessagePresetExportXML, m_messagePresets, m_xmlElement, "MessagePreset");
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetsListExportXML::MessagePresetsListExportXML(
  MR::MessageType messageType,
  const wchar_t* messageName)
: m_xmlElement(new XMLElement("MessagePresets"))
{
  m_xmlElement->setUIntAttribute("messageType", static_cast<unsigned int>(messageType));
  m_xmlElement->setAttribute("messageName", messageName);
}

//----------------------------------------------------------------------------------------------------------------------
/// MessagePresetLibraryExportXML
//----------------------------------------------------------------------------------------------------------------------
MessagePresetLibraryExportXML::~MessagePresetLibraryExportXML()
{
  // Destroy message presets
  uint32_t numMessagePresets = (uint32_t)m_messagePresets.size();
  for (uint32_t i = 0 ; i < numMessagePresets ; ++i)
  {
    delete m_messagePresets[i];
  }

  delete m_xmlElement;
}

//----------------------------------------------------------------------------------------------------------------------
const char* MessagePresetLibraryExportXML::getName() const
{
  return "MessagePresetLibrary";
}

//----------------------------------------------------------------------------------------------------------------------
GUID MessagePresetLibraryExportXML::getGUID() const
{
  GUID guid = m_xmlElement->getGUID();
  return guid;
}

//----------------------------------------------------------------------------------------------------------------------
const char* MessagePresetLibraryExportXML::getDestFilename() const
{
  return m_destinationFilename.c_str();
}

//----------------------------------------------------------------------------------------------------------------------
bool MessagePresetLibraryExportXML::write()
{
  bool result = m_xmlElement->saveFile(m_destinationFilename.c_str());
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetsListExport* MessagePresetLibraryExportXML::createMessagePresetsList(
  MR::MessageType messageType,
  const wchar_t* messageName)
{
  MessagePresetsListExportXML* messagePresets = new MessagePresetsListExportXML(messageType, messageName);
  m_xmlElement->LinkEndChild(messagePresets->m_xmlElement);
  m_messagePresets.push_back(messagePresets);
  return messagePresets;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t MessagePresetLibraryExportXML::getNumMessages() const
{
  uint32_t count = static_cast<uint32_t>(m_messagePresets.size());
  return count;
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetsListExport* MessagePresetLibraryExportXML::getMessagePresets(uint32_t index)
{
  MessagePresetsListExportXML* messagePresets = m_messagePresets[index];
  return messagePresets;
}

//----------------------------------------------------------------------------------------------------------------------
const MessagePresetsListExport* MessagePresetLibraryExportXML::getMessagePresets(uint32_t index) const
{
  MessagePresetsListExportXML* messagePresets = m_messagePresets[index];
  return messagePresets;
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetLibraryExportXML::MessagePresetLibraryExportXML(
  XMLElement* element,
  const std::string& destinationFilename)
: m_xmlElement(element),
  m_destinationFilename(destinationFilename)
{
  BuildNodeList(MessagePresetsListExportXML, m_messagePresets, m_xmlElement, "MessagePresets");
}

//----------------------------------------------------------------------------------------------------------------------
MessagePresetLibraryExportXML::MessagePresetLibraryExportXML(GUID guid, const wchar_t* destinationFilename)
: m_xmlElement(new XMLElement("MessagePresetLibrary")),
  m_destinationFilename(toUTF8(destinationFilename))
{
  m_xmlElement->setGUID(guid);
}

}
