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
#ifndef NM_DEFAULTDATAMANAGER_H
#define NM_DEFAULTDATAMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMMemoryAllocator.h"
#include "NMPlatform/NMVectorContainer.h"
#include "comms/runtimeTargetInterface.h"
//----------------------------------------------------------------------------------------------------------------------

class IControllerMgr;
class IPhysicsMgr;
class NetworkDefRecordManager;
class NetworkInstanceRecordManager;
class RuntimeTargetContext;

namespace MR
{
class Network;
struct NodeBinEntry;
struct OutputCPPin;
}

//----------------------------------------------------------------------------------------------------------------------
/// \class DefaultDataManager
/// \brief Implements MCOMMS::DataManagementInterface.
/// \ingroup RuntimeTarget
//----------------------------------------------------------------------------------------------------------------------
class DefaultDataManager :
  public MCOMMS::DataManagementInterface
{
public:

  typedef bool (CommandHandler)(const char* command);

public:

  DefaultDataManager(
    RuntimeTargetContext* context,
    MCOMMS::NetworkLifecycleInterface* networkLifecycle,
    IPhysicsMgr* physicsMgr);

  virtual ~DefaultDataManager() {}

  /// \brief This custom assignment operator is required as the compiler cannot generate the default assignment operator
  ///  due to const members.
  DefaultDataManager & operator= (const DefaultDataManager & other) {*this = other; return *this;}

  void onConnectionClosed(MCOMMS::Connection* connection);

  /// \brief Set frame index.
  void setFrameIndex(uint32_t frame);

  /// \brief Gets the current step interval.
  float getStepInterval();

  /// \brief Gets the current expected frame rate for the RTT main loop.
  float getFrameRate() const;

  void setCommandHandler(CommandHandler* commandHandler) { m_commandHandler = commandHandler; }

  void serialiseAndBufferDebugAttribData(MR::Network* net, const MR::NodeBinEntry* entry, MCOMMS::Connection* connection);
  void serialiseAndBufferDebugOutputCPAttribData(
    MR::Network *net,
    const MR::OutputCPPin *outputCPPin,
    const MR::NodeID owningNodeID,
    const MR::PinIndex pinIndex,
    const MR::AttribDataSemantic semantic,
    MCOMMS::Connection *connection);

  //--------------------------------------------------------------------------------------------------------------------
  // [DataManagementInterface]
public:

  virtual bool serializeTxAnimRigDefData(const MCOMMS::GUID& networkGUID, MR::AnimSetIndex index, MCOMMS::Connection* connection) NM_OVERRIDE;
  virtual uint32_t getMaxTransformCount(MCOMMS::InstanceID id);
  virtual uint32_t getCurrentTransformCount(MCOMMS::InstanceID id);

  virtual bool getLocalTransforms(MCOMMS::InstanceID id, uint32_t numTransforms, NMP::PosQuat* buffer);
  virtual bool getWorldTransforms(MCOMMS::InstanceID id, uint32_t numTransforms, NMP::PosQuat* buffer);
  virtual bool getTransformChange(MCOMMS::InstanceID id, NMP::Vector3& deltaTranslation, NMP::Quat& deltaOrientation);

  virtual uint32_t getActiveNodeCount(MCOMMS::InstanceID instanceID);
  virtual uint32_t getActiveNodes(MCOMMS::InstanceID instanceID, MCOMMS::commsNodeID* nodeIDs, uint32_t maxNodeIDs);

  virtual MCOMMS::commsNodeID getRootStateNodeID(MCOMMS::InstanceID instanceID, MCOMMS::commsNodeID statMachineNodeID);

  virtual uint32_t getTriggeredNodeEventMessageCount(MCOMMS::InstanceID id) NM_OVERRIDE;
  virtual uint32_t getTriggeredNodeEventMessages(MCOMMS::InstanceID id, MCOMMS::NetworkMessageID* messageIDs, uint32_t maxMessages) NM_OVERRIDE;

  virtual bool getScatterBlendWeightData(
    MCOMMS::InstanceID id, MCOMMS::commsNodeID scatterBlendNodeID,
    float (&barycentricWeights)[3],
    MCOMMS::commsNodeID (&childNodeIDs)[3],
    float (&desiredMotionParams)[2],
    float (&achievedMotionParams)[2],
    bool& wasProjected);

  virtual bool enableOutputDataBuffering(
    MCOMMS::InstanceID     id,
    MCOMMS::commsNodeID    nodeID,
    MR::NodeOutputDataType dataType,
    bool                   enable);

  virtual bool setDebugOutputFlags(MCOMMS::InstanceID id, uint32_t flags);

  virtual bool setDebugOutputOnNodes(
    MCOMMS::InstanceID      instanceID,
    bool                    on,
    bool                    allNodes,
    bool                    allSemantics,
    uint32_t                numNodeIDs,
    MCOMMS::commsNodeID*    nodeIDs,
    uint32_t                numSemantics,
    MR::AttribDataSemantic* semantics);

  virtual uint32_t getNodeOutputCount(MCOMMS::InstanceID instanceID);
  virtual MCOMMS::commsNodeID getNodeOutputNodeID(MCOMMS::InstanceID instanceID, uint32_t outputIndex);
  virtual MR::NodeOutputDataType getNodeOutputDataType(MCOMMS::InstanceID instanceID, uint32_t outputIndex);

  virtual uint32_t getNodeOutputDataLength(
    MCOMMS::InstanceID      instanceID,
    uint32_t                outputIndex,
    MR::NodeID&             owningNodeID,
    MR::NodeID&             targetNodeID,
    MR::AttribDataType&     attribType,
    MR::AttribDataSemantic& attribSemantic,
    MR::AnimSetIndex&       animSetIndex,
    MR::FrameCount&         validFrame);

  virtual bool serializeTxControlParamAndOpNodeAttribData(MCOMMS::InstanceID instanceID, MCOMMS::Connection* connection) NM_OVERRIDE;
  virtual bool serializeTxTreeNodeAttribData(MCOMMS::InstanceID instanceID, MCOMMS::Connection* connection) NM_OVERRIDE;
  virtual bool serializeTxNodeTimings(MCOMMS::InstanceID instanceID, MCOMMS::Connection* connection) NM_OVERRIDE;

  virtual const char* getSemanticName(MR::AttribDataSemantic semantic);

  virtual bool getNodeOutputData(
    MCOMMS::InstanceID instanceID,
    uint32_t           outputIndex,
    void*              destBuffer,
    uint32_t           bufferLength);

  virtual uint32_t getActiveAnimationSet(MCOMMS::InstanceID instanceID);
  virtual bool setActiveAnimationSet(MCOMMS::InstanceID instanceID, uint32_t animSetIndex);

  virtual uint32_t getAnimSetCount(MCOMMS::InstanceID id) const;
  virtual uint32_t getNodeCount(MCOMMS::InstanceID instanceID);
  virtual uint32_t getNodeTypeID(MCOMMS::InstanceID instanceID, MCOMMS::commsNodeID nodeID);
  virtual const char* getNodeName(MCOMMS::InstanceID instanceID, MCOMMS::commsNodeID nodeID);
  virtual uint32_t getNodeTagCount(MCOMMS::InstanceID id, MCOMMS::commsNodeID nodeID);
  virtual const char* getNodeTag(MCOMMS::InstanceID id, MCOMMS::commsNodeID nodeID, uint32_t index);
  virtual const char* getMessageName(MCOMMS::InstanceID instanceID, MCOMMS::NetworkMessageID messageID);
  virtual MCOMMS::NetworkMessageID getMessageID(MCOMMS::InstanceID instanceID, const char* utf8Name);

  virtual uint32_t getAnimSetCount(const MCOMMS::GUID& id) const;
  virtual uint32_t getNodeCount(const MCOMMS::GUID& networkDef) const;
  virtual uint32_t getMessageCount(const MCOMMS::GUID& networkDef) const;
  virtual MR::NodeType getNodeTypeID(const MCOMMS::GUID& guid, MCOMMS::commsNodeID nodeID) const;
  virtual const char* getNodeName(const MCOMMS::GUID& networkDef, MCOMMS::commsNodeID nodeID) const;
  virtual uint32_t getNodeTagCount(const MCOMMS::GUID& guid, MCOMMS::commsNodeID nodeID) const;
  virtual const char* getNodeTag(const MCOMMS::GUID& guid, MCOMMS::commsNodeID nodeID, uint32_t index) const;
  virtual const char* getMessageName(const MCOMMS::GUID& networkDef, MCOMMS::NetworkMessageID messageID) const;
  virtual uint32_t getMessageIDs(const MCOMMS::GUID& networkDef, MCOMMS::NetworkMessageID* messageIDs, uint32_t maxRequestIDs) const;

  virtual bool setControlParameter(
    MCOMMS::InstanceID     instanceID,
    MCOMMS::commsNodeID    nodeID,
    MR::NodeOutputDataType type,
    const void*            cparamData);

  virtual bool setCurrentState(MCOMMS::InstanceID instanceID, MCOMMS::commsNodeID stateMachineNodeID, MCOMMS::commsNodeID newRootStateID);

  virtual bool broadcastMessage(MCOMMS::InstanceID id, const MR::Message& message);
  virtual bool sendMessage(MCOMMS::InstanceID id, MCOMMS::commsNodeID nodeID, const MR::Message& message);

  virtual bool executeCommand(MCOMMS::InstanceID instanceID, const char* command);

  virtual bool setStepInterval(float dt);
  virtual bool setFrameRate(float fps);
  virtual uint32_t getFrameIndex();

  virtual bool canSetControlParameters() const;
  virtual bool canSetNetworkAnimSet() const;
  virtual bool canSendStateMachineRequests() const;

  virtual bool setRootTransform(MCOMMS::InstanceID id, const NMP::PosQuat& rootTransform);

  virtual void beginSimulation();
  virtual void endSimulation();
  virtual bool isSimulating() const;

public:

  static IControllerMgr* getControllerMgr();
  static IPhysicsMgr* getPhysicsMgr();

protected:

  MCOMMS::NetworkLifecycleInterface* const    m_networkLifecycle;
  NetworkDefRecordManager* const              m_networkDefManager;
  NetworkInstanceRecordManager* const         m_networkInstanceManager;

  uint32_t                                    m_frameIndex;     ///< The frame index.
  float                                       m_stepInterval;   ///< The update step interval.
  float                                       m_frameRate;      ///< Expected main loop frame rate.
  CommandHandler*                             m_commandHandler;
  bool                                        m_isSimulating;
};

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_DEFAULTDATAMANAGER_H
//----------------------------------------------------------------------------------------------------------------------
