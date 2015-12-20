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
#include "morpheme/AnimSource/mrAnimSourceQSA.h"
#include "morpheme/mrAnimSampleTasks.inl"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Sample transforms for QSA.
void TaskSampleTransformsFromAnimSourceQSA(Dispatcher::TaskParameters* parameters)
{
  subTaskSampleTransformsFromAnimSource<AnimSourceQSA>(
    parameters,
    ANIM_TYPE_QSA,
    AnimSourceQSA::computeAtTime,
    true);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskUpdateDeltaTrajectoryFromTrajectorySourceQSA(Dispatcher::TaskParameters* parameters)
{
  subTaskUpdateDeltaTrajectoryFromTrajectorySource<TrajectorySourceQSA>(
    parameters,
    ANIM_TYPE_QSA,
    TrajectorySourceQSA::computeTrajectoryTransformAtTime);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskUpdateDeltaTrajectoryAndTransformsFromSourceQSA(Dispatcher::TaskParameters* parameters)
{
  subTaskUpdateDeltaTrajectoryAndTransformsFromSource<AnimSourceQSA, TrajectorySourceQSA>(
    parameters,
    ANIM_TYPE_QSA,
    AnimSourceQSA::computeAtTime,
    TrajectorySourceQSA::computeTrajectoryTransformAtTime,
    true);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
