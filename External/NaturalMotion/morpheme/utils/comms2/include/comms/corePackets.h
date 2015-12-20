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
#ifndef MCOMMS_COREPACKETS_H
#define MCOMMS_COREPACKETS_H
//----------------------------------------------------------------------------------------------------------------------

#include "comms/mcomms.h"
#include "comms/packet.h"
#include "comms/attribute.h"
#include "comms/networkDataBuffer.h"
#include "NMPlatform/NMMathUtils.h"
#include "NMPlatform/NMMemory.h"
#include "NMPlatform/NMBuffer.h"

// Suppress warning coming from xstring in PS3 SDK.
#ifdef NM_COMPILER_SNC
  #pragma diag_push
  #pragma diag_suppress=1669
#endif

#include <string.h>

#ifdef NM_COMPILER_SNC
  #pragma diag_pop
#endif

#include "../../../SDK/sharedDefines/mSharedDefines.h"
#include "../../../SDK/sharedDefines/mAnimDebugInterface.h"
#include "../../../SDK/sharedDefines/mCoreDebugInterface.h"
#include "../../../SDK/sharedDefines/mPhysicsDebugInterface.h"

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

#pragma pack(push, 4)

//----------------------------------------------------------------------------------------------------------------------
// Some commands require a reply.
struct ReplyPacket : public PacketBase
{
  enum ResultValue
  {
    kResultSuccess = 0,
    kResultFailure = 0xFFFFFFFF,
  };

  inline ReplyPacket(uint32_t requestId, uint16_t requestedCmd);

  inline void serialize();
  inline void deserialize();

  uint32_t    m_requestId;
  ResultValue m_result;
  uint16_t    m_requestedCmd; ///< Holds the original request type.

  uint8_t m_pad[2];
};

//----------------------------------------------------------------------------------------------------------------------
struct IdentificationCmdPacket : public CmdPacketBase
{
  inline IdentificationCmdPacket();

  uint8_t    m_connectProtocolVersion;
};

//----------------------------------------------------------------------------------------------------------------------
/// game->app 'handshake' packet sent to configure the application.
/// Its arrival indicates that we have a working connection and the game
/// is aware of our presence.
/// The packet also describes the capabilities of the runtime target.
/// Note that the capabilities are fully supported only by the master connection (eg: the one that can step the
/// network(s). A connection passes the capabilities back even if not master. The behaviour might although be
/// affected by not being a master connection.
struct IdentificationReplyPacket : public ReplyPacket
{
  // Play control mode.
  enum
  {
    PLAYBACKCTRL_NO_SUPPORT         = 0,     // No playback control support means: asynchronous stepping only,
                                             // with no possibility of pausing the network.
    PLAYBACKCTRL_SYNCH_STEPPING     = 1,     // This flag adds the ability to work in synch with connect.
                                             // Once connected, the runtime waits for a step command on the master connection
                                             // to update the network(s).
    PLAYBACKCTRL_PAUSE              = 2,
    PLAYBACKCTRL_VARIABLE_TIME_STEP = 4,     // RTT uses the step value from connect to update the network(s). In asynchronous mode
                                             // the value used is the last one that was sent from connect.
    PLAYBACKCTRL_FULL_SUPPORT       = 0xFF
  };

  // Scene objects support.
  enum
  {
    SCENEOBJECTS_NO_SUPPORT      = 0,     // No support for scene objects at all.
    SCENEOBJECTS_READ_OBJECTS    = 1,     // Can just read the available objects and their attributes.
    SCENEOBJECTS_CREATE_DESTROY  = 2,     // Can create and destroy objects.
    SCENEOBJECTS_EDIT_ATTRIBUTES = 4,     // Can edit scene objects' attributes. Note that this doesn't need the create/destroy
                                          // flag to be set. A RTT could be able to set attributes but not create objects.
    SCENEOBJECTS_FULL_SUPPORT    = 0xFF
  };

  // Physics Manipulation Support
  enum
  {
    PHYSICSMANIP_NO_SUPPORT                 = 0,     // No support for manipulating physics objects
    PHYSICSMANIP_CREATE_EDIT_CONSTRAINTS    = 1,     // Can create and edit constraints to apply mouse forces and pin objects in the scene
    PHYSICSMANIP_APPLY_FORCES               = 2,      // Can apply forces to physics objects
    PHYSICSMANIP_FULL_SUPPORT               = 0xFF
  };

  // Animation Browser Support
  enum
  {
    ANIMBROWSER_NO_SUPPORT       = 0,
    ANIMBROWSER_LOAD_ANIM_SOURCE = 1,    // Ability to load an animation source on the target.
    ANIMBROWSER_SET_ANIM_TIME    = 2,    // Ability to set the time on the current animation.
    ANIMBROWSER_FULL_SUPPORT     = 0xFF
  };

  enum
  {
    NETWORKMGMT_NO_SUPPORT               = 0,
    NETWORKMGMT_LOAD_UNLOAD_DEFINITION   = 1,     //
    NETWORKMGMT_CREATE_DESTROY_INSTANCE  = 2,     //
    NETWORKMGMT_CONTROL_PARAMETERS       = 4,     //
    NETWORKMGMT_STATE_MACHINE_REQUESTS   = 8,     //
    NETWORKMGMT_SET_NETWORK_ANIMSET      = 16,
    NETWORKMGMT_FULL_SUPPORT             = 0xFF
  };

