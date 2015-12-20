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
#ifndef ER_SPU_DEFINES_H
#define ER_SPU_DEFINES_H

#include <cell/spurs.h>
#include "NMPlatform/NMPlatform.h"

//----------------------------------------------------------------------------------------------------------------------
namespace ER
{

typedef enum
{
  SPU_RESULT_SHUTTING_DOWN = 0x0002,
} SPUExecuteResult;

//----------------------------------------------------------------------------------------------------------------------
typedef union SPUExeArgument
{
  struct
  {
    // This ensures that this DMA structure is a multiple of 16 bytes
    PPU_POINTER(void) payload __attribute__ ((aligned(16)));
    float timeStep;
    uint32_t pad0;
    uint32_t pad1;
    // This event is sent from SPU to PPU to inform the PPU when the SPU has shut down
    //PPU_POINTER(CellSpursEventFlag) eventSPUShutdown;
  };
#ifdef NM_HOST_CELL_PPU
  CellSpursTaskArgument spursArgument;
#elif defined(NM_HOST_CELL_SPU)
  vec_uint4 uiQWord;
#endif
} SPUArgument;

//----------------------------------------------------------------------------------------------------------------------
NM_ASSERT_COMPILE_TIME(sizeof(SPUExeArgument) == 16);

} // namespace MR
#endif //MR_SPU_DEFINES_H
