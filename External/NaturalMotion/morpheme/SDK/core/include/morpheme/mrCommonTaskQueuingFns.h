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
#ifndef MR_COMMON_TASK_QUEUING_FNS
#define MR_COMMON_TASK_QUEUING_FNS
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Common queuing function declarations
Task* queueInitUnitLengthSyncEventTrack(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* queueInitEmptyEventTrackDurationSet(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);
Task* queueInitSampledEventsTrack(NodeDef* node, TaskQueue* queue, Network* net, TaskParameter* dependentParameter);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_COMMON_TASK_QUEUING_FNS
//----------------------------------------------------------------------------------------------------------------------
