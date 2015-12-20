// Copyright (c) 2013 NaturalMotion.  All Rights Reserved.
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
#ifndef MR_NODE_OPERATOR_TIME_LAG_INPUT_H
#define MR_NODE_OPERATOR_TIME_LAG_INPUT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
#include "NMPlatform/NMRingBuffer.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTimeLagInputDef
/// \brief Specifies the configuration of an OperatorTimeLagInput node.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataTimeLagInputDef : public AttribData
{
public:  
  /// \brief The method this time lag operator node will use to buffer input values.
  enum BufferingMode
  {
    BUFFERING_MODE_EVERY_FRAME = 1,        ///< Buffers a value every frame.
    BUFFERING_MODE_FIXED_SAMPLE_INTERVALS  ///< Buffers values at at a fixed sampling rate.
  };

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataTimeLagInputDef* init(
    NMP::Memory::Resource& resource,
    BufferingMode          bufferingMode,     ///< The method this time lag operator node will use to buffer input values.
    float                  sampleFrequency,   ///< How many samples to take per second.
    uint32_t               sampleBufferSize,  ///< What is the maximum number of samples we can store at the above frequency.
    uint16_t               refCount = 0);

  static AttribDataHandle create(
    NMP::MemoryAllocator*  allocator,
    BufferingMode          bufferingMode,     ///< The method this time lag operator node will use to buffer input values.
    float                  sampleFrequency,   ///< How many samples to take per second.
    uint32_t               sampleBufferSize,  ///< What is the maximum number of samples we can store at the above frequency.
    uint16_t               refCount = 0);

  NM_INLINE AttribDataTimeLagInputDef() { setType(ATTRIB_TYPE_TIME_LAG_DEF); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return ATTRIB_TYPE_TIME_LAG_DEF; }

  /// For Manager registration.
  static void locate(AttribData* target);
  static void dislocate(AttribData* target);

public:
  BufferingMode m_bufferingMode;         ///< The method this time lag operator node will use to buffer input values.
  float         m_sampleFrequency;       ///< How many samples to take per second.
  float         m_sampleInterval;        ///< What is the interval between each recorded sample. 
                                         ///<  Values will not be recorded at exactly this interval because the frame rate may well be different.
  float         m_maximumSamplingPeriod; ///< What is the maximum recoding period that can be supported at the specified recording frequency.
  uint32_t      m_sampleBufferSize;      ///< What is the maximum number of samples we can store at the above frequency.
};

//----------------------------------------------------------------------------------------------------------------------
/// \class MR::AttribDataTimeLagInput
/// \brief Stores a changing value over a number of frames.
/// \ingroup AttribData
///
/// Each frames value is stored with an accumulated time value, indicating when it was created.
/// This class uses ring buffers to store the time values and the templated frame values (float, int vector3 etc). 
//----------------------------------------------------------------------------------------------------------------------
template <typename T_bufferingType, size_t T_AttribDataTypeEnum>
class AttribDataTimeLagInput : public AttribData
{
public: 
  //------------------------------
  static NMP::Memory::Format getMemoryRequirements(
    uint32_t frameBufferSize)
  {
    // Add space for the class itself.
    NMP::Memory::Format result(sizeof(AttribDataTimeLagInput), MR_ATTRIB_DATA_ALIGNMENT);

    // Add space for the buffers.
    result += NMP::RingBuffer<float>::getMemoryRequirements(frameBufferSize);
    result += NMP::RingBuffer<T_bufferingType>::getMemoryRequirements(frameBufferSize);

    return result;
  }

  //------------------------------
  static AttribDataTimeLagInput* init(
    NMP::Memory::Resource& resource,
    uint32_t               frameBufferSize,
    uint16_t               refCount = 0)
  {
    // The class itself.
    NMP::Memory::Format memReqsHdr(sizeof(AttribDataTimeLagInput), MR_ATTRIB_DATA_ALIGNMENT);
    AttribDataTimeLagInput* result = (AttribDataTimeLagInput*) resource.alignAndIncrement(memReqsHdr);

    result->m_accumulatedActiveTime = 0.0f;
    result->m_durationSinceLastSample = 0.0f;

    // The buffers.
    result->m_times = NMP::RingBuffer<float>::init(resource, frameBufferSize);
    result->m_values = NMP::RingBuffer<T_bufferingType>::init(resource, frameBufferSize);

    result->setType(T_AttribDataTypeEnum);
    result->setRefCount(refCount);

    return result;
  }
  
  //------------------------------
  static AttribDataHandle create(
    NMP::MemoryAllocator* allocator,
    uint32_t              frameBufferSize,
    uint16_t              refCount = 0)
  {
    AttribDataHandle handle;

    NMP::Memory::Format memReqs = AttribDataTimeLagInput::getMemoryRequirements(frameBufferSize);
    NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
    NMP_ASSERT(resource.ptr);
    AttribDataTimeLagInput* attribData = AttribDataTimeLagInput::init(resource, frameBufferSize, refCount);

    // Backchain the allocator so we know what to free this attribData with when we destroy it.
    attribData->m_allocator = allocator;

    handle.m_format     = memReqs;
    handle.m_attribData = attribData;

    return handle;
  }

