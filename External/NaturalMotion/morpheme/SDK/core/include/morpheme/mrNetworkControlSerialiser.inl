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

//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format NetworkControlDescriptor::getMemoryRequirements(NetworkDef* netDef)
{
  const uint32_t numControlParamNodes = netDef->getNumControlParameterNodes();

  NMP::Memory::Format fmt(sizeof(NetworkControlDescriptor));
  fmt += NMP::Memory::Format(sizeof(ControlParameterDescriptor) * numControlParamNodes);
  fmt += NMP::Memory::Format(sizeof(uint32_t) * netDef->getNumNodeDefs()); // m_nodeIDPacketEntryMap
  fmt += NMP::Memory::Format(sizeof(bool) * netDef->getNumMessages()); // m_serialiseMessageIDs
  fmt += NMP::Memory::Format(sizeof(NodeID) * netDef->getNumMessages()); // m_messageTargetState

  return fmt;
}

//----------------------------------------------------------------------------------------------------------------------
NetworkControlDescriptor::ControlParameterDescriptor& NetworkControlDescriptor::getControlParamDescriptor(NodeID controlParamNodeID)
{
  NMP_ASSERT(controlParamNodeID < m_networkDef->getNumNodeDefs());
  NMP_ASSERT(m_nodeIDPacketEntryMap[controlParamNodeID] != INVALID_NODE_ID);
  return m_entries[m_nodeIDPacketEntryMap[controlParamNodeID]];
}

//----------------------------------------------------------------------------------------------------------------------
const NetworkControlDescriptor::ControlParameterDescriptor& NetworkControlDescriptor::getControlParamDescriptor(NodeID controlParamNodeID) const
{
  NMP_ASSERT(controlParamNodeID < m_networkDef->getNumNodeDefs());
  NMP_ASSERT(m_nodeIDPacketEntryMap[controlParamNodeID] != INVALID_NODE_ID);
  return m_entries[m_nodeIDPacketEntryMap[controlParamNodeID]];
}

//----------------------------------------------------------------------------------------------------------------------
NetworkControlDescriptor::ControlParameterDescriptor& NetworkControlDescriptor::getControlParamDescriptorByIndex(uint32_t descriptorIndex)
{
  NMP_ASSERT(descriptorIndex < m_networkDef->getNumControlParameterNodes());
  return m_entries[descriptorIndex];
}

