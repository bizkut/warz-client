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
#include "comms/debugPackets.h"

#include "sharedDefines/mPhysicsDebugInterface.h"

#include "comms/debugDrawObjects.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MCOMMS
{
//----------------------------------------------------------------------------------------------------------------------
/// NodeOutputDataPacket
//----------------------------------------------------------------------------------------------------------------------
void NodeOutputDataPacket::deserialize()
{
  NMP::netEndianSwap(m_dataLength);
  NMP::netEndianSwap(m_nodeOutputType);
  NMP::netEndianSwap(m_owningNodeID);
  NMP::netEndianSwap(m_targetNodeID);
  NMP::netEndianSwap(m_type);
  NMP::netEndianSwap(m_semantic);
  NMP::netEndianSwap(m_semanticNameToken);
  NMP::netEndianSwap(m_animSetIndex);
  NMP::netEndianSwap(m_validFrame);
  NMP::netEndianSwap(m_pinIndex);

  if (m_dataLength == 0)
  {
    return;
  }
  // Swap the data as uint32s
  uint32_t* data = (uint32_t*)(getData());

  switch (m_type)
  {
  case MR::ATTRIB_TYPE_BOOL:
    MR::AttribBoolOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_UINT:
    MR::AttribUIntOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_PHYSICS_OBJECT_POINTER:
    MR::AttribPhysicsObjectPointerOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_INT:
    MR::AttribIntOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_FLOAT:
    MR::AttribFloatOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_VECTOR3:
    MR::AttribVector3OutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_VECTOR4:
    MR::AttribVector4OutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_BOOL_ARRAY:
    MR::AttribBoolArrayOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_UINT_ARRAY:
    MR::AttribUIntArrayOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_UINT64_ARRAY:
    MR::AttribUInt64ArrayOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_INT_ARRAY:
    MR::AttribIntArrayOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_FLOAT_ARRAY:
    MR::AttribFloatArrayOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_UPDATE_PLAYBACK_POS:
    MR::AttribUpdatePlaybackPosOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_PLAYBACK_POS:
    MR::AttribPlaybackPosOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS:
    MR::AttribUpdateSyncEventPlaybackPosOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_TRANSFORM_BUFFER:
    MR::AttribTransformBufferOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM:
    MR::AttribTrajectoryDeltaTransformOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_SYNC_EVENT_TRACK:
    MR::AttribSyncEventTrackOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_DURATION_EVENT_TRACK_SET:
    MR::AttribDurationEventTrackSetOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_VELOCITY:
    MR::AttribVelocityOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER:
    MR::AttribSampledEventsOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_SOURCE_EVENT_TRACKS:
    MR::AttribSourceEventTrackSetOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_STATE_MACHINE:
    MR::AttribStateMachineOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_TRANSIT_SYNC_EVENTS:
    MR::AttribTransitSyncEventsOutputData::deserialiseTx(data);
    break;
  case MR::ATTRIB_TYPE_LOCK_FOOT_STATE:
    MR::AttribLockFootStateOutputData::deserialiseTx(data);
    break;
  default:
    uint32_t numWords = m_dataLength / sizeof(uint32_t);
    NMP_ASSERT((m_dataLength % sizeof(uint32_t)) == 0);
    for (uint32_t i = 0; i < numWords; ++i)
    {
      NMP::netEndianSwap(data[i]);
    }
    break;
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// DebugDrawPacket
//----------------------------------------------------------------------------------------------------------------------
void DebugDrawPacket::deserialize()
{
  NMP::netEndianSwap(m_debugDrawType);
  NMP::netEndianSwap(m_sourceNetworkID);
  NMP::netEndianSwap(m_sourceNodeID);
  NMP::netEndianSwap(m_sourceModuleName);
  NMP::netEndianSwap(m_dataLength);
  NMP::netEndianSwap(m_sourceFrame);
  NMP::netEndianSwap(m_sourceLimbIndex);

  if (m_dataLength == 0)
  {
    return;
  }

  // Swap the data as uint32s
  uint32_t* data = (uint32_t*)(getData());

  // Find out what type of def data it is then swap it.
  switch (m_debugDrawType)
  {
  case DDTYPE_POINT:
    DebugDrawPoint::deserialiseTx(data);
    break;
  case DDTYPE_SPHERE:
    DebugDrawSphere::deserialiseTx(data);
    break;
  case DDTYPE_LINE:
    DebugDrawLine::deserialiseTx(data);
    break;
  case DDTYPE_POLY_LINE:
    DebugDrawPolyLine::deserialiseTx(data);
    break;
  case DDTYPE_VECTOR:
    DebugDrawVector::deserialiseTx(data);
    break;
  case DDTYPE_ARROW_HEAD:
    DebugDrawArrowHead::deserialiseTx(data);
    break;
  case DDTYPE_TWIST_ARC:
    DebugDrawTwistArc::deserialiseTx(data);
    break;
  case DDTYPE_PLANE:
    DebugDrawPlane::deserialiseTx(data);
    break;
  case DDTYPE_TRIANGLE:
    DebugDrawTriangle::deserialiseTx(data);
    break;
  case DDTYPE_MATRIX:
    DebugDrawMatrix::deserialiseTx(data);
    break;
  case DDTYPE_CONE_AND_DIAL:
    DebugDrawConeAndDial::deserialiseTx(data);
    break;
  case DDTYPE_CONTACT_POINT:
    DebugDrawContactPoint::deserialiseTx(data);
    break;
  case DDTYPE_CONTACT_POINT_DETAILED:
    DebugDrawContactPointDetailed::deserialiseTx(data);
    break;
  case DDTYPE_CHARACTER_ROOT:
    DebugDrawCharacterControllerRoot::deserialiseTx(data);
    break;
  case DDTYPE_BOX:
    DebugDrawBox::deserialiseTx(data);
    break;
  case DDTYPE_SPHERE_SWEEP:
    DebugDrawSphereSweep::deserialiseTx(data);
    break;
  case DDTYPE_TEXT:
    DebugDrawText::deserialiseTx(data);
    break;
  case DDTYPE_ENVIRONMENT_PATCH:
    DebugDrawEnvironmentPatch::deserialiseTx(data);
    break;
  default:
    // If the type is not recognised, do the best we can to endian swap the data and send it anyway.
    uint32_t numWords = m_dataLength / sizeof(uint32_t);
    NMP_ASSERT((m_dataLength % sizeof(uint32_t)) == 0);
    for (uint32_t i = 0; i < numWords; ++i)
    {
      NMP::netEndianSwap(data[i]);
    }
    break;
  }
}

} // namespace MCOMMS

//----------------------------------------------------------------------------------------------------------------------
