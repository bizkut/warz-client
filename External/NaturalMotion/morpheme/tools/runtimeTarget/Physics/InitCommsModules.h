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
#ifndef NM_INITCOMMSMODULES_H
#define NM_INITCOMMSMODULES_H
//----------------------------------------------------------------------------------------------------------------------
#include "comms/commsServer.h"

//----------------------------------------------------------------------------------------------------------------------
bool initCommsServerModules(MCOMMS::CommsServer* commsServer);

//----------------------------------------------------------------------------------------------------------------------
bool termCommsServerModules(MCOMMS::CommsServer* commsServer);

//----------------------------------------------------------------------------------------------------------------------
#endif //NM_INITCOMMSMODULES_H