  // Platform support
  enum
  {
    PLATFORM_USES_BIG_ENDIAN = 1,
  };

  inline IdentificationReplyPacket(uint32_t requestId);

  inline void deserialize();
  inline void serialize();

  static const uint32_t kRttIDLen = 4;

  char       m_rttID[kRttIDLen];           /// < Runtime target identifier.

  static const uint32_t kPhysicEngineIDLen = 32;

  char       m_physicsEngineID[kPhysicEngineIDLen];

  uint8_t    m_protocolVersion;    /// < Version code for making sure connect network code matches up with comms'.

  uint8_t    m_maxNetworkDefs;     /// < Max number of network definitions.
  uint16_t   m_maxInstances;       /// < Max number of network instances.

  uint16_t   m_flags;              /// < Reserved for future use

  struct SimpleFlags
  {
    SimpleFlags& operator=(const SimpleFlags& rhs)
    {
      m_data = rhs.m_data;
      return *this;
    }

    SimpleFlags& operator=(uint8_t data)
    {
      m_data = data;
      return *this;
    }

    bool isFlagEnabled(uint8_t flag)
    {
      return (m_data & flag) != 0;
    }

    void setFlag(uint8_t flag)
    {
      m_data |= flag;
    }

    void clearFlag(uint8_t flag)
    {
      m_data &= ~flag;
    }

    uint8_t m_data;
  };

  SimpleFlags m_sceneObjectsSupportFlags;
  SimpleFlags m_networkManagementSupportFlags;
  SimpleFlags m_physicsManipulationSupportFlags;
  SimpleFlags m_animBrowserSupportFlags;
  SimpleFlags m_playbackControlSupportFlags;
  SimpleFlags m_platformCapabilitiesFlags;
};

//----------------------------------------------------------------------------------------------------------------------
/// Marker packet sent before the step/update.
struct BeginFramePacket : public PacketBase
{
  inline BeginFramePacket(float timeStep);

  inline void deserialize();
  inline void serialize();

  float        m_timeStep;
  uint32_t     m_frameId;
  uint32_t     m_numNetworkDefs;
  uint32_t     m_numNetworkInstances;
};

//----------------------------------------------------------------------------------------------------------------------
/// Marker packet sent after the step/update.
struct EndFramePacket : public PacketBase
{
  inline EndFramePacket();

  inline void deserialize();
  inline void serialize();

  uint32_t  m_stepRequestId;
};

//----------------------------------------------------------------------------------------------------------------------
struct BeginFrameSegmentPacket : public PacketBase
{
  inline BeginFrameSegmentPacket(uint8_t segmentId);
  inline void serialize();
  inline void deserialize();

  uint8_t  m_segmentId;
  uint32_t m_segmentPayload;
};

//----------------------------------------------------------------------------------------------------------------------
struct EndFrameSegmentPacket : public PacketBase
{
  inline EndFrameSegmentPacket(uint8_t segmentId);
  inline void serialize();
  inline void deserialize();

  uint8_t m_segmentId;
};

//----------------------------------------------------------------------------------------------------------------------
struct BeginInstanceDataPacket : public PacketBase
{
  inline BeginInstanceDataPacket(InstanceID instanceId);
  inline void serialize();
  inline void deserialize();

  InstanceID m_instanceId;
  uint32_t   m_instanceDataPayload;
};

//----------------------------------------------------------------------------------------------------------------------
struct EndInstanceDataPacket : public PacketBase
{
  inline EndInstanceDataPacket(InstanceID instanceId);
  inline void serialize();
  inline void deserialize();

  InstanceID m_instanceId;
};

//----------------------------------------------------------------------------------------------------------------------
struct BeginInstanceSectionPacket : public PacketBase
{
  inline BeginInstanceSectionPacket(uint8_t sectionId);
  inline void serialize();
  inline void deserialize();

  uint8_t  m_sectionId;
  uint32_t m_sectionPayload;
};

//----------------------------------------------------------------------------------------------------------------------
struct EndInstanceSectionPacket : public PacketBase
{
  inline EndInstanceSectionPacket(uint8_t sectionId);
  inline void serialize();
  inline void deserialize();

  uint8_t m_sectionId;
};

//----------------------------------------------------------------------------------------------------------------------
/// sent when a new string has been cached on the runtime host
struct AddStringToCachePacket : public PacketBase
{
  inline AddStringToCachePacket(const char* string, StringToken token);

  inline void deserialize();
  inline void serialize();

  StringToken m_token;

  static const size_t   maxStrLen = 2048;
  char        m_string[maxStrLen];
};

//----------------------------------------------------------------------------------------------------------------------
struct PingCmdPacket : public CmdPacketBase
{
  inline PingCmdPacket(uint32_t it);

  inline void serialize();
  inline void deserialize();

  uint32_t m_id;
};

