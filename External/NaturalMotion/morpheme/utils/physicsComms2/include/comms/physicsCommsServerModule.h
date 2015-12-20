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
#ifndef MCOMMS_PHYSICSCOMMSSERVER_MODULE_H
#define MCOMMS_PHYSICSCOMMSSERVER_MODULE_H
//----------------------------------------------------------------------------------------------------------------------

#include "comms/commsServerModule.h"

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief
//----------------------------------------------------------------------------------------------------------------------
class PhysicsCommsServerModule : public CommsServerModule
{
public:
  friend class CoreCommandsHandler;

  PhysicsCommsServerModule();
  virtual ~PhysicsCommsServerModule();

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
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_PHYSICSCOMMSSERVER_MODULE_H
//----------------------------------------------------------------------------------------------------------------------
