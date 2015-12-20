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
#include "morpheme/mrTaskUtilities.h"
#include "morpheme/mrMirroredAnimMapping.h"
#include "morpheme/AnimSource/mrAnimSource.h"
//----------------------------------------------------------------------------------------------------------------------
#ifdef NM_HOST_CELL_SPU
  #include "morphemeSPU/mrDispatcherSPU.h"
  #include "NMPlatform/ps3/NMSPUDMAController.h"
#endif

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void TaskInitEventTrackDurationSetFromSource(Dispatcher::TaskParameters* parameters)
{
  AttribDataSourceEventTrackSet* sourceEventTracks = parameters->getInputAttrib<AttribDataSourceEventTrackSet>(1, ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS);
  AttribDataBool* loopable = parameters->getInputAttrib<AttribDataBool>(2, ATTRIB_SEMANTIC_LOOP);
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(3, ATTRIB_SEMANTIC_SOURCE_ANIM);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

#ifdef NM_HOST_CELL_SPU
  NMP_ASSERT(parameters->m_dispatcher);
  AttribDataSourceEventTrackSet::prepDurationEventTrackSetForSPU(sourceEventTracks, parameters->m_dispatcher->getTempMemoryAllocator());
#endif // NM_HOST_CELL_SPU

  // Work out the total number of tracks and events required in the output set.
  uint32_t numEventTracks = sourceEventTracks->m_numDurEventTracks;
  uint32_t numEvents = 0;
  for (uint32_t i = 0; i < numEventTracks; ++i)
  {
    numEvents += sourceEventTracks->m_sourceDurEventTracks[i]->getNumEvents();
  }

  // Allocate the memory to store our output attribute.
  AttribDataDurationEventTrackSetCreateDesc desc(numEventTracks, numEvents);
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->createOutputAttrib<AttribDataDurationEventTrackSet>(0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, &desc);

  // Initialise the duration event track set from the source event tracks.
  durationEventTrackSet->m_durationEventTrackSet->initFromSourceDef(
    sourceAnim->m_clipStartFraction,
    (sourceAnim->m_clipEndFraction - sourceAnim->m_clipStartFraction),
    loopable->m_value,
    numEventTracks,
    sourceEventTracks->m_sourceDurEventTracks,
    &(syncEventTrack->m_syncEventTrack),
    sourceAnim->m_playBackwards);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskInitEmptyEventTrackDurationSet(Dispatcher::TaskParameters* parameters)
{
  AttribDataDurationEventTrackSetCreateDesc desc(0, 0);
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->createOutputAttrib<AttribDataDurationEventTrackSet>(0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, &desc);
  (void)durationEventTrackSet;
}

//----------------------------------------------------------------------------------------------------------------------
void TaskSampleEventsFromSourceTracksIncDurationEvents(Dispatcher::TaskParameters* parameters)
{
  // MORPH-21360: Make event sampling better at handling delta times that greater than the duration of the sync track.
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSourceEventTrackSet* sourceEventTracks = parameters->getInputAttrib<AttribDataSourceEventTrackSet>(2, ATTRIB_SEMANTIC_SOURCE_EVENT_TRACKS);
  AttribDataPlaybackPos* fractionTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(3, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataSourceAnim* sourceAnim = parameters->getInputAttrib<AttribDataSourceAnim>(4, ATTRIB_SEMANTIC_SOURCE_ANIM);
  AttribDataBool* loopable = parameters->getInputAttrib<AttribDataBool>(5, ATTRIB_SEMANTIC_LOOP);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(6, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);

#ifdef NM_HOST_CELL_SPU
  NMP_ASSERT(parameters->m_dispatcher);
  AttribDataSourceEventTrackSet::prepDiscreteEventTrackSetForSPU(sourceEventTracks, parameters->m_dispatcher->getTempMemoryAllocator());
  AttribDataSourceEventTrackSet::prepCurveEventTrackSetForSPU(sourceEventTracks, parameters->m_dispatcher->getTempMemoryAllocator());
#endif // NM_HOST_CELL_SPU

  //-----------------------------
  // Work out what the true fractions are.
  float trueSampleStartFraction;
  float trueSampleEndFraction;
  float trueSampleDeltaFraction;
  calculateTrueEventSamplingTimeFractions(
    fractionTimePos,
    sourceAnim,
    trueSampleStartFraction,
    trueSampleEndFraction,
    trueSampleDeltaFraction);

  // Find triggered discrete events.
  uint32_t totalNumTriggeredDiscreteEvents = 0;
  EventDefDiscrete* triggeredDiscreteEvents[MAX_NUM_DISCRETE_EVENTS];
  uint32_t numSampledDiscreteTracks = 0;
  SampledDiscreteTrack sampledDiscreteTracks[MAX_NUM_DISCRETE_EVENTS];
  findTriggeredDiscreteEvents(
    numSampledDiscreteTracks,
    sampledDiscreteTracks,
    totalNumTriggeredDiscreteEvents,
    triggeredDiscreteEvents,
    sourceEventTracks,
    sourceAnim,
    loopable->m_value,
    trueSampleStartFraction,
    trueSampleDeltaFraction,
    fractionTimePos->m_inclusiveStart);

  //-----------------------------
  // Sampling duration event buffers.
  SampledCurveEventsBuffer workingSampledDurationEventsBuffer;
  SampledCurveEvent workingSampledDurationEventsArray[MAX_NUM_CURVE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_CURVE_EVENTS, workingSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionTimePos->m_currentPosAdj,
    fractionTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopable->m_value);

  // Calculate the total number of sampled curve events we need to output.
  uint32_t totalNumSampledCurveEvents = sourceEventTracks->m_numCurveEventTracks + workingSampledDurationEventsBuffer.getNumSampledEvents();

  //-----------------------------
  // Now we know how many events we will need to output we can create our output events buffer.
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumSampledCurveEvents);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  //-----------------------------
  // Fill in triggered discrete events buffer.
  uint32_t j = 0;
  for (uint32_t i = 0; i < numSampledDiscreteTracks; ++i)
  {
    for (uint32_t k = 0; k < sampledDiscreteTracks[i].m_numTriggeredEvents; ++k, ++j)
    {
      sampledEvents->m_discreteBuffer->setEvent(
        j,
        sampledDiscreteTracks[i].m_userData,
        sampledDiscreteTracks[i].m_runtimeID,
        triggeredDiscreteEvents[j]->getUserData());
    }
  }

  //-----------------------------
  // Sample curve events.
  for (uint32_t i = 0; i < sourceEventTracks->m_numCurveEventTracks; ++i)
  {
    EventTrackDefCurve* currentSourceTrack = sourceEventTracks->m_sourceCurveEventTracks[i];
    uint32_t runtimeTrackID = currentSourceTrack->getTrackID();
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    sampledEvents->m_curveBuffer->sampleCurveTrackAndAddToBuffer(
      currentSourceTrack,         // Events track to sample from.
      trueSampleEndFraction,      // What point to sample at.
      runtimeTrackID);            // The runtime ID of the source event track.
  }

  //-----------------------------
  // Copy sampled duration events from the working buffer into our output buffer.
  for (uint32_t i = 0; i < workingSampledDurationEventsBuffer.getNumSampledEvents(); ++i)
  {
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    const SampledCurveEvent* sampledEvent = workingSampledDurationEventsBuffer.getSampledEvent(i);
    sampledEvents->m_curveBuffer->addSampledEvent(sampledEvent);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCombine2SampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* source0SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source1SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(3, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataPlaybackPos* fractionalTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(5, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(6, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_sampledEventsNumWeights == 1);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(7, ATTRIB_SEMANTIC_LOOP);

  //-----------------------------
  // Find out if the blend should be loopable.
  bool loopable = loopableAttrib->m_value;

  //-----------------------------
  // Sampling duration event buffers.
  SampledCurveEventsBuffer workingSampledDurationEventsBuffer;
  SampledCurveEvent workingSampledDurationEventsArray[MAX_NUM_CURVE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_CURVE_EVENTS, workingSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionalTimePos->m_currentPosAdj,
    fractionalTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopable);
  //-----------------------------
  // Calculate the event count totals that we need to output.
  uint32_t totalNumTriggeredDiscreteEvents =
    source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t totalNumCurveEventSamples =
    source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    workingSampledDurationEventsBuffer.getNumSampledEvents();

  //-----------------------------
  // Now we know how many events we will need to output we can create our output events buffer.
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  //-----------------------------
  // Combine both input buffers into the output.
  float weight = 1.0f - blendWeights->m_sampledEventsWeights[0];
  sampledEvents->m_discreteBuffer->combine(source0SampledEventsBuffer->m_discreteBuffer, source1SampledEventsBuffer->m_discreteBuffer, weight);
  sampledEvents->m_curveBuffer->combine(source0SampledEventsBuffer->m_curveBuffer, source1SampledEventsBuffer->m_curveBuffer, weight);

  //-----------------------------
  // Copy sampled duration events from the working buffer into our output buffer.
  for (uint32_t i = 0; i < workingSampledDurationEventsBuffer.getNumSampledEvents(); ++i)
  {
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    const SampledCurveEvent* sampledEvent = workingSampledDurationEventsBuffer.getSampledEvent(i);
    sampledEvents->m_curveBuffer->addSampledEvent(sampledEvent);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskAdd2SampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* source0SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source1SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(3, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataPlaybackPos* fractionalTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(5, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(6, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_sampledEventsNumWeights == 1);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(7, ATTRIB_SEMANTIC_LOOP);

  //-----------------------------
  // Find out if the blend should be loopable.
  bool loopable = loopableAttrib->m_value;

  //-----------------------------
  // Sampling duration event buffers.
  SampledCurveEventsBuffer workingSampledDurationEventsBuffer;
  SampledCurveEvent workingSampledDurationEventsArray[MAX_NUM_DISCRETE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_DISCRETE_EVENTS, workingSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionalTimePos->m_currentPosAdj,
    fractionalTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopable);
  //-----------------------------
  // Calculate the event count totals that we need to output.
  uint32_t totalNumTriggeredDiscreteEvents =
    source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t totalNumCurveEventSamples =
    source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    workingSampledDurationEventsBuffer.getNumSampledEvents();

  //-----------------------------
  // Now we know how many events we will need to output we can create our output events buffer.
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  //-----------------------------
  // Add both input discrete buffers together in the output (preserves the blend weight).
  sampledEvents->m_discreteBuffer->additiveCombine(
    source0SampledEventsBuffer->m_discreteBuffer, 
    source1SampledEventsBuffer->m_discreteBuffer,
    blendWeights->m_sampledEventsWeights[0]);

  //-----------------------------
  // Add both input curve buffers into our output (preserves the blend weight).
  sampledEvents->m_curveBuffer->fill(source0SampledEventsBuffer->m_curveBuffer);
  uint32_t startingIndex = sampledEvents->m_curveBuffer->getNumSampledEvents();
  sampledEvents->m_curveBuffer->fillAtAndScale(startingIndex, source1SampledEventsBuffer->m_curveBuffer, blendWeights->m_sampledEventsWeights[0]);

  //-----------------------------
  // Copy sampled duration events from the working buffer into our output buffer.
  for (uint32_t i = 0; i < workingSampledDurationEventsBuffer.getNumSampledEvents(); ++i)
  {
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    const SampledCurveEvent* sampledEvent = workingSampledDurationEventsBuffer.getSampledEvent(i);
    sampledEvents->m_curveBuffer->addSampledEvent(sampledEvent);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskAddSampledDurationEventsToSampleEventBuffer(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents*          sourceSampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataDurationEventTrackSet*  durationEventTrackSet     = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack*         syncEventTrack            = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataPlaybackPos*            fractionalTimePos         = parameters->getInputAttrib<AttribDataPlaybackPos>(4, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBool*                   loopableAttrib            = parameters->getInputAttrib<AttribDataBool>(5, ATTRIB_SEMANTIC_LOOP);

  //-----------------------------
  // Find out if the blend should be loopable.
  bool loopable = loopableAttrib->m_value;

  //-----------------------------
  // Sampling duration event buffers.
  SampledCurveEventsBuffer workingSampledDurationEventsBuffer;
  SampledCurveEvent workingSampledDurationEventsArray[MAX_NUM_CURVE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_CURVE_EVENTS, workingSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionalTimePos->m_currentPosAdj,
    fractionalTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopable);
  //-----------------------------
  // Calculate the event count totals that we need to output.
  uint32_t totalNumTriggeredDiscreteEvents =
    sourceSampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t totalNumCurveEventSamples =
    sourceSampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    workingSampledDurationEventsBuffer.getNumSampledEvents();

  //-----------------------------
  // Now we know how many events we will need to output we can create our output events buffer.
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  //-----------------------------
  // Copy both input buffers into the output.
  sampledEvents->m_discreteBuffer->fillAt(0, sourceSampledEventsBuffer->m_discreteBuffer);
  sampledEvents->m_curveBuffer->fill(sourceSampledEventsBuffer->m_curveBuffer);

  //-----------------------------
  // Copy sampled duration events from the working buffer into our output buffer.
  for (uint32_t i = 0; i < workingSampledDurationEventsBuffer.getNumSampledEvents(); ++i)
  {
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    const SampledCurveEvent* sampledEvent = workingSampledDurationEventsBuffer.getSampledEvent(i);
    sampledEvents->m_curveBuffer->addSampledEvent(sampledEvent);
  }
}


//----------------------------------------------------------------------------------------------------------------------
void TaskCombine2x2SampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* source0SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source1SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source2SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(3, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source3SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(4, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(5, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(6, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataPlaybackPos* fractionalTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(7, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(8, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(9, ATTRIB_SEMANTIC_LOOP);

  //-----------------------------
  // Find out if the blend should be loopable.
  bool loopable = loopableAttrib->m_value;

  NMP_ASSERT(blendWeights->m_sampledEventsNumWeights == 2);
  NMP_ASSERT(blendWeights->m_sampledEventsWeights[0] >= 0.0f && blendWeights->m_sampledEventsWeights[0] <= 1.0f);
  NMP_ASSERT(blendWeights->m_sampledEventsWeights[1] >= 0.0f && blendWeights->m_sampledEventsWeights[1] <= 1.0f);

  //-----------------------------
  // Sampling duration event buffers.
  SampledCurveEventsBuffer  workingSampledDurationEventsBuffer;
  SampledCurveEvent woringSampledDurationEventsArray[MAX_NUM_CURVE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_CURVE_EVENTS, woringSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionalTimePos->m_currentPosAdj,
    fractionalTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopable);
  //-----------------------------
  // Calculate the temporary event count totals and those that we need to output.

  uint32_t temp0NumTriggeredDiscreteEvents = source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
      source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t temp1NumTriggeredDiscreteEvents = source2SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
      source3SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t totalNumTriggeredDiscreteEvents = temp0NumTriggeredDiscreteEvents + temp1NumTriggeredDiscreteEvents;

  uint32_t temp0NumCurveEventSamples = source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
                                       source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
  uint32_t temp1NumCurveEventSamples = source2SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
                                       source3SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
  uint32_t totalNumCurveEventSamples = temp0NumCurveEventSamples + temp1NumCurveEventSamples +  workingSampledDurationEventsBuffer.getNumSampledEvents();

  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();

  //-----------------------------
  // Now we know how many events we will need to output we can create our temporary and output events buffers.
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  // Store a roll back point on the fast heap to restore back to after we have done our local working.
  AttribDataHandle temp0SampledEventsHandle = AttribDataSampledEvents::create(childAllocator, temp0NumTriggeredDiscreteEvents, temp0NumCurveEventSamples);
  AttribDataHandle temp1SampledEventsHandle = AttribDataSampledEvents::create(childAllocator, temp1NumTriggeredDiscreteEvents, temp1NumCurveEventSamples);
  AttribDataSampledEvents* temp0SampledEvents = (AttribDataSampledEvents*)temp0SampledEventsHandle.m_attribData;
  AttribDataSampledEvents* temp1SampledEvents = (AttribDataSampledEvents*)temp1SampledEventsHandle.m_attribData;
  

  //-----------------------------
  // Combine both input buffers into the output.
  float blendWeightX = blendWeights->m_sampledEventsWeights[0];
  float blendWeightY = blendWeights->m_sampledEventsWeights[1];
  temp0SampledEvents->m_discreteBuffer->combine(source0SampledEventsBuffer->m_discreteBuffer, source1SampledEventsBuffer->m_discreteBuffer, 1.0f - blendWeightX);
  temp0SampledEvents->m_curveBuffer->combine(source0SampledEventsBuffer->m_curveBuffer, source1SampledEventsBuffer->m_curveBuffer, 1.0f - blendWeightX);
  temp1SampledEvents->m_discreteBuffer->combine(source2SampledEventsBuffer->m_discreteBuffer, source3SampledEventsBuffer->m_discreteBuffer, 1.0f - blendWeightX);
  temp1SampledEvents->m_curveBuffer->combine(source2SampledEventsBuffer->m_curveBuffer, source3SampledEventsBuffer->m_curveBuffer, 1.0f - blendWeightX);
  sampledEvents->m_discreteBuffer->combine(temp0SampledEvents->m_discreteBuffer, temp1SampledEvents->m_discreteBuffer, 1.0f - blendWeightY);
  sampledEvents->m_curveBuffer->combine(temp0SampledEvents->m_curveBuffer, temp1SampledEvents->m_curveBuffer, 1.0f - blendWeightY);

  //-----------------------------
  // Copy sampled duration events from the working buffer into our output buffer.
  for (uint32_t i = 0; i < workingSampledDurationEventsBuffer.getNumSampledEvents(); ++i)
  {
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    const SampledCurveEvent* sampledEvent = workingSampledDurationEventsBuffer.getSampledEvent(i);
    sampledEvents->m_curveBuffer->addSampledEvent(sampledEvent);
  }

  // Free the temp buffers by rolling back the temp memory allocator.
  childAllocator->memFree(temp0SampledEventsHandle.m_attribData);
  childAllocator->memFree(temp1SampledEventsHandle.m_attribData);

  tempAllocator->destroyChildAllocator(childAllocator);
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleCombineSampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* source0SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source1SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(2, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* source2SampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(3, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(4, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(5, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataPlaybackPos* fractionalTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(6, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(7, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(8, ATTRIB_SEMANTIC_LOOP); 

  //-----------------------------
  // Find out if the blend should be loopable.
  bool loopable = loopableAttrib->m_value;

  //-----------------------------
  // Sampling duration event buffers.
  SampledCurveEventsBuffer  workingSampledDurationEventsBuffer;
  SampledCurveEvent woringSampledDurationEventsArray[MAX_NUM_CURVE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_CURVE_EVENTS, woringSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionalTimePos->m_currentPosAdj,
    fractionalTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopable);

  //-----------------------------
  // Calculate the temporary event count totals and those that we need to output.
  uint32_t tempNumTriggeredDiscreteEvents =
    source0SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents() +
    source1SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();

  uint32_t totalNumTriggeredDiscreteEvents =
    tempNumTriggeredDiscreteEvents + source2SampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();

  uint32_t tempNumCurveEventSamples =
    source0SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    source1SampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
    
  uint32_t totalNumCurveEventSamples = tempNumCurveEventSamples +
    source2SampledEventsBuffer->m_curveBuffer->getNumSampledEvents() +
    workingSampledDurationEventsBuffer.getNumSampledEvents();

  //-----------------------------
  // Create the output events buffers (before any working memory).
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  // Working events buffers
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
  NMP::TempMemoryAllocator* childAllocator = tempAllocator->createChildAllocator();
  AttribDataHandle tempSampledEventsHandle = AttribDataSampledEvents::create(childAllocator, tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
  AttribDataSampledEvents* tempSampledEvents = (AttribDataSampledEvents*)tempSampledEventsHandle.m_attribData;

  //-----------------------------
  // Combine both input buffers into the output.
  NMP_ASSERT(blendWeights->m_eventsNumWeights == 2);
  float omx = 1.0f - blendWeights->m_eventsWeights[0];
  float omy = 1.0f - blendWeights->m_eventsWeights[1];

  tempSampledEvents->m_discreteBuffer->combine(
    source0SampledEventsBuffer->m_discreteBuffer,
    source1SampledEventsBuffer->m_discreteBuffer,
    omx);

  tempSampledEvents->m_curveBuffer->combine(
    source0SampledEventsBuffer->m_curveBuffer,
    source1SampledEventsBuffer->m_curveBuffer,
    omx);

  sampledEvents->m_discreteBuffer->combine(
    tempSampledEvents->m_discreteBuffer,
    source2SampledEventsBuffer->m_discreteBuffer,
    omy);

  sampledEvents->m_curveBuffer->combine(
    tempSampledEvents->m_curveBuffer,
    source2SampledEventsBuffer->m_curveBuffer,
    omy);

  //-----------------------------
  // Copy sampled duration events from the working buffer into our output buffer.
  for (uint32_t i = 0; i < workingSampledDurationEventsBuffer.getNumSampledEvents(); ++i)
  {
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    const SampledCurveEvent* sampledEvent = workingSampledDurationEventsBuffer.getSampledEvent(i);
    sampledEvents->m_curveBuffer->addSampledEvent(sampledEvent);
  }

  // Free the temp buffers by rolling back the temp memory allocator.
  childAllocator->memFree(tempSampledEventsHandle.m_attribData);

  tempAllocator->destroyChildAllocator(childAllocator);
}

#ifdef NM_HOST_CELL_SPU
//----------------------------------------------------------------------------------------------------------------------
void TaskCombineAllSampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters)
{
  // Get number of connections
  uint32_t numConnections = parameters->m_numParameters - 6;

  MR::DispatcherSPU* dispatcher = (MR::DispatcherSPU*)parameters->m_dispatcher;
  NMP::TempMemoryAllocator* tempAllocator = dispatcher->getTempMemoryAllocator();

  //-----------------------------
  // Fetch the parameters
  uint32_t paramIndex = 1;
  FetchParameterHelper(&parameters->m_parameters[paramIndex], tempAllocator);

  paramIndex++;
  FetchParameterHelper(&parameters->m_parameters[paramIndex], tempAllocator);

  paramIndex++;
  FetchParameterHelper(&parameters->m_parameters[paramIndex], tempAllocator);

  paramIndex++;
  FetchParameterHelper(&parameters->m_parameters[paramIndex], tempAllocator);

  paramIndex++;
  FetchParameterHelper(&parameters->m_parameters[paramIndex], tempAllocator);

  uint32_t weightIndex = paramIndex + numConnections;
  FetchParameterHelper(&parameters->m_parameters[weightIndex], tempAllocator);
  // Wait on completion of DMA  from PPU memory.
  NMP::SPUDMAController::dmaWaitAll();

  //-----------------------------
  // Now write the local store value into the param.
  // Duration Event Track Set
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet::relocate(durationEventTrackSet);
  // Sync Event Track
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  // Playback Position
  AttribDataPlaybackPos* fractionalTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(3, ATTRIB_SEMANTIC_FRACTION_POS);
  // Is loopable
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(4, ATTRIB_SEMANTIC_LOOP);

  //-----------------------------
  // Load the first sampled events buffer from main memory
  AttribDataSampledEvents* sourceSampledEvents0 = parameters->getInputAttrib<AttribDataSampledEvents>(5, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents::relocate(sourceSampledEvents0);

  //-----------------------------
  // Get the normalised blend weights
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(weightIndex, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataFloatArray::relocate(weights);

  //-----------------------------
  // Find out if the blend should be loopable.
  bool loopable = loopableAttrib->m_value;

  //-----------------------------
  // Sampling duration event buffers.
  SampledCurveEventsBuffer  workingSampledDurationEventsBuffer;
  SampledCurveEvent workingSampledDurationEventsArray[MAX_NUM_CURVE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_CURVE_EVENTS, workingSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionalTimePos->m_currentPosAdj,
    fractionalTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopable);

  //-----------------------------
  // Output
  AttribDataSampledEvents* sampledEvents = NULL;

  float accumulatedWeight = weights->m_values[0];
  
  AttribDataSampledEvents* eventsAccum = NULL;
  // Load the next sampled events buffer
  paramIndex = 6;

  uint32_t dmaTags = FetchParameterHelper(&parameters->m_parameters[paramIndex], tempAllocator);
  NMP::SPUDMAController::dmaWait(dmaTags);
  AttribDataSampledEvents* eventsAux = parameters->getInputAttrib<AttribDataSampledEvents>(paramIndex, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents::relocate(eventsAux);
  // Find out the size of the biggest input
  NMP::Memory::Format maxFormat = parameters->m_parameters[5].m_attribDataHandle.m_format; // size of the first sampled events buffer
  uint32_t currentIndex = paramIndex;
  for (uint32_t i=0; i<numConnections; ++i)
  {
    if (parameters->m_parameters[currentIndex].m_attribDataHandle.m_format.size > maxFormat.size)
    {
      maxFormat = parameters->m_parameters[currentIndex].m_attribDataHandle.m_format;
    }
    currentIndex++;
  }
  NMP::Memory::Resource attrResource = NMPAllocatorAllocateFromFormat(tempAllocator, maxFormat);
  uint32_t tempNumTriggeredDiscreteEvents = 0;
  uint32_t tempNumCurveEventSamples = 0;
  uint32_t tempIndex = 0;
  // normalised blend weight
  float weight = accumulatedWeight / (accumulatedWeight + weights->m_values[1]);
  // Safely estimate the requirement for the temporary events buffer set.
  tempNumTriggeredDiscreteEvents = sourceSampledEvents0->m_discreteBuffer->getNumTriggeredEvents() + eventsAux->m_discreteBuffer->getNumTriggeredEvents();
  tempNumCurveEventSamples = sourceSampledEvents0->m_curveBuffer->getNumSampledEvents() + eventsAux->m_curveBuffer->getNumSampledEvents();
  // If we have just 2 sources create the output directly 
  if (numConnections == 2)
  {
    tempNumCurveEventSamples += workingSampledDurationEventsBuffer.getNumSampledEvents();
    // Create the output events buffer
    AttribDataSampledEventsCreateDesc desc(tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
    sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);
    sampledEvents->m_discreteBuffer->combine(sourceSampledEvents0->m_discreteBuffer, eventsAux->m_discreteBuffer, 1.0f - weight);
    sampledEvents->m_curveBuffer->combine(sourceSampledEvents0->m_curveBuffer, eventsAux->m_curveBuffer, 1.0f - weight);
  }
  else
  {
    AttribDataHandle tempSampledEventsHandle = AttribDataSampledEvents::create(tempAllocator, tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
    eventsAccum = (AttribDataSampledEvents*)tempSampledEventsHandle.m_attribData;
    eventsAccum->m_discreteBuffer->combine(sourceSampledEvents0->m_discreteBuffer, eventsAux->m_discreteBuffer, 1.0f - weight);
    eventsAccum->m_curveBuffer->combine(sourceSampledEvents0->m_curveBuffer, eventsAux->m_curveBuffer, 1.0f - weight);
    ++tempIndex;
  }
  paramIndex++;
  // At this point we have already evaluated the first 2 sampled event buffers
  for (uint32_t i=2; i < numConnections; i++)
  {
    // Load next transform buffer, but not allocate new memory for it
    dmaTags = NMP::SPUDMAController::dmaGet(
      attrResource.ptr, 
      (uint32_t)(parameters->m_parameters[paramIndex].m_attribDataHandle.m_attribData), 
      maxFormat.size);
    NMP::SPUDMAController::dmaWait(dmaTags);
    parameters->m_parameters[paramIndex].m_attribDataHandle.m_attribData = (MR::AttribData*)(attrResource.ptr);
    sourceSampledEvents0 = parameters->getInputAttrib<AttribDataSampledEvents>(paramIndex, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
    AttribDataSampledEvents::relocate(sourceSampledEvents0);
    paramIndex++;
    // normalised blend weight
    weight = accumulatedWeight / (accumulatedWeight + weights->m_values[i]);
    // If is the last source: create the output
    if (i == numConnections-1)
    {
      // Safely estimate the requirement for the temporary events buffer set.
      tempNumTriggeredDiscreteEvents = eventsAccum->m_discreteBuffer->getNumTriggeredEvents() + sourceSampledEvents0->m_discreteBuffer->getNumTriggeredEvents();
      tempNumCurveEventSamples = eventsAccum->m_curveBuffer->getNumSampledEvents() + sourceSampledEvents0->m_curveBuffer->getNumSampledEvents() + workingSampledDurationEventsBuffer.getNumSampledEvents();
      // Create the output events buffer
      AttribDataSampledEventsCreateDesc desc(tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
      sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);
      sampledEvents->m_discreteBuffer->combine(eventsAccum->m_discreteBuffer, sourceSampledEvents0->m_discreteBuffer, 1.0f - weight);
      sampledEvents->m_curveBuffer->combine(eventsAccum->m_curveBuffer, sourceSampledEvents0->m_curveBuffer, 1.0f - weight);
      break;
    }
    else
    {
      // Safely estimate the requirement for the temporary events buffer set.
      tempNumTriggeredDiscreteEvents = eventsAccum->m_discreteBuffer->getNumTriggeredEvents() + sourceSampledEvents0->m_discreteBuffer->getNumTriggeredEvents();
      tempNumCurveEventSamples = eventsAccum->m_curveBuffer->getNumSampledEvents() + sourceSampledEvents0->m_curveBuffer->getNumSampledEvents();
      AttribDataHandle tempSampledEventsHandle = AttribDataSampledEvents::create(tempAllocator, tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
      eventsAux = (AttribDataSampledEvents*)tempSampledEventsHandle.m_attribData;
      eventsAux->m_discreteBuffer->combine(eventsAccum->m_discreteBuffer, sourceSampledEvents0->m_discreteBuffer, 1.0f - weight);
      eventsAux->m_curveBuffer->combine(eventsAccum->m_curveBuffer, sourceSampledEvents0->m_curveBuffer, 1.0f - weight);
      ++tempIndex;
    }
    eventsAccum->m_discreteBuffer = eventsAux->m_discreteBuffer;
    eventsAccum->m_curveBuffer = eventsAux->m_curveBuffer;
    accumulatedWeight += weights->m_values[i];
  }
  NMP_ASSERT(sampledEvents);

  //-----------------------------
  // Copy sampled duration events from the working buffer into our output buffer.
  for (uint32_t i = 0; i < workingSampledDurationEventsBuffer.getNumSampledEvents(); ++i)
  {
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    const SampledCurveEvent* sampledEvent = workingSampledDurationEventsBuffer.getSampledEvent(i);
    sampledEvents->m_curveBuffer->addSampledEvent(sampledEvent);
  }
}
#else
//----------------------------------------------------------------------------------------------------------------------
// MORPH-21361: Safely roll back the tempAllocator so that we don't leave temporary working data hanging around. Care must
// be taken because output attribs can be created (createOutputAttrib) on the tempAllocator (temporary memory allocator).
void TaskCombineAllSampledEventsBuffersAndSampleDurationEvents(Dispatcher::TaskParameters* parameters)
{
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(2, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataPlaybackPos* fractionalTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(3, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(4, ATTRIB_SEMANTIC_LOOP);
  
  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);

  // Get all the input sampled events buffers.
  uint32_t numConnections = parameters->m_numParameters - 6; // Get num active child input connections.
  NMP::Memory::Format memReq(sizeof(AttribDataSampledEvents*) * numConnections, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPAllocatorAllocateFromFormat(tempAllocator, memReq);
  AttribDataSampledEvents** sourceSampledEvents = (AttribDataSampledEvents**)memRes.ptr;
  uint32_t param = 5;
  for (uint32_t i = 0; i < numConnections; i++)
  {
    sourceSampledEvents[i] = parameters->getInputAttrib<AttribDataSampledEvents>(param, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
    param++;
  }

  // Get the normalised blend weights
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(param, ATTRIB_SEMANTIC_CP_FLOAT);
    
  //-----------------------------
  // Sampling duration event buffers.
  SampledCurveEventsBuffer  workingSampledDurationEventsBuffer;
  SampledCurveEvent workingSampledDurationEventsArray[MAX_NUM_CURVE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_CURVE_EVENTS, workingSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionalTimePos->m_currentPosAdj,
    fractionalTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopableAttrib->m_value);

  // Output
  AttribDataSampledEvents* sampledEvents = NULL;

  float accumulatedWeight = weights->m_values[0];
  
  // Temp array of AttribDataSampledEvents, size = numConnections - 2
  NMP::Memory::Format memReqTempBuffer(sizeof(AttribDataSampledEvents) * numConnections - 2, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memResTempBuffer = NMPAllocatorAllocateFromFormat(tempAllocator, memReqTempBuffer);
  AttribDataSampledEvents** tempBuffer = (AttribDataSampledEvents**)memResTempBuffer.ptr;
  uint32_t tempNumTriggeredDiscreteEvents = 0;
  uint32_t tempNumCurveEventSamples = 0;
  uint32_t tempIndex = 0;
  // normalised blend weight
  float weight = accumulatedWeight / (accumulatedWeight + weights->m_values[1]);
  // Safely estimate the requirement for the temporary events buffer set.
  tempNumTriggeredDiscreteEvents = sourceSampledEvents[0]->m_discreteBuffer->getNumTriggeredEvents() + sourceSampledEvents[1]->m_discreteBuffer->getNumTriggeredEvents();
  tempNumCurveEventSamples = sourceSampledEvents[0]->m_curveBuffer->getNumSampledEvents() + sourceSampledEvents[1]->m_curveBuffer->getNumSampledEvents();
  // If we have just 2 sources create the output directly 
  if (numConnections == 2)
  {
    tempNumCurveEventSamples += workingSampledDurationEventsBuffer.getNumSampledEvents();
    // Create the output events buffer
    AttribDataSampledEventsCreateDesc desc(tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
    sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);
    sampledEvents->m_discreteBuffer->combine(sourceSampledEvents[0]->m_discreteBuffer, sourceSampledEvents[1]->m_discreteBuffer, 1.0f - weight);
    sampledEvents->m_curveBuffer->combine(sourceSampledEvents[0]->m_curveBuffer, sourceSampledEvents[1]->m_curveBuffer, 1.0f - weight);
  }
  else
  {
    AttribDataHandle tempSampledEventsHandle = AttribDataSampledEvents::create(tempAllocator, tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
    tempBuffer[tempIndex] = (AttribDataSampledEvents*)tempSampledEventsHandle.m_attribData;
    tempBuffer[tempIndex]->m_discreteBuffer->combine(sourceSampledEvents[0]->m_discreteBuffer, sourceSampledEvents[1]->m_discreteBuffer, 1.0f - weight);
    tempBuffer[tempIndex]->m_curveBuffer->combine(sourceSampledEvents[0]->m_curveBuffer, sourceSampledEvents[1]->m_curveBuffer, 1.0f - weight);
    ++tempIndex;
  }

  // At this point we have already combined the first two buffers
  for (uint32_t i = 2; i < numConnections; ++i)
  {
    // normalised blend weight
    weight = accumulatedWeight / (accumulatedWeight + weights->m_values[i]);
    // If is the last source: create the output
    if (i == numConnections - 1)
    {
      // Safely estimate the requirement for the temporary events buffer set.
      tempNumTriggeredDiscreteEvents = tempBuffer[tempIndex-1]->m_discreteBuffer->getNumTriggeredEvents() + sourceSampledEvents[i]->m_discreteBuffer->getNumTriggeredEvents();
      tempNumCurveEventSamples = tempBuffer[tempIndex-1]->m_curveBuffer->getNumSampledEvents() + sourceSampledEvents[i]->m_curveBuffer->getNumSampledEvents() + workingSampledDurationEventsBuffer.getNumSampledEvents();
      // Create the output events buffer
      AttribDataSampledEventsCreateDesc desc(tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
      sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);
      sampledEvents->m_discreteBuffer->combine(tempBuffer[tempIndex-1]->m_discreteBuffer, sourceSampledEvents[i]->m_discreteBuffer, 1.0f - weight);
      sampledEvents->m_curveBuffer->combine(tempBuffer[tempIndex-1]->m_curveBuffer, sourceSampledEvents[i]->m_curveBuffer, 1.0f - weight);
      break;
    }
    else
    {
      // Safely estimate the requirement for the temporary events buffer set.
      tempNumTriggeredDiscreteEvents = tempBuffer[tempIndex-1]->m_discreteBuffer->getNumTriggeredEvents() + sourceSampledEvents[i]->m_discreteBuffer->getNumTriggeredEvents();
      tempNumCurveEventSamples = tempBuffer[tempIndex-1]->m_curveBuffer->getNumSampledEvents() + sourceSampledEvents[i]->m_curveBuffer->getNumSampledEvents();
      AttribDataHandle tempSampledEventsHandle = AttribDataSampledEvents::create(tempAllocator, tempNumTriggeredDiscreteEvents, tempNumCurveEventSamples);
      tempBuffer[tempIndex] = (AttribDataSampledEvents*)tempSampledEventsHandle.m_attribData;
      tempBuffer[tempIndex]->m_discreteBuffer->combine(tempBuffer[tempIndex-1]->m_discreteBuffer, sourceSampledEvents[i]->m_discreteBuffer, 1.0f - weight);
      tempBuffer[tempIndex]->m_curveBuffer->combine(tempBuffer[tempIndex-1]->m_curveBuffer, sourceSampledEvents[i]->m_curveBuffer, 1.0f - weight);
      ++tempIndex;
    }
    accumulatedWeight += weights->m_values[i];
  }
  NMP_ASSERT(sampledEvents);

  //-----------------------------
  // Copy sampled duration events from the working buffer into our output buffer.
  for (uint32_t i = 0; i < workingSampledDurationEventsBuffer.getNumSampledEvents(); ++i)
  {
    NMP_ASSERT(sampledEvents->m_curveBuffer);
    const SampledCurveEvent* sampledEvent = workingSampledDurationEventsBuffer.getSampledEvent(i);
    sampledEvents->m_curveBuffer->addSampledEvent(sampledEvent);
  }
}
#endif

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2DurationEventTrackSets(Dispatcher::TaskParameters* parameters)
{
  AttribDataDurationEventTrackSet* input0DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet* input1DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(4, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  NMP_ASSERT(blendWeights->m_eventsNumWeights == 1);
  NMP_ASSERT(blendWeights->m_eventsWeights[0] >= 0.0f && blendWeights->m_eventsWeights[0] <= 1.0f);
  AttribDataUInt* durationEventMatchingOp = parameters->getInputAttrib<AttribDataUInt>(5, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(6, ATTRIB_SEMANTIC_LOOP);

  bool loopable = loopableAttrib->m_value;

  // Safely estimate the requirement for our output events buffer set.
  uint32_t maxNumTracks;
  uint32_t maxNumEvents;
  EventTrackDurationSet::estimateBlendRequirements(
    input0DurationEventTrackSet->m_durationEventTrackSet,
    input1DurationEventTrackSet->m_durationEventTrackSet,
    syncEventTrack->m_syncEventTrack.getNumEvents(),
    maxNumTracks,
    maxNumEvents);

  // Now we know how many events we will need to output we can create our output events buffer set.
  AttribDataDurationEventTrackSetCreateDesc desc(maxNumTracks, maxNumEvents);
  AttribDataDurationEventTrackSet* blendedDurationEventTrackSet = parameters->createOutputAttrib<AttribDataDurationEventTrackSet>(0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, &desc);

  // Do the blend.
  blendedDurationEventTrackSet->m_durationEventTrackSet->blend(
    input0DurationEventTrackSet->m_durationEventTrackSet,
    input1DurationEventTrackSet->m_durationEventTrackSet,
    &syncEventTrack->m_syncEventTrack,  // Space within which blended events must be defined.
    blendWeights->m_eventsWeights[0],   // Blend weighting.
    0.5f,                               // At what proportional weight do we select one events details over the other.
    durationEventMatchingOp->m_value,   // Whether and how to match up the events from each track.
    false,                              // If an event from one of the sources has not been matched to one in the other source should we just discard it?
    DURATION_EVENT_BLEND_OP_TYPE_BLEND, // Operation to perform on events that have been matched up.
    loopable,                           //
    1.0f,                               // Within what range do we decide to match events together.
    syncEventTrack->m_syncEventTrack.getStartEventIndex());
}

//----------------------------------------------------------------------------------------------------------------------
void TaskBlend2x2DurationEventTrackSets(Dispatcher::TaskParameters* parameters)
{
  // MORPH-21362: Correctly release temporary working memory.

  AttribDataDurationEventTrackSet* input0DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet* input1DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet* input2DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(3, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet* input3DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(4, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(5, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(6, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  AttribDataUInt* durationEventMatchingOp = parameters->getInputAttrib<AttribDataUInt>(7, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(8, ATTRIB_SEMANTIC_LOOP);

  bool loopable = loopableAttrib->m_value;

  // Cap the blend weights to lie within normalised limits.
  float blendWeightX = blendWeights->m_eventsWeights[0];
  float blendWeightY = blendWeights->m_eventsWeights[1];
  NMP_ASSERT(blendWeightX >= 0.0f && blendWeightX <= 1.0f);
  NMP_ASSERT(blendWeightY >= 0.0f && blendWeightY <= 1.0f);

  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);
 
  // Safely estimate the requirement for first temporary events buffer set.
  uint32_t maxNumTracks;
  uint32_t maxNumEvents;
  EventTrackDurationSet::estimateBlendRequirements(
    input0DurationEventTrackSet->m_durationEventTrackSet,
    input1DurationEventTrackSet->m_durationEventTrackSet,
    syncEventTrack->m_syncEventTrack.getNumEvents(),
    maxNumTracks,
    maxNumEvents);

  // Now we know how many events we will need to output we can create our output events buffer set.
  EventTrackDurationSet* temp0BlendedDurationEventTrackSet = EventTrackDurationSet::create(tempAllocator, maxNumTracks, maxNumEvents);

  // Safely estimate the requirement for second temporary events buffer set.
  EventTrackDurationSet::estimateBlendRequirements(
    input2DurationEventTrackSet->m_durationEventTrackSet,
    input3DurationEventTrackSet->m_durationEventTrackSet,
    syncEventTrack->m_syncEventTrack.getNumEvents(),
    maxNumTracks,
    maxNumEvents);

  // Now we know how many events we will need to output we can create our output events buffer set.
  EventTrackDurationSet* temp1BlendedDurationEventTrackSet = EventTrackDurationSet::create(tempAllocator, maxNumTracks, maxNumEvents);

  // Do the blend.
  temp0BlendedDurationEventTrackSet->blend(
    input0DurationEventTrackSet->m_durationEventTrackSet,
    input1DurationEventTrackSet->m_durationEventTrackSet,
    &syncEventTrack->m_syncEventTrack,  // Space within which blended events must be defined.
    blendWeightX,                       // Blend weighting.
    0.5f,                               // At what proportional weight do we select one events details over the other.
    durationEventMatchingOp->m_value,   // Whether and how to match up the events from each track.
    false,                              // If an event from one of the sources has not been matched to one in the other source should we just discard it?
    DURATION_EVENT_BLEND_OP_TYPE_BLEND, // Operation to perform on events that have been matched up.
    loopable,                           //
    1.0f,                               // Within what range do we decide to match events together.
    0);                                 // Track set 1 event offset

  temp1BlendedDurationEventTrackSet->blend(
    input2DurationEventTrackSet->m_durationEventTrackSet,
    input3DurationEventTrackSet->m_durationEventTrackSet,
    &syncEventTrack->m_syncEventTrack,  // Space within which blended events must be defined.
    blendWeightX,                       // Blend weighting.
    0.5f,                               // At what proportional weight do we select one events details over the other.
    durationEventMatchingOp->m_value,   // Whether and how to match up the events from each track.
    false,                              // If an event from one of the sources has not been matched to one in the other source should we just discard it?
    DURATION_EVENT_BLEND_OP_TYPE_BLEND, // Operation to perform on events that have been matched up.
    loopable,                           //
    1.0f,                               // Within what range do we decide to match events together.
    0);                                 // Track set 1 event offset

  // Safely estimate the requirement for our output events buffer set.
  EventTrackDurationSet::estimateBlendRequirements(
    temp0BlendedDurationEventTrackSet,
    temp1BlendedDurationEventTrackSet,
    syncEventTrack->m_syncEventTrack.getNumEvents(),
    maxNumTracks,
    maxNumEvents);

  // Now we know how many events we will need to output we can create our output events buffer set.
  AttribDataDurationEventTrackSetCreateDesc desc(maxNumTracks, maxNumEvents);
  AttribDataDurationEventTrackSet* blendedDurationEventTrackSet = parameters->createOutputAttrib<AttribDataDurationEventTrackSet>(0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, &desc);

  blendedDurationEventTrackSet->m_durationEventTrackSet->blend(
    temp0BlendedDurationEventTrackSet,
    temp1BlendedDurationEventTrackSet,
    &syncEventTrack->m_syncEventTrack,  // Space within which blended events must be defined.
    blendWeightY,                       // Blend weighting.
    0.5f,                               // At what proportional weight do we select one events details over the other.
    durationEventMatchingOp->m_value,   // Whether and how to match up the events from each track.
    false,                              // If an event from one of the sources has not been matched to one in the other source should we just discard it?
    DURATION_EVENT_BLEND_OP_TYPE_BLEND, // Operation to perform on events that have been matched up.
    loopable,                           //
    1.0f,                               // Within what range do we decide to match events together.
    syncEventTrack->m_syncEventTrack.getStartEventIndex());                                 // Track set 1 event offset.
}

//----------------------------------------------------------------------------------------------------------------------
void TaskTriangleBlendDurationEventTrackSets(Dispatcher::TaskParameters* parameters)
{
  // MORPH-21362: Correctly release temporary working memory.

  AttribDataDurationEventTrackSet* input0DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet* input1DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet* input2DurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(3, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(4, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataBlendWeights* blendWeights = parameters->getInputAttrib<AttribDataBlendWeights>(5, ATTRIB_SEMANTIC_BLEND_WEIGHTS);
  AttribDataUInt* durationEventMatchingOp = parameters->getInputAttrib<AttribDataUInt>(6, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(7, ATTRIB_SEMANTIC_LOOP);

  NMP_ASSERT(blendWeights->m_eventsNumWeights == 2);
  bool loopable = loopableAttrib->m_value;

  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator =  parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);

  // Safely estimate the requirement for first temporary events buffer set.
  uint32_t maxNumTracks;
  uint32_t maxNumEvents;
  EventTrackDurationSet::estimateBlendRequirements(
    input0DurationEventTrackSet->m_durationEventTrackSet,
    input1DurationEventTrackSet->m_durationEventTrackSet,
    syncEventTrack->m_syncEventTrack.getNumEvents(),
    maxNumTracks,
    maxNumEvents);

  // Now we know how many events we will need to output we can create our output events buffer set.
  EventTrackDurationSet* tempBlendedDurationEventTrackSet = EventTrackDurationSet::create(tempAllocator, maxNumTracks, maxNumEvents);

  // Do the blend.
  tempBlendedDurationEventTrackSet->blend(
    input0DurationEventTrackSet->m_durationEventTrackSet,
    input1DurationEventTrackSet->m_durationEventTrackSet,
    &syncEventTrack->m_syncEventTrack,  // Space within which blended events must be defined.
    blendWeights->m_eventsWeights[0],   // Blend weighting.
    0.5f,                               // At what proportional weight do we select one events details over the other.
    durationEventMatchingOp->m_value,   // Whether and how to match up the events from each track.
    false,                              // If an event from one of the sources has not been matched to one in the other source should we just discard it?
    DURATION_EVENT_BLEND_OP_TYPE_BLEND, // Operation to perform on events that have been matched up.
    loopable,                           //
    1.0f,                               // Within what range do we decide to match events together.
    0);                                 // Track set 1 event offset

  // Safely estimate the requirement for our output events buffer set.
  EventTrackDurationSet::estimateBlendRequirements(
    tempBlendedDurationEventTrackSet,
    input2DurationEventTrackSet->m_durationEventTrackSet,
    syncEventTrack->m_syncEventTrack.getNumEvents(),
    maxNumTracks,
    maxNumEvents);

  // Now we know how many events we will need to output we can create our output events buffer set.
  AttribDataDurationEventTrackSetCreateDesc desc(maxNumTracks, maxNumEvents);
  AttribDataDurationEventTrackSet* blendedDurationEventTrackSet = parameters->createOutputAttrib<AttribDataDurationEventTrackSet>(0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, &desc);

  blendedDurationEventTrackSet->m_durationEventTrackSet->blend(
    tempBlendedDurationEventTrackSet,
    input2DurationEventTrackSet->m_durationEventTrackSet,
    &syncEventTrack->m_syncEventTrack,  // Space within which blended events must be defined.
    blendWeights->m_eventsWeights[1],   // Blend weighting.
    0.5f,                               // At what proportional weight do we select one events details over the other.
    durationEventMatchingOp->m_value,   // Whether and how to match up the events from each track.
    false,                              // If an event from one of the sources has not been matched to one in the other source should we just discard it?
    DURATION_EVENT_BLEND_OP_TYPE_BLEND, // Operation to perform on events that have been matched up.
    loopable,                           //
    1.0f,                               // Within what range do we decide to match events together.
    syncEventTrack->m_syncEventTrack.getStartEventIndex());                                 // Track set 1 event offset.
}

#ifdef NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// SPU implementation.
void TaskBlendAllDurationEventTrackSets(Dispatcher::TaskParameters* parameters)
{
  // Get number of connections
  uint32_t numChildInputs = parameters->m_numParameters - 5;

  MR::DispatcherSPU* dispatcher = (MR::DispatcherSPU*)parameters->m_dispatcher;
  NMP::MemoryAllocator* tempAllocator = dispatcher->getTempMemoryAllocator();
  
  //-----------------------------
  // DMA Fetch the input parameters.
  FetchParameterHelper(&parameters->m_parameters[1], tempAllocator);

  FetchParameterHelper(&parameters->m_parameters[2], tempAllocator);

  FetchParameterHelper(&parameters->m_parameters[3], tempAllocator);

  FetchParameterHelper(&parameters->m_parameters[4], tempAllocator);

  uint32_t weightIndex = parameters->m_numParameters - 1;
  FetchParameterHelper(&parameters->m_parameters[weightIndex], tempAllocator);
  
  // Wait on completion of DMA  from PPU memory.
  NMP::SPUDMAController::dmaWaitAll();

  //-----------------------------
  // Initialise the input parameters.
  
  // Sync event track.
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
    
  // Duration event matching op.
  AttribDataUInt* durationEventMatchingOp = parameters->getInputAttrib<AttribDataUInt>(2, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
   
  // Is loopable?
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_LOOP);
 
  // The first input Duration Event Track Sets.
  AttribDataDurationEventTrackSet* currentAttrib = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(4, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet::relocate(currentAttrib);
  EventTrackDurationSet* currentSet = currentAttrib->m_durationEventTrackSet;
    
  // The normalised blend weights
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(weightIndex, ATTRIB_SEMANTIC_CP_FLOAT);
  AttribDataFloatArray::relocate(weights);

  //-----------------------------
  // Find out the size of the biggest input duration event track set and allocate a resource of this size
  NMP::Memory::Format maxFormat = parameters->m_parameters[4].m_attribDataHandle.m_format; // size of the first duration event track set.
  for (uint32_t i = 5; i < weightIndex; ++i)
  {
    if (parameters->m_parameters[i].m_attribDataHandle.m_format.size > maxFormat.size)
      maxFormat = parameters->m_parameters[i].m_attribDataHandle.m_format;
  }
  NMP::Memory::Resource attrResource = NMPAllocatorAllocateFromFormat(tempAllocator, maxFormat);
   
  //-----------------------------
  // Blend each EventTrackDurationSet in turn.
  AttribDataDurationEventTrackSet* blendedDurationEventTrackSet = NULL; // Output set.
  uint32_t maxNumTracks = 0;
  uint32_t maxNumEvents = 0;
  float weight;
  float accumulatedWeight = weights->m_values[0];
  AttribDataDurationEventTrackSet *nextAttrib;
  EventTrackDurationSet* nextSet;
  EventTrackDurationSet* workingSet;
  uint32_t dmaTags = 0;
  uint32_t paramIndex = 5; // Second duration event track set

  for (uint32_t i = 1; i < numChildInputs; ++i, ++paramIndex)
  {
    // Load next event track set in to already allocated memory.
    dmaTags = NMP::SPUDMAController::dmaGet(
                attrResource.ptr, 
                (uint32_t)(parameters->m_parameters[paramIndex].m_attribDataHandle.m_attribData), 
                maxFormat.size);
    NMP::SPUDMAController::dmaWait(dmaTags);
    parameters->m_parameters[paramIndex].m_attribDataHandle.m_attribData = (MR::AttribData*)(attrResource.ptr);
    nextAttrib = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(paramIndex, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
    AttribDataDurationEventTrackSet::relocate(nextAttrib);
    nextSet = nextAttrib->m_durationEventTrackSet;

    // normalised blend weight
    weight = accumulatedWeight / (accumulatedWeight + weights->m_values[i]);

    // Safely estimate the requirement for the temporary events buffer set.
    EventTrackDurationSet::estimateBlendRequirements(
                currentSet,
                nextSet,
                syncEventTrack->m_syncEventTrack.getNumEvents(),
                maxNumTracks,
                maxNumEvents);

    // Now we know how many events we will need to output we can create the temp events buffer set.
    if (i == (numChildInputs - 1))
    {
      // This is the last set to process so build the result straight in to the output parameter.
      AttribDataSampledEventsCreateDesc desc(maxNumTracks, maxNumEvents);
      blendedDurationEventTrackSet = parameters->createOutputAttrib<AttribDataDurationEventTrackSet>(0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, &desc);
      workingSet = blendedDurationEventTrackSet->m_durationEventTrackSet;
    }
    else
    {
      // MORPH-21362: Correctly release temporary working memory.
      workingSet = EventTrackDurationSet::create(tempAllocator, maxNumTracks, maxNumEvents);
    }
    

    // Do the blend.
    workingSet->blend(
                currentSet,
                nextSet,
                &syncEventTrack->m_syncEventTrack,  // Space within which blended events must be defined.
                1.0f - weight,                      // Blend weighting.
                0.5f,                               // At what proportional weight do we select one events details over the other.
                durationEventMatchingOp->m_value,   // Whether and how to match up the events from each track.
                false,                              // If an event from one of the sources has not been matched to one in the other source should we just discard it?
                DURATION_EVENT_BLEND_OP_TYPE_BLEND, // Operation to perform on events that have been matched up.
                loopableAttrib->m_value,            //
                1.0f,                               // Within what range do we decide to match events together.
                (i == (numChildInputs - 1)) ? syncEventTrack->m_syncEventTrack.getStartEventIndex() : 0);                                 // Track set 1 event offset
   
    currentSet = workingSet;
    accumulatedWeight += weights->m_values[i];
  }

  NMP_ASSERT(blendedDurationEventTrackSet);
}

#else // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
// Implementation for platforms other than SPU.
void TaskBlendAllDurationEventTrackSets(Dispatcher::TaskParameters* parameters)
{
  // Get temp allocator.
  NMP::TempMemoryAllocator* tempAllocator = parameters->m_dispatcher->getTempMemoryAllocator();
  NMP_ASSERT(tempAllocator);

  //-----------------------------
  // Get input parameters.
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(1, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataUInt* durationEventMatchingOp = parameters->getInputAttrib<AttribDataUInt>(2, ATTRIB_SEMANTIC_DURATION_EVENT_MATCHING_OP);
  AttribDataBool* loopableAttrib = parameters->getInputAttrib<AttribDataBool>(3, ATTRIB_SEMANTIC_LOOP);
  AttribDataFloatArray* weights = parameters->getInputAttrib<AttribDataFloatArray>(parameters->m_numParameters - 1, ATTRIB_SEMANTIC_CP_FLOAT); // Normalised blend weights.

  //-----------------------------
  // Get the input Duration Event Track Sets in to a working array.
  uint32_t numChildInputs = parameters->m_numParameters - 5;
  NMP_ASSERT(numChildInputs > 1);
  NMP::Memory::Format memReq(sizeof(AttribDataDurationEventTrackSet*) * numChildInputs, NMP_VECTOR_ALIGNMENT);
  NMP::Memory::Resource memRes = NMPAllocatorAllocateFromFormat(tempAllocator, memReq);
  AttribDataDurationEventTrackSet** sourceDurationTracks = (AttribDataDurationEventTrackSet**)memRes.ptr;
  uint32_t paramIndex = 4;
  for (uint32_t i = 0; i < numChildInputs; ++i)
  {
    sourceDurationTracks[i] = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(paramIndex, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
    paramIndex++;
  }


  //-----------------------------
  // Blend each EventTrackDurationSet in turn.
  AttribDataDurationEventTrackSet* blendedDurationEventTrackSet = NULL; // Output set.
  EventTrackDurationSet* workingSet;
  uint32_t maxNumTracks = 0;
  uint32_t maxNumEvents = 0;
  float weight;
  float accumulatedWeight = weights->m_values[0];
  
  EventTrackDurationSet* currentSet = sourceDurationTracks[0]->m_durationEventTrackSet;
  
  for (uint32_t i = 1; i < numChildInputs; ++i)
  {
    EventTrackDurationSet* nextSet = sourceDurationTracks[i]->m_durationEventTrackSet;

    // normalised blend weight
    weight = accumulatedWeight / (accumulatedWeight + weights->m_values[i]);
    
    // Safely estimate the requirement for the temporary events buffer set.
    EventTrackDurationSet::estimateBlendRequirements(
              currentSet,
              nextSet,
              syncEventTrack->m_syncEventTrack.getNumEvents(),
              maxNumTracks,
              maxNumEvents);
    
    // Now we know how many events we will need to output we can create the temp events buffer set.
    if (i == (numChildInputs - 1))
    {
      // This is the last set to process so build the result straight in to the output parameter.
      AttribDataSampledEventsCreateDesc desc(maxNumTracks, maxNumEvents);
      blendedDurationEventTrackSet = parameters->createOutputAttrib<AttribDataDurationEventTrackSet>(0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, &desc);
      workingSet = blendedDurationEventTrackSet->m_durationEventTrackSet;
    }
    else
    {
      // MORPH-21362: Correctly release temporary working memory.
      workingSet = EventTrackDurationSet::create(tempAllocator, maxNumTracks, maxNumEvents);
    }

    // Do the blend.
    workingSet->blend(
              currentSet,
              nextSet,
              &syncEventTrack->m_syncEventTrack,  // Space within which blended events must be defined.
              1.0f - weight,                      // Blend weighting.
              0.5f,                               // At what proportional weight do we select one events details over the other.
              durationEventMatchingOp->m_value,   // Whether and how to match up the events from each track.
              false,                              // If an event from one of the sources has not been matched to one in the other source should we just discard it?
              DURATION_EVENT_BLEND_OP_TYPE_BLEND, // Operation to perform on events that have been matched up.
              loopableAttrib->m_value,            //
              1.0f,                               // Within what range do we decide to match events together.
              (i == (numChildInputs - 1)) ? syncEventTrack->m_syncEventTrack.getStartEventIndex() : 0);                                 // Track set 1 event offset
   
    currentSet = workingSet;
    accumulatedWeight += weights->m_values[i];
  }
}

#endif // NM_HOST_CELL_SPU

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeDurationEventTrackSet(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_DURATION_EVENT_TRACK_SET);
  AttribDataDurationEventTrackSet* inputAttrib = (AttribDataDurationEventTrackSet*) refAttrib;

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::MemoryAllocator* allocator = parameters->m_parameters[1].m_attribDataHandle.m_attribData->m_allocator;
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    inputAttrib->copyAndPack(resource, allocator, parameters->m_parameters[1].m_attribDataHandle.m_attribData->getRefCount());
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskMirrorDurationEvents(Dispatcher::TaskParameters* parameters)
{
  AttribDataDurationEventTrackSet* inputDurationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(0, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataMirroredAnimMapping* animMirrorMappingAttrib = parameters->getInputAttrib<AttribDataMirroredAnimMapping>(2, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING);

  // Duplicate the input track in our output track.
  validateTaskParam(1, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET, TPARAM_FLAG_OUTPUT, parameters->m_parameters);
  NMP::Memory::Format memReqs = inputDurationEventTrackSet->getPackedInstanceMemoryRequirements();
  NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
  AttribDataDurationEventTrackSet* outputDurationEventTrackSet = inputDurationEventTrackSet->copyAndPack(resource, parameters->m_parameters[1].m_attribDataHandle.m_attribData->getRefCount());
  parameters->m_parameters[1].m_attribDataHandle.m_attribData = outputDurationEventTrackSet;
  parameters->m_parameters[1].m_attribDataHandle.m_format = memReqs;

  // Having duplicated it, mirror the events and track ID's.
  // Iterate over all the tracks.
  for (uint32_t i = 0; i != outputDurationEventTrackSet->m_durationEventTrackSet->getNumTracks(); ++i)
  {
    EventTrackDuration* track = outputDurationEventTrackSet->m_durationEventTrackSet->getTrack(i);

    // mirror track userdata.
    uint32_t trackId = animMirrorMappingAttrib->findTrackIDMapping(track->getUserData());
    track->setUserData(trackId);

    // MORPH-21363: Mirror the runtime track id.

    // mirror event userdata.
    EventDuration* event = track->getEventsListHead();
    while (event)
    {
      uint32_t eventID = animMirrorMappingAttrib->findEventMappingID(event->getUserData());
      event->setUserData(eventID);
      event = event->getNext(track->getPool());
    }
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskMirrorSampledAndSampleDurationEvents(Dispatcher::TaskParameters* parameters)
{
  AttribDataSampledEvents* sourceSampledEventsBuffer = parameters->getInputAttrib<AttribDataSampledEvents>(1, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  AttribDataDurationEventTrackSet* durationEventTrackSet = parameters->getInputAttrib<AttribDataDurationEventTrackSet>(2, ATTRIB_SEMANTIC_DURATION_EVENT_TRACK_SET);
  AttribDataSyncEventTrack* syncEventTrack = parameters->getInputAttrib<AttribDataSyncEventTrack>(3, ATTRIB_SEMANTIC_SYNC_EVENT_TRACK);
  AttribDataPlaybackPos* fractionalTimePos = parameters->getInputAttrib<AttribDataPlaybackPos>(4, ATTRIB_SEMANTIC_FRACTION_POS);
  AttribDataBool* loopableAttrib = parameters->getOptionalInputAttrib<AttribDataBool>(5, ATTRIB_SEMANTIC_LOOP);
  AttribDataMirroredAnimMapping* mirroredAnimMapping = parameters->getInputAttrib<AttribDataMirroredAnimMapping>(6, ATTRIB_SEMANTIC_MIRRORED_ANIM_MAPPING);

  //-----------------------------
  // Find out if the blend should be loopable.
  bool loopable = true;
  if (loopableAttrib)
    loopable = loopableAttrib->m_value;

  //-----------------------------
  // Sampling duration event buffer.
  SampledCurveEventsBuffer workingSampledDurationEventsBuffer;
  SampledCurveEvent workingSampledDurationEventsArray[MAX_NUM_CURVE_EVENTS];
  workingSampledDurationEventsBuffer.init(MAX_NUM_CURVE_EVENTS, workingSampledDurationEventsArray);
  durationEventTrackSet->m_durationEventTrackSet->sample(
    &workingSampledDurationEventsBuffer,
    fractionalTimePos->m_currentPosAdj,
    fractionalTimePos->m_previousPosAdj,
    &(syncEventTrack->m_syncEventTrack),
    loopable);

  //-----------------------------
  // Calculate the event count totals that we need to output.
  uint32_t totalNumTriggeredDiscreteEvents = sourceSampledEventsBuffer->m_discreteBuffer->getNumTriggeredEvents();
  uint32_t sourceSampledCount = sourceSampledEventsBuffer->m_curveBuffer->getNumSampledEvents();
  uint32_t workingSampledCount = workingSampledDurationEventsBuffer.getNumSampledEvents();
  uint32_t totalNumCurveEventSamples = sourceSampledCount + workingSampledCount;

  // create the output sampled events buffer
  AttribDataSampledEventsCreateDesc desc(totalNumTriggeredDiscreteEvents, totalNumCurveEventSamples);
  AttribDataSampledEvents* sampledEvents = parameters->createOutputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER, &desc);

  // fill with sampled discreet events from source
  sampledEvents->m_discreteBuffer->fillAt(0, sourceSampledEventsBuffer->m_discreteBuffer);

  // combine sampled duration events with input events
  sampledEvents->m_curveBuffer->combine(sourceSampledEventsBuffer->m_curveBuffer, &workingSampledDurationEventsBuffer, 0.5f);

  // mirror all events
  sampledEvents->m_curveBuffer->setNumSampledEvents(sourceSampledCount); // This prevents us from mirroring duration event samples that
                                                                         // have already been mirrored in the duration event tracks themselves.
  subTaskMirrorSampledEvents(sampledEvents, sampledEvents, mirroredAnimMapping);
  sampledEvents->m_curveBuffer->setNumSampledEvents(totalNumCurveEventSamples);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
