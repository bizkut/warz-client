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
#include "morpheme/mrNetworkControlSerialiser.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkControlSerialiser::getMemoryRequirements(const NetworkControlDescriptor& controlDescriptor)
{
  const uint32_t numControlParams = controlDescriptor.getNumControlParamDescriptors();

  const NetworkDef* netDef = controlDescriptor.getNetworkDef();

  NMP::Memory::Format fmt(sizeof(NetworkControlSerialiser));
  fmt += NMP::Memory::Format(sizeof(AttribData*) * numControlParams); // m_controlParamData
  fmt += NMP::Memory::Format(sizeof(bool) * numControlParams); // m_controlParamDirty
  fmt += NMP::Memory::Format(sizeof(bool) * netDef->getNumMessages()); // m_messageState
  fmt += NMP::Memory::Format(sizeof(bool) * netDef->getNumMessages()); // m_messageBroadcast

  // Add the memory requirements of each index of m_controlParamData
  for (uint32_t controlParamIndex = 0; controlParamIndex < numControlParams; ++controlParamIndex)
  {
    const NetworkControlDescriptor::ControlParameterDescriptor& entry = 
      controlDescriptor.getControlParamDescriptorByIndex(controlParamIndex);
    switch (entry.m_type)
    {
    case ATTRIB_TYPE_BOOL:
      fmt += NMP::Memory::Format(sizeof(AttribDataBool));
      break;

    case ATTRIB_TYPE_UINT:
      fmt += NMP::Memory::Format(sizeof(AttribDataUInt));
      break;

    case ATTRIB_TYPE_INT:
      fmt += NMP::Memory::Format(sizeof(AttribDataInt));
      break;

    case ATTRIB_TYPE_FLOAT:
      fmt += NMP::Memory::Format(sizeof(AttribDataFloat));
      break;

    case ATTRIB_TYPE_VECTOR3:
      fmt += NMP::Memory::Format(sizeof(AttribDataVector3));
      break;

    case ATTRIB_TYPE_VECTOR4:
      fmt += NMP::Memory::Format(sizeof(AttribDataVector4));
      break;

    default:
      NMP_ASSERT_FAIL_MSG("Unsupported type");
      break;
    }
  }

  return fmt;
}

