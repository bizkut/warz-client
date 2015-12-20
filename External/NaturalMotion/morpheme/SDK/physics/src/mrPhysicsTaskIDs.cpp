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
#include "morpheme/mrCoreTaskIDs.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

#if defined(NM_HOST_CELL_PPU)
extern const char _binary_task_mrSPU_PHYSICS_TRANSITTASKS_elf_start[];
#endif // defined(NM_HOST_CELL_PPU)

namespace MR
{
#if defined(NM_HOST_CELL_PPU)
extern void registerSPUTaskFnOverlayElf(const char* taskFnElfStart, uint8_t overlayID);
#endif
} // namespace MR

#if defined(NM_HOST_CELL_PPU) && defined(NM_COMPILER_SNC)
// Temporary fix for problem with SNC build, disagreement with GCC about namespacing
extern const char _binary_task_mrSPU_PHYSICS_TRANSITTASKS_elf_start[];
#endif // defined(NM_HOST_CELL_PPU) && defined(NM_COMPILER_SNC)

//----------------------------------------------------------------------------------------------------------------------
// N.B. Deliberately outside of namespace due 'extern const char _binary_task_mrSPU_##_taskOverlayName##_elf_start[]'
//----------------------------------------------------------------------------------------------------------------------
bool MR::CoreTaskIDs::registerNMPhysicsTasks(Dispatcher* dispatcher)
{
#ifndef NM_HOST_CELL_SPU

  // Physics Tasks
  MR_DISPATCHERREGISTERFN(TaskApplyPhysicsJointLimitsTransforms,          MR::CoreTaskIDs::MR_TASKID_APPLYPHYSICSJOINTLIMITSTRANSFORMS);
  MR_DISPATCHERREGISTERFN(TaskPhysicsUpdateAnimatedTrajectory,            MR::CoreTaskIDs::MR_TASKID_PHYSICSUPDATEANIMATEDTRAJECTORY);
  MR_DISPATCHERREGISTERFN(TaskPhysicsUpdatePhysicalTrajectoryPostPhysics, MR::CoreTaskIDs::MR_TASKID_PHYSICSUPDATEPHYSICALTRAJECTORYPOSTPHYSICS);
  MR_DISPATCHERREGISTERFN(TaskPhysicsUpdateTransformsPrePhysics,          MR::CoreTaskIDs::MR_TASKID_PHYSICSUPDATETRANSFORMSPREPHYSICS);
  MR_DISPATCHERREGISTERFN(TaskPhysicsUpdateTransformsPostPhysics,         MR::CoreTaskIDs::MR_TASKID_PHYSICSUPDATETRANSFORMSPOSTPHYSICS);
  MR_DISPATCHERREGISTERFN(TaskSetNonPhysicsTransforms,                    MR::CoreTaskIDs::MR_TASKID_SETNONPHYSICSTRANSFORMS);

  MR_DISPATCHERREGISTERFN(TaskExpandLimitsTransforms,                     MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRANSFORMS_EXPANDLIMITS);
  MR_DISPATCHERREGISTERFN(TaskExpandLimitsDeltaAndTransforms,             MR::CoreTaskIDs::MR_TASKID_PASSTHROUGHTRAJECTORYDELTAANDTRANSFORMS_EXPANDLIMITS);

  // Physics Grouper Tasks
  MR_DISPATCHERREGISTERFN(TaskPhysicsGrouperUpdateTransforms,           MR::CoreTaskIDs::MR_TASKID_PHYSICSGROUPERUPDATETRANSFORMS);
  MR_DISPATCHERREGISTERFN(TaskPhysicsGrouperUpdateTransformsPrePhysics, MR::CoreTaskIDs::MR_TASKID_PHYSICSGROUPERUPDATETRANSFORMSPREPHYSICS);
  MR_DISPATCHERREGISTERFN(TaskPhysicsGrouperUpdateTrajectory,           MR::CoreTaskIDs::MR_TASKID_PHYSICSGROUPERUPDATETRAJECTORY);
  MR_DISPATCHERREGISTERFN(TaskPhysicsGrouperGetOutputMaskBase,          MR::CoreTaskIDs::MR_TASKID_PHYSICSGROUPERGETOUPUTMASKBASE);
  MR_DISPATCHERREGISTERFN(TaskPhysicsGrouperGetOutputMaskOverride,      MR::CoreTaskIDs::MR_TASKID_PHYSICSGROUPERGETOUPUTMASKOVERRIDE);

  // Physics Transit Tasks
  MR_DISPATCHERREGISTERFN(TaskPhysicalDeadBlendTransformBuffs, MR::CoreTaskIDs::MR_TASKID_PHYSICALDEADBLENDTRANSFORMBUFFS);

#endif // NM_HOST_CELL_SPU

  MR_REGISTER_PHYSICS_TRANSITTASKS(0);

  return true;
}

//----------------------------------------------------------------------------------------------------------------------
