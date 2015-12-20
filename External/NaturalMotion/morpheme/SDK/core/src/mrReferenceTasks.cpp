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
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeBool(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataBool>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_BOOL);
  AttribDataBool* inputAttrib = (AttribDataBool*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataBool::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataBool* outputAttrib = (AttribDataBool*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeUInt(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataUInt>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_UINT);
  AttribDataUInt* inputAttrib = (AttribDataUInt*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataUInt::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataUInt* outputAttrib = (AttribDataUInt*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeFloat(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataFloat>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_FLOAT);
  AttribDataFloat* inputAttrib = (AttribDataFloat*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataFloat::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataFloat* outputAttrib = (AttribDataFloat*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeVector3(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataVector3>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_VECTOR3);
  AttribDataVector3* inputAttrib = (AttribDataVector3*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataVector3::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataVector3* outputAttrib = (AttribDataVector3*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeVector4(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataVector4>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_VECTOR4);
  AttribDataVector4* inputAttrib = (AttribDataVector4*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataVector4::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataVector4* outputAttrib = (AttribDataVector4*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeBoolArray(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataBoolArray>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_BOOL_ARRAY);
  AttribDataBoolArray* inputAttrib = (AttribDataBoolArray*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataBoolArray::getMemoryRequirements(inputAttrib->m_numValues);

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataBoolArray* outputAttrib = (AttribDataBoolArray*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
    AttribDataBoolArray::relocate(outputAttrib);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeIntArray(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataIntArray>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_INT_ARRAY);
  AttribDataIntArray* inputAttrib = (AttribDataIntArray*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataIntArray::getMemoryRequirements(inputAttrib->m_numValues);

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataIntArray* outputAttrib = (AttribDataIntArray*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
    AttribDataIntArray::relocate(outputAttrib);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeUIntArray(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataUIntArray>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_UINT_ARRAY);
  AttribDataUIntArray* inputAttrib = (AttribDataUIntArray*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataIntArray::getMemoryRequirements(inputAttrib->m_numValues);

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataUIntArray* outputAttrib = (AttribDataUIntArray*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
    AttribDataUIntArray::relocate(outputAttrib);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeFloatArray(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataFloatArray>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_FLOAT_ARRAY);
  AttribDataFloatArray* inputAttrib = (AttribDataFloatArray*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataFloatArray::getMemoryRequirements(inputAttrib->m_numValues);

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataFloatArray* outputAttrib = (AttribDataFloatArray*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
    AttribDataFloatArray::relocate(outputAttrib);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeUpdatePlaybackPos(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataUpdatePlaybackPos>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_UPDATE_PLAYBACK_POS);
  AttribDataUpdatePlaybackPos* inputAttrib = (AttribDataUpdatePlaybackPos*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataUpdatePlaybackPos::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataUpdatePlaybackPos* outputAttrib = (AttribDataUpdatePlaybackPos*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypePlaybackPos(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataPlaybackPos>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_PLAYBACK_POS);
  AttribDataPlaybackPos* inputAttrib = (AttribDataPlaybackPos*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataPlaybackPos::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataPlaybackPos* outputAttrib = (AttribDataPlaybackPos*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeUpdateSyncEventPlaybackPos(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataUpdateSyncEventPlaybackPos>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_UPDATE_SYNC_EVENT_PLAYBACK_POS);
  AttribDataUpdateSyncEventPlaybackPos* inputAttrib = (AttribDataUpdateSyncEventPlaybackPos*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataUpdateSyncEventPlaybackPos::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataUpdateSyncEventPlaybackPos* outputAttrib = (AttribDataUpdateSyncEventPlaybackPos*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeTransformBuffer(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataTransformBuffer>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_TRANSFORM_BUFFER);
  AttribDataTransformBuffer* inputAttrib = (AttribDataTransformBuffer*) refAttrib;
  NMP::Memory::Format memReqs = inputAttrib->getInstanceMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataTransformBuffer* outputAttrib = (AttribDataTransformBuffer*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
    AttribDataTransformBuffer::relocate(outputAttrib);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeTrajectoryDeltaTransform(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataTrajectoryDeltaTransform>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_TRAJECTORY_DELTA_TRANSFORM);
  AttribDataTrajectoryDeltaTransform* inputAttrib = (AttribDataTrajectoryDeltaTransform*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataTrajectoryDeltaTransform::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataTrajectoryDeltaTransform* outputAttrib = (AttribDataTrajectoryDeltaTransform*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeTransform(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataTransform>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_TRANSFORM);
  AttribDataTransform* inputAttrib = (AttribDataTransform*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataTransform::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataTransform* outputAttrib = (AttribDataTransform*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeSyncEventTrack(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters == 2);

  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataSyncEventTrack>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_SYNC_EVENT_TRACK);
  AttribDataSyncEventTrack* inputAttrib = (AttribDataSyncEventTrack*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataSyncEventTrack::getMemoryRequirements();

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataSyncEventTrack* outputAttrib = (AttribDataSyncEventTrack*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskCreateReferenceToInputAttribTypeSampledEvents(Dispatcher::TaskParameters* parameters)
{
  NMP_ASSERT(parameters->m_numParameters >= 2);

  // Get the input attrib.
  AttribDataSampledEvents* refAttrib = parameters->getInputAttrib<AttribDataSampledEvents>(0, parameters->m_parameters[0].m_attribAddress.m_semantic);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* inputAttrib = (AttribDataSampledEvents*) refAttrib;
  NMP::Memory::Format memReqs = AttribDataSampledEvents::getMemoryRequirements(
                                  refAttrib->m_discreteBuffer->getNumTriggeredEvents(),
                                  refAttrib->m_curveBuffer->getNumSampledEvents());

  if (parameters->m_parameters[1].m_taskParamFlags.areSet(TPARAM_FLAG_NEEDS_CREATE))
  {
    NMP::Memory::Resource resource = {parameters->m_parameters[1].m_attribDataHandle.m_attribData, parameters->m_parameters[1].m_attribDataHandle.m_format};
    NMP_ASSERT(resource.ptr);
    AttribDataSampledEvents* outputAttrib = (AttribDataSampledEvents*) resource.ptr;
    NMP::Memory::memcpy(((uint8_t*)outputAttrib) + sizeof(AttribData), ((uint8_t*)inputAttrib) + sizeof(AttribData), memReqs.size - sizeof(AttribData));
    AttribDataSampledEvents::relocate(outputAttrib);
  }
}

//----------------------------------------------------------------------------------------------------------------------
void TaskSampledEventsBufferEmitMessageAndPassThrough(Dispatcher::TaskParameters* parameters)
{
  // Get the input attrib.
  AttribData* refAttrib = parameters->getInputAttrib<AttribDataSampledEvents>(0, ATTRIB_SEMANTIC_SAMPLED_EVENTS_BUFFER);
  NMP_ASSERT(refAttrib && refAttrib->getType() == ATTRIB_TYPE_SAMPLED_EVENTS_BUFFER);
  AttribDataSampledEvents* sampledEventsAttrib = (AttribDataSampledEvents*) refAttrib;
  NMP_ASSERT(sampledEventsAttrib);

  AttribDataIntArray* eventUserDataIDsAttrib = parameters->getOptionalInputAttrib<AttribDataIntArray>(2, ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  
  // construct emitted messages
  EmittedMessagesUtil<3, EmittedMessageMap::Names> emittedMessages(parameters);

  if (eventUserDataIDsAttrib)  
  {
    NMP_ASSERT(eventUserDataIDsAttrib->getType() == ATTRIB_TYPE_INT_ARRAY);
    int32_t *eventUserDataIDs = eventUserDataIDsAttrib->m_values;
 
    // Search the triggered discrete events array from the child Node for the specified event user data IDs.
    NMP_ASSERT(eventUserDataIDsAttrib->m_numValues < 33);
    for (uint32_t i = 0; i < eventUserDataIDsAttrib->m_numValues; ++i)
    {
      if (sampledEventsAttrib->m_discreteBuffer->findIndexOfEventWithSourceEventUserData(0, eventUserDataIDs[i]) != INVALID_EVENT_INDEX)
      {
        // Emit the corresponding message if event user data is found.
        emittedMessages.set((EmittedMessageMap::Names)(1 << i));
      }
    }
  }

  // Pass through sampled events buffer
  TaskCreateReferenceToInputAttribTypeSampledEvents(parameters);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
