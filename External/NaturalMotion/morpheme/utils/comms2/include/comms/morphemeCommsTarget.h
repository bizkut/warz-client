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
#ifndef MC_MORPHEMECOMMSTARGET_H
#define MC_MORPHEMECOMMSTARGET_H
//----------------------------------------------------------------------------------------------------------------------
#include "../../../../SDK/sharedDefines/mSharedDefines.h"
#include "comms/mcomms.h"
#include "comms/assetManagerPackets.h"
#include "comms/corePackets.h"
#include "comms/attribute.h"
#include "comms/sceneObject.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
struct CmdPacketBase;

class Attribute;
class Connection;

//----------------------------------------------------------------------------------------------------------------------
/// Connect talks to this
//----------------------------------------------------------------------------------------------------------------------
class RuntimeTarget
{
public:
  enum StatusCode
  {
    kNetworkError,
    kValid
  };

  RuntimeTarget(NMP::SocketWrapper commandsSocket);
  ~RuntimeTarget();

  struct CommandsConnectionInfo
  {
    CommandsConnectionInfo() :
      bytesSent(0),
      packetsSent(0),
      sendCycles(0)
    {
    }

    uint32_t bytesSent;
    uint32_t packetsSent;
    uint32_t sendCycles;
  };

  void getCommandsConnectionInfo(CommandsConnectionInfo& info);

  bool pingConnection();

  CmdRequestID loadNetworkDefinition(
    const GUID&          guid,
    const char*          networkDefName,
    MCOMMS::Connection* connection = 0);

  CmdRequestID createNetworkInstance(
    const GUID&         guid,
    const char*         instanceName,
    uint32_t            animSetIndex,
    const NMP::Vector3& startPosition,
    const NMP::Quat&    startRotation);

  CmdRequestID referenceNetworkInstance(InstanceID instanceID);
  CmdRequestID unreferenceNetworkInstance(InstanceID instanceID);

  CmdRequestID startSession(float frameRate);
  CmdRequestID stopSession();
  CmdRequestID pauseSession(bool pause);

  CmdRequestID destroyNetworkInstance(InstanceID instance);
  CmdRequestID destroyNetworkDefinition(const GUID& guid);

  CmdRequestID clearCachedData();

  bool enableAsynchStepping(bool enable);
  CmdRequestID stepAllInstances(float dt, float fps);

  bool setDebugOutputFlags(InstanceID id, uint32_t flags);

  // All possible semantics will be transmitted from all active nodes.
  bool setNodeDebugOutputAllOn(InstanceID instanceID);

  // All node attribute transmission will be turned off.
  bool setNodeDebugOutputAllOff(InstanceID instanceID);

  // Transmission of all semantics on a set of nodes will be turned on or off.
  // The state of node semantics outside of this groups will be unaffected.
  bool setNodeDebugOutputSomeNodesAllSemantics(
    InstanceID instanceID,
    uint32_t   numNodes,
    uint16_t*  nodeIDs,
    bool       on);

  // Transmission of a set of semantics on all nodes will be turned on or off.
  // The state of node semantics outside of this groups will be unaffected.
  bool setNodeDebugOutputAllNodesSomeSemantics(
    InstanceID instanceID,
    uint32_t   numSemantics,
    uint16_t*  semantics,
    bool       on);

  // Transmission of a set of semantics on a set of nodes will be turned on or off.
  // The state of node semantics outside of this groups will be unaffected.
  bool setNodeDebugOutputSomeNodesSomeSemantics(
    InstanceID instanceID,
    uint32_t   numNodes,
    uint16_t*  nodeIDs,
    uint32_t   numSemantics,
    uint16_t*  semantics,
    bool       on);

  bool enableOutputDataBuffering(InstanceID id, commsNodeID nodeID, MR::NodeOutputDataType dataType, bool enable);

  bool setRootTransform(InstanceID instanceID, const NMP::PosQuat& rootTransform);
  bool setControlParameter(InstanceID instanceID, commsNodeID nodeID, MR::NodeOutputDataType type, const void* cparamData);
  bool sendRequest(InstanceID instanceID, commsNodeID stateMachineNodeID, NetworkMessageID requestID);
  bool broadcastRequest(InstanceID instanceID, NetworkMessageID requestID);