  NM_INLINE AttribDataTimeLagInput() { setType(T_AttribDataTypeEnum); setRefCount(0); }
  NM_INLINE static AttribDataType getDefaultType() { return T_AttribDataTypeEnum; }
  
  //------------------------------
  static void locate(AttribData* target)
  {
    AttribDataTimeLagInput* result = (AttribDataTimeLagInput*)target;

    AttribData::locate(target);

    NMP::endianSwap(result->m_accumulatedActiveTime);
    NMP::endianSwap(result->m_durationSinceLastSample);

    // Buffers.
    REFIX_SWAP_PTR_RELATIVE(NMP::RingBuffer<float>, result->m_times, result);
    REFIX_SWAP_PTR_RELATIVE(NMP::RingBuffer<T_bufferingType>, result->m_values, result);

    result->m_times->locate();
    result->m_values->locate();
  }

  //------------------------------
  static void dislocate(AttribData* target)
  {
    AttribDataTimeLagInput* result = (AttribDataTimeLagInput*)target;

    result->m_times->dislocate();
    result->m_values->dislocate();

    UNFIX_SWAP_PTR_RELATIVE(NMP::RingBuffer<float>, result->m_times, result);
    UNFIX_SWAP_PTR_RELATIVE(NMP::RingBuffer<T_bufferingType>, result->m_values, result);

    NMP::endianSwap(result->m_accumulatedActiveTime);
    NMP::endianSwap(result->m_durationSinceLastSample);

    AttribData::dislocate(target);
  }

public:
  float m_accumulatedActiveTime;   ///< The duration for which this node has been actively buffering.
  float m_durationSinceLastSample; ///< 

  NMP::RingBuffer<float>*           m_times;  ///< The time when each sample was buffered.
  NMP::RingBuffer<T_bufferingType>* m_values; ///< Buffered samples.
};

