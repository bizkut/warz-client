// Copyright (c) 2009 NaturalMotion.  All Rights Reserved.
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
# pragma once
#endif
#ifndef M_CORE_DEBUG_INTERFACE_INL
#define M_CORE_DEBUG_INTERFACE_INL
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
/// \file mCoreDebugInterface.inl
/// \brief 
/// \ingroup CoreModule
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

// ensure all these structs are the same layout for all compilers
#ifndef NM_HOST_CELL_SPU
  #pragma pack(push, 4)
#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
/// AttribBoolOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribBoolOutputData *AttribBoolOutputData::endianSwap(void *buffer)
{
  AttribBoolOutputData *data = (AttribBoolOutputData *) buffer;
  NMP::netEndianSwap(data->m_value);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribBoolOutputData *AttribBoolOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribUIntOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribUIntOutputData *AttribUIntOutputData::endianSwap(void *buffer)
{
  AttribUIntOutputData *data = (AttribUIntOutputData *) buffer;
  NMP::netEndianSwap(data->m_value);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribUIntOutputData *AttribUIntOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribPhysicsObjectPointerOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribPhysicsObjectPointerOutputData *AttribPhysicsObjectPointerOutputData::endianSwap(void *buffer)
{
  AttribPhysicsObjectPointerOutputData *data = (AttribPhysicsObjectPointerOutputData *) buffer;
  NMP::netEndianSwap(data->m_value);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribPhysicsObjectPointerOutputData *AttribPhysicsObjectPointerOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribIntOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribIntOutputData *AttribIntOutputData::endianSwap(void *buffer)
{
  AttribIntOutputData *data = (AttribIntOutputData *) buffer;
  NMP::netEndianSwap(data->m_value);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribIntOutputData *AttribIntOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribFloatOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribFloatOutputData *AttribFloatOutputData::endianSwap(void *buffer)
{
  AttribFloatOutputData *data = (AttribFloatOutputData *) buffer;
  NMP::netEndianSwap(data->m_value);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribFloatOutputData *AttribFloatOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribVector3OutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribVector3OutputData *AttribVector3OutputData::endianSwap(void *buffer)
{
  AttribVector3OutputData *data = (AttribVector3OutputData *) buffer;
  NMP::netEndianSwap(data->m_value);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribVector3OutputData *AttribVector3OutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribVector4OutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribVector4OutputData *AttribVector4OutputData::endianSwap(void *buffer)
{
  AttribVector4OutputData *data = (AttribVector4OutputData *) buffer;
  NMP::netEndianSwap(data->m_value);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribVector4OutputData *AttribVector4OutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribBoolArrayOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool AttribBoolArrayOutputData::getValue(uint32_t index)
{
  NMP_ASSERT(index < m_numValues);
  return ((bool*)(this + 1))[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool *AttribBoolArrayOutputData::getValues()
{
  return ((bool*)(this + 1));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribBoolArrayOutputData *AttribBoolArrayOutputData::deserialiseTx(void *buffer)
{
  AttribBoolArrayOutputData *data = (AttribBoolArrayOutputData *) buffer;
  NMP::netEndianSwap(data->m_numValues);
  for(uint32_t i = 0; i < data->m_numValues; ++i)
  {
    NMP::netEndianSwap(data->getValues()[i]);
  }
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribIntArrayOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t AttribIntArrayOutputData::getValue(uint32_t index)
{
  NMP_ASSERT(index < m_numValues);
  return ((int32_t*)(this+1))[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE int32_t *AttribIntArrayOutputData::getValues()
{
  return ((int32_t*)(this+1));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribIntArrayOutputData *AttribIntArrayOutputData::deserialiseTx(void *buffer)
{
  AttribIntArrayOutputData *data = (AttribIntArrayOutputData *) buffer;
  NMP::netEndianSwap(data->m_numValues);
  for(uint32_t i = 0; i < data->m_numValues; ++i)
  {
    NMP::netEndianSwap(data->getValues()[i]);
  }
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribUIntArrayOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t AttribUIntArrayOutputData::getValue(uint32_t index)
{
  NMP_ASSERT(index < m_numValues);
  return ((uint32_t*)(this+1))[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint32_t *AttribUIntArrayOutputData::getValues()
{
  return ((uint32_t*)(this+1));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribUIntArrayOutputData *AttribUIntArrayOutputData::deserialiseTx(void *buffer)
{
  AttribUIntArrayOutputData *data = (AttribUIntArrayOutputData *) buffer;
  NMP::netEndianSwap(data->m_numValues);
  for(uint32_t i = 0; i < data->m_numValues; ++i)
  {
    NMP::netEndianSwap(data->getValues()[i]);
  }
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribUInt64ArrayOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint64_t AttribUInt64ArrayOutputData::getValue(uint32_t index)
{
  NMP_ASSERT(index < m_numValues);
  return ((uint64_t*)(this+1))[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE uint64_t *AttribUInt64ArrayOutputData::getValues()
{
  return ((uint64_t*)(this+1));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribUInt64ArrayOutputData *AttribUInt64ArrayOutputData::deserialiseTx(void *buffer)
{
  AttribUInt64ArrayOutputData *data = (AttribUInt64ArrayOutputData *) buffer;
  NMP::netEndianSwap(data->m_numValues);
  for(uint32_t i = 0; i < data->m_numValues; ++i)
  {
    NMP::netEndianSwap(data->getValues()[i]);
  }
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribFloatArrayOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float AttribFloatArrayOutputData::getValue(uint32_t index)
{
  NMP_ASSERT(index < m_numValues);
  return ((float*)(this+1))[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float *AttribFloatArrayOutputData::getValues()
{
  return ((float*)(this+1));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribFloatArrayOutputData *AttribFloatArrayOutputData::deserialiseTx(void *buffer)
{
  AttribFloatArrayOutputData *data = (AttribFloatArrayOutputData *) buffer;
  NMP::netEndianSwap(data->m_numValues);
  for (uint32_t i = 0; i < data->m_numValues; ++i)
  {
    NMP::netEndianSwap(data->getValues()[i]);
  }
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribUpdatePlaybackPosOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribUpdatePlaybackPosOutputData *AttribUpdatePlaybackPosOutputData::endianSwap(void *buffer)
{
  AttribUpdatePlaybackPosOutputData *data = (AttribUpdatePlaybackPosOutputData *) buffer;
  NMP::netEndianSwap(data->m_isFraction);
  NMP::netEndianSwap(data->m_value);
  NMP::netEndianSwap(data->m_isAbs);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribUpdatePlaybackPosOutputData *AttribUpdatePlaybackPosOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribPlaybackPosOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribPlaybackPosOutputData *AttribPlaybackPosOutputData::endianSwap(void *buffer)
{
  AttribPlaybackPosOutputData *data = (AttribPlaybackPosOutputData *) buffer;
  NMP::netEndianSwap(data->m_currentPosAdj);
  NMP::netEndianSwap(data->m_previousPosAdj);
  NMP::netEndianSwap(data->m_currentPosReal);
  NMP::netEndianSwap(data->m_previousPosReal);
  NMP::netEndianSwap(data->m_delta);
  NMP::netEndianSwap(data->m_setWithAbs);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribPlaybackPosOutputData *AttribPlaybackPosOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribUpdateSyncEventPlaybackPosOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribUpdateSyncEventPlaybackPosOutputData *AttribUpdateSyncEventPlaybackPosOutputData::endianSwap(void *buffer)
{
  AttribUpdateSyncEventPlaybackPosOutputData *data = (AttribUpdateSyncEventPlaybackPosOutputData *) buffer;
  NMP::netEndianSwap(data->m_absPosAdjIndex);
  NMP::netEndianSwap(data->m_absPosAdjFraction);
  NMP::netEndianSwap(data->m_absPosRealIndex);
  NMP::netEndianSwap(data->m_absPosRealFraction);
  NMP::netEndianSwap(data->m_deltaPosIndex);
  NMP::netEndianSwap(data->m_deltaPosFraction);
  NMP::netEndianSwap(data->m_isAbs);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribUpdateSyncEventPlaybackPosOutputData *AttribUpdateSyncEventPlaybackPosOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribTransformBufferOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE NMP::BitArray *AttribTransformBufferOutputData::getUsedFlags()
{
  return ((NMP::BitArray *)(((char *)this) + m_usedFlagsPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribTransformBufferOutputData::Transform *AttribTransformBufferOutputData::getTransforms()
{
  return ((Transform *)(((char *)this) + m_transformsPtrOffset));
}
 
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribTransformBufferOutputData *AttribTransformBufferOutputData::deserialiseTx(void *buffer)
{
  AttribTransformBufferOutputData *data = (AttribTransformBufferOutputData *) buffer;

  NMP::netEndianSwap(data->m_numTransforms);
  NMP::netEndianSwap(data->m_usedFlagsPtrOffset);
  NMP::netEndianSwap(data->m_transformsPtrOffset);
  
  // Endian swap the BitArray and Transforms data using the old method for now.
  uint32_t transformSize = (uint32_t) sizeof(AttribTransformBufferOutputData::Transform);
  uint32_t numWords = (uint32_t) (NMP::Memory::align(
                                     NMP::BitArray::getMemoryRequirements(data->m_numTransforms).size + (transformSize * data->m_numTransforms), 
                                     NMP_NATURAL_TYPE_ALIGNMENT)
                                     / 4);
  uint32_t *block = (uint32_t *)(data->getUsedFlags());
  NMP::netEndianSwapArray(block, numWords);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribTrajectoryDeltaTransformOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribTrajectoryDeltaTransformOutputData *AttribTrajectoryDeltaTransformOutputData::endianSwap(void *buffer)
{
  AttribTrajectoryDeltaTransformOutputData *data = (AttribTrajectoryDeltaTransformOutputData *) buffer;
  NMP::netEndianSwap(data->m_deltaPos);
  NMP::netEndianSwap(data->m_deltaAtt);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribTrajectoryDeltaTransformOutputData *AttribTrajectoryDeltaTransformOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribTransformOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribTransformOutputData *AttribTransformOutputData::endianSwap(void *buffer)
{
  AttribTransformOutputData *data = (AttribTransformOutputData *) buffer;
  NMP::netEndianSwap(data->m_pos);
  NMP::netEndianSwap(data->m_att);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribTransformOutputData *AttribTransformOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribUIntOutputData::Event
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribSyncEventTrackOutputData::Event::endianSwap()
{
  NMP::netEndianSwap(m_startTime);
  NMP::netEndianSwap(m_duration);
  NMP::netEndianSwap(m_userData);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribSyncEventTrackOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSyncEventTrackOutputData::Event *AttribSyncEventTrackOutputData::getEvent(uint32_t index)
{
  NMP_ASSERT(index < m_numEvents);
  return &(((Event*)(this + 1))[index]);
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSyncEventTrackOutputData::Event *AttribSyncEventTrackOutputData::getEvents()
{
  return ((Event*)(this + 1));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSyncEventTrackOutputData *AttribSyncEventTrackOutputData::deserialiseTx(void *buffer)
{
  AttribSyncEventTrackOutputData *data = (AttribSyncEventTrackOutputData *) buffer;

  NMP::netEndianSwap(data->m_startEventIndex);
  NMP::netEndianSwap(data->m_numEvents);
  NMP::netEndianSwap(data->m_duration);
    
  for(uint32_t i = 0; i < data->m_numEvents; ++i)
  {
    data->getEvent(i)->endianSwap();
  }
    
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribSampledEventsOutputData::TriggeredEvent
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribSampledEventsOutputData::TriggeredEvent::endianSwap()
{
  NMP::netEndianSwap(m_sourceTrackUserData);
  NMP::netEndianSwap(m_sourceEventUserData);
  NMP::netEndianSwap(m_blendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribSampledEventsOutputData::SampledEvent
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribSampledEventsOutputData::SampledEvent::endianSwap()
{
  NMP::netEndianSwap(m_sourceTrackUserData);
  NMP::netEndianSwap(m_sourceEventUserData);
  NMP::netEndianSwap(m_value);
  NMP::netEndianSwap(m_blendWeight);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribSampledEventsOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSampledEventsOutputData::TriggeredEvent *AttribSampledEventsOutputData::getTriggeredDiscreteEvents()
{
  return ((TriggeredEvent *)(((char *)this) + m_triggeredDiscreteEventsPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSampledEventsOutputData::SampledEvent *AttribSampledEventsOutputData::getSampledCurveEvents()
{
  return ((SampledEvent *)(((char *)this) + m_sampledCurveEventsPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSampledEventsOutputData *AttribSampledEventsOutputData::deserialiseTx(void *buffer)
{
  AttribSampledEventsOutputData *data = (AttribSampledEventsOutputData *) buffer;

  NMP::netEndianSwap(data->m_numTriggeredDiscreteEvents);
  NMP::netEndianSwap(data->m_triggeredDiscreteEventsPtrOffset);
  NMP::netEndianSwap(data->m_numSampledCurveEvents);
  NMP::netEndianSwap(data->m_sampledCurveEventsPtrOffset);

  TriggeredEvent *triggeredEvents = data->getTriggeredDiscreteEvents();
  for (uint32_t i = 0; i < data->m_numTriggeredDiscreteEvents; ++i)
  {
    triggeredEvents[i].endianSwap();
  }

  SampledEvent *sampledEvents = data->getSampledCurveEvents();
  for (uint32_t i = 0; i < data->m_numSampledCurveEvents; ++i)
  {
    sampledEvents[i].endianSwap();
  }

  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribDurationEventTrackSetOutputData::Event
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDurationEventTrackSetOutputData::Event::endianSwap()
{
  NMP::netEndianSwap(m_userData);
  NMP::netEndianSwap(m_syncEventSpaceStartPoint);
  NMP::netEndianSwap(m_syncEventSpaceMidPoint);
  NMP::netEndianSwap(m_syncEventSpaceDuration);
  NMP::netEndianSwap(m_normalisedStartPoint);
  NMP::netEndianSwap(m_normalisedDuration); 
  NMP::netEndianSwap(m_weight);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribDurationEventTrackSetOutputData::Track
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDurationEventTrackSetOutputData::Event *AttribDurationEventTrackSetOutputData::Track::getEvents()
{
  return ((Event *)(((char *)this) + m_eventsPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribDurationEventTrackSetOutputData::Track::deserialiseTx()
{
  NMP::netEndianSwap(m_numEvents);
  NMP::netEndianSwap(m_runtimeID);
  NMP::netEndianSwap(m_userData);
  NMP::netEndianSwap(m_numSyncEvents);
  NMP::netEndianSwap(m_eventsPtrOffset);
  Event *events = getEvents();
  for(uint32_t i = 0; i < m_numEvents; ++i)
  {
    events[i].endianSwap();
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribDurationEventTrackSetOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDurationEventTrackSetOutputData::Track *AttribDurationEventTrackSetOutputData::getTracks()
{
  return ((Track *)(((char *)this) + m_tracksPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDurationEventTrackSetOutputData *AttribDurationEventTrackSetOutputData::deserialiseTx(void *buffer)
{
  AttribDurationEventTrackSetOutputData *data = (AttribDurationEventTrackSetOutputData *) buffer;

  NMP::netEndianSwap(data->m_numEventTracks);
  NMP::netEndianSwap(data->m_tracksPtrOffset);
  Track *tracks = data->getTracks();
  for(uint32_t i = 0; i < data->m_numEventTracks; ++i)
  {
    tracks[i].deserialiseTx();
  }
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribVelocityOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribVelocityOutputData *AttribVelocityOutputData::endianSwap(void *buffer)
{
  AttribVelocityOutputData *data = (AttribVelocityOutputData *) buffer;
  NMP::netEndianSwap(data->m_linearVel);
  NMP::netEndianSwap(data->m_angularVel);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribVelocityOutputData *AttribVelocityOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribSourceDiscreteEventTrackSetOutputData::Event
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribSourceEventTrackSetOutputData::Event::endianSwap()
{
  NMP::netEndianSwap(m_startTime);
  NMP::netEndianSwap(m_userData);
}

//----------------------------------------------------------------------------------------------------------------------
// AttribSourceDiscreteEventTrackSetOutputData::Track
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSourceEventTrackSetOutputData::Event *AttribSourceEventTrackSetOutputData::Track::getEvents()
{
  return ((Event *)(((char *)this) + m_eventsPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribSourceEventTrackSetOutputData::Track::deserialiseTx()
{
  NMP::netEndianSwap(m_numEvents);
  NMP::netEndianSwap(m_userData);
  NMP::netEndianSwap(m_eventsPtrOffset);
  Event *events = getEvents();
  for(uint32_t i = 0; i < m_numEvents; ++i)
  {
    events[i].endianSwap();
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribSourceEventTrackSetOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSourceEventTrackSetOutputData::Track *AttribSourceEventTrackSetOutputData::getTracks()
{
  return ((Track *)(((char *)this) + m_tracksPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribSourceEventTrackSetOutputData *AttribSourceEventTrackSetOutputData::deserialiseTx(void *buffer)
{
  AttribSourceEventTrackSetOutputData *data = (AttribSourceEventTrackSetOutputData *) buffer;

  NMP::netEndianSwap(data->m_numEventTracks);
  NMP::netEndianSwap(data->m_tracksPtrOffset);
  Track *tracks = data->getTracks();
  for(uint32_t i = 0; i < data->m_numEventTracks; ++i)
  {
    tracks[i].deserialiseTx();
  }
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribStateMachineOutputData::Condition
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribStateMachineOutputData::Condition::endianSwap()
{
  NMP::netEndianSwap(m_satisfied);
  NMP::netEndianSwap(m_type);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribStateMachineOutputData::PossibleTransition
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribStateMachineOutputData::Condition *AttribStateMachineOutputData::PossibleTransition::getConditions()
{
  return ((Condition *)(((char *)this) + m_conditionsPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE void AttribStateMachineOutputData::PossibleTransition::deserialiseTx()
{
  NMP::netEndianSwap(m_transitionStateID);
  NMP::netEndianSwap(m_transitionNodeID);
  NMP::netEndianSwap(m_numConditions);
  NMP::netEndianSwap(m_conditionsPtrOffset);
  Condition *conditions = getConditions();
  for(uint32_t i = 0; i < m_numConditions; ++i)
  {
    conditions[i].endianSwap();
  }
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribStateMachineOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribStateMachineOutputData::PossibleTransition *AttribStateMachineOutputData::getPossibleTransitions()
{
  return ((PossibleTransition *)(((char *)this) + m_possibleTransitionPtrOffset));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribStateMachineOutputData *AttribStateMachineOutputData::deserialiseTx(void *buffer)
{
  AttribStateMachineOutputData *data = (AttribStateMachineOutputData *) buffer;
  NMP::netEndianSwap(data->m_activeStateID);
  NMP::netEndianSwap(data->m_targetStateID);
  NMP::netEndianSwap(data->m_activeNodeID);
  NMP::netEndianSwap(data->m_targetNodeID);
  NMP::netEndianSwap(data->m_numActiveConditions);
  NMP::netEndianSwap(data->m_numPossibleTransitions);
  NMP::netEndianSwap(data->m_possibleTransitionPtrOffset);
  NMP::netEndianSwap(data->m_active);
  
  PossibleTransition *trasitions = data->getPossibleTransitions();
  for(uint32_t i = 0; i < data->m_numPossibleTransitions; ++i)
  {
    trasitions[i].deserialiseTx();
  }
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribTransitSyncEventsOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribTransitSyncEventsOutputData *AttribTransitSyncEventsOutputData::endianSwap(void *buffer)
{
  AttribTransitSyncEventsOutputData *data = (AttribTransitSyncEventsOutputData *) buffer;
  NMP::netEndianSwap(data->m_transitionEventCount);
  NMP::netEndianSwap(data->m_completed);
  NMP::netEndianSwap(data->m_reversed);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribTransitSyncEventsOutputData *AttribTransitSyncEventsOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribLockFootStateOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribLockFootStateOutputData *AttribLockFootStateOutputData::endianSwap(void *buffer)
{
  AttribLockFootStateOutputData *data = (AttribLockFootStateOutputData *) buffer;
  NMP::netEndianSwap(data->m_trajectoryPos);
  NMP::netEndianSwap(data->m_trajectoryAtt);
  NMP::netEndianSwap(data->m_previousEndEffectorPosition);
  NMP::netEndianSwap(data->m_lockPosition);
  NMP::netEndianSwap(data->m_tracking);
  NMP::netEndianSwap(data->m_previouslyInFootStep);
  NMP::netEndianSwap(data->m_firstFootStep);
  return data;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribLockFootStateOutputData *AttribLockFootStateOutputData::deserialiseTx(void *buffer)
{
  return endianSwap(buffer);
}

//----------------------------------------------------------------------------------------------------------------------
/// AttribDataBlendNxMOutputData
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float AttribDataBlendNxMOutputData::getValueX(uint32_t index)
{
  NMP_ASSERT(index < m_blendX);
  return ((float*)(this+2))[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float AttribDataBlendNxMOutputData::getValueY(uint32_t index)
{
  NMP_ASSERT(index < m_blendY);
  return ((float*)(this + 2 + m_blendX))[index];
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float *AttribDataBlendNxMOutputData::getValueWeightsX()
{
  return ((float*)(this+2));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE float *AttribDataBlendNxMOutputData::getValueWeightsY()
{
  return ((float*)(this + 2 + m_blendX));
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE AttribDataBlendNxMOutputData *AttribDataBlendNxMOutputData::deserialiseTx(void *buffer)
{
  AttribDataBlendNxMOutputData *data = (AttribDataBlendNxMOutputData *) buffer;
  NMP::netEndianSwap(data->m_blendX);
  for (uint32_t i = 0; i < data->m_blendX; ++i)
  {
    NMP::netEndianSwap(data->getValueWeightsX()[i]);
  }
  NMP::netEndianSwap(data->m_blendY);
  for (uint32_t i = 0; i < data->m_blendY; ++i)
  {
    NMP::netEndianSwap(data->getValueWeightsY()[i]);
  }
  return data;
}

// restore the packing alignment set before this file
#ifndef NM_HOST_CELL_SPU
  #pragma pack(pop)
#endif // NM_HOST_CELL_SPU

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // M_CORE_DEBUG_INTERFACE_H
//----------------------------------------------------------------------------------------------------------------------
