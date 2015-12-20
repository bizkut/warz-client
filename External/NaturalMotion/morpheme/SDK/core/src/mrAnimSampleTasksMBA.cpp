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
#include "morpheme/mrCommonTasks.h"
#include "morpheme/mrNetwork.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/AnimSource/mrAnimSourceMBA.h"
#include "morpheme/mrAnimSampleTasks.inl"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Sample transforms for MBA.
void TaskSampleTransformsFromAnimSourceMBA(Dispatcher::TaskParameters* parameters)
{
  subTaskSampleTransformsFromAnimSource<AnimSourceMBA>(
    parameters,
    ANIM_TYPE_MBA,
    AnimSourceMBA::computeAtTime,
    false);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskUpdateDeltaTrajectoryFromTrajectorySourceMBA(Dispatcher::TaskParameters* parameters)
{
  subTaskUpdateDeltaTrajectoryFromTrajectorySource<TrajectorySourceMBA>(
    parameters,
    ANIM_TYPE_MBA,
    TrajectorySourceMBA::computeTrajectoryTransformAtTime);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskUpdateDeltaTrajectoryAndTransformsFromSourceMBA(Dispatcher::TaskParameters* parameters)
{
  subTaskUpdateDeltaTrajectoryAndTransformsFromSource<AnimSourceMBA, TrajectorySourceMBA>(
    parameters,
    ANIM_TYPE_MBA,
    AnimSourceMBA::computeAtTime,
    TrajectorySourceMBA::computeTrajectoryTransformAtTime,
    false);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
