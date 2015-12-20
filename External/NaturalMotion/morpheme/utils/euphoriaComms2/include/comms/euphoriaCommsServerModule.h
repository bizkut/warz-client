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
#ifndef MCOMMS_EUPHORIACOMMSSERVER_MODULE_H
#define MCOMMS_EUPHORIACOMMSSERVER_MODULE_H
//----------------------------------------------------------------------------------------------------------------------

#include "sharedDefines/mEuphoriaDebugInterface.h"

#include "comms/commsServerModule.h"
#include "comms/euphoriaCommandsHandler.h"

namespace MCOMMS
{
class EuphoriaCommandsHandler;
class EuphoriaDataManagementInterface;
class NetworkDataBuffer;

//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class EuphoriaCommsServerModule : public CommsServerModule
{
public:
  EuphoriaCommsServerModule();
  virtual ~EuphoriaCommsServerModule();

  // CommsServerModule overrides.
  virtual void sendDefData(const GUID& networkGUID, Connection* connection) NM_OVERRIDE;
  virtual void sendInstanceStaticData(const GUID& networkGUID, InstanceID instanceID, Connection* connection) NM_OVERRIDE;
  virtual void sendInstanceFrameData(InstanceID instanceID, Connection* connection) NM_OVERRIDE;
  virtual void sendFrameData(Connection* connection) NM_OVERRIDE;
  virtual void postFrameUpdate() NM_OVERRIDE;
  virtual void preFrameUpdate() NM_OVERRIDE;
  virtual void onStartSession(Connection* connection) NM_OVERRIDE;
  virtual void onStopSession(Connection* connection) NM_OVERRIDE;
  virtual void onConnectionClosed(Connection* connection) NM_OVERRIDE;

protected:
  EuphoriaCommandsHandler m_commandsHandler;

  bool serializeEuphoriaControlFrameData(
    NetworkDataBuffer* networkDataBuffer,
    EuphoriaDataManagementInterface* euphoriaDataManager,
    InstanceID instanceID,
    ER::DebugControlID controlID,
    ER::DebugControlDataType controlDataType,
    uint8_t limbIndex);
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_EUPHORIACOMMSSERVER_MODULE_H
//----------------------------------------------------------------------------------------------------------------------
