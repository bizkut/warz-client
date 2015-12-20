// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
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
#ifndef NM_EUPHORIARUNTIMETARGETSIMPLE_H
#define NM_EUPHORIARUNTIMETARGETSIMPLE_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/euphoriaDataManagementInterface.h"
#include "comms/physicsRuntimeTargetSimple.h"
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
/// \class SimpleEuphoriaPhysicsDataManager
/// \brief Implements PhysicsDataManagementInterface.
/// \ingroup MorphemeComms
///
/// A simple implementation of the physics data management interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
///
/// This partial class declares overrides SimplePhysicsDataManager serialization to send back additional euphoria data
/// to connect.
//----------------------------------------------------------------------------------------------------------------------
class SimpleEuphoriaPhysicsDataManager :
  public SimplePhysicsDataManager
{
  // Construction/destruction
protected:

  SimpleEuphoriaPhysicsDataManager();

public:

  virtual ~SimpleEuphoriaPhysicsDataManager() {}

  virtual bool serializeTxPhysicsRigFrameData(InstanceID id, Connection* connection) NM_OVERRIDE;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class SimpleEuphoriaDataManager
/// \brief Implements DataManagementInterface.
/// \ingroup MorphemeComms
///
/// A simple implementation of the euphoria data management interface. Documentation for individual methods has been
/// omitted for brevity but is available on the method declaration of the interface this class is derived from.
///
/// This partial class declares two new pure virtual methods findNetworkByInstanceID() and findNetworkDefByGuid() which
/// provide the methods implemented with access to the client specific ID to network and GUID to network definition
/// translations.
//----------------------------------------------------------------------------------------------------------------------
class SimpleEuphoriaDataManager :
  public EuphoriaDataManagementInterface
{
  // Construction/destruction
protected:

  SimpleEuphoriaDataManager();

public:

  virtual ~SimpleEuphoriaDataManager() {}

  /// \brief Helper method to convert from a instance ID to network instance.
  /// The mapping between the two is entirely defined by the client. It is also expected that the client already
  /// maintains a list of available network instances for general use.
  virtual MR::Network* findNetworkByInstanceID(InstanceID id) const = 0;

  /// \brief Helper method to convert from a network definition GUID to a network definition.
  /// It is expected that the client already maintains a list of available network definitions for general use.
  virtual MR::NetworkDef* findNetworkDefByGuid(const GUID& guid) const = 0;

  virtual uint32_t getModuleCount(InstanceID instanceID) const NM_OVERRIDE;
  virtual const char* getModuleParentName(InstanceID id, uint32_t moduleIndex) const NM_OVERRIDE;
  virtual const char* getModuleName(InstanceID instanceID, uint32_t moduleIndex) const NM_OVERRIDE;

  virtual bool isModuleDebugEnabled(InstanceID instanceID, uint32_t moduleIndex) const NM_OVERRIDE;

  virtual const ER::LimbControlAmounts* getLimbControlAmounts(InstanceID instanceID, uint8_t limbIndex, float& stiffnessFraction) const NM_OVERRIDE;

  virtual bool enableModuleDebug(InstanceID instanceID, uint32_t moduleIndex, bool enable) NM_OVERRIDE;

  virtual uint32_t getDebugControlCount(InstanceID instanceID) NM_OVERRIDE;
  virtual bool getDebugControlInfo(
    InstanceID instanceID,
    uint32_t index,
    const char** controlName,
    ER::DebugControlID* controlId,
    ER::DebugControlFlags* controlFlags,
    ER::DebugControlDataType* controlDataType) NM_OVERRIDE;

  virtual uint32_t getNumControlTypes(InstanceID id) const NM_OVERRIDE;
  virtual const char* getControlTypeName(InstanceID id, uint32_t controlTypeIndex) NM_OVERRIDE;

  virtual bool setDebugControl(InstanceID instanceID, ER::DebugControlID controlId, uint8_t limbIndex, bool value) NM_OVERRIDE;
  virtual bool getDebugControl(InstanceID instanceID, ER::DebugControlID controlId, uint8_t limbIndex, bool* value) NM_OVERRIDE;
  virtual bool setDebugControl(InstanceID instanceID, ER::DebugControlID controlId, uint8_t limbIndex, int32_t value) NM_OVERRIDE;
  virtual bool getDebugControl(InstanceID instanceID, ER::DebugControlID controlId, uint8_t limbIndex, int32_t* value) NM_OVERRIDE;
  virtual bool setDebugControl(InstanceID instanceID, ER::DebugControlID controlId, uint8_t limbIndex, uint32_t value) NM_OVERRIDE;
  virtual bool getDebugControl(InstanceID instanceID, ER::DebugControlID controlId, uint8_t limbIndex, uint32_t* value) NM_OVERRIDE;
  virtual bool setDebugControl(InstanceID instanceID, ER::DebugControlID controlId, uint8_t limbIndex, float value) NM_OVERRIDE;
  virtual bool getDebugControl(InstanceID instanceID, ER::DebugControlID controlId, uint8_t limbIndex, float* value) NM_OVERRIDE;

  virtual uint32_t getLimbCount(InstanceID instanceID) const NM_OVERRIDE;
  virtual uint32_t serializeLimbTx(InstanceID instanceID, uint32_t limbIndex, void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;

  /// \brief serializes details of a Euphoria body.
  uint32_t serializeBodyTx(InstanceID instanceID, void* outputBuffer, uint32_t outputBufferSize) const NM_OVERRIDE;
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // NM_RUNTIMETARGETSIMPLE_H
//----------------------------------------------------------------------------------------------------------------------
