// Copyright (c) 2012 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetworkLogger.h"
#include "morpheme/mrManager.h"
//----------------------------------------------------------------------------------------------------------------------

#if defined(NM_HOST_CELL_PPU)
  #include "../../SPU/mrSPUTaskObjects.h"
#endif // defined(NM_HOST_CELL_PPU)

//----------------------------------------------------------------------------------------------------------------------
// NMCoreTaskIDs
//----------------------------------------------------------------------------------------------------------------------
namespace MR
{
#if defined(NM_HOST_CELL_PPU)
extern void registerSPUTaskFnOverlayElf(const char* taskFnElfStart, uint8_t overlayID);
#endif // defined(NM_HOST_CELL_PPU)

#if !defined(NM_HOST_CELL_SPU)
//----------------------------------------------------------------------------------------------------------------------
TaskID CoreTaskIDs::getCreateReferenceToInputTaskID(MR::AttribDataSemantic semantic)
{
  return MR::Manager::getInstance().getCreateReferenceToInputTaskID(semantic);
}
#endif // !defined(NM_HOST_CELL_SPU)

//----------------------------------------------------------------------------------------------------------------------
bool CoreTaskIDs::registerNMCoreTasks(Dispatcher* dispatcher)
{
  MR::Manager& manager = MR::Manager::getInstance();

  // Initialize default attrib semantic to reference task ID mappings
  const uint32_t numEntries = manager.getNumRegisteredAttribSemantics();
  for (uint32_t i = 0; i < numEntries; ++i)
  {
    manager.setCreateReferenceToInputTaskID(i, MR_TASKID_MAX + i); // These come after all statically defined task IDs.
  }

  // Register node tasks
  MR_REGISTER_ANIMSAMPLETASKSASA(1);
  MR_REGISTER_ANIMSAMPLETASKSMBA(1);
  MR_REGISTER_ANIMSAMPLETASKSNSA(1);
  MR_REGISTER_ANIMSAMPLETASKSQSA(1);
  MR_REGISTER_TRAJECTORYSAMPLETASKSASA(1);
  MR_REGISTER_TRAJECTORYSAMPLETASKSMBA(1);
  MR_REGISTER_TRAJECTORYSAMPLETASKSNSA(1);
  MR_REGISTER_TRAJECTORYSAMPLETASKSQSA(1);
  MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSASA(1);
  MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSMBA(1);
  MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSNSA(1);
  MR_REGISTER_TRAJECTORYANDTRANSFORMSSAMPLETASKSQSA(1);
  MR_REGISTER_BLENDTRANSFORMTASKS(1);
  MR_REGISTER_BLENDTRAJECTORYANDTRANSFORMSTASKS(1);
  MR_REGISTER_BLENDTRANSFORMSPASSDESTTRAJTRASKS(1);
  MR_REGISTER_BLENDTRANSFORMSPASSSOURCETRAJTRASKS(1);
  MR_REGISTER_BLENDALLTASKS(1);
  MR_REGISTER_COMMONTASKS(1);
  MR_REGISTER_EXTRACTJOINTINFOTASKS(1);
  MR_REGISTER_EXTRACTJOINTINFOTRAJECTORYANDTRANSFORMSTASKS(1);
  MR_REGISTER_EVENTTRACKDURATIONTASKS(1);
  MR_REGISTER_MIRRORTASKS(1);
  MR_REGISTER_FOOTLOCKIKTASKS(1);
  MR_REGISTER_FOOTLOCKTRAJECTORYANDTRANSFORMIKTASKS(1);
  MR_REGISTER_GUNAIMIKTASKS(1);
  MR_REGISTER_GUNAIMTRAJECTORYANDTRANSFORMSIKTASKS(1);
  MR_REGISTER_HEADLOOKIKTASKS(1);
  MR_REGISTER_HEADLOOKTRAJECTORYANDTRANSFORMSIKTASKS(1);
  MR_REGISTER_TRAJECTORYBLENDTASKS(1);
  MR_REGISTER_FEATHERTRAJECTORYANDTRANSFORMBLENDTASKS(1);
  MR_REGISTER_TRANSITTASKS(1);
  MR_REGISTER_TWOBONEIKTASKS(1);
  MR_REGISTER_HIPSIKTASKS(1);
  MR_REGISTER_MODIFYTRANSFORMTASKS(1);
  MR_REGISTER_RETARGETTASKS(1);
  MR_REGISTER_BASICUNEVENTERRAINIKTRANSFORMTASKS(1);
  MR_REGISTER_BASICUNEVENTERRAINIKSETUPTASKS(1);
  MR_REGISTER_PREDICTIVEUNEVENTERRAINIKSETUPTASKS(1);
  MR_REGISTER_BLEND2x2TASKS(1);
  MR_REGISTER_TRIANGLEBLENDTASKS(1);
  MR_REGISTER_SCALETASKS(1);

#ifndef NM_HOST_CELL_SPU
  // Basic uneven terrain tasks that can't run on SPU
  MR_DISPATCHERREGISTERFN(TaskBasicUnevenTerrainFootLiftingTarget, MR::CoreTaskIDs::MR_TASKID_BASIC_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET);
  
  // Predictive uneven terrain tasks that can't run on SPU
  MR_DISPATCHERREGISTERFN(TaskPredictiveUnevenTerrainFootLiftingTarget, MR::CoreTaskIDs::MR_TASKID_PREDICTIVE_UNEVEN_TERRAIN_FOOT_LIFTING_TARGET);

  // Deadblend Transit Tasks
  MR_DISPATCHERREGISTERFN(TaskDeadBlendCacheState, MR::CoreTaskIDs::MR_TASKID_DEADBLENDCACHESTATE);
#endif

  // Register common reference tasks. Primarily of importance to attributes that are passed between nodes.
  MR_REGISTER_REFERENCETASKS(1);

  return true;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
