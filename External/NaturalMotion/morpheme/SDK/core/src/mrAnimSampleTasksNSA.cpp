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
#include "morpheme/AnimSource/mrAnimSourceNSA.h"
#include "morpheme/mrAnimSampleTasks.inl"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Sample transforms for NSA.
void TaskSampleTransformsFromAnimSourceNSA(Dispatcher::TaskParameters* parameters)
{
  subTaskSampleTransformsFromAnimSource<AnimSourceNSA>(
    parameters,
    ANIM_TYPE_NSA,
    AnimSourceNSA::computeAtTime,
    false);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskUpdateDeltaTrajectoryFromTrajectorySourceNSA(Dispatcher::TaskParameters* parameters)
{
  subTaskUpdateDeltaTrajectoryFromTrajectorySource<TrajectorySourceNSA>(
    parameters,
    ANIM_TYPE_NSA,
    TrajectorySourceNSA::computeTrajectoryTransformAtTime);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskUpdateDeltaTrajectoryAndTransformsFromSourceNSA(Dispatcher::TaskParameters* parameters)
{
  subTaskUpdateDeltaTrajectoryAndTransformsFromSource<AnimSourceNSA, TrajectorySourceNSA>(
    parameters,
    ANIM_TYPE_NSA,
    AnimSourceNSA::computeAtTime,
    TrajectorySourceNSA::computeTrajectoryTransformAtTime,
    false);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
