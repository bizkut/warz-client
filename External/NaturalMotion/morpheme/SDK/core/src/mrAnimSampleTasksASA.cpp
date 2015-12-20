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
#include "morpheme/mrNetwork.h"
#include "morpheme/mrCommonTasks.h"
#include "morpheme/AnimSource/mrAnimSource.h"
#include "morpheme/AnimSource/mrAnimSourceASA.h"
#include "morpheme/mrAnimSampleTasks.inl"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Sample transforms for ASA.
void TaskSampleTransformsFromAnimSourceASA(Dispatcher::TaskParameters* parameters)
{
  subTaskSampleTransformsFromAnimSource<AnimSourceASA>(
    parameters,
    ANIM_TYPE_ASA,
    AnimSourceASA::computeAtTime,
    false);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskUpdateDeltaTrajectoryFromTrajectorySourceASA(Dispatcher::TaskParameters* parameters)
{
  subTaskUpdateDeltaTrajectoryFromTrajectorySource<TrajectorySourceASA>(
    parameters,
    ANIM_TYPE_ASA,
    TrajectorySourceASA::computeTrajectoryTransformAtTime);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskUpdateDeltaTrajectoryAndTransformsFromSourceASA(Dispatcher::TaskParameters* parameters)
{
  subTaskUpdateDeltaTrajectoryAndTransformsFromSource<AnimSourceASA, TrajectorySourceASA>(
    parameters,
    ANIM_TYPE_ASA,
    AnimSourceASA::computeAtTime,
    TrajectorySourceASA::computeTrajectoryTransformAtTime,
    false);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
