
//----------------------------------------------------------------------------------------------------------------------
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_EMIT_MESSAGE_ON_DISCRETE_EVENT_H
#define MR_NODE_EMIT_MESSAGE_ON_DISCRETE_EVENT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// Queuing function declarations
Task* nodeEmitMessageOnDiscreteEventQueueSampledEventsBuffers(
  NodeDef*        node,
  TaskQueue*      queue,
  Network*        net,
  TaskParameter*  dependentParameter);

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_EMIT_MESSAGE_ON_DISCRETE_EVENT_H
//----------------------------------------------------------------------------------------------------------------------