//----------------------------------------------------------------------------------------------------------------------
NetworkControlSerialiser* NetworkControlSerialiser::createNetworkControlSerialiser(
  Network* network,
  const NetworkControlDescriptor& controlDescriptor)
{
  const NMP::Memory::Format memReqs = getMemoryRequirements(controlDescriptor);
  NMP::Memory::Resource memResource = NMPMemoryAllocateFromFormat(memReqs);

  const NetworkDef* netDef = network->getNetworkDef();
  const uint32_t numControlParamDescriptors = controlDescriptor.getNumControlParamDescriptors();
  const uint32_t numMessages = netDef->getNumMessages();

  NetworkControlSerialiser* inputLayer = (NetworkControlSerialiser*)memResource.alignAndIncrement(NMP::Memory::Format(sizeof(NetworkControlSerialiser)));
  inputLayer->m_descriptor = &controlDescriptor;
  inputLayer->m_controlParamData = (AttribData**)memResource.alignAndIncrement(NMP::Memory::Format(sizeof(AttribData*) * numControlParamDescriptors));
  inputLayer->m_controlParamDirty = (bool*)memResource.alignAndIncrement(NMP::Memory::Format(sizeof(bool) * numControlParamDescriptors));
  inputLayer->m_messageState = (bool*)memResource.alignAndIncrement(NMP::Memory::Format(sizeof(bool) * numMessages));
  inputLayer->m_messageBroadcast = (bool*)memResource.alignAndIncrement(NMP::Memory::Format(sizeof(bool) * numMessages));
  inputLayer->m_network = network;

  // Initialise all the control param descriptors
  for (uint32_t controlParamIndex = 0; controlParamIndex < numControlParamDescriptors; ++controlParamIndex)
  {
    const NetworkControlDescriptor::ControlParameterDescriptor& packetEntry = controlDescriptor.getControlParamDescriptorByIndex(controlParamIndex);
    inputLayer->m_controlParamDirty[controlParamIndex] = false;
    switch (packetEntry.m_type)
    {
    case ATTRIB_TYPE_BOOL:
      inputLayer->m_controlParamData[controlParamIndex] = createAttribData<AttribDataBool>(memResource, packetEntry, netDef);
      inputLayer->setControlParam(packetEntry.m_nodeID, (AttribDataBool*)inputLayer->m_controlParamData[controlParamIndex]);
      break;

    case ATTRIB_TYPE_UINT:
      inputLayer->m_controlParamData[controlParamIndex] = createAttribData<AttribDataUInt>(memResource, packetEntry, netDef);
      inputLayer->setControlParam(packetEntry.m_nodeID, (AttribDataUInt*)inputLayer->m_controlParamData[controlParamIndex]);
      break;

    case ATTRIB_TYPE_INT:
      inputLayer->m_controlParamData[controlParamIndex] = createAttribData<AttribDataInt>(memResource, packetEntry, netDef);
      inputLayer->setControlParam(packetEntry.m_nodeID, (AttribDataInt*)inputLayer->m_controlParamData[controlParamIndex]);
      break;

    case ATTRIB_TYPE_FLOAT:
      inputLayer->m_controlParamData[controlParamIndex] = createAttribData<AttribDataFloat>(memResource, packetEntry, netDef);
      inputLayer->setControlParam(packetEntry.m_nodeID, (AttribDataFloat*)inputLayer->m_controlParamData[controlParamIndex]);
      break;

    case ATTRIB_TYPE_VECTOR3:
      inputLayer->m_controlParamData[controlParamIndex] = createAttribData<AttribDataVector3>(memResource, packetEntry, netDef);
      inputLayer->setControlParam(packetEntry.m_nodeID, (AttribDataVector3*)inputLayer->m_controlParamData[controlParamIndex]);
      break;

    case ATTRIB_TYPE_VECTOR4:
      inputLayer->m_controlParamData[controlParamIndex] = createAttribData<AttribDataVector4>(memResource, packetEntry, netDef);
      inputLayer->setControlParam(packetEntry.m_nodeID, (AttribDataVector4*)inputLayer->m_controlParamData[controlParamIndex]);
      break;

    default:
      NMP_ASSERT_FAIL_MSG("Unspported data type.");
      break;
    }
  }

  NMP_ASSERT_MSG(memResource.format.size == 0, "Not all allocated memory was used");

  // Initialise the state of all messages
  for (uint32_t i = 0; i < numMessages; ++i)
  {
    inputLayer->m_messageBroadcast[i] = false;
    inputLayer->m_messageState[i] = false;
  }

  return inputLayer;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::readPacketEntry(
  const NetworkControlDescriptor::ControlParameterDescriptor& pd,
  AttribData* attribData,
  const char* packetData,
  uint32_t& bitOffset)
{
  switch (pd.m_type)
  {
  case ATTRIB_TYPE_BOOL:
    {
      AttribDataBool* boolAttribData = (AttribDataBool*)attribData;
      boolAttribData->m_value = readBool(packetData, bitOffset);
      m_network->setControlParameter(pd.m_nodeID, boolAttribData);
      break;
    }

  case ATTRIB_TYPE_UINT:
    {
      AttribDataUInt* uintAttribData = (AttribDataUInt*)attribData;
      uintAttribData->m_value = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
      m_network->setControlParameter(pd.m_nodeID, uintAttribData);
      break;
    }

  case ATTRIB_TYPE_INT:
    {
      AttribDataInt* intAttribData = (AttribDataInt*)attribData;
      intAttribData->m_value = readInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
      m_network->setControlParameter(pd.m_nodeID, intAttribData);
      break;
    }

  case ATTRIB_TYPE_FLOAT:
    {
      AttribDataFloat* floatAttribData = (AttribDataFloat*)attribData;
      if (pd.m_quantiseFloat)
      {
        uint32_t quantisedFloat = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
        floatAttribData->m_value = unquantiseToFloat(pd, quantisedFloat);
      }
      else
      {
        uint32_t floatAsUInt32 = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
        floatAttribData->m_value = *(float*)(void*)&floatAsUInt32;
      }
      m_network->setControlParameter(pd.m_nodeID, floatAttribData);
      break;
    }

  case ATTRIB_TYPE_VECTOR3:
    {
      AttribDataVector3* vector3AttribData = (AttribDataVector3*)attribData;

      uint32_t floatAsUInt32X = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
      uint32_t floatAsUInt32Y = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
      uint32_t floatAsUInt32Z = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);

      if (pd.m_quantiseFloat)
      {
        vector3AttribData->m_value.x = unquantiseToFloat(pd, floatAsUInt32X);
        vector3AttribData->m_value.y = unquantiseToFloat(pd, floatAsUInt32Y);
        vector3AttribData->m_value.z = unquantiseToFloat(pd, floatAsUInt32Z);
      }
      else
      {
        vector3AttribData->m_value.x = *(float*)(void*)&floatAsUInt32X;
        vector3AttribData->m_value.y = *(float*)(void*)&floatAsUInt32Y;
        vector3AttribData->m_value.z = *(float*)(void*)&floatAsUInt32Z;
      }
      m_network->setControlParameter(pd.m_nodeID, vector3AttribData);
      break;
    }

  case ATTRIB_TYPE_VECTOR4:
    {
      AttribDataVector4* vector4AttribData = (AttribDataVector4*)attribData;

      uint32_t floatAsUInt32X = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
      uint32_t floatAsUInt32Y = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
      uint32_t floatAsUInt32Z = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);
      uint32_t floatAsUInt32W = readUInt32(packetData, bitOffset, pd.m_bitsOfPrecision);

      if (pd.m_quantiseFloat)
      {
        vector4AttribData->m_value.x = unquantiseToFloat(pd, floatAsUInt32X);
        vector4AttribData->m_value.y = unquantiseToFloat(pd, floatAsUInt32Y);
        vector4AttribData->m_value.z = unquantiseToFloat(pd, floatAsUInt32Z);
        vector4AttribData->m_value.w = unquantiseToFloat(pd, floatAsUInt32W);
      }
      else
      {
        vector4AttribData->m_value.x = *(float*)(void*)&floatAsUInt32X;
        vector4AttribData->m_value.y = *(float*)(void*)&floatAsUInt32Y;
        vector4AttribData->m_value.z = *(float*)(void*)&floatAsUInt32Z;
        vector4AttribData->m_value.w = *(float*)(void*)&floatAsUInt32W;
      }
      m_network->setControlParameter(pd.m_nodeID, vector4AttribData);
      break;
    }

  default:
    NMP_ASSERT_FAIL_MSG("Unsupported attrib data type");
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::setFromPacket(const void* packet, uint32_t bitOffset)
{
  const char* packetData = (const char*)packet;

  const uint32_t numPacketEntries = m_descriptor->getNumControlParamDescriptors();
  for (uint32_t packetEntryIndex = 0; packetEntryIndex < numPacketEntries; ++packetEntryIndex)
  {
    const NetworkControlDescriptor::ControlParameterDescriptor& packetEntry = m_descriptor->getControlParamDescriptorByIndex(packetEntryIndex);
    if (packetEntry.m_serialise)
    {
      m_controlParamDirty[packetEntryIndex] = true;
      readPacketEntry(packetEntry, m_controlParamData[packetEntryIndex], packetData, bitOffset);
    }
  }

  // read the messages
  const uint32_t numMessages = m_descriptor->getNetworkDef()->getNumMessages();
  for (uint32_t i = 0; i < numMessages; ++i)
  {
    if (m_descriptor->serialiseMessageID(i))
    {
      const bool broadcastThisMessage = readBool(packetData, bitOffset);
      const bool messageStatus = readBool(packetData, bitOffset);
      if (broadcastThisMessage)
      {
        Message msg((MessageID)i, MESSAGE_TYPE_REQUEST, messageStatus, NULL, 0);
        NodeID targetNodeID = m_descriptor->getMessageTargetNode((MessageID)i);
        if (targetNodeID == INVALID_NODE_ID)
        {
          m_network->broadcastMessage(msg);
        }
        else
        {
          m_network->sendMessage(targetNodeID, msg);
        }
      }
    }
  }

  return bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::setFromDeltaPacket(const void* packet, uint32_t bitOffset)
{
  const char* packetData = (const char*)packet;

  // The first bit of all packets is whether or not this is a delta packet.
  const bool deltaPacket = readBool(packetData, bitOffset);

  if (!deltaPacket)
  {
    return setFromPacket(packetData, bitOffset);
  }

  const uint32_t numPacketEntries = m_descriptor->getNumControlParamDescriptors();
  for (uint32_t packetEntryIndex = 0; packetEntryIndex < numPacketEntries; ++packetEntryIndex)
  {
    const NetworkControlDescriptor::ControlParameterDescriptor& packetEntry = m_descriptor->getControlParamDescriptorByIndex(packetEntryIndex);
    if (packetEntry.m_serialise)
    {
      bool paramChanged = readBool(packetData, bitOffset);
      if (paramChanged)
      {
        m_controlParamDirty[packetEntryIndex] = true;
        readPacketEntry(packetEntry, m_controlParamData[packetEntryIndex], packetData, bitOffset);
      }
    }
  }

  // read the messages
  const uint32_t numMessages = m_descriptor->getNetworkDef()->getNumMessages();
  for (uint32_t i = 0; i < numMessages; ++i)
  {
    if (m_descriptor->serialiseMessageID(i))
    {
      bool broadcastThisMessage = readBool(packetData, bitOffset);
      if (broadcastThisMessage)
      {
        const bool messageStatus = readBool(packetData, bitOffset);
        Message msg((MessageID)i, MESSAGE_TYPE_REQUEST, messageStatus, NULL, 0);
        NodeID targetNodeID = m_descriptor->getMessageTargetNode((MessageID)i);
        if (targetNodeID == INVALID_NODE_ID)
        {
          m_network->broadcastMessage(msg);
        }
        else
        {
          m_network->sendMessage(targetNodeID, msg);
        }
      }
    }
  }

  return bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::writePacketEntry(
  const NetworkControlDescriptor::ControlParameterDescriptor& pd,
  AttribData* attribData,
  char* packetData,
  uint32_t& bitOffset) const
{
  switch (pd.m_type)
  {
  case ATTRIB_TYPE_BOOL:
    writeBool(packetData, bitOffset, ((AttribDataBool*)attribData)->m_value);
    break;

  case ATTRIB_TYPE_FLOAT:
    if (pd.m_quantiseFloat)
    {
      uint32_t quantisedValue = quantiseFloat(pd, ((AttribDataFloat*)attribData)->m_value);
      writeUInt32(packetData, bitOffset, quantisedValue, pd.m_bitsOfPrecision);
    }
    else
    {
      AttribDataFloat* floatAttribData = (AttribDataFloat*)attribData;
      uint32_t floatAsUInt = *(uint32_t*)(void*)&floatAttribData->m_value;
      writeUInt32(packetData, bitOffset, floatAsUInt, pd.m_bitsOfPrecision);
    }
    break;

  case ATTRIB_TYPE_UINT:
    writeUInt32(packetData, bitOffset, ((AttribDataUInt*)attribData)->m_value, pd.m_bitsOfPrecision);
    break;

  case ATTRIB_TYPE_INT:
    writeInt32(packetData, bitOffset, ((AttribDataInt*)attribData)->m_value, pd.m_bitsOfPrecision);
    break;

  case ATTRIB_TYPE_VECTOR3:
    if (pd.m_quantiseFloat)
    {
      uint32_t quantisedValue = quantiseFloat(pd, ((AttribDataVector3*)attribData)->m_value.x);
      writeUInt32(packetData, bitOffset, quantisedValue, pd.m_bitsOfPrecision);
      quantisedValue = quantiseFloat(pd, ((AttribDataVector3*)attribData)->m_value.y);
      writeUInt32(packetData, bitOffset, quantisedValue, pd.m_bitsOfPrecision);
      quantisedValue = quantiseFloat(pd, ((AttribDataVector3*)attribData)->m_value.z);
      writeUInt32(packetData, bitOffset, quantisedValue, pd.m_bitsOfPrecision);
    }
    else
    {
      uint32_t floatAsUInt32 = *(uint32_t*)(void*)&((AttribDataVector3*)attribData)->m_value.x;
      writeUInt32(packetData, bitOffset, floatAsUInt32, pd.m_bitsOfPrecision);
      floatAsUInt32 = *(uint32_t*)(void*)&((AttribDataVector3*)attribData)->m_value.y;
      writeUInt32(packetData, bitOffset, floatAsUInt32, pd.m_bitsOfPrecision);
      floatAsUInt32 = *(uint32_t*)(void*)&((AttribDataVector3*)attribData)->m_value.z;
      writeUInt32(packetData, bitOffset, floatAsUInt32, pd.m_bitsOfPrecision);
    }
    break;

  case ATTRIB_TYPE_VECTOR4:
    if (pd.m_quantiseFloat)
    {
      uint32_t quantisedValue = quantiseFloat(pd, ((AttribDataVector3*)attribData)->m_value.x);
      writeUInt32(packetData, bitOffset, quantisedValue, pd.m_bitsOfPrecision);
      quantisedValue = quantiseFloat(pd, ((AttribDataVector3*)attribData)->m_value.y);
      writeUInt32(packetData, bitOffset, quantisedValue, pd.m_bitsOfPrecision);
      quantisedValue = quantiseFloat(pd, ((AttribDataVector3*)attribData)->m_value.z);
      writeUInt32(packetData, bitOffset, quantisedValue, pd.m_bitsOfPrecision);
      quantisedValue = quantiseFloat(pd, ((AttribDataVector3*)attribData)->m_value.w);
      writeUInt32(packetData, bitOffset, quantisedValue, pd.m_bitsOfPrecision);
    }
    else
    {
      uint32_t floatAsUInt32 = *(uint32_t*)(void*)&((AttribDataVector3*)attribData)->m_value.x;
      writeUInt32(packetData, bitOffset, floatAsUInt32, pd.m_bitsOfPrecision);
      floatAsUInt32 = *(uint32_t*)(void*)&((AttribDataVector3*)attribData)->m_value.y;
      writeUInt32(packetData, bitOffset, floatAsUInt32, pd.m_bitsOfPrecision);
      floatAsUInt32 = *(uint32_t*)(void*)&((AttribDataVector3*)attribData)->m_value.z;
      writeUInt32(packetData, bitOffset, floatAsUInt32, pd.m_bitsOfPrecision);
      floatAsUInt32 = *(uint32_t*)(void*)&((AttribDataVector3*)attribData)->m_value.w;
      writeUInt32(packetData, bitOffset, floatAsUInt32, pd.m_bitsOfPrecision);
    }
    break;

  default:
    NMP_ASSERT_FAIL_MSG("Unsupported attrib data type");
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::createPacket(void* packetMemory, uint32_t bitOffset) const
{
  char* packetData = (char*)packetMemory;

#ifdef NMP_ENABLE_ASSERTS
  m_descriptor->validatePacketMemoryRequirements();
  uint32_t startingBitOffset = bitOffset;
#endif

  const uint32_t numPacketEntries = m_descriptor->getNumControlParamDescriptors();
  for (uint32_t packetEntryIndex = 0; packetEntryIndex < numPacketEntries; ++packetEntryIndex)
  {
    const NetworkControlDescriptor::ControlParameterDescriptor& packetEntry = m_descriptor->getControlParamDescriptorByIndex(packetEntryIndex);
    if (packetEntry.m_serialise)
    {
      m_controlParamDirty[packetEntryIndex] = false;
      writePacketEntry(packetEntry, m_controlParamData[packetEntryIndex], packetData, bitOffset);
    }
  }

  // Serialise the message states
  const uint32_t numMessages = m_descriptor->getNetworkDef()->getNumMessages();
  for (uint32_t i = 0; i < numMessages; ++i)
  {
    if (m_descriptor->serialiseMessageID((MessageID)i))
    {
      // To keep packet sizes consistent the message state is always serialised even if the message wasn't broadcasted.
      writeBool(packetData, bitOffset, m_messageBroadcast[i]);
      writeBool(packetData, bitOffset, m_messageState[i]);
    }
  }

  NMP_ASSERT((bitOffset - startingBitOffset) == m_descriptor->getPacketMemoryBitRequirements(1));

  return bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::createDeltaPacket(void* packetMemory, uint32_t bitOffset) const
{
  char* packetData = (char*)packetMemory;

#ifdef NMP_ENABLE_ASSERTS
  uint32_t startingBitOffset = bitOffset;
#endif

  const uint32_t deltaPacketBitReqs = calculateDeltaPacketBitRequirements();
  const uint32_t standardPacketBitReqs = m_descriptor->getPacketMemoryBitRequirements(1) + 1; // + 1 for the delta packet flag
  if (deltaPacketBitReqs >= standardPacketBitReqs)
  {
    // A delta packet will be larger than a standard packet so serialise a standard packet instead

    // The first bit of all packets is whether or not this is a delta packet.
    writeBool(packetData, bitOffset, false);
    return createPacket(packetMemory, bitOffset);
  }

  // The first bit of all packets is whether or not this is a delta packet.
  writeBool(packetData, bitOffset, true);

  const uint32_t numPacketEntries = m_descriptor->getNumControlParamDescriptors();
  for (uint32_t packetEntryIndex = 0; packetEntryIndex < numPacketEntries; ++packetEntryIndex)
  {
    const NetworkControlDescriptor::ControlParameterDescriptor& packetEntry = m_descriptor->getControlParamDescriptorByIndex(packetEntryIndex);
    if (packetEntry.m_serialise)
    {
      // Write the dirty state of the control param
      writeBool(packetData, bitOffset, m_controlParamDirty[packetEntryIndex]);

      if (m_controlParamDirty[packetEntryIndex])
      {
        m_controlParamDirty[packetEntryIndex] = false;
        writePacketEntry(packetEntry, m_controlParamData[packetEntryIndex], packetData, bitOffset);
      }
    }
  }

  // Serialise the message states
  const uint32_t numMessages = m_descriptor->getNetworkDef()->getNumMessages();
  for (uint32_t i = 0; i < numMessages; ++i)
  {
    if (m_descriptor->serialiseMessageID((MessageID)i))
    {
      writeBool(packetData, bitOffset, m_messageBroadcast[i]);
      if (m_messageBroadcast[i])
      {
        writeBool(packetData, bitOffset, m_messageState[i]);
        m_messageBroadcast[i] = false;
      }
    }
  }

  NMP_ASSERT((bitOffset - startingBitOffset) == deltaPacketBitReqs);

  return bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::calculateDeltaPacketBitRequirements() const
{
  // 1 bit is required to determine if this is a delta packet
  uint32_t bitRequirements = 1;

  uint32_t numControlParamEntries = m_descriptor->getNumControlParamDescriptors();
  for (uint32_t i = 0; i < numControlParamEntries; ++i)
  {
    const NetworkControlDescriptor::ControlParameterDescriptor& packetEntry = m_descriptor->getControlParamDescriptorByIndex(i);
    if (packetEntry.m_serialise)
    {
      // 1 bit for this entries dirty flag 
      ++bitRequirements;

      if (m_controlParamDirty[i])
      {
        switch (packetEntry.m_type)
        {
        case ATTRIB_TYPE_BOOL:
          ++bitRequirements;
          break;

        case ATTRIB_TYPE_UINT:
        case ATTRIB_TYPE_FLOAT:
        case ATTRIB_TYPE_INT:
          bitRequirements += packetEntry.m_bitsOfPrecision;
          break;

        case ATTRIB_TYPE_VECTOR3:
          bitRequirements += packetEntry.m_bitsOfPrecision * 3;
          break;

        case ATTRIB_TYPE_VECTOR4:
          bitRequirements += packetEntry.m_bitsOfPrecision * 4;
          break;

        default:
          NMP_ASSERT_FAIL_MSG("Unsupported");
          break;
        }
      }
    }
  }

  const uint32_t numMessages = m_descriptor->getNetworkDef()->getNumMessages();
  for (uint32_t i = 0; i < numMessages; ++i)
  {
    if (m_descriptor->serialiseMessageID((MessageID)i))
    {
      ++bitRequirements;
      if (m_messageBroadcast[i])
      {
        ++bitRequirements;
      }
    }
  }

  return bitRequirements;
}

//----------------------------------------------------------------------------------------------------------------------
NetworkControlDescriptor* NetworkControlDescriptor::createPacketDescriptor(NetworkDef* netDef)
{
  const uint32_t numControlParamNodes = netDef->getNumControlParameterNodes();
  const uint32_t numMessages = netDef->getNumMessages();

  NMP::Memory::Format memReqs = getMemoryRequirements(netDef);
  NMP::Memory::Resource memResource = NMPMemoryAllocateFromFormat(memReqs);

  NetworkControlDescriptor* descriptor = (NetworkControlDescriptor*)memResource.alignAndIncrement(
    NMP::Memory::Format(sizeof(NetworkControlDescriptor)));

  descriptor->m_entries = (ControlParameterDescriptor*)memResource.alignAndIncrement(
    NMP::Memory::Format(sizeof(ControlParameterDescriptor) * numControlParamNodes));

  NMP::Memory::Format nodeIDPacketEntryMapMemReqs(sizeof(uint32_t) * netDef->getNumNodeDefs());
  descriptor->m_nodeIDPacketEntryMap = (uint32_t*)memResource.alignAndIncrement(nodeIDPacketEntryMapMemReqs);
  descriptor->m_serialiseMessageIDs = (bool*)memResource.alignAndIncrement(NMP::Memory::Format(sizeof(bool) * numMessages));
  descriptor->m_messageTargetState = (NodeID*)memResource.alignAndIncrement(NMP::Memory::Format(sizeof(NodeID) * numMessages));
  descriptor->m_networkDef = netDef;

  // clear the node id map and the message id state
  memset(descriptor->m_nodeIDPacketEntryMap, 0xFFFFFFFF, netDef->getNumNodeDefs() * sizeof(uint32_t));
  memset(descriptor->m_serialiseMessageIDs, 0xFFFFFFFF, sizeof(bool) * netDef->getNumMessages());

  for (uint32_t i = 0; i < numMessages; ++i)
  {
    descriptor->m_messageTargetState[i] = INVALID_NODE_ID;
  }

  // Build the control param node id to packet entry map
  NodeID* controlParamNodes = (NodeID*)NMPMemoryAlloc(sizeof(NodeID) * numControlParamNodes);
  NMP_ASSERT(controlParamNodes);
  netDef->getControlParameterNodeIDs(controlParamNodes, numControlParamNodes);

  for (uint32_t controlParamIndex = 0; controlParamIndex < numControlParamNodes; ++controlParamIndex)
  {
    const NodeID controlParamNodeID = controlParamNodes[controlParamIndex];
    descriptor->m_nodeIDPacketEntryMap[controlParamNodeID] = controlParamIndex;
    for (uint32_t semanticIndex = ATTRIB_SEMANTIC_CP_BOOL; semanticIndex < ATTRIB_SEMANTIC_CP_VECTOR4; ++semanticIndex)
    {
      AttribDataHandle* attribData = netDef->getOptionalAttribDataHandle((AttribDataSemantic)semanticIndex, controlParamNodeID, 0);
      if (attribData && attribData->m_attribData)
      {
        ControlParameterDescriptor& packetEntry = descriptor->getControlParamDescriptorByIndex(controlParamIndex);
        packetEntry.m_type = attribData->m_attribData->getType();
        packetEntry.m_semantic = (AttribDataSemantic)semanticIndex;
        packetEntry.m_nodeID = controlParamNodes[controlParamIndex];
        packetEntry.m_bitsOfPrecision = 32;
        packetEntry.m_quantiseFloat = false;
        packetEntry.m_quantiseFactor = 1.0f;
        packetEntry.m_quantiseOffset = 0.0f;
        packetEntry.m_serialise = true;
        break;
      }
    }
  }

  // Make sure all memory was used
  NMP_ASSERT(memResource.format.size == 0);

  descriptor->updatePacketMemoryRequirements();

  NMP::Memory::memFree(controlParamNodes);

  return descriptor;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlDescriptor::updatePacketMemoryRequirements()
{
  m_numBitsInPacket = calculatePacketMemoryRequirements();
}

#ifdef NMP_ENABLE_ASSERTS
//----------------------------------------------------------------------------------------------------------------------
void NetworkControlDescriptor::validatePacketMemoryRequirements() const
{
  uint32_t numBitsInPacket = calculatePacketMemoryRequirements();
  NMP_ASSERT_MSG(numBitsInPacket == m_numBitsInPacket, "NetworkDescriptor packet memory requirements is out of date "
    "call updatePacketMemoryRequirements");
}
#endif// NMP_ENABLE_ASSERTS

//----------------------------------------------------------------------------------------------------------------------
} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
