

#include "Euphoria/Nodes/erNodeOperatorOrientationInFreeFall.h"

#include "physics/mrPhysics.h"
#include "Euphoria/erBody.h"
#include "Euphoria/erLimb.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"

namespace ER
{
  //----------------------------------------------------------------------------------------------------------------------
  // AttribDataOperatorOrientationInFreeFallDef functions.
  //----------------------------------------------------------------------------------------------------------------------
  void AttribDataOperatorOrientationInFreeFallDef::locate(MR::AttribData* target)
  {
    MR::AttribData::locate(target);

    static_cast< AttribDataOperatorOrientationInFreeFallDef* >(target)->endianSwap();
  }

  //----------------------------------------------------------------------------------------------------------------------
  void AttribDataOperatorOrientationInFreeFallDef::dislocate(MR::AttribData* target)
  {
    MR::AttribData::dislocate(target);

    static_cast< AttribDataOperatorOrientationInFreeFallDef* >(target)->endianSwap();
  }

  //----------------------------------------------------------------------------------------------------------------------
  void AttribDataOperatorOrientationInFreeFallDef::endianSwap()
  {
    NMP::endianSwap(m_startOrientationTime);
    NMP::endianSwap(m_stopOrientationTime);

    NMP::endianSwap(m_startOrientationTransitionTime);
    NMP::endianSwap(m_stopOrientationTransitionTime);

    NMP::endianSwap(m_weightOutputBeforeOrientation);
    NMP::endianSwap(m_weightOutputDuringOrientation);
    NMP::endianSwap(m_weightOutputAfterOrientation);

    NMP::endianSwap(m_startOrientationAtTimeBeforeImpact);
    NMP::endianSwap(m_stopOrientationAtTimeBeforeImpact);
  }

  //----------------------------------------------------------------------------------------------------------------------
  AttribDataOperatorOrientationInFreeFallDef* AttribDataOperatorOrientationInFreeFallDef::create(
    NMP::MemoryAllocator* allocator,
    uint16_t              refCount)
  {
    AttribDataOperatorOrientationInFreeFallDef* result;

    NMP::Memory::Format memReqs = AttribDataOperatorOrientationInFreeFallDef::getMemoryRequirements();
    NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
    NMP_ASSERT(resource.ptr);
    result = AttribDataOperatorOrientationInFreeFallDef::init(resource, refCount);

    // Store the allocator so we know where to free this attribData from when we destroy it.
    result->m_allocator = allocator;

    return result;
  }

