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
#ifndef MCOMMS_EUPHORIACOMMANDSHANDLER_H
#define MCOMMS_EUPHORIACOMMANDSHANDLER_H
//----------------------------------------------------------------------------------------------------------------------
#include "NMPlatform/NMPlatform.h"

#include "comms/commandsHandler.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// \brief Handles all euphoria related commands.
//----------------------------------------------------------------------------------------------------------------------
class EuphoriaCommandsHandler : public CommandsHandler
{
public:
  EuphoriaCommandsHandler(RuntimeTargetInterface* target);
  virtual ~EuphoriaCommandsHandler();

  virtual bool doHandleCommand(CmdPacketBase* commandPacket) NM_OVERRIDE;
  virtual void clear() NM_OVERRIDE;

private:
  bool handleEnableModuleDebug(CmdPacketBase* baseCommandPacket);
  bool handleSetEuphoriaDebugControl(CmdPacketBase* baseCommandPacket);
};

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
#endif // MCOMMS_EUPHORIACOMMANDSHANDLER_H
//----------------------------------------------------------------------------------------------------------------------
