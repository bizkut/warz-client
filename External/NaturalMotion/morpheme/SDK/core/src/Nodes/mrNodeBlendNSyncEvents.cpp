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
#include "morpheme/Nodes/mrNodeBlendNSyncEvents.h"
#include "morpheme/mrCoreTaskIDs.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
NodeID nodeBlendNSyncEventsFindGeneratingNodeForSemantic(
  NodeID              NMP_UNUSED(callingNodeID),
  bool                NMP_UNUSED(fromParent),   // Was this query from a parent or child node.
  AttribDataSemantic  semantic,
  NodeDef*            node,
  Network*            NMP_UNUSED(network))
{
  NodeID result = INVALID_NODE_ID;

  switch (semantic)
  {
    case ATTRIB_SEMANTIC_TIME_POS:
    case ATTRIB_SEMANTIC_UPDATE_SYNC_EVENT_POS:
    case ATTRIB_SEMANTIC_SYNC_EVENT_TRACK:
    case ATTRIB_SEMANTIC_TRANSFORM_BUFFER:
    case ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER:
    case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM:
    case ATTRIB_SEMANTIC_TRAJECTORY_DELTA_TRANSFORM_BUFFER:
    case ATTRIB_SEMANTIC_FRACTION_POS:
    case ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET:
      result = node->getNodeID();
      break;
    default:
      break;
    }

  return result;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
