// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef ER_SPU_H
#define ER_SPU_H

#include "NMPlatform/NMPlatform.h"
#include <cell/spurs.h>

namespace ER
{

class SPU
{
public:
  static void init(uint32_t numSpus = 5);
  static void term();

  static void LaunchSPUTask(void* payload, float timeStep, void* codePackage);

protected:
  // This event is fired by the SPU when it shuts down.  It will set different flags depending on what
  //  the status was when it shut down.
  static CellSpursEventFlag m_eventSPUShutdown __attribute__ ((aligned(CELL_SPURS_EVENT_FLAG_ALIGN)));
};

}

#endif // ER_SPU_H
