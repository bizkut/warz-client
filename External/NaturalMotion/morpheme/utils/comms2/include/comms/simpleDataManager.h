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
#ifndef MCOMMS_SIMPLEDATAMANAGER_H
#define MCOMMS_SIMPLEDATAMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/runtimeTargetInterface.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class Network;
class NetworkDef;
}

//----------------------------------------------------------------------------------------------------------------------
namespace MCOMMS
{

//----------------------------------------------------------------------------------------------------------------------
/// \class SimpleDataManager
/// \brief Implements DataManagementInterface.
/// \ingroup MorphemeComms
///
/// A simple implementation of the data management interface. Documentation for individual methods has been omitted for
/// brevity but is available on the method declaration of the interface this class is derived from.
///
/// This partial class declares two new pure virtual methods findNetworkByInstanceID() and findNetworkDefByGuid() which
/// provide the methods implemented with access to the client specific ID to network and GUID to network definition
/// translations.
///
/// Notes:
///
/// 1) This implementation does not accumulate a characters trajectory and getWorldTransforms() will always place the
///    character at the origin if a character controller is not provided.
///
/// 2) Per-node debug output is not supported by this implementation as it would require per instance dynamic store. The
///    methods relevant to monitoring node debug output are null implementations.
///
/// 3) Setting the animation set index is not supported as the animation set index must be cached and applied before the
///    next update to ensure as consistent few on the network data at all times. The network update is under client
///    control and therefore out of reach of this manager.
///
/// 4) No commands are supported by this implementation, executeCommand() will simply return false for any command
///    received.
///
/// 5) The temporary allocator of the respective network is used in getWorldTransforms() and getActiveNodes(). If this
///    conflicts with how the temporary allocators are setup for a specific project these methods need to be
///    re-implemented on client side.
//----------------------------------------------------------------------------------------------------------------------
class SimpleDataManager :
  public DataManagementInterface
{
  // Construction/destruction
protected:

  SimpleDataManager();

public:

  virtual ~SimpleDataManager() {}

  /// \brief Helper method to convert from a instance ID to network instance.
  /// The mapping between the two is entirely defined by the client. It is also expected that the client already
  /// maintains a list of available network instances for general use.
  virtual MR::Network* findNetworkByInstanceID(InstanceID id) const = 0;

  /// \brief Helper method to convert from a network definition GUID to a network definition.
  /// It is expected that the client already maintains a list of available network definitions for general use.
  virtual MR::NetworkDef* findNetworkDefByGuid(const GUID& guid) const = 0;

  /// \brief Sets the frame index returned to the COMMS server by getFrameIndex().
  void setFrameIndex(uint32_t frameIndex) { m_frameIndex = frameIndex; }

  /// \brief Returns the step interval [s] as set by the COMMS server via setStepInterval().
  float getStepInterval() const { return m_stepInterval; }
  
  // [DataManagementInterface]
  //
  // Most of these implementations are simple proxies for the respective methods on the network or network definition.
public:

  virtual bool canSendStateMachineRequests() const NM_OVERRIDE { return true; }
  virtual bool canSetControlParameters() const NM_OVERRIDE { return true; }

  virtual const char* getSemanticName(MR::AttribDataSemantic semantic) NM_OVERRIDE;

  virtual bool serializeTxAnimRigDefData(
    const MCOMMS::GUID& networkGUID,
    MR::AnimSetIndex index,
    MCOMMS::Connection* connection) NM_OVERRIDE;

  virtual uint32_t getMaxTransformCount(InstanceID id) NM_OVERRIDE;
  virtual uint32_t getCurrentTransformCount(InstanceID id) NM_OVERRIDE;

  virtual bool getLocalTransforms(InstanceID id, uint32_t numTransforms, NMP::PosQuat* buffer) NM_OVERRIDE;
  /// This implementation of getWorldTransforms() uses identity for the position and orientation if a character
  /// controller is not available on the network.
  virtual bool getWorldTransforms(InstanceID id, uint32_t numTransforms, NMP::PosQuat* buffer) NM_OVERRIDE;
  virtual bool getTransformChange(InstanceID id, NMP::Vector3& deltaTranslation, NMP::Quat& deltaOrientation) NM_OVERRIDE;

  virtual uint32_t getActiveNodeCount(InstanceID id) NM_OVERRIDE;
  virtual uint32_t getActiveNodes(InstanceID id, commsNodeID* nodeIDs, uint32_t maxNodeIDs) NM_OVERRIDE;
  virtual commsNodeID getRootStateNodeID(InstanceID id, commsNodeID stateMachineNodeID) NM_OVERRIDE;

  virtual uint32_t getTriggeredNodeEventMessageCount(InstanceID id) NM_OVERRIDE;
  virtual uint32_t getTriggeredNodeEventMessages(InstanceID id, NetworkMessageID* messageIDs, uint32_t maxMessages) NM_OVERRIDE;

  virtual bool getScatterBlendWeightData(
    MCOMMS::InstanceID id, MCOMMS::commsNodeID scatterBlendNodeID,
    float (&barycentricWeights)[3],
    MCOMMS::commsNodeID (&childNodeIDs)[3],
    float (&desiredMotionParams)[2],
    float (&achievedMotionParams)[2],
    bool& wasProjected) NM_OVERRIDE;

  virtual uint32_t getActiveAnimationSet(InstanceID id) NM_OVERRIDE;

  virtual uint32_t getAnimSetCount(InstanceID id) const NM_OVERRIDE;
  virtual uint32_t getNodeCount(InstanceID id) NM_OVERRIDE;
  virtual MR::NodeType getNodeTypeID(InstanceID id, commsNodeID nodeID) NM_OVERRIDE;
  virtual const char* getNodeName(InstanceID id, commsNodeID nodeID) NM_OVERRIDE;
  virtual uint32_t getNodeTagCount(InstanceID id, commsNodeID nodeID) NM_OVERRIDE;
  virtual const char* getNodeTag(InstanceID id, commsNodeID nodeID, uint32_t index) NM_OVERRIDE;
  virtual const char* getMessageName(InstanceID id, NetworkMessageID requestID) NM_OVERRIDE;
  virtual NetworkMessageID getMessageID(InstanceID id, const char* utf8Name) NM_OVERRIDE;

  virtual uint32_t getAnimSetCount(const GUID& guid) const NM_OVERRIDE;
  virtual uint32_t getNodeCount(const GUID& guid) const NM_OVERRIDE;
  virtual MR::NodeType getNodeTypeID(const GUID& guid, commsNodeID nodeID) const NM_OVERRIDE;
  virtual uint32_t getMessageCount(const GUID& guid) const NM_OVERRIDE;
  virtual const char* getNodeName(const GUID& guid, commsNodeID nodeID) const NM_OVERRIDE;
  virtual uint32_t getNodeTagCount(const GUID& guid, commsNodeID nodeID) const NM_OVERRIDE;
  virtual const char* getNodeTag(const GUID& guid, commsNodeID nodeID, uint32_t index) const NM_OVERRIDE;
  virtual const char* getMessageName(const GUID& guid, NetworkMessageID messageID) const NM_OVERRIDE;
  virtual uint32_t getMessageIDs(const GUID& guid, NetworkMessageID* messageIDs, uint32_t maxMessageIDs) const NM_OVERRIDE;

  virtual uint32_t getFrameIndex() NM_OVERRIDE;
  virtual bool setStepInterval(float dt) NM_OVERRIDE;
  virtual bool setFrameRate(float fps) NM_OVERRIDE;

  virtual bool sendMessage(InstanceID id, commsNodeID nodeID, const MR::Message& message) NM_OVERRIDE;

  virtual bool broadcastMessage(InstanceID id, const MR::Message& message) NM_OVERRIDE;

  virtual bool setCurrentState(InstanceID id, commsNodeID stateMachineNodeID, commsNodeID newRootStateID) NM_OVERRIDE;
  virtual bool setControlParameter(InstanceID id, commsNodeID nodeID, MR::NodeOutputDataType type, const void* cparamData) NM_OVERRIDE;

  virtual bool setDebugOutputFlags(InstanceID id, uint32_t flags) NM_OVERRIDE;

  virtual bool setDebugOutputOnNodes(
    InstanceID              id,
    bool                    on,
    bool                    allNodes,
    bool                    allSemantics,
    uint32_t                numNodeIDs,
    commsNodeID*            nodeIDs,
    uint32_t                numSemantics,
    MR::AttribDataSemantic* semantics) NM_OVERRIDE;

  virtual bool enableOutputDataBuffering(
    InstanceID             id,
    commsNodeID            nodeID,
    MR::NodeOutputDataType dataType,
    bool                   enable) NM_OVERRIDE;

  virtual void onConnectionClosed(Connection* connection) NM_OVERRIDE;

  // [DataManagementInterface]
  //
  // This part of the interface is not supported, all methods below are null implementations.
public:

  virtual bool canSetNetworkAnimSet() const NM_OVERRIDE { return false; }

  virtual uint32_t getNodeOutputCount(InstanceID id) NM_OVERRIDE;
  virtual commsNodeID getNodeOutputNodeID(InstanceID id, uint32_t outputIndex) NM_OVERRIDE;
  virtual MR::NodeOutputDataType getNodeOutputDataType(InstanceID id, uint32_t outputIndex) NM_OVERRIDE;

  virtual uint32_t getNodeOutputDataLength(
    InstanceID              id,
    uint32_t                outputIndex,
    MR::NodeID&             owningNodeID,
    MR::NodeID&             targetNodeID,
    MR::AttribDataType&     attribType,
    MR::AttribDataSemantic& attribSemantic,
    MR::AnimSetIndex&       animSetIndex,
    MR::FrameCount&         validFrame) NM_OVERRIDE;

  virtual bool getNodeOutputData(InstanceID id, uint32_t outputIndex, void* destBuffer, uint32_t bufferLength) NM_OVERRIDE;

  virtual bool serializeTxNodeTimings(InstanceID id, Connection* connection) NM_OVERRIDE;
  virtual bool serializeTxControlParamAndOpNodeAttribData(InstanceID id, Connection* connection) NM_OVERRIDE;
  virtual bool serializeTxTreeNodeAttribData(InstanceID id, Connection* connection) NM_OVERRIDE;

  virtual bool setActiveAnimationSet(InstanceID id, uint32_t animSetIndex) NM_OVERRIDE;
  virtual bool executeCommand(InstanceID id, const char* command) NM_OVERRIDE;

  virtual bool setRootTransform(InstanceID id, const NMP::PosQuat& rootTransform) NM_OVERRIDE;

  virtual bool isSimulating() const NM_OVERRIDE;
  virtual void beginSimulation() NM_OVERRIDE;
  virtual void endSimulation() NM_OVERRIDE;

private:

  uint32_t    m_frameIndex;
  float       m_stepInterval;
  float       m_frameRate;
  bool        m_isSimulating;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_SIMPLEDATAMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
