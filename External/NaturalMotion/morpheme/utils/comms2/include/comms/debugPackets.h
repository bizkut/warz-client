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
#ifndef MCOMMS_DEBUGPACKETS_H
#define MCOMMS_DEBUGPACKETS_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBuffer.h"

#include "sharedDefines/mCoreDebugInterface.h"

#include "comms/debugDrawObjects.h"
#include "comms/mcomms.h"
#include "comms/networkDataBuffer.h"
#include "comms/packet.h"
//----------------------------------------------------------------------------------------------------------------------

// Suppress warning coming from xstring in PS3 SDK.
#ifdef NM_COMPILER_SNC
  #pragma diag_push
  #pragma diag_suppress=1669
#endif

#ifdef NM_COMPILER_SNC
  #pragma diag_pop
#endif

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

#pragma pack(push, 4)

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct PersistentDataPacket : public PacketBase
{
  // This constructor shouldn't be normally used, but it's useful
  // to partially read the packet, retrieve the actual size and then read the data.
  inline PersistentDataPacket();

  /// \brief Create a packet of persistent global data
  static inline PersistentDataPacket* create(
    NetworkDataBuffer*     buffer,
    int32_t                outputDataType,
    uint32_t               dataLen);

  /// \brief Create a packet of persistent def data
  static inline PersistentDataPacket* create(
    NetworkDataBuffer*     buffer,
    const GUID&            networkGUID,
    int32_t                outputDataType,
    uint32_t               dataLen);

  /// \brief Create a packet of persistent instance data
  static inline PersistentDataPacket* create(
    NetworkDataBuffer*     buffer,
    const InstanceID&      instanceID,
    int32_t                outputDataType,
    uint32_t               dataLen);

  /// \note the data for this packet must be deserialized separately.
  inline void deserialize();
  /// \note the data for this packet must be serialized separately.
  inline void serialize();

  enum PersistentDataType
  {
    kGlobalData,    ///< persistent data is global ie scene objects, m_networkGUID and m_instanceID will be invalid
    kDefData,       ///< persistent data is specific to one network def, m_networkGUID will be valid
    kInstanceData   ///< persistent data is specific to one network instance, m_instanceID will be valid
  };
  PersistentDataType m_persistentDataType;

  GUID        m_networkGUID;  ///< only valid when m_persistentDataType is set to kDefData
  InstanceID  m_instanceID;   ///< only valid when m_persistentDataType is set to kInstanceData

  int32_t     m_outputDataType; ///< Type of data this packet holds
  uint32_t    m_dataLength;     ///< Size of data this packet holds in bytes.

  /// \brief Get packet's data, use in conjunction with m_outputDataType to cast to the correct type.
  inline void *getData();
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
struct FrameDataPacket : public PacketBase
{
  // This constructor shouldn't be normally used, but it's useful
  // to partially read the packet, retrieve the actual size and then read the data.
  inline FrameDataPacket();

  /// \brief Create a packet of persistent global data
  static inline FrameDataPacket* create(
    NetworkDataBuffer*     buffer,
    InstanceID             instanceID,
    int32_t                outputDataType,
    uint32_t               dataLen);

  /// \note the data for this packet must be deserialized separately.
  inline void deserialize();
  /// \note the data for this packet must be serialized separately.
  inline void serialize();

  enum FrameDataType
  {
    kGlobalData,    ///< persistent data is global ie scene objects, m_instanceID will be invalid
    kInstanceData   ///< persistent data is specific to one network instance, m_instanceID will be valid
  };
  FrameDataType m_frameDataType;

  InstanceID  m_instanceID;   ///< only valid when m_frameDataType is set to kInstanceData

  int32_t     m_dataType;     ///< Type of data this packet holds
  uint32_t    m_dataLength;   ///< Size of data this packet holds in bytes.

  /// \brief Get packet's data, use in conjunction with m_outputDataType to cast to the correct type.
  inline void *getData();
};

//----------------------------------------------------------------------------------------------------------------------
struct NodeOutputDataPacket : public PacketBase
{
  // This constructor shouldn't be normally used, but it's useful
  // to partially read the packet, retrieve the actual size and then read the data.
  inline NodeOutputDataPacket();

  // Requires data needed to specify a full AttribAddress.
  static inline NodeOutputDataPacket* create(
    NetworkDataBuffer*     buffer,
    uint32_t               dataLen,
    commsNodeID            owningNodeID,
    commsNodeID            targetNodeID,
    MR::AttribDataType     type,
    MR::AttribDataSemantic semantic,
    StringToken            semanticName,
    MR::AnimSetIndex       animSetIndex,
    MR::FrameCount         validFrame,
    MR::PinIndex           pinIndex = MR::INVALID_PIN_INDEX);

  void deserialize();
  inline void serialize();

  // The following data forms an MR::AttribAddress. Uniquely identifies any piece of AttribData on a Network or NetworkDef.
  commsNodeID             m_owningNodeID;       // Which node this AttribData belongs to.
  commsNodeID             m_targetNodeID;       // Indicates that this data has been created for use by a specific node (Usually the parent or a child).
  //  Can be INVALID_NODE_ID if there is no specific target.
  MR::AttribDataType      m_type;               // Identifies the content and structure of the proceeding data block.
  MR::AttribDataSemantic  m_semantic;           // Gives context to the type. e.g. ATTRIB_TYPE_VECTOR3 could have semantic ATTRIB_SEMANTIC_TARGET or ATTRIB_SEMANTIC_POSITION etc.
  StringToken             m_semanticNameToken;
  MR::AnimSetIndex        m_animSetIndex;       // Allows addressing of animation set specific attribute data.
  //  Bits of AttribData with different AnimSetIndexes will be created when the networks ActiveAnimSet changes.
  MR::FrameCount          m_validFrame;         // The frame index for which this data is valid. Usually the frame it was created or VALID_FOREVER.
  MR::PinIndex            m_pinIndex;           // The pin index that this data was attached to, if any or INVALID_PIN_INDEX

  uint32_t                m_nodeOutputType;     // NODE_OUTPUT_DATA_TYPE_... from RuntimeNodeInspector::convertAttribSemanticToNodeOutputDataType(
  // TODO: Stop using this, and start using MR::AttribDataType, MR::AttribDataSemantic etc
  //       to determine the use and meaning of one of these packets of AttribData.

  uint32_t                m_dataLength;         // Size of the proceeding attribute data in bytes.

  inline void* getData();
};

//----------------------------------------------------------------------------------------------------------------------
// Array of profile timings for elements of node processing from a frame of Network update
// (updateConnections, task queuing, task evaluation).
struct FrameNodeTimingsPacket : public PacketBase
{
  static inline FrameNodeTimingsPacket* create(
    NetworkDataBuffer*           buffer,
    InstanceID                   networkInstanceID,
    uint32_t                     numNodeTimings,
    const MR::NodeTimingElement* nodeTimingElements);

  inline void deserialize();
  inline void serialize();

  InstanceID m_networkInstanceID;
  uint32_t   m_numNodeTimings;

  inline MR::NodeTimingElement* getNodeTimingElements();
};

//----------------------------------------------------------------------------------------------------------------------
struct ProfilePointTimingPacket : public PacketBase
{
  inline ProfilePointTimingPacket();

  inline void deserialize();
  inline void serialize();

  StringToken   m_name;
  float         m_timing;
  InstanceID    m_networkInstanceID;
};

//----------------------------------------------------------------------------------------------------------------------
struct MorphemeScratchpadPacket : public PacketBase
{
  inline MorphemeScratchpadPacket(InstanceID networkId, ValueUnion::Type paramType);

  inline void deserialize();
  inline void serialize();

  ValueUnion  m_data;
  StringToken m_tag;
  uint16_t    m_sourceLineNumber;  ///< Code line number that originated the scratch pad request.
  InstanceID  m_sourceNetworkID;
  commsNodeID m_sourceNodeID;
  StringToken m_sourceModuleName;
  uint32_t    m_sourceFrame;       ///< The frame that this data belongs to. 
};

//----------------------------------------------------------------------------------------------------------------------
struct DebugDrawPacket : public PacketBase
{
  static inline DebugDrawPacket* create(
    uint8_t*           buffer,            // Where the packet is to be created, must be >= (sizeof(DebugDrawPacket) + dataLength)
    DebugDrawTypes     debugDrawType,     // Line, circle, torque arc etc.
    InstanceID         sourceNetworkID,   // ID of the source network instance.
    commsNodeID        sourceNodeID,      // Which node has initiated the draw command. INVALID_NODE_ID if its not from a specific node.
    StringToken        sourceTagName,     // NULL if not from a specific module.
    MR::FrameCount     sourceFrame,       // What frame this data is from. Useful when attempting to buffer data in frame sequence.
    MR::LimbIndex      sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    uint32_t           dataLength);       // Size of packet data following the base DebugDrawPacket structure.

  static inline DebugDrawPacket* create(
    NetworkDataBuffer* buffer,            // Where the packet is to be created.
    DebugDrawTypes     debugDrawType,     // Line, circle, torque arc etc.
    InstanceID         sourceNetworkID,   // ID of the source network instance.
    commsNodeID        sourceNodeID,      // Which node has initiated the draw command. INVALID_NODE_ID if its not from a specific node.
    StringToken        sourceTagName,     // NULL if not from a specific module.
    MR::FrameCount     sourceFrame,       // What frame this data is from. Useful when attempting to buffer data in frame sequence.
    MR::LimbIndex      sourceLimbIndex,   // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.
    uint32_t           dataLength);       // Size of packet data following the base DebugDrawPacket structure.

  inline void serialize();
  void deserialize();

  // The following block of data allows for filtering of debug draw commands.
  DebugDrawTypes m_debugDrawType;      // Identifies the content and structure of the proceeding data block.
  InstanceID     m_sourceNetworkID;    // ID of the source network instance.
  commsNodeID    m_sourceNodeID;       // Which node has initiated the draw command. INVALID_NODE_ID if its not from a specific node.
  StringToken    m_sourceModuleName;   // MCOMMS_INVALID_STRINGTOKEN if not from a specific module.
  uint32_t       m_sourceFrame;        // The frame that this data belongs to. 
  MR::LimbIndex  m_sourceLimbIndex;    // The limb index. INVALID_LIMB_INDEX if not for any limb and in all morpheme data.

  uint32_t       m_dataLength;         // Size of the proceeding attribute data in bytes.
  inline void* getData();
  inline const void* getData() const;
};

#pragma pack(pop)

} // namespace MCOMMS

#include "debugPackets.inl"

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_DEBUGPACKETS_H
//----------------------------------------------------------------------------------------------------------------------
