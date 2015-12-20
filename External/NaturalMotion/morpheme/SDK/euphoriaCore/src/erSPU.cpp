// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#include "euphoria/erSPU.h"

#ifndef NM_HOST_CELL_PPU
  #error "erSPU.cpp is PS3-PPU only!"
#endif // NM_HOST_CELL_PPU

#include "NMPlatform/ps3/NMSPUManager.h"
#include "euphoria/erSPUDefines.h"

namespace ER
{

CellSpursEventFlag SPU::m_eventSPUShutdown __attribute__ ((aligned(CELL_SPURS_EVENT_FLAG_ALIGN)));

//----------------------------------------------------------------------------------------------------------------------
void SPU::init(uint32_t numSpus)
{
  // Init Spurs if the app hasn't already done it
  if (!NMP::SPUManager::initialised())
  {
    NMP::SPUManager::init(numSpus);
  }

  NMP::SPUManager::setupEventFlag(&m_eventSPUShutdown, false);
}

//----------------------------------------------------------------------------------------------------------------------
void SPU::term()
{
  NMP::SPUManager::termEventFlag(&m_eventSPUShutdown);
}

//----------------------------------------------------------------------------------------------------------------------
void SPU::LaunchSPUTask(void* payload, float timeStep, void* codePackage)
{
  SPUArgument spuArgs __attribute__ ((aligned(CELL_SPURS_EVENT_FLAG_ALIGN)));

  spuArgs.payload = payload;
  spuArgs.timeStep = timeStep;
  //spuArgs.eventSPUShutdown = &m_eventSPUShutdown;

  CellSpursTaskId taskId;
#ifdef NMP_ENABLE_ASSERTS
  int createResult =
#endif // NMP_ENABLE_ASSERTS
  cellSpursCreateTask2(NMP::SPUManager::getSpursTaskset(), &taskId,
    codePackage, &(spuArgs.spursArgument), NULL);

  NMP_ASSERT_MSG(createResult == CELL_OK, "Error starting euphoria task: %i\n", createResult);

//  uint16_t uiMask = SPU_RESULT_SHUTTING_DOWN;
//#ifdef NMP_ENABLE_ASSERTS
//  int waitResult =
//#endif // NMP_ENABLE_ASSERTS
//  cellSpursEventFlagWait(&m_eventSPUShutdown,
//    &uiMask, CELL_SPURS_EVENT_FLAG_OR);
//  NMP_ASSERT_MSG(waitResult == CELL_OK, "Network task completion event flag receive wasn't successful\n");

  // Join the task
  int exitCode = 0;
#ifdef NMP_ENABLE_ASSERTS
  uint32_t cellResult =
#endif // NM_DEBUG
    cellSpursJoinTask2(NMP::SPUManager::getSpursTaskset(), taskId, &exitCode);
  NMP_ASSERT_MSG((cellResult == CELL_OK), "cellSpursJoinTask2 failed %x", cellResult);

  cellSpursEventFlagClear(&m_eventSPUShutdown, 0xFFFF);
}

}
