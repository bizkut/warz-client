// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NETWORK_CONTROL_SERIALISER_H
#define MR_NETWORK_CONTROL_SERIALISER_H
//----------------------------------------------------------------------------------------------------------------------
#include "mrNetwork.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MR
{

class NetworkControlSerialiser;

//----------------------------------------------------------------------------------------------------------------------
/// ControlParameterDescriptor float quantisation
///
/// Float quantisation is the process of mapping a range of floating point numbers to an unsigned int range so that its 
/// precision can be clamped. Care must be taken to make sure there is sufficient precision left to represent the values
/// required and that the quantise factor and offset is set up correctly. The following are examples of parameters that
/// produce common ranges:
///
/// quantisation equation: (val + m_quantiseOffset) * m_quantiseFactor
///
/// 5 bits of range 0.0f - 1.0f
/// m_bitsOfPrecision = 5
/// m_quantiseFactor = 31.0f (2^5 - 1 scales 1.0f to the maximum value of 5 bit uint)
/// m_quantiseOffset = 0.0f (The range starts at 0.0f so no offset is required)
///
/// 6 bits of range -1.0f - 1.0f
/// m_bitsOfPrecision = 6
/// m_quantiseFactor = 31.0f ((2^6 / 2) - 1. The divide by 2 represents the size of the range we wish to cover)
/// m_quantiseOffset = 1.0f (The range starts at -1.0f so an offset of 1 will shift the lowest possible value to 0.0f)
///
/// 7 bits of range 1.0f - 5.0f
/// m_bitsOfPrecision = 7
/// m_quatiseFactor = 31.0f ((2^7 / 4) - 1
/// m_quantiseOffset = -1.0f (The range starts at 1.0f so an offset of -1 will shift the lowest possible value to 0.0f)



//----------------------------------------------------------------------------------------------------------------------
/// \class MR::NetworkControlDescriptor
/// \brief Stores serialisation and compressions properties of control parameters and message IDs used by the
/// NetworkControlSerialiser. A NetworkControlDescriptor is tied to a specific network definition and may be used by
/// multiple NetworkControlSerialisers
class NetworkControlDescriptor
{
public:
  /// \brief Stores the compression settings for a single control parameter
  struct ControlParameterDescriptor
  {
    AttribDataSemantic  m_semantic;         /// The semantic used in the network node bin that stores this data.
    AttribDataType      m_type;             /// The morpheme attrib data type that this control parameter represents.
    uint32_t            m_bitsOfPrecision;  ///< The number of bits of precisions used when serialising this param.
                                            ///< Manipulating this value allows you to trade memory for precision.
    float               m_quantiseFactor;   ///< Used when quantising floats if m_quatiseFloat is true.
    float               m_quantiseOffset;   ///< Used when quantising floats if m_quatiseFloat is true.
    NodeID              m_nodeID;           ///< The network node ID of the control parameter
    bool                m_quantiseFloat;    ///< If true then all float values in the attrib data will go through the following
                                            ///< equation before being converted to a uint32_t:
                                            ///< result = (val + m_quantiseOffset) * m_quantiseFactor. The inverse is applied
                                            /// when the data is unquantised.
    bool                m_serialise;        ///< If false this control param will not be serialised into packets
  };

  /// \brief retrieves the memory requirements of a NetworkControlDescriptor instance
  static NM_INLINE NMP::Memory::Format getMemoryRequirements(NetworkDef* network);

  /// \brief creates a NetworkControlDescriptor. By default all control parameters and messages will be serialised.
  static NetworkControlDescriptor* createPacketDescriptor(NetworkDef* network);

  /// \brief retrieves a control parameter descriptor
  NM_INLINE ControlParameterDescriptor& getControlParamDescriptor(NodeID controlParamNodeID);
  NM_INLINE const ControlParameterDescriptor& getControlParamDescriptor(NodeID controlParamNodeID) const;

  /// \brief retrieves a control parameter descriptor
  NM_INLINE ControlParameterDescriptor& getControlParamDescriptorByIndex(uint32_t descriptorIndex);
  NM_INLINE const ControlParameterDescriptor& getControlParamDescriptorByIndex(uint32_t descriptorIndex) const;

  /// \brief Returns the number of control param descriptors
  NM_INLINE uint32_t getNumControlParamDescriptors() const;

  /// \brief Updates the packet memory requirements. If the precision, serialisation state of any messages or control 
  /// parameters changes then this must be called before a packet is created.
  void updatePacketMemoryRequirements();

  /// \brief Returns control param descriptor index associated with the control param node. INVALID_NODE_ID will be
  /// returned if the controlParamNodeID is not a control parameter.
  NM_INLINE uint32_t getControlParamIndexFromNodeID(NodeID controlParamNodeID) const;

  /// \brief Retrieves the network definition associated with this serialisation descriptor.
  NM_INLINE const NetworkDef* getNetworkDef() const;

  /// \brief Returns the number of bits required to store a network packet that is created using NetworkControlSerialiser::createPacket.
  /// To avoid wasting memory due to packing and alignment it is recommended to make a single packet for all networks 
  /// that share the same definition.
  NM_INLINE uint32_t getPacketMemoryBitRequirements(uint32_t numNetworks) const;

  /// \brief Returns the memory requirements of a packet.
  NM_INLINE const NMP::Memory::Format getPacketMemoryRequirements(uint32_t numNetworks) const;

  /// \brief Returns the serialisation state of the message ID.
  NM_INLINE bool serialiseMessageID(MessageID id) const;

  /// \brief Sets the serialisation sate of the messageID.
  NM_INLINE void setSerialiseMessageID(MessageID id, bool serialise);

  /// \brief Sets the target node that this message is sent to.
  NM_INLINE void setMessageTargetNode(MessageID id, NodeID targetNode);

  /// \brief Retrieves the target node that meessages with this id are sent to.
  NM_INLINE NodeID getMessageTargetNode(MessageID id) const;

#ifdef NMP_ENABLE_ASSERTS
  /// \brief Asserts that the packet memory requirements matches the requirements calculated from the control parameter
  /// description entries.
  void validatePacketMemoryRequirements() const;
#endif//NMP_ENABLE_ASSERTS

protected:
  NetworkControlDescriptor(); ///< use createPacketDescriptor or buildFullNetworkPacketDescriptor to instantiate a packet descriptor

  /// \brief Returns the number of bits required to store a standard packet
  NM_INLINE uint32_t calculatePacketMemoryRequirements() const;

  const NetworkDef*           m_networkDef;           ///< The network def this descriptor is bound too.
  uint32_t                    m_numBitsInPacket;      ///< The number of bits required to store a standard packet.
  ControlParameterDescriptor* m_entries;              ///< Array of control parameter descriptions. One for every control
                                                      ///< param in the network.
  uint32_t*                   m_nodeIDPacketEntryMap; ///< Maps a control param node id to its index in m_entries.
  bool*                       m_serialiseMessageIDs;  ///< Flag for each message id, if true it will be serialised into
                                                      ///< the input packet
  NodeID*                     m_messageTargetState;   ///< The target node that messages are sent too. A value of INVALID_NODE id
                                                      ///< will cause the message to be broadcasted
};

//----------------------------------------------------------------------------------------------------------------------
/// \class NetworkControlSerialiser
/// \brief Uses include network synchronisation across a connection where bandwidth is an issue, saving input to produce
/// replays. Quantisation and precision clamping occurs when a control parameter is set. This is to ensure that the network
/// behavior is consistent when control parameters are set directly from a packet. Packets can be created as either 
/// standard or delta packets. Standard packets will never vary in size, if you have specific bandwidth requirements that
/// must never be exceeded then use standard packets. Delta packets only store control parameters which have changed. 
/// A delta packet will at maximum be 1 bit larger than a standard packet per network but usually be smaller.
/// Types supported: Broadcast messages. Bool, Int, UInt, Vector3 and Vector4 control parameters.
/// 
/// See the NetworkControlSerialiser tutorial for example code
class NetworkControlSerialiser
{
public:
  /// \brief Calculates the memory requirements of a NetworkControlSerialiser.
  static NMP::Memory::Format getMemoryRequirements(const NetworkControlDescriptor& packetDescriptor);

  /// \brief Creates a NetworkControlSerialiser using the default allocator in NMP.
  static NetworkControlSerialiser* createNetworkControlSerialiser(Network* network, const NetworkControlDescriptor& packetDescriptor);

  /// \brief Sets the network control parameter. If the control param descriptor has quantisation enabled or a precision
  /// that differs from the default that the value applied to the network may differ slightly to that provided in the
  /// attribDataClass
  template <typename _AttribDataClass>
  NM_INLINE void setControlParam(NodeID owningNodeID, const _AttribDataClass* attribDataClass);

  /// \brief Broadcasts a message to the network. The message target node in the descriptor must be set to 
  /// INVALID_NODE_ID for the relevant message ID.
  NM_INLINE uint32_t broadcastMessage(const Message& message);

  /// \brief Reset the internal message broadcast data for the serialiser if we do not broadcast a message.
  NM_INLINE void setMessageNotBroadcast(MessageID messageID);

  /// \brief Sends a message to the network. The target node in the descriptor must be equal to the targetNode argument
  /// for the relevant message ID.
  NM_INLINE bool sendMessage(
    NodeID targetNode,      ///< Id of node to receive the message
    const Message& message  ///< Message data send to the node.
    );

  /// \brief Marks the control parameter as dirty so that it will be serialised in the next delta packet
  NM_INLINE void dirtyControlParam(NodeID owningNodeID);

  /// \brief Marks the control parameter as dirty so that it will be serialised in the next delta packet
  NM_INLINE void dirtyAllControlParams();

  /// \brief Deserialises the packet and applies all control parameter and messages to the network. setFromPacket must
  /// be used in conjunction with createPacket.
  uint32_t setFromPacket(const void* packet, uint32_t bitOffset);

  /// \brief Deserialises the packet and applies all control parameter and messages to the network. setFromDeltaPacket must
  /// be used in conjunction with createDeltaPacket.
  uint32_t setFromDeltaPacket(const void* packet, uint32_t bitOffset);

  /// \brief Creates a packet containing the value of all control parameters and the state of all message ids applied
  /// to the network since the last packet was created.
  uint32_t createPacket(void* packetMemory, uint32_t bitOffset) const;

  /// \brief Creates a a delta packet containing the value of all control parameters that have changed and the state of 
  /// all message ids applied to the network since the last packet was created. The maximum space used by a delta packet
  /// is NetworkControlDescriptor::getBitMemoryRequirements() + 1. Assuming control parameters don't always change delta
  /// packets will be smaller on average.
  uint32_t createDeltaPacket(void* packetMemory, uint32_t bitOffset) const;

  /// \brief Calculates the bit requirements of a delta packet given the current dirty state of control parameters. This
  /// value may change after any call to setControlParam.
  uint32_t calculateDeltaPacketBitRequirements() const;

  /// \brief Creation and setters of packets for arrays of serialisers. Ensures no padding occurs between network packets
  /// giving optimal memory usage.
  static NM_INLINE uint32_t setFromPacket(const void* packetMemory, uint32_t bitOffset, NetworkControlSerialiser** serialisers, uint32_t numSerialisers);
  static NM_INLINE uint32_t setFromDeltaPacket(const void* packetMemory, uint32_t bitOffset, NetworkControlSerialiser** serialisers, uint32_t numSerialisers);
  static NM_INLINE uint32_t createPacket(void* packetMemory, uint32_t bitOffset, NetworkControlSerialiser** serialisers, uint32_t numSerialisers);
  static NM_INLINE uint32_t createDeltaPacket(void* packetMemory, uint32_t bitOffset, NetworkControlSerialiser** serialisers, uint32_t numSerialisers);

protected:
  /// \brief Use createNetworkControlSerialiser instead
  NetworkControlSerialiser() {}

  /// \brief Expands a variable precision int stored in a uint32_t to a negative int if the sign bit is set
  static NM_INLINE int32_t expandToInt(
    uint32_t val,         ///< The value to expand
    uint32_t intPrecision ///< The precision of the integer. This is used to locate to the sign bit
    );

  /// \brief Reads length number of bits into a 32 bit natural type.
  static NM_INLINE uint32_t readUInt32(
    const char* data,       ///< The source buffer
    uint32_t& offsetInBits, ///< The offset from data to begin reading. This value is progressed by the number of bits read.
    uint32_t length         ///< The number of bits to read
    );

  /// \brief Reads length number of bits into a 32 bit natural type.
  static NM_INLINE int32_t readInt32(
    const char* data,       ///< The source buffer
    uint32_t& offsetInBits, ///< The offset from data to begin reading. This value is progressed by the number of bits read.
    uint32_t length         ///< The number of bits to read
    );

  /// \brief Reads a single bit as a bool.
  static NM_INLINE bool readBool(const char* data, uint32_t& offsetInBits);

  /// \brief Writes a 32 bit natural type into the destination buffer.
  static NM_INLINE void writeUInt32(
    char* dest,           ///< The destination buffer.
    uint32_t& bitOffset,  ///< The bit offset from dest to begin writing. bitsToWrite is progressed by the number of bits written
    uint32_t dataToWrite, ///< The source data to write
    uint32_t bitsToWrite  ///< The number of bits to write to the buffer
    );

  /// \brief Writes a 32 bit natural type into the destination buffer.
  static NM_INLINE void writeInt32(
    char* dest,           ///< The destination buffer.
    uint32_t& bitOffset,  ///< The bit offset from dest to begin writing. bitsToWrite is progressed by the number of bits written
    int32_t dataToWrite,        ///< The source data to write
    uint32_t bitsToWrite  ///< The number of bits to write to the buffer
    );

  /// \brief Writes a boolean value a single bit
  static NM_INLINE void writeBool(char* dest, uint32_t& bitOffset, bool value);

  /// \brief Writes the attrib data stored in m_controlParamData[packedEntryIndex] to the data buffer
  void writePacketEntry(
    const NetworkControlDescriptor::ControlParameterDescriptor& pd,
    AttribData* attribData,
    char* dataBuffer,
    uint32_t& bitOffset) const;

  /// \brief Reads the attrib data stored in m_controlParamData[packedEntryIndex] to the data buffer
  void readPacketEntry(
    const NetworkControlDescriptor::ControlParameterDescriptor& pd,
    AttribData* attribData,
    const char* dataBuffer,
    uint32_t& bitOffset);

  /// \brief Creates the attrib data described by the descriptor
  template <typename _AttribDataClass>
  static NM_INLINE _AttribDataClass* createAttribData(
    NMP::Memory::Resource& resource,
    const NetworkControlDescriptor::ControlParameterDescriptor& describtor,
    const NetworkDef* networkDef);

  static NM_INLINE uint32_t quantiseFloat(const NetworkControlDescriptor::ControlParameterDescriptor& pd, float value);
  static NM_INLINE float unquantiseToFloat(const NetworkControlDescriptor::ControlParameterDescriptor& pd, uint32_t value);

  /// \brief Clamps the precision of the value by discarding the (32 - precision) most significant bits.
  static NM_INLINE uint32_t clampPrecision(uint32_t precision, uint32_t value);
  static NM_INLINE uint32_t clampPrecision(uint32_t precision, int32_t value);

  /// \brief Compresses the attrib data then decompresses it. Once an attrib data has gone through this function its value
  /// is the same as it originated from a packet
  static NM_INLINE void compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& pd, AttribDataBool* value);
  static NM_INLINE void compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& pd, AttribDataInt* value);
  static NM_INLINE void compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& pd, AttribDataUInt* value);
  static NM_INLINE void compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& pd, AttribDataFloat* value);
  static NM_INLINE void compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& pd, AttribDataVector3* value);
  static NM_INLINE void compressDecompressData(const NetworkControlDescriptor::ControlParameterDescriptor& pd, AttribDataVector4* value);

  Network*                        m_network; ///< The network that all control parameter and messages will be applied to.
  const NetworkControlDescriptor* m_descriptor; ///< The descriptor containing compression properties of the network inputs.
  AttribData**                    m_controlParamData; ///< Local copy of all control parameters.
  bool*                           m_controlParamDirty; ///< Flag for each control parameter which is set when the value
                                                       ///< changes. Delta packets are generated based on these flags.
  bool*                           m_messageState; ///< The message state of the last broadcasted message stored by messageID
  bool*                           m_messageBroadcast; ///< Flag for each messageID set if a broadcast message has been sent
                                                      ///< since the last packet was created.
};

}// namespace MR

#include "mrNetworkControlSerialiser.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NETWORK_CONTROL_SERIALISER_H
//----------------------------------------------------------------------------------------------------------------------