//----------------------------------------------------------------------------------------------------------------------
// Node functions.
//----------------------------------------------------------------------------------------------------------------------
/// Templated cp pin update function.
/// e.g. nodeOperatorTimeLagInputOutputCPUpdate<int32_t, MR::AttribDataInt, MR::ATTRIB_TYPE_TIME_LAG_INPUT_INT>
/// Stores the changing input value, in an internal ring buffer, over n seconds.
/// Outputs the value in the buffer from n seconds ago and throws away anything older.
/// While the buffer has no times older than n seconds, it outputs the oldest value it has.
template <class T_bufferingType, class T_AttribDataType, size_t T_AttribDataTypeEnum>
AttribData* nodeOperatorTimeLagInputOutputCPUpdate(
  NodeDef* node,
  PinIndex outputCPPinIndex,
  Network* net)
{
  NMP_ASSERT(outputCPPinIndex < 2); // We have 2 output pins
  AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Update the connected input control parameters.
  T_AttribDataType* inputCPAttrib = net->updateInputCPConnection<T_AttribDataType>(node->getInputCPConnection(0), animSet);
  AttribDataFloat* inputCPLagTimeAttrib = net->updateInputCPConnection<AttribDataFloat>(node->getInputCPConnection(1), animSet);
  
  //-------------------
  // Get the output attributes.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *passThroughOutputCPPin = nodeBin->getOutputCPPin(0);
  T_AttribDataType* passThroughOutputCPAttrib = passThroughOutputCPPin->getAttribData<T_AttribDataType>();

  OutputCPPin *laggedOutputCPPin = nodeBin->getOutputCPPin(1);
  T_AttribDataType* laggedOutputCPAttrib = laggedOutputCPPin->getAttribData<T_AttribDataType>();

  //-------------------
  // Set the pass through value.
  passThroughOutputCPAttrib->m_value = inputCPAttrib->m_value;

  //-------------------
  // Get the Networks update time attribute (Global delta time).
  const uint32_t currFrameNo = net->getCurrentFrameNo();
  AttribDataUpdatePlaybackPos* netUpdateTimeAttrib = net->getOptionalAttribData<AttribDataUpdatePlaybackPos>(
    ATTRIB_SEMANTIC_UPDATE_TIME_POS,
    NETWORK_NODE_ID,
    INVALID_NODE_ID,
    currFrameNo);
  NMP_ASSERT(netUpdateTimeAttrib);

  //-------------------
  // Get node def data.
  AttribDataTimeLagInputDef* nodeStateDef = node->getAttribData<AttribDataTimeLagInputDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  //-------------------
  // Perform the lag task.

  // This returns the first frames value that falls below the delay time interval.
  // How close this is to the delay time itself is dependent on frame sampling rate.
  // Note that this mean that when the node is first activated the value input on this
  // frame will be returned until the node has been active for longer than the delay time.

  // Get the internal node state data.
  AttribDataTimeLagInput<T_bufferingType, T_AttribDataTypeEnum>* internalStateAttrib = 
    net->getAttribData< AttribDataTimeLagInput<T_bufferingType, T_AttribDataTypeEnum> >(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID());

  if (passThroughOutputCPPin->m_lastUpdateFrame != (currFrameNo - 1))
  {
    // We were not updated last frame, so clear our history and start again from here.
    internalStateAttrib->m_times->clear();
    internalStateAttrib->m_values->clear();
    internalStateAttrib->m_accumulatedActiveTime = netUpdateTimeAttrib->m_value;
    internalStateAttrib->m_durationSinceLastSample = netUpdateTimeAttrib->m_value;
  }
  else
  {
    // We were updated last frame, so continue accumulating time on the existing buffering session.
    internalStateAttrib->m_accumulatedActiveTime += netUpdateTimeAttrib->m_value;
    internalStateAttrib->m_durationSinceLastSample += netUpdateTimeAttrib->m_value;
  }
    
  if (nodeStateDef->m_bufferingMode == AttribDataTimeLagInputDef::BUFFERING_MODE_EVERY_FRAME || 
      (internalStateAttrib->m_durationSinceLastSample >= nodeStateDef->m_sampleInterval))
  {
    // Add our new accumulated time and input value to the buffer head.
    internalStateAttrib->m_times->forcePushHead(&(internalStateAttrib->m_accumulatedActiveTime));
    internalStateAttrib->m_values->forcePushHead(&(inputCPAttrib->m_value));
    internalStateAttrib->m_durationSinceLastSample = 0.0f;
  }
  
  // Remove entries from the tail until the difference between the head and tail times is less than the desired lag time.
  // Use this tail value as our lagged output.
  internalStateAttrib->m_values->getNthEntryFromTail(laggedOutputCPAttrib->m_value, 0);
  float timeHead = 0;
  float timeTail = 0;
  internalStateAttrib->m_times->getNthEntryFromHead(timeHead, 0);
  internalStateAttrib->m_times->getNthEntryFromTail(timeTail, 0);

  // Make sure the lag time is not negative.
  float timeLagValue = NMP::maximum<float>(0.0f, inputCPLagTimeAttrib->m_value);

  while ((timeHead - timeTail) > timeLagValue)
  {
    internalStateAttrib->m_values->getNthEntryFromTail(laggedOutputCPAttrib->m_value, 0);
    internalStateAttrib->m_times->popTail();
    internalStateAttrib->m_values->popTail();
    internalStateAttrib->m_times->getNthEntryFromHead(timeHead, 0);
    internalStateAttrib->m_times->getNthEntryFromTail(timeTail, 0);
  }
  
  
  //-------------------
  // The requested output pin will automatically be flagged as having been updated this frame,
  // but we need to make sure that all pins we have updated are correctly flagged.
  passThroughOutputCPPin->m_lastUpdateFrame = currFrameNo;
  laggedOutputCPPin->m_lastUpdateFrame = currFrameNo;


  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

//----------------------------------------------------------------------------------------------------------------------
/// Templated node initialisation function.
/// e.g. nodeOperatorTimeLagInputInitInstance<float, MR::AttribDataFloat, MR::ATTRIB_TYPE_TIME_LAG_INPUT_FLOAT>
/// Initialise the pass through and lagged pin output AttribDatas.
template <class T_bufferingType, class T_AttribDataType, size_t T_AttribDataTypeEnum>
void nodeOperatorTimeLagInputInitInstance(NodeDef* node, Network* net)
{
  // Create the pin output attributes.
  NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  OutputCPPin *passThroughOutputCPPin = nodeBin->getOutputCPPin(0);
  passThroughOutputCPPin->m_attribDataHandle = T_AttribDataType::create(net->getPersistentMemoryAllocator());
  passThroughOutputCPPin->m_lastUpdateFrame = 0;

  OutputCPPin *laggedOutputCPPin = nodeBin->getOutputCPPin(1);
  laggedOutputCPPin->m_attribDataHandle = T_AttribDataType::create(net->getPersistentMemoryAllocator());
  laggedOutputCPPin->m_lastUpdateFrame = 0;

  // Initialise any pins that are using default data.
  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }

  // Get node def data.
  AttribDataTimeLagInputDef* nodeStateDef = node->getAttribData<AttribDataTimeLagInputDef>(ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  // Create the internal state data.
  AttribDataHandle handle = AttribDataTimeLagInput<T_bufferingType, T_AttribDataTypeEnum>::create(
                                                                            net->getPersistentMemoryAllocator(), 
                                                                            nodeStateDef->m_sampleBufferSize);
  AttribAddress stateAddress(ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, node->getNodeID(), INVALID_NODE_ID, VALID_FOREVER);
  net->addAttribData(stateAddress, handle, LIFESPAN_FOREVER);
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_TIME_LAG_INPUT_H
//----------------------------------------------------------------------------------------------------------------------