  //----------------------------------------------------------------------------------------------------------------------
  NMP::Memory::Format AttribDataOperatorOrientationInFreeFallDef::getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(AttribDataOperatorOrientationInFreeFallDef), MR_ATTRIB_DATA_ALIGNMENT);
    // Multiple of the attrib data alignment
    result.size = NMP::Memory::align(result.size, MR_ATTRIB_DATA_ALIGNMENT);
    return result;
  }

  //----------------------------------------------------------------------------------------------------------------------
  AttribDataOperatorOrientationInFreeFallDef* AttribDataOperatorOrientationInFreeFallDef::init(
    NMP::Memory::Resource& resource,
    uint16_t               refCount)
  {
    NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
    resource.align(MR_ATTRIB_DATA_ALIGNMENT);
    AttribDataOperatorOrientationInFreeFallDef* const result =
      static_cast< AttribDataOperatorOrientationInFreeFallDef* >(resource.ptr);
    resource.increment(sizeof(AttribDataOperatorOrientationInFreeFallDef));
    resource.align(MR_ATTRIB_DATA_ALIGNMENT);

    result->setType(ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_DEF);
    result->setRefCount(refCount);

    result->m_startOrientationTime = 0.0f;
    result->m_stopOrientationTime = 0.0f;

    result->m_startOrientationTransitionTime = 0.0f;
    result->m_stopOrientationTransitionTime = 0.0f;

    result->m_weightOutputBeforeOrientation = 0.0f;
    result->m_weightOutputDuringOrientation = 0.0f;
    result->m_weightOutputAfterOrientation = 0.0f;

    result->m_startOrientationAtTimeBeforeImpact = false;
    result->m_stopOrientationAtTimeBeforeImpact  = true;

    return result;
  }

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorOrientationInFreeFallState functions.
//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorOrientationInFreeFallState::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataOperatorOrientationInFreeFallState), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorOrientationInFreeFallState* AttribDataOperatorOrientationInFreeFallState::init(
  NMP::Memory::Resource& resource,
  uint16_t refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorOrientationInFreeFallState* result =
    static_cast< AttribDataOperatorOrientationInFreeFallState* >(resource.ptr);
  resource.increment(sizeof(AttribDataOperatorOrientationInFreeFallState));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_ORIENTATIONINFREEFALL_STATE);
  result->setRefCount(refCount);

  result->m_timeInFreefall = 0.0f;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorOrientationInFreeFallState::locate(MR::AttribData* target)
{
  MR::AttribData::locate(target);

  static_cast< AttribDataOperatorOrientationInFreeFallState* >(target)->endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorOrientationInFreeFallState::dislocate(MR::AttribData* target)
{
  MR::AttribData::dislocate(target);

  static_cast< AttribDataOperatorOrientationInFreeFallState* >(target)->endianSwap();
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataHandle AttribDataOperatorOrientationInFreeFallState::create(NMP::MemoryAllocator* allocator)
{
  MR::AttribDataHandle result;

  const NMP::Memory::Format memReqs(getMemoryRequirements());

  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorOrientationInFreeFallState::endianSwap()
{
  NMP::endianSwap(m_timeInFreefall);
}


//----------------------------------------------------------------------------------------------------------------------
// Operator Orientation In Freefall functions.
//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
template< typename attributeType, typename valueType > static void writeToOutputCPPin(
  const MR::PinIndex OutCPPinID,
  const valueType& value,
  MR::NodeDef* const node,
  MR::Network* const net)
{
  NMP_ASSERT(net);  // Null check.
  NMP_ASSERT(node); // Null check.

  if (net && node)
  {
    MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
    MR::OutputCPPin* const outputCPPin(nodeBin->getOutputCPPin(OutCPPinID));

    // Create the output attributes if they don't already exist.
    if (!outputCPPin->m_attribDataHandle.m_attribData)
    {
      outputCPPin->m_attribDataHandle = attributeType::create(net->getPersistentMemoryAllocator(), value);
    }
    else
    {
      // Write value to existing output pin
      attributeType* const outCPAttrib(outputCPPin->getAttribData<attributeType>());

      if (outCPAttrib)
      {
        outCPAttrib->m_value = value;
      }
    }

    // The requested output pin will automatically be flagged as having been updated this frame,
    // but we need to make sure that all pins we have updated are correctly flagged.
    outputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();
  }
}

//----------------------------------------------------------------------------------------------------------------------
static AttribDataOperatorOrientationInFreeFallState* getStateData(MR::NodeDef* node, MR::Network* net)
{
  MR::NodeID nodeID = node->getNodeID();
  MR::NodeBinEntry* stateEntry = net->getAttribDataNodeBinEntry(
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    nodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME);
  NMP_ASSERT(stateEntry);

  return stateEntry->getAttribData<AttribDataOperatorOrientationInFreeFallState>();
}

//----------------------------------------------------------------------------------------------------------------------
static bool isCharacterInContact(MR::Network* const net)
{
  NMP_ASSERT(net); // Null check.

  const MR::PhysicsRig* const physicsRig(MR::getPhysicsRig(net));
  NMP_ASSERT(physicsRig); // Null check.

  const ER::Body* const euphoriaBody(ER::Body::getFromPhysicsRig(physicsRig));
  NMP_ASSERT(euphoriaBody); // Null check.

  return euphoriaBody->isInContact();
}

//----------------------------------------------------------------------------------------------------------------------
// Determine whether or not the character is in freefall and update the time in freefall accordingly.
static void updateTimeInFreefall(
  MR::Network* const net,
  AttribDataOperatorOrientationInFreeFallState* const stateData)
{
  if (isCharacterInContact(net))
  {
    stateData->m_timeInFreefall = 0.0f;
  }
  else
  {
    stateData->m_timeInFreefall += net->getLastUpdateTimeStep();
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Operator Orientation In Freefall init function.
void nodeOperatorOrientationInFreeFallInitInstance(MR::NodeDef* node, MR::Network* net)
{
  // create per-instance state data.
    MR::NodeID nodeID = node->getNodeID();
    MR::AttribDataHandle handle =
      AttribDataOperatorOrientationInFreeFallState::create(net->getPersistentMemoryAllocator());
    MR::AttribAddress stateAddress(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeID, MR::INVALID_NODE_ID, MR::VALID_FOREVER);
    net->addAttribData(stateAddress, handle, MR::LIFESPAN_FOREVER);
    if(node->getNumReflexiveCPPins() > 0)
    {
      nodeInitPinAttribDataInstance(node, net);
    }
}

//----------------------------------------------------------------------------------------------------------------------
// Operator Orientation In Freefall update function.
MR::AttribData* nodeOperatorOrientationInFreeFallOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, /// The output pin we have been asked to update.
  MR::Network* net)
{
  NMP_ASSERT(node); // Null check.
  NMP_ASSERT(net);  // Null check.

  NMP_ASSERT(outputCPPinIndex < NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_MAX); // Pin index range check.

  static const float timeToImpactEpsilon(0.0001f); // Assume no hazard when time to impact is less than this threshold.

  MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Get the attribute data.
  AttribDataOperatorOrientationInFreeFallDef* const attribNodeDef(
    node->getAttribData<AttribDataOperatorOrientationInFreeFallDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF));

  // Update the input parameters.
  MR::AttribDataVector3* const inputOrientationCP(
    net->updateOptionalInputCPConnection<MR::AttribDataVector3>(
    node->getInputCPConnection(NODE_OPERATOR_ORIENTATIONINFREEFALL_IN_CP_PINID_ORIENTATION), animSet));

  MR::AttribDataFloat* const inputTimeToImpactCP(
    net->updateOptionalInputCPConnection<MR::AttribDataFloat>(
    node->getInputCPConnection(NODE_OPERATOR_ORIENTATIONINFREEFALL_IN_CP_PINID_TIMETOIMPACT), animSet));

  // Retrive state data.
  AttribDataOperatorOrientationInFreeFallState* const stateData(getStateData(node, net));
  NMP_ASSERT(stateData); // Null check;

  updateTimeInFreefall(net, stateData);

  // initialise output params.
  NMP::Vector3 orientationOutput(NMP::Vector3::InitTypeZero);
  float        weightOutput(0.0f);

  // Do nothing when not in freefall.
  if (stateData->m_timeInFreefall > 0.0f)
  {
    // A TimeToImpact of zero is considered invalid and indicates that no hazard has been detected. Replace zero 
    // TimeToImpact with float max to simplify following calculations.
    const float timeToImpact( 
      inputTimeToImpactCP && (inputTimeToImpactCP->m_value > timeToImpactEpsilon)
      ? inputTimeToImpactCP->m_value
      : FLT_MAX);

    const float timeInFreefall(stateData->m_timeInFreefall);

    // Determine the weight that should be used to interpolate between no orientation (0) and the input orientation (1).
    // - This is complicated because times to start and stop driving the character's orientation can change as new 
    //   hazards are detected or the character exits and re-enters freefall.

    // 1 - Calculate the time (in seconds) since the we began driving the character's orientation. This can be negative,
    //     indicating that we have not yet started.
    const float timeAfterStart =
      (attribNodeDef->m_startOrientationAtTimeBeforeImpact) 
      ? attribNodeDef->m_startOrientationTime - timeToImpact
      : timeInFreefall - attribNodeDef->m_startOrientationTime;

    // 2 - Calculate the time (in seconds) before we should finished driving the character's orientation. This can be 
    //     negative, indicating that we have passed the point where we should stop.
    const float timeBeforeStop =
      (attribNodeDef->m_stopOrientationAtTimeBeforeImpact) 
      ? timeToImpact - attribNodeDef->m_stopOrientationTime
      : attribNodeDef->m_stopOrientationTime - timeInFreefall;

    // 3 - Calculate the weight with which to blend in the input orientation. We start blending in the orientation at
    //     "start time" and finish blending out the orientation at "stop time". This means that the "during" blend 
    //     weight should be 0 at "start time" and at "stop time".
    const float beforeWeight(
      NMP::clampValue(1.0f - (timeAfterStart / attribNodeDef->m_startOrientationTransitionTime), 0.0f, 1.0f));
    const float afterWeight(
      NMP::clampValue(1.0f - (timeBeforeStop / attribNodeDef->m_stopOrientationTransitionTime), 0.0f, 1.0f));
    const float duringWeight(1.0f - (beforeWeight + afterWeight));
  
    // Calculate weight output by interpolating between before, during and after weight attribs.
    weightOutput =
      (beforeWeight * attribNodeDef->m_weightOutputBeforeOrientation) +
      (duringWeight * attribNodeDef->m_weightOutputDuringOrientation) +
      (afterWeight  * attribNodeDef->m_weightOutputAfterOrientation);

    if (inputOrientationCP)
    {
      // Calculate output orientation by scaling input orientation by "during" weight.
      orientationOutput = inputOrientationCP->m_value * duringWeight;
    }
  }

  // Write to output CPs
  writeToOutputCPPin< MR::AttribDataVector3 >(
    NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_ORIENTATION, orientationOutput, node, net);
  writeToOutputCPPin< MR::AttribDataFloat >
    (NODE_OPERATOR_ORIENTATIONINFREEFALL_OUT_CP_PINID_WEIGHT, weightOutput, node, net);

  // Return the value of the requested output cp pin.
  MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

}