//----------------------------------------------------------------------------------------------------------------------
const NetworkControlDescriptor::ControlParameterDescriptor& NetworkControlDescriptor::getControlParamDescriptorByIndex(uint32_t descriptorIndex) const
{
  NMP_ASSERT(descriptorIndex < m_networkDef->getNumControlParameterNodes());
  return m_entries[descriptorIndex];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlDescriptor::getNumControlParamDescriptors() const
{
  return m_networkDef->getNumControlParameterNodes();
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlDescriptor::getControlParamIndexFromNodeID(NodeID controlParamNodeID) const
{
  NMP_ASSERT(controlParamNodeID < m_networkDef->getNumNodeDefs());
  NMP_ASSERT(m_nodeIDPacketEntryMap[controlParamNodeID] != INVALID_NODE_ID);
  return m_nodeIDPacketEntryMap[controlParamNodeID];
}

//----------------------------------------------------------------------------------------------------------------------
const NetworkDef* NetworkControlDescriptor::getNetworkDef() const
{
  return m_networkDef;
}

//----------------------------------------------------------------------------------------------------------------------
const NMP::Memory::Format NetworkControlDescriptor::getPacketMemoryRequirements(uint32_t numPackets) const
{
  // + 7 ensures that the byte count will be rounded up rather than down
  uint32_t totalNumBits = numPackets * m_numBitsInPacket;
  uint32_t numBytesRequired = (uint32_t)NMP::Memory::align(totalNumBits, 32) / 4;
  return NMP::Memory::Format(numBytesRequired, NMP_NATURAL_TYPE_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlDescriptor::getPacketMemoryBitRequirements(uint32_t numNetworks) const
{
  return m_numBitsInPacket * numNetworks;
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkControlDescriptor::serialiseMessageID(MessageID id) const
{
  NMP_ASSERT(id < m_networkDef->getNumMessages());
  return m_serialiseMessageIDs[id];
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlDescriptor::setSerialiseMessageID(MessageID id, bool serialise)
{
  NMP_ASSERT(id < m_networkDef->getNumMessages());
  m_serialiseMessageIDs[id] = serialise;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlDescriptor::setMessageTargetNode(MessageID id, NodeID targetNode)
{
  NMP_ASSERT(id < m_networkDef->getNumMessages());
  m_messageTargetState[id] = targetNode;
}

//----------------------------------------------------------------------------------------------------------------------
NodeID NetworkControlDescriptor::getMessageTargetNode(MessageID id) const
{
  NMP_ASSERT(id < m_networkDef->getNumMessages());
  return m_messageTargetState[id];
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlDescriptor::calculatePacketMemoryRequirements() const
{
  uint32_t numBitsInPacket = 0;

  const uint32_t numControlParams = m_networkDef->getNumControlParameterNodes();
  for (uint32_t i = 0; i < numControlParams; ++i)
  {
    const ControlParameterDescriptor& packetEntry = getControlParamDescriptorByIndex(i);
    if (packetEntry.m_serialise)
    {
      NMP_ASSERT(packetEntry.m_bitsOfPrecision <= 32 && packetEntry.m_bitsOfPrecision >= 1);

      switch (packetEntry.m_type)
      {
      case ATTRIB_TYPE_BOOL:
        ++numBitsInPacket;
        break;

      case ATTRIB_TYPE_UINT:
      case ATTRIB_TYPE_FLOAT:
      case ATTRIB_TYPE_INT:
        numBitsInPacket += packetEntry.m_bitsOfPrecision;
        break;

      case ATTRIB_TYPE_VECTOR3:
        numBitsInPacket += packetEntry.m_bitsOfPrecision * 3;
        break;

      case ATTRIB_TYPE_VECTOR4:
        numBitsInPacket += packetEntry.m_bitsOfPrecision * 4;
        break;

      default:
        NMP_ASSERT_FAIL_MSG("Unsupported");
        break;
      }
    }
  }

  const uint32_t numNetworkMessages =  m_networkDef->getNumMessages();
  for (uint32_t i = 0; i < numNetworkMessages; ++i)
  {
    if (m_serialiseMessageIDs[i])
    {
      numBitsInPacket += 2;
    }
  }

  return numBitsInPacket;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::readUInt32(const char* data, uint32_t& offsetInBits, uint32_t length)
{
  uint32_t byteOffset = offsetInBits >> 3;
  uint32_t bitsToShift = offsetInBits - (byteOffset << 3);

  uint32_t result;
  if (bitsToShift + length > 32)
  {
    // The data to read crosses a 4 byte boundary. It's more convenient to read it directly into a uint64_t. It's safe to
    // read 64 bits because the memory requirements of a data packet is padded to 4 byte boundaries and we are reading
    // from at least 64 bits before the end of the buffer.
    uint64_t unalignedData = *(uint64_t*)(&data[byteOffset]);
    uint64_t alignedData = unalignedData >> bitsToShift;
    uint64_t lengthMask = ULLONG_MAX;
    lengthMask = lengthMask << length;
    alignedData = alignedData & ~lengthMask;
    result = (uint32_t)alignedData;
  }
  else
  {
    uint32_t unalignedData = *(uint32_t*)(&data[byteOffset]);
    uint32_t alignedData = unalignedData >> bitsToShift;
    uint32_t lengthMask = (uint32_t)(ULLONG_MAX << length);
    alignedData = alignedData & ~lengthMask;
    result = (uint32_t)alignedData;
  }

  offsetInBits += length;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
int32_t NetworkControlSerialiser::expandToInt(uint32_t val, uint32_t intPrecision)
{
  uint32_t signTestMask = 1 << (intPrecision - 1);
  if ((val & signTestMask) == 0)
  {
    return val;
  }
  else
  {
    uint32_t negMask = (uint32_t)(ULLONG_MAX << intPrecision);
    return val | negMask;
  }
}

//----------------------------------------------------------------------------------------------------------------------
int32_t NetworkControlSerialiser::readInt32(const char* data, uint32_t& offsetInBits, uint32_t length)
{
  uint32_t val = readUInt32(data, offsetInBits, length);
  return expandToInt(val, length);
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkControlSerialiser::readBool(const char* data, uint32_t& offsetInBits)
{
  uint32_t byteOffset = offsetInBits >> 3;
  uint32_t bitsToShift = offsetInBits - (byteOffset << 3);
  uint32_t mask = 1 << bitsToShift;
  offsetInBits += 1;
  return ((uint32_t)(data[byteOffset]) & mask) != 0;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::writeUInt32(char* dest, uint32_t& bitOffset, uint32_t dataToWrite, uint32_t bitsToWrite)
{
  NMP_ASSERT(bitsToWrite != 0);

  uint32_t byteBoundary = bitOffset >> 3;
  uint32_t byteBoundaryInBits = byteBoundary << 3;
  uint32_t base = *(uint32_t*)&dest[byteBoundary];
  uint32_t bitsToShift = bitOffset - byteBoundaryInBits;
  uint32_t mask = (uint32_t)(ULLONG_MAX << bitsToShift);

  uint32_t dataToWriteShifted = dataToWrite;
  dataToWriteShifted = dataToWriteShifted << bitsToShift;
  
  uint32_t dataCombined = (base & ~mask) | (dataToWriteShifted & mask);
  *((uint32_t*)&dest[byteBoundary]) = dataCombined;

  if ((bitsToShift + bitsToWrite) > 32)
  {
    // The data to write extends into the next 32 bits so handle that
    uint32_t nextByteBoundary = byteBoundary + 4;
    uint32_t dataShiftedAgain = dataToWrite >> (32 - bitsToShift);
    *(uint32_t*)&dest[nextByteBoundary] = dataShiftedAgain;
  }
  bitOffset += bitsToWrite;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::writeInt32(char* dest, uint32_t& bitOffset, int32_t dataToWrite, uint32_t bitsToWrite)
{
  writeUInt32(dest, bitOffset, (uint32_t)dataToWrite, bitsToWrite);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::writeBool(char* dest, uint32_t& bitOffset, bool value)
{
  uint32_t byteBoundary = bitOffset >> 3;
  char* destByte = &dest[byteBoundary];
  uint8_t mask = 0xFF;
  uint8_t bitsToShift = (char)(bitOffset - (byteBoundary << 3));
  mask = mask << bitsToShift;

  uint8_t dataToWrite = value;
  dataToWrite = dataToWrite << bitsToShift;
  uint8_t destValue = (*destByte & ~mask) | (dataToWrite & mask);
  *destByte = (char)(destValue);
  ++bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
// NetworkControlSerialiser
//----------------------------------------------------------------------------------------------------------------------
template <typename _AttribDataClass>
_AttribDataClass* NetworkControlSerialiser::createAttribData(NMP::Memory::Resource& resource, const NetworkControlDescriptor::ControlParameterDescriptor& packetEntry, const NetworkDef* networkDef)
{
  const _AttribDataClass* netDefAttribData = networkDef->getAttribData<_AttribDataClass>(packetEntry.m_semantic, packetEntry.m_nodeID, 0);
  void* newAttribDataMemory = (_AttribDataClass*)resource.alignAndIncrement(NMP::Memory::Format(sizeof(_AttribDataClass)));
  _AttribDataClass* packetControlParamData = (_AttribDataClass*)newAttribDataMemory;
  packetControlParamData->setType(netDefAttribData->getType());
  _AttribDataClass::copy(netDefAttribData, packetControlParamData);
  return packetControlParamData;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::quantiseFloat(const NetworkControlDescriptor::ControlParameterDescriptor& pe, float value)
{
  uint32_t result = (uint32_t)((value + pe.m_quantiseOffset) * pe.m_quantiseFactor + 0.5f);
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
float NetworkControlSerialiser::unquantiseToFloat(const NetworkControlDescriptor::ControlParameterDescriptor& pe, uint32_t value)
{
  float result = (float)value;
  result = (result / pe.m_quantiseFactor) - pe.m_quantiseOffset;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
template <typename AttribDataClass>
void NetworkControlSerialiser::setControlParam(NodeID owningNodeID, const AttribDataClass* inputAttribData)
{
  uint32_t packetEntryID = m_descriptor->getControlParamIndexFromNodeID(owningNodeID);

  AttribDataClass adjustedInput;

  AttribDataClass::copy(inputAttribData, &adjustedInput);

  const NetworkControlDescriptor::ControlParameterDescriptor& cd = m_descriptor->getControlParamDescriptorByIndex(packetEntryID);

  // 
  compressDecompressData(cd, &adjustedInput);

  AttribDataClass* serialiserAttribData = (AttribDataClass*)m_controlParamData[packetEntryID];
  if (!AttribDataClass::compare(&adjustedInput, serialiserAttribData))
  { 
    m_controlParamDirty[packetEntryID] = true;
    AttribDataClass::copy(&adjustedInput, serialiserAttribData);
    m_network->setControlParameter(owningNodeID, serialiserAttribData);
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::setFromPacket(const void* packetMemory, uint32_t bitOffset, NetworkControlSerialiser** serialisers, uint32_t numSerialisers)
{
  for (uint32_t serialiserIndex = 0; serialiserIndex < numSerialisers; ++serialiserIndex)
  {
    bitOffset = serialisers[serialiserIndex]->setFromPacket(packetMemory, bitOffset);
  }
  return bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::setFromDeltaPacket(const void* packetMemory, uint32_t bitOffset, NetworkControlSerialiser** serialisers, uint32_t numSerialisers)
{
  for (uint32_t serialiserIndex = 0; serialiserIndex < numSerialisers; ++serialiserIndex)
  {
    bitOffset = serialisers[serialiserIndex]->setFromDeltaPacket(packetMemory, bitOffset);
  }
  return bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::createPacket(void* packetMemory, uint32_t bitOffset, NetworkControlSerialiser** serialisers, uint32_t numSerialisers)
{
  for (uint32_t serialiserIndex = 0; serialiserIndex < numSerialisers; ++serialiserIndex)
  {
    bitOffset = serialisers[serialiserIndex]->createPacket(packetMemory, bitOffset);
  }
  return bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::createDeltaPacket(void* packetMemory, uint32_t bitOffset, NetworkControlSerialiser** serialisers, uint32_t numSerialisers)
{
  for (uint32_t serialiserIndex = 0; serialiserIndex < numSerialisers; ++serialiserIndex)
  {
    bitOffset = serialisers[serialiserIndex]->createDeltaPacket(packetMemory, bitOffset);
  }
  return bitOffset;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::clampPrecision(uint32_t precision, uint32_t value)
{
  // If the mask 0xFFFFFFFF is shifted by 32 it produces 0xFFFFFFFF. We need it to produce 0x0 in order to get the behavior
  // required. Shifting a uint64_t with the mask 0xFFFFFFFFFFFFFFFF produces 0xFFFFFFFF00000000 which when casted to a
  // uint32_t will produce the correct behaviour.
  uint32_t mask = (uint32_t)(ULLONG_MAX << precision);
  uint32_t result = value & ~mask;
  return result;
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::clampPrecision(uint32_t precision, int32_t value)
{
  return clampPrecision(precision, (uint32_t)value);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& NMP_UNUSED(cd), AttribDataBool* NMP_UNUSED(value))
{
  // Bool control params don't get compressed at all
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& cd, AttribDataInt* value)
{
  uint32_t clampedValue = clampPrecision(cd.m_bitsOfPrecision, value->m_value);
  value->m_value = expandToInt(clampedValue, cd.m_bitsOfPrecision);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& cd, AttribDataUInt* value)
{
  value->m_value = clampPrecision(cd.m_bitsOfPrecision, value->m_value);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& cd, AttribDataFloat* value)
{
  if (cd.m_quantiseFloat)
  {
    uint32_t quantisedValue = quantiseFloat(cd, value->m_value);
    quantisedValue = clampPrecision(cd.m_bitsOfPrecision, quantisedValue);
    value->m_value = unquantiseToFloat(cd, quantisedValue);
  }
  else
  {
    NMP_ASSERT_MSG(cd.m_bitsOfPrecision == 32, "If reducing the precision of a float it must be quantised first.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& cd, AttribDataVector3* value)
{
  if (cd.m_quantiseFloat)
  {
    uint32_t quantisedValueX = quantiseFloat(cd, value->m_value.x);
    uint32_t quantisedValueY = quantiseFloat(cd, value->m_value.y);
    uint32_t quantisedValueZ = quantiseFloat(cd, value->m_value.z);
    quantisedValueX = clampPrecision(cd.m_bitsOfPrecision, quantisedValueX);
    quantisedValueY = clampPrecision(cd.m_bitsOfPrecision, quantisedValueY);
    quantisedValueZ = clampPrecision(cd.m_bitsOfPrecision, quantisedValueZ);
    value->m_value.x = unquantiseToFloat(cd, quantisedValueX);
    value->m_value.y = unquantiseToFloat(cd, quantisedValueY);
    value->m_value.z = unquantiseToFloat(cd, quantisedValueZ);
  }
  else
  {
    NMP_ASSERT_MSG(cd.m_bitsOfPrecision == 32, "If reducing the precision of a float it must be quantised first.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& cd, AttribDataVector4* value)
{
  if (cd.m_quantiseFloat)
  {
    uint32_t quantisedValueX = quantiseFloat(cd, value->m_value.x);
    uint32_t quantisedValueY = quantiseFloat(cd, value->m_value.y);
    uint32_t quantisedValueZ = quantiseFloat(cd, value->m_value.z);
    uint32_t quantisedValueW = quantiseFloat(cd, value->m_value.w);
    quantisedValueX = clampPrecision(cd.m_bitsOfPrecision, quantisedValueX);
    quantisedValueY = clampPrecision(cd.m_bitsOfPrecision, quantisedValueY);
    quantisedValueZ = clampPrecision(cd.m_bitsOfPrecision, quantisedValueZ);
    quantisedValueW = clampPrecision(cd.m_bitsOfPrecision, quantisedValueW);
    value->m_value.x = unquantiseToFloat(cd, quantisedValueX);
    value->m_value.y = unquantiseToFloat(cd, quantisedValueY);
    value->m_value.z = unquantiseToFloat(cd, quantisedValueZ);
    value->m_value.w = unquantiseToFloat(cd, quantisedValueW);
  }
  else
  {
    NMP_ASSERT_MSG(cd.m_bitsOfPrecision == 32, "If reducing the precision of a float it must be quantised first.");
  }
}

//----------------------------------------------------------------------------------------------------------------------
uint32_t NetworkControlSerialiser::broadcastMessage(const Message& message)
{
  NMP_ASSERT_MSG(m_descriptor->getMessageTargetNode(message.m_id) == INVALID_NODE_ID, "Message target node does not match"
    " the node stored in the descriptor. Packets built with this descriptor will send the message to the node stored"
    " in the descriptor.");
  NMP_ASSERT(message.m_type == MESSAGE_TYPE_REQUEST);
  m_messageState[message.m_id] = message.m_status;
  m_messageBroadcast[message.m_id] = true;
  return m_network->broadcastMessage(message);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::setMessageNotBroadcast(MessageID messageID)
{
  m_messageState[messageID] = false;
  m_messageBroadcast[messageID] = false;
}

//----------------------------------------------------------------------------------------------------------------------
bool NetworkControlSerialiser::sendMessage(NodeID targetNode, const Message& message)
{
  NMP_ASSERT_MSG(m_descriptor->getMessageTargetNode(message.m_id) == targetNode, "Message target node does not match"
    " the node stored in the descriptor. Packets built with this descriptor will send the message to the node stored"
    " in the descriptor.");
  return m_network->sendMessage(targetNode, message);
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::dirtyControlParam(NodeID owningNodeID)
{
  uint32_t controlParamIndex = m_descriptor->getControlParamIndexFromNodeID(owningNodeID);
  m_controlParamDirty[controlParamIndex] = true;
}

//----------------------------------------------------------------------------------------------------------------------
void NetworkControlSerialiser::dirtyAllControlParams()
{
  const uint32_t numControlParameterNodes = m_descriptor->getNetworkDef()->getNumControlParameterNodes();
  memset(m_controlParamDirty, 0xFFFFFFFF, sizeof(bool) * numControlParameterNodes);
}

//----------------------------------------------------------------------------------------------------------------------
}// namespace MR