//----------------------------------------------------------------------------------------------------------------------
struct PingDataPacket : public PacketBase
{
  inline PingDataPacket(uint32_t id);

  inline void serialize();
  inline void deserialize();

  uint32_t m_id;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct SetControlParameterCmdPacket : public CmdPacketBase
{
  inline SetControlParameterCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID              m_instanceId;
  commsNodeID             m_controlParamId;
  commsNodeOutputDataType m_type;
  ValueUnion              m_controlParamValue;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct SetRootTransformCmdPacket : public CmdPacketBase
{
  inline SetRootTransformCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID    m_instanceId;
  NMP::PosQuat  m_transform;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct SetAnimationSetCmdPacket : public CmdPacketBase
{
  inline SetAnimationSetCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID    m_instanceId;
  uint32_t      m_animSetIndex;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct SetAssetManagerAnimationSetCmdPacket : public CmdPacketBase
{
  inline SetAssetManagerAnimationSetCmdPacket();

  inline void deserialize();
  inline void serialize();

  uint32_t      m_animSetIndex;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct SendRequestCmdPacket : public CmdPacketBase
{
  inline SendRequestCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID        m_instanceId;
  NetworkMessageID  m_networkRequestID;
  commsNodeID       m_nodeId;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct BroadcastRequestCmdPacket : public CmdPacketBase
{
  inline BroadcastRequestCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID        m_instanceId;
  NetworkMessageID  m_networkRequestID;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct SetCurrentStateCmdPacket : public CmdPacketBase
{
  inline SetCurrentStateCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID    m_instanceId;
  commsNodeID   m_stateMachineID;
  commsNodeID   m_rootStateID;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct ExecuteCommandCmdPacket : public CmdPacketBase
{
  inline ExecuteCommandCmdPacket();
  static inline ExecuteCommandCmdPacket* create(InstanceID instanceID, const char* command);

  inline void deserialize();
  inline void serialize();
  inline char* getCommand() const;

  InstanceID   m_instanceId;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct EnableOutputDataCmdPacket : public CmdPacketBase
{
  inline EnableOutputDataCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID              m_instanceID;
  commsNodeID             m_nodeID;
  commsNodeOutputDataType m_outputDataType;
  uint32_t                m_enable;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct SetDebugOutputFlagsCmdPacket : public CmdPacketBase
{
  inline SetDebugOutputFlagsCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID m_instanceID;
  uint32_t   m_debugOutputFlags;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
/// Sets
//----------------------------------------------------------------------------------------------------------------------
struct SetDebugOutputOnNodesCmdPacket : public CmdPacketBase
{
  // All possible semantics will be transmitted from all active nodes.
  static inline SetDebugOutputOnNodesCmdPacket* createAllOn(NetworkDataBuffer* buffer);

  // All node attribute transmission will be turned off. (May be better to do this via SetDebugOutputFlagsCmdPacket so that per node flag state is maintained?)
  static inline SetDebugOutputOnNodesCmdPacket* createAllOff(NetworkDataBuffer* buffer);

  // Transmission of all semantics on a set of nodes will be turned on or off.
  // The state of node semantics outside of this groups will be unaffected.
  static inline SetDebugOutputOnNodesCmdPacket* createSomeNodesAllSemantics(
    NetworkDataBuffer* buffer,
    bool               on,     // Or off.
    uint32_t           numNodes);

  // Transmission of a set of semantics on all nodes will be turned on or off.
  // The state of node semantics outside of this groups will be unaffected.
  static inline SetDebugOutputOnNodesCmdPacket* createAllNodesSomeSemantics(
    NetworkDataBuffer* buffer,
    bool               on,     // Or off.
    uint32_t           numSemantics);

  // Transmission of a set of semantics on a set of nodes will be turned on or off.
  // The state of node semantics outside of this groups will be unaffected.
  static inline SetDebugOutputOnNodesCmdPacket* createSomeNodesSomeSemantics(
    NetworkDataBuffer* buffer,
    bool               on,     // Or off.
    uint32_t           numNodes,
    uint32_t           numSemantics);

  // Memory required to store an instance with the specified requirements.
  static inline uint32_t getRequiredMemorySize(
    uint32_t numNodes = 0,
    uint32_t numSemantics = 0);

  inline void deserialize();
  inline void serialize();
  inline uint16_t* getNodeIDs();
  inline uint16_t* getSemantics();

  InstanceID m_instanceID;
  bool       m_on;           // Turn output on or off.
  bool       m_allNodes;     // Affect all or a set of nodes.
  uint32_t   m_numNodeIDs;   // Num proceeding NodeIDs (array).
  bool       m_allSemantics; // Affects all or a set of semantics.
  uint32_t   m_numSemantics; // Num proceeding Semantics (array).
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct CreateNetworkInstanceCmdPacket : public CmdPacketBase
{
  static inline CreateNetworkInstanceCmdPacket* create(
    NetworkDataBuffer* buffer,
    const char*        instanceName);

  // Memory required to store an instance with the specified requirements.
  static inline uint32_t getRequiredMemorySize(
    const char* instanceName);

  inline void deserialize();
  inline void serialize();

  inline const char* getNetworkInstanceName() const;

  GUID          m_networkGUID;
  uint32_t      m_initialAnimSetIndex;
  Vec3          m_characterStartPosition;
  Vec4          m_characterStartRotation;

private:
  inline CreateNetworkInstanceCmdPacket();

  inline void setNetworkInstanceName(const char* instanceName);
  inline char* getNetworkInstanceName();
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct ReferenceNetworkInstanceCmdPacket : public CmdPacketBase
{
  inline ReferenceNetworkInstanceCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID    m_instanceId;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct ReferenceNetworkInstanceReplyPacket : public ReplyPacket
{
  inline ReferenceNetworkInstanceReplyPacket(uint32_t requestId);

  inline void deserialize();
  inline void serialize();

  InstanceID    m_instanceId;
  StringToken   m_nameToken;
  GUID          m_guid;
  uint32_t      m_initialAnimSetIndex;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct UnreferenceNetworkInstanceCmdPacket : public CmdPacketBase
{
  inline UnreferenceNetworkInstanceCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID    m_instanceId;
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkDefinitionDestroyedCtrlPacket : public PacketBase
{
  inline NetworkDefinitionDestroyedCtrlPacket();

  inline void deserialize();
  inline void serialize();

  GUID m_guid;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct LoadNetworkCmdPacket : public CmdPacketBase
{
  inline LoadNetworkCmdPacket();

  inline void deserialize();
  inline void serialize();

  GUID  m_networkGUID;
  static const uint32_t kMaxNetworkNameLength = 128;
  char  m_networkName[kMaxNetworkNameLength];
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct DestroyNetworkDefinitionCmdPacket : public CmdPacketBase
{
  inline DestroyNetworkDefinitionCmdPacket();

  inline void deserialize();
  inline void serialize();

  GUID    m_networkGUID;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct DestroyNetworkInstanceCmdPacket : public CmdPacketBase
{
  inline DestroyNetworkInstanceCmdPacket();

  inline void deserialize();
  inline void serialize();

  InstanceID  m_instanceId;
};

//----------------------------------------------------------------------------------------------------------------------
struct StepModeCmdPacket : public CmdPacketBase
{
  inline StepModeCmdPacket();

  inline void deserialize();
  inline void serialize();

  enum StepMode
  {
    kSynchronous,
    kAsynchronous,
  };

  uint8_t m_stepMode;
};

//----------------------------------------------------------------------------------------------------------------------
/// \ingroup packettypes
//----------------------------------------------------------------------------------------------------------------------
struct StepCmdPacket : public CmdPacketBase
{
  inline StepCmdPacket();

  inline void deserialize();
  inline void serialize();

  float    m_deltaTime;
  float    m_frameRate;  ///< Allows adjusting the frame rate of the runtime target while the session is running.
};

//----------------------------------------------------------------------------------------------------------------------
struct PreviewChannelsPacket : public PacketBase
{
  static inline PreviewChannelsPacket* create(NetworkDataBuffer* buffer, uint16_t numChannels);

  inline PreviewChannelsPacket();

  inline void deserialize();
  inline void serialize();

  inline Channel* getChannels();

  uint16_t   m_numChannels;
  InstanceID m_instanceId;
};

//----------------------------------------------------------------------------------------------------------------------
struct ClearCachedDataCmdPacket : public CmdPacketBase
{
  inline ClearCachedDataCmdPacket();

  inline void deserialize();
  inline void serialize();
};

//----------------------------------------------------------------------------------------------------------------------
/// This command should be sent after creating a network instance that connect wants to start debugging.
struct StartSessionCmdPacket : public CmdPacketBase
{
  inline StartSessionCmdPacket();

  inline void deserialize();
  inline void serialize();

  float m_frameRate;
};

//----------------------------------------------------------------------------------------------------------------------
struct StopSessionCmdPacket : public CmdPacketBase
{
  inline StopSessionCmdPacket();

  inline void deserialize();
  inline void serialize();
};

//----------------------------------------------------------------------------------------------------------------------
struct PauseSessionCmdPacket : public CmdPacketBase
{
  inline PauseSessionCmdPacket();

  inline void deserialize();
  inline void serialize();

  bool m_pause;
};

//----------------------------------------------------------------------------------------------------------------------
struct CreateConstraintCmdPacket : public CmdPacketBase
{
  inline CreateConstraintCmdPacket();

  inline void deserialize();
  inline void serialize();

  enum
  {
    CONSTRAIN_ORIENTATION = (1 << 0),
    CONSTRAINT_AT_COM = (1 << 1),
  };
  uint64_t     m_constraintGUID;
  uint32_t     m_physicsEngineObjectID;
  uint32_t     m_constraintFlags;
  Vec3 m_localSpaceConstraintPosition;
  Vec3 m_worldSpaceConstraintPosition;
  Vec3 m_sourceRayWSPositionHint;
  Vec3 m_sourceRayWSDirectionHint;
};

//----------------------------------------------------------------------------------------------------------------------
struct MoveConstraintCmdPacket : public CmdPacketBase
{
  inline MoveConstraintCmdPacket();
  inline void deserialize();
  inline void serialize();

  uint64_t     m_constraintGUID;
  Vec3 m_worldSpaceConstraintPosition;
};

//----------------------------------------------------------------------------------------------------------------------
struct RemoveConstraintCmdPacket : public CmdPacketBase
{
  inline RemoveConstraintCmdPacket();
  inline void deserialize();
  inline void serialize();

  uint64_t     m_constraintGUID;
};

//----------------------------------------------------------------------------------------------------------------------
struct ApplyForceCmdPacket : public CmdPacketBase
{
  inline ApplyForceCmdPacket();

  inline void deserialize();
  inline void serialize();

  enum
  {
    MODE_FORCE = (1 << 0),
    MODE_IMPULSE = (1 << 1),
    MODE_VELOCITY_CHANGE = (1 << 2),
    APPLY_AT_COM= (1 << 3),
  };
  uint32_t     m_physicsEngineObjectID;
  uint32_t     m_flags;
  Vec3 m_force;
  Vec3 m_localSpacePosition;
  Vec3 m_worldSpacePosition;
};

//----------------------------------------------------------------------------------------------------------------------
struct AnimRigDefPacket :
  public PacketBase
{
  GUID        m_networkGUID; 
  uint32_t    m_dataLength;

  static inline AnimRigDefPacket* create(
    NetworkDataBuffer*          buffer,
    const GUID&                 networkGUID,
    uint32_t                    dataLen);

  inline void serialize();
  inline void deserialize();

  inline void *getData();
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkDefPacket : public PacketBase
{
  inline NetworkDefPacket();

  inline void deserialize();
  inline void serialize();

  GUID        m_networkGUID;
  StringToken m_nameToken;
  uint32_t    m_numAnimSets;
};

//----------------------------------------------------------------------------------------------------------------------
/// Mark the beginning of the description of a piece of persistent data.
struct BeginPersistentPacket : public PacketBase
{
  inline BeginPersistentPacket(uint8_t defType);

  inline void deserialize();
  inline void serialize();

  uint8_t m_dataType;
};

//----------------------------------------------------------------------------------------------------------------------
/// Mark the end of the description of a piece of persistent data.
struct EndPersistentPacket : public PacketBase
{
  inline EndPersistentPacket();

  inline void deserialize();
  inline void serialize();
};

//----------------------------------------------------------------------------------------------------------------------
struct NodeDefPacket : public PacketBase
{
  static inline NodeDefPacket* create(
    NetworkDataBuffer* buffer, 
    commsNodeID        nodeID,
    MR::NodeType       nodeTypeID,
    StringToken        nameToken,
    uint32_t           numTokens);

  inline void deserialize();
  inline void serialize();
  inline StringToken* getTagTokens() const;

  commsNodeID  m_nodeID;
  MR::NodeType m_nodeTypeID;
  StringToken  m_nameToken;
  uint32_t     m_numTagTokens;

private:
  inline NodeDefPacket();
};

//----------------------------------------------------------------------------------------------------------------------
struct MessageDefPacket : public PacketBase
{
  inline MessageDefPacket();

  inline void deserialize();
  inline void serialize();

  NetworkMessageID  m_messageID;
  StringToken       m_nameToken;
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkInstancePacket : public PacketBase
{
  inline NetworkInstancePacket();

  inline void deserialize();
  inline void serialize();

  InstanceID m_networkId;
  uint32_t   m_nodeOutputCount;
  uint32_t   m_frameIndex;
  uint32_t   m_animSetIndex;
  uint32_t   m_maxTransformCount;

  uint32_t   m_numActiveNodes;
  uint32_t   m_numRootStateMachineNodes;
};

//----------------------------------------------------------------------------------------------------------------------
struct ConnectActiveInstancePacket : public PacketBase
{
  inline ConnectActiveInstancePacket(InstanceID activeInstanceId);

  inline void deserialize();
  inline void serialize();

  InstanceID m_activeInstanceId;
};

//----------------------------------------------------------------------------------------------------------------------
struct TransformBufferPacket : public PacketBase
{
  // The default constructor is useful to partially read the packet.
  inline TransformBufferPacket();

  static inline TransformBufferPacket* create(NetworkDataBuffer* buffer, uint32_t numTransforms);

  inline void deserialize();
  inline void serialize();

  inline NMP::PosQuat* getTransforms();

  uint32_t       m_numTransforms;
  uint8_t        pad1[4];
  NMP::PosQuat  m_poseChange;
};


//----------------------------------------------------------------------------------------------------------------------
struct MessageBufferCmdPacket : public CmdPacketBase
{
  // The default constructor is useful to partially read the packet.
  inline MessageBufferCmdPacket();

  static inline MessageBufferCmdPacket* create(NetworkDataBuffer* buffer,
    InstanceID  instanceId,
    bool sendToAssetManager,
    NetworkMessageID  networkRequestID,
    MR::MessageType requestTypeID, 
    commsNodeID broadcastOrNodeID,
    uint32_t dataBufferSize,
    void * dataBuffer);

  static inline uint32_t getRequiredMemorySize(uint32_t bufferSize);

  inline void deserialize();
  inline void serialize();

  inline void* getDataBuffer();

  InstanceID  m_instanceId;
  uint32_t m_sendToAssetManager;
  NetworkMessageID  m_networkMessageID;
  MR::MessageType m_messageTypeID; 
  commsNodeID  m_broadcastOrNodeID; // INVALID_NODE_ID means to broadcast
  uint32_t m_dataBufferSize; 
};

//----------------------------------------------------------------------------------------------------------------------
struct ID32sPacket : public PacketBase
{
  static inline ID32sPacket* create(NetworkDataBuffer* buffer, uint32_t numIDs);

  inline void deserialize();
  inline void serialize();

  inline bool find(CommsID32 id);

  inline CommsID32* getIDs() const;

  uint32_t  m_numIDs;

private:
  inline ID32sPacket(); // avoids call.
};

//----------------------------------------------------------------------------------------------------------------------
struct ID16sPacket : public PacketBase
{
  static inline ID16sPacket* create(NetworkDataBuffer* buffer, uint32_t numIDs);

  inline void deserialize();
  inline void serialize();

  inline bool find(CommsID16 id);

  inline CommsID16* getIDs();

  uint32_t  m_numIDs;

private:
  inline ID16sPacket(); // avoids call.
};

//----------------------------------------------------------------------------------------------------------------------
struct ActiveInstancesPacket : public PacketBase
{
  static inline ActiveInstancesPacket* create(NetworkDataBuffer* buffer, uint32_t numNetInstances);
  inline void serialize();
  inline void deserialize();
  inline InstanceID* getInstances() const;
  uint32_t m_numNetInstances;
};

//----------------------------------------------------------------------------------------------------------------------
struct ActiveNodesPacket : public ID16sPacket
{
  static inline ActiveNodesPacket* create(NetworkDataBuffer* buffer, uint32_t numNodes);

private:
  inline ActiveNodesPacket(); // avoids call.
};

//----------------------------------------------------------------------------------------------------------------------
struct NodeEventMessagesPacket : public ID32sPacket
{
  static inline NodeEventMessagesPacket* create(NetworkDataBuffer* buffer, uint32_t numNodes);

private:
  inline NodeEventMessagesPacket(); // avoids call.
};

//----------------------------------------------------------------------------------------------------------------------
struct RootStateMachineNodesPacket : public ID16sPacket
{
  static inline RootStateMachineNodesPacket* create(NetworkDataBuffer* buffer, uint32_t numNodes);

private:
  inline RootStateMachineNodesPacket(); // avoids call.
};

//----------------------------------------------------------------------------------------------------------------------
struct ActiveSceneObjectsPacket : public ID32sPacket
{
  static inline ActiveSceneObjectsPacket* create(NetworkDataBuffer* buffer, uint32_t numSceneObjects);

private:
  inline ActiveSceneObjectsPacket(); // avoid calls.
};

//----------------------------------------------------------------------------------------------------------------------
struct FilenamePacket : public PacketBase
{
  static inline FilenamePacket* create(NetworkDataBuffer* buffer, const char* str);

  inline void deserialize();
  inline void serialize();

  inline char* getStr() const;

private:
  inline FilenamePacket(); // avoid calls.
};

//----------------------------------------------------------------------------------------------------------------------
struct FileSizeRequestPacket : public FilenamePacket
{
  static inline FileSizeRequestPacket* create(NetworkDataBuffer* buffer, const char* str);
};

//----------------------------------------------------------------------------------------------------------------------
struct FileRequestPacket : public FilenamePacket
{
  static inline FileRequestPacket* create(NetworkDataBuffer* buffer, const char* str);
};

//----------------------------------------------------------------------------------------------------------------------
struct FileSizePacket : public PacketBase
{
  inline FileSizePacket();

  inline void deserialize();
  inline void serialize();

  uint32_t m_fileSize;
};

//----------------------------------------------------------------------------------------------------------------------
struct FilePacket : public PacketBase
{
  static inline FilePacket* create(uint32_t fileSize);

  inline FilePacket();

  inline void deserialize();
  inline void serialize();

  inline char* getData() const;

  uint32_t   m_dataSize;
};

//----------------------------------------------------------------------------------------------------------------------
struct ConnectScriptCommandPacket : public PacketBase
{
  static inline ConnectScriptCommandPacket* create(NetworkDataBuffer* buffer, const char* str);

  inline void deserialize();
  inline void serialize();

  inline char* getStr() const;

private:
  inline ConnectScriptCommandPacket(); // avoid calls.
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkCreatedReplyPacket : public ReplyPacket
{
  inline NetworkCreatedReplyPacket(
    uint32_t requestId, InstanceID instanceId, const GUID& guid, StringToken instanceName, uint32_t animSetIndex);

  inline void deserialize();
  inline void serialize();

  InstanceID  m_instanceId;
  GUID        m_networkGUID;
  uint32_t    m_initialAnimSetIndex;
  StringToken m_instanceName;
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkDestroyedReplyPacket : public ReplyPacket
{
  inline NetworkDestroyedReplyPacket(uint32_t requestId, InstanceID instanceId);

  inline void deserialize();
  inline void serialize();

  InstanceID m_instanceId;
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkDefLoadedReplyPacket : public ReplyPacket
{
  inline NetworkDefLoadedReplyPacket(uint32_t requestId);

  inline void deserialize();
  inline void serialize();

  bool m_loaded;
  GUID m_networkGUID;
};

//----------------------------------------------------------------------------------------------------------------------
struct NetworkDefDestroyedReplyPacket : public ReplyPacket
{
  inline NetworkDefDestroyedReplyPacket(uint32_t requestId, const GUID& guid);

  inline void deserialize();
  inline void serialize();

  GUID m_networkGUID;
};

//----------------------------------------------------------------------------------------------------------------------
struct ConnectFrameDataPacket : public PacketBase
{
  inline ConnectFrameDataPacket();

  inline void deserialize();
  inline void serialize();

  uint8_t       m_pad[8]; //
  NMP::Vector3  m_rootTranslation;
  NMP::Quat     m_rootOrientation;
  NMP::Vector3  m_previousRootTranslation;
  NMP::Quat     m_previousRootOrientation;
};

//----------------------------------------------------------------------------------------------------------------------
struct SceneObjectDestroyedPacket : public PacketBase
{
  inline SceneObjectDestroyedPacket();

  inline void deserialize();
  inline void serialize();

  uint32_t m_sceneObjectId;
};

//----------------------------------------------------------------------------------------------------------------------
struct DestroySceneObjectCmdPacket : public CmdPacketBase
{
  inline DestroySceneObjectCmdPacket();

  inline void deserialize();
  inline void serialize();

  uint32_t m_sceneObjectId;
};

//----------------------------------------------------------------------------------------------------------------------
struct BeginSceneObjectCmdPacket : public CmdPacketBase
{
  inline BeginSceneObjectCmdPacket();

  inline void deserialize();
  inline void serialize();

  uint32_t m_numAttributes;
};

//----------------------------------------------------------------------------------------------------------------------
struct EndSceneObjectCmdPacket : public CmdPacketBase
{
  inline EndSceneObjectCmdPacket();

  inline void deserialize();
  inline void serialize();
};

//----------------------------------------------------------------------------------------------------------------------
struct AttributeCmdPacket : public CmdPacketBase
{
  static inline AttributeCmdPacket* create(const Attribute* attribute);

  inline void deserialize();
  inline void serialize();

  inline Attribute::Descriptor* getAttributeDescriptor();
  inline void* getAttributeData();
};

//----------------------------------------------------------------------------------------------------------------------
struct SetAttributeCmdPacket : public CmdPacketBase
{
  static inline SetAttributeCmdPacket* create(uint32_t objectId, const Attribute* attribute);

  inline void deserialize();
  inline void serialize();

  inline Attribute::Descriptor* getAttributeDescriptor();

  inline void* getAttributeData();

  uint32_t m_sceneObjectId;
};

//----------------------------------------------------------------------------------------------------------------------
struct SetEnvironmentAttributeCmdPacket : public CmdPacketBase
{
  static inline SetEnvironmentAttributeCmdPacket* create(const Attribute* attribute);

  inline void deserialize();
  inline void serialize();

  inline Attribute::Descriptor* getAttributeDescriptor();

  inline void* getAttributeData();
};

//----------------------------------------------------------------------------------------------------------------------
struct SceneObjectDefPacket : public PacketBase
{
  inline SceneObjectDefPacket();

  inline void deserialize();
  inline void serialize();

  uint32_t m_sceneObjectId;
  uint32_t m_numAttributes;
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Scene object attribute definition. Used to describe static attributes and initial value of dynamic ones.
struct AttributeDefPacket : public PacketBase
{
  static inline AttributeDefPacket* create(NetworkDataBuffer* buffer, const Attribute* attribute);

  inline void deserialize();
  inline void serialize();

  inline Attribute::Descriptor* getAttributeDescriptor();
  inline void* getAttributeData();

private:
  inline AttributeDefPacket();
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Scene object attribute update. Used to send the updated data for dynamic attributes.
struct AttributeUpdatePacket : public PacketBase
{
  static inline AttributeUpdatePacket* create(NetworkDataBuffer* buffer, const Attribute* attribute);

  inline void deserialize();
  inline void serialize();

  inline Attribute::Descriptor* getAttributeDescriptor();
  inline void* getAttributeData();

private:
  inline AttributeUpdatePacket();
};

//----------------------------------------------------------------------------------------------------------------------
struct SceneObjectUpdatePacket : public PacketBase
{
  inline SceneObjectUpdatePacket();

  inline void deserialize();
  inline void serialize();

  uint32_t m_sceneObjectId;
  uint32_t m_numDynamicAttributes;
};

//----------------------------------------------------------------------------------------------------------------------
struct DownloadSceneObjectsCmdPacket : public CmdPacketBase
{
  inline DownloadSceneObjectsCmdPacket();

  inline void deserialize();
  inline void serialize();
};

//----------------------------------------------------------------------------------------------------------------------
struct DownloadGlobalDataCmdPacket : public CmdPacketBase
{
  enum
  {
    GBLDATAFLAGS_NONE         = 0,
    GBLDATAFLAGS_STRING_CACHE = (1 << 0),
    GBLDATAFLAGS_ALL          = 0xFFFFFFFF,
  };

  inline DownloadGlobalDataCmdPacket();

  inline void deserialize();
  inline void serialize();

  uint32_t m_globalDataFlags;
};

//----------------------------------------------------------------------------------------------------------------------
struct DownloadFrameDataCmdPacket : public CmdPacketBase
{
  inline DownloadFrameDataCmdPacket();

  inline void deserialize();
  inline void serialize();
};

//----------------------------------------------------------------------------------------------------------------------
struct DownloadNetworkDefinitionCmdPacket : public CmdPacketBase
{
  inline DownloadNetworkDefinitionCmdPacket();

  inline void deserialize();
  inline void serialize();

  GUID m_guid;
};

//----------------------------------------------------------------------------------------------------------------------
struct BeginDispatcherTaskExecutePacket : public PacketBase
{
  inline BeginDispatcherTaskExecutePacket();

  inline void deserialize();
  inline void serialize();

  InstanceID  m_networkInstanceID;
  MR::TaskID  m_taskid;
  commsNodeID m_owningNodeID;
  StringToken m_taskNameTag;
  uint32_t    m_sourceFrame;       ///< The frame that this data belongs to. 
};

//----------------------------------------------------------------------------------------------------------------------
struct AddDispatcherTaskExecuteParameterPacket : public PacketBase
{
  inline AddDispatcherTaskExecuteParameterPacket();

  inline void deserialize();
  inline void serialize();

  StringToken m_semanticName;
  commsNodeID m_owningNodeID;
  commsNodeID m_targetNodeID;
  uint32_t    m_validFrame;
  uint16_t    m_lifespan;
  bool        m_isInput;
  bool        m_isOutput;
};

//----------------------------------------------------------------------------------------------------------------------
struct EndDispatcherTaskExecutePacket : public PacketBase
{
  inline EndDispatcherTaskExecutePacket();

  inline void deserialize();
  inline void serialize();
};

//----------------------------------------------------------------------------------------------------------------------
struct StateMachineMessageEventMsgPacket : public PacketBase
{
  inline StateMachineMessageEventMsgPacket();

  inline void deserialize();
  inline void serialize();

  NetworkMessageID m_messageID;      ///< ID of a request that has been sent to 1 or more state machines. INVALID_MESSAGE_ID if clearing all requests.
  commsNodeID      m_targetSMNodeID; ///< INVALID_NODE_ID means that this is a broadcast request that goes to all state machines.
  InstanceID       m_networkInstanceID;
  bool             m_set;            ///< true = setting the request on, false = setting the request off.
};

//----------------------------------------------------------------------------------------------------------------------
/// \brief Used to communicate the status of the target. This packet is broadcast to all the connections, regardless
/// if they are actively debugging, paused or not. This is an experimental feature so for the moment it sends the
/// list of active instances only. This information might be redundant as it gets sent with the frame update anyway,
/// but the main purpose is to keep connect up to date of what's happening on the target even when an entire frame
/// is not sent. This could easily replace the ping mechanism.
struct TargetStatusCtrlPacket : public PacketBase
{
  struct ActiveInstance
  {
    static const uint32_t kMaxInstanceNameLen = 32;
    InstanceID  m_id;
    GUID        m_guid;
    char        m_name[kMaxInstanceNameLen];
  };

  static inline TargetStatusCtrlPacket* create(
    NetworkDataBuffer* buffer,
    uint32_t           numNetInstances,
    uint32_t           numNetDefinitions);

  inline void serialize();
  inline void deserialize();

  inline ActiveInstance* getInstances() const;
  inline uint32_t getNumInstances() const { return m_numNetInstances; }
  inline GUID* getGUIDs() const;

  uint32_t m_numNetInstances;
  uint32_t m_numNetDefinitions;
};

//----------------------------------------------------------------------------------------------------------------------
struct ScatterBlendWeightsPacket : public PacketBase
{
  inline ScatterBlendWeightsPacket();

  static inline ScatterBlendWeightsPacket* create(
    NetworkDataBuffer* buffer,
    commsNodeID        scatterBlendNodeID,
    float              barycentricWeights[3],
    commsNodeID        childNodeIDs[3],
    float              desiredMotionParams[2],
    float              achievedMotionParams[2],
    bool               wasProjected);

  inline void deserialize();
  inline void serialize();

  commsNodeID m_scatterBlendNodeID;
  commsNodeID m_childNodeIDs[3];
  float       m_barycentricWeights[3];

  // These in the space of the scatter blend native units. i.e.
  // Angle - radians
  // Distance - runtime units
  float       m_desiredMotionParams[2];
  float       m_achievedMotionParams[2];
  bool        m_wasProjected;
};

#include "corePackets.inl"

#pragma pack(pop)

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_COREPACKETS_H
//----------------------------------------------------------------------------------------------------------------------