  bool sendMessage(InstanceID instanceID, bool sendToAssetManager, commsNodeID nodeID, NetworkMessageID requestID,
    MR::MessageType typeID, void * dataBuffer, size_t bufferSize);
  bool broadcastMessage(InstanceID instanceID, bool sendToAssetManager, NetworkMessageID requestID,
    MR::MessageType typeID, void * dataBuffer, size_t bufferSize);

  bool setCurrentState(InstanceID instanceID, commsNodeID stateMachineNodeID, commsNodeID newRootStateID);
  bool executeCommand(InstanceID id, const char* utf8Command);
  bool setAnimSet(InstanceID instanceID, uint32_t animSetIndex);
  bool setAssetManagerAnimSet(uint32_t animSetIndex);

  CmdRequestID setAnimationBrowserAnim(const char* animBrowserAnim);
  CmdRequestID setAnimationBrowserNetwork(
    const char* animBrowserNetwork,
    const char* animationSourceNodeName,
    const char* retargetingSourceNodeName,
    const char* rescalingSourceNodeName);
  CmdRequestID loadAnimationBrowserData(const char* compressionType);
  CmdRequestID setAnimationBrowserTime(float time);

  CmdRequestID downloadAnimationBrowserEventDetectionData(float minTime, uint32_t numFrames, float dt);

  /// Physics manipulation
  CmdRequestID createConstraint(uint64_t constraintGUID, uint32_t physicsEngineObjectID,
    const NMP::Vector3& localSpaceConstraintPosition, const NMP::Vector3& worldSpaceConstraintPosition,
    bool constrainOrientation, bool constrainAtCentreOfMass,
    const NMP::Vector3& sourceRayWSPositionHint, const NMP::Vector3& sourceRayWSDirectionHint
    );

  CmdRequestID moveConstraint(uint64_t constraintGUID, const NMP::Vector3& worldSpaceConstraintPosition);

  CmdRequestID removeConstraint(uint64_t constraintGUID);

  CmdRequestID applyForce(uint32_t physicsEngineObjectID, bool isForce, bool isImpulse, bool isVelocityChange,
     const NMP::Vector3& force, bool applyAtCOM,const NMP::Vector3& localSpacePosition,
     const NMP::Vector3& worldSpacePosition);

 /// Allocates and initializes a SceneObject.
  bool createSceneObject(uint32_t numAttr, const Attribute* const* attributes);

  /// Frees all attributes and data (i.e. assumes that data has been allocated via createAttributeData above),
  /// and the scene object.  Sends a request to the target to destroy to the object.
  bool destroySceneObjectOnTarget(SceneObjectID objectID);
  bool setSceneObjectAttribute(SceneObjectID objectID, const Attribute* attribute);

  bool setEnvironmentAttribute(const Attribute* attribute);

  CmdRequestID downloadSceneObjects();
  CmdRequestID downloadNetworkDefinition(const GUID& guid);
  CmdRequestID downloadGlobalData();
  CmdRequestID downloadFrameData();
  CmdRequestID identifyRuntimeTarget();

  StatusCode getStatusCode() const;

  bool sendCommandPacket(CmdPacketBase& cmdPacket);

  static const CmdRequestID INVALID_CMD_REQUEST_ID = 0xffffffff;

protected:
  uint32_t sendBuffer(NMP::SocketWrapper socket, const void* buffer, uint32_t bufferSize);
  void setNetworkError();

  static const unsigned int CHAR_BUFFER_SIZE = 512;

  static CmdRequestID       sm_requestID;
  static uint32_t           sm_lastPingID;

  NMP::SocketWrapper*       m_commandsSocket;
  char                      m_charBuffer[CHAR_BUFFER_SIZE];
  StatusCode                m_statusCode;

  CommandsConnectionInfo    m_commandsConnectionInfo;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MC_MORPHEMECOMMSTARGET_H
//----------------------------------------------------------------------------------------------------------------------
