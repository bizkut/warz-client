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
#include "assetProcessor/MessageBuilder.h"

#include "nmtl/pod_vector.h"

#include "assetProcessor/NodeBuilder.h"
//----------------------------------------------------------------------------------------------------------------------

namespace AP
{

//----------------------------------------------------------------------------------------------------------------------
// MessageDistributorBuilder functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format MessageDistributorBuilder::getMemoryRequirements(
  const ME::MessageExport* messageExport,
  const ME::MessagePresetsListExport* messagePresetsExport,
  const nmtl::pod_vector<MR::NodeID>& interestedNodes,
  AssetProcessor* processor)
{
  MR::MessageType messageType = messageExport->getMessageType();

  NMP::Memory::Format result(sizeof(MR::MessageDistributor), NMP_NATURAL_TYPE_ALIGNMENT);

  //-------------------------
  // add space for the m_nodeIDs array.
  uint32_t numNodeIDs = (uint32_t)interestedNodes.size();
  result += NMP::Memory::Format(numNodeIDs * sizeof(MR::NodeID), NMP_NATURAL_TYPE_ALIGNMENT);

  if (messagePresetsExport)
  {
    //-------------------------
    // get the builder for this type
    MessageBuilder* messageBuilder = processor->getMessageBuilder(messageType);
    if (!messageBuilder)
    {
      processor->outputMessageError(messageType, "Cannot find message builder.");
    }
    NMP_VERIFY_MSG(
      messageBuilder,
      "Cannot find message builder for message with MessageType %i, perhaps it was not registered",
      messageType);

    //-------------------------
    // add space for the m_messagePresets array.
    uint32_t numMessagePresets = messagePresetsExport->getNumMessagePresets();
    result += NMP::Memory::Format(numMessagePresets * sizeof(MR::Message*), NMP_NATURAL_TYPE_ALIGNMENT);

    //-------------------------
    // the size of all the strings in the string table combined
    uint32_t stringTableDataSize = 0;

    //-------------------------
    // add the size required by the presets themselves
    for (uint32_t i = 0; i != numMessagePresets; ++i)
    {
      const ME::MessagePresetExport* messagePreset = messagePresetsExport->getMessagePreset(i);

      const char* presetName = messagePreset->getName();
      uint32_t presetNameLength = (uint32_t)(NMP_STRLEN(presetName) + 1);
      stringTableDataSize += presetNameLength;

      result += messageBuilder->getMemoryRequirements(messageExport, messagePreset, processor);
    }

    //-------------------------
    // add the size required by the presets string table
    result += NMP::OrderedStringTable::getMemoryRequirements(numMessagePresets, stringTableDataSize);
  }

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::MessageDistributor* MessageDistributorBuilder::init(
  NMP::Memory::Resource& memRes,
  const ME::MessageExport* messageExport,
  const ME::MessagePresetsListExport* messagePresetsExport,
  const nmtl::pod_vector<MR::NodeID>& interestedNodes,
  AssetProcessor* processor)
{
  MR::MessageType messageType = messageExport->getMessageType();

  uint32_t numNodeIDs = (uint32_t)interestedNodes.size();
  uint32_t numMessagePresets = 0;
  
  MessageBuilder* messageBuilder = NULL;

  if (messagePresetsExport)
  {
    //-------------------------
    // get the builder for this type
    messageBuilder = processor->getMessageBuilder(messageType);
    if (!messageBuilder)
    {
      processor->outputMessageError(messageType, "Cannot find message builder.");
    }
    NMP_VERIFY_MSG(
      messageBuilder,
      "Cannot find message builder for message with MessageType %i, perhaps it was not registered",
      messageType);

    numMessagePresets = messagePresetsExport->getNumMessagePresets();
  }

  nmtl::pod_vector<const char*> presetNames(numMessagePresets, "");

  //-------------------------
  // initialise the distributor
  NMP::Memory::Format format(sizeof(MR::MessageDistributor), NMP_NATURAL_TYPE_ALIGNMENT);
  MR::MessageDistributor* distributor = (MR::MessageDistributor*)memRes.alignAndIncrement(format);

  distributor->m_messageID = messageExport->getMessageID();
  distributor->m_messageType = messageExport->getMessageType();
  distributor->m_numNodeIDs = numNodeIDs;
  distributor->m_numMessagePresets = numMessagePresets;

  //-------------------------
  // initialise the nodes array
  if (numNodeIDs > 0)
  {
    format.set(numNodeIDs * sizeof(MR::NodeID), NMP_NATURAL_TYPE_ALIGNMENT);
    distributor->m_nodeIDs = (MR::NodeID*)memRes.alignAndIncrement(format);

    NMP::Memory::memcpy(distributor->m_nodeIDs, &interestedNodes[0], sizeof(MR::NodeID) * numNodeIDs);
  }
  else
  {
    distributor->m_nodeIDs = 0;
  }

  distributor->m_messagePresets = 0;
  distributor->m_messagePresetIndexNamesTable = 0;

  if (numMessagePresets > 0)
  {
    //-------------------------
    // initialise the array of message presets
    format.set(numMessagePresets * sizeof(MR::Message**), NMP_NATURAL_TYPE_ALIGNMENT);
    distributor->m_messagePresets = (MR::Message**)memRes.alignAndIncrement(format);

    for (uint32_t i = 0; i != numMessagePresets; ++i)
    {
      const ME::MessagePresetExport* messagePresetExport = messagePresetsExport->getMessagePreset(i);

      distributor->m_messagePresets[i] = messageBuilder->init(memRes, messageExport, messagePresetExport, processor);

      presetNames[i] = messagePresetExport->getName();
    }

    //-------------------------
    // build the string table
    distributor->m_messagePresetIndexNamesTable =
      NMP::OrderedStringTable::init(memRes, numMessagePresets, &presetNames[0]);
  }

  return distributor;
}

} // namespace AP

//----------------------------------------------------------------------------------------------------------------------
