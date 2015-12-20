// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

//----------------------------------------------------------------------------------------------------------------------
#include "physics/Nodes/mrNodeOperatorApplyImpulse.h"
#include "physics/mrPhysics.h"
#include "physics/mrPhysicsRig.h"
#include "morpheme/mrNetworkLogger.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorApplyImpulseDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorApplyImpulseDef::locate(MR::AttribData* target)
{
  AttribDataOperatorApplyImpulseDef* result = (AttribDataOperatorApplyImpulseDef*)target;

  MR::AttribData::locate(target);
  NMP::endianSwap(result->m_impulseType);
  NMP::endianSwap(result->m_multiplyByTimestep);
  NMP::endianSwap(result->m_directionInWorldSpace);
  NMP::endianSwap(result->m_positionInWorldSpace);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorApplyImpulseDef::dislocate(MR::AttribData* target)
{
  AttribDataOperatorApplyImpulseDef* result = (AttribDataOperatorApplyImpulseDef*)target;

  MR::AttribData::dislocate(target);
  NMP::endianSwap(result->m_impulseType);
  NMP::endianSwap(result->m_multiplyByTimestep);
  NMP::endianSwap(result->m_directionInWorldSpace);
  NMP::endianSwap(result->m_positionInWorldSpace);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorApplyImpulseDef* AttribDataOperatorApplyImpulseDef::create(
  NMP::MemoryAllocator* allocator,
  int                   impulseType,
  bool                  positionInWorldSpace,
  bool                  directionInWorldSpace,
  uint16_t              refCount)
{
  AttribDataOperatorApplyImpulseDef* result;

  NMP::Memory::Format memReqs = AttribDataOperatorApplyImpulseDef::getMemoryRequirements();
  NMP::Memory::Resource resource = NMPAllocatorAllocateFromFormat(allocator, memReqs);
  NMP_ASSERT(resource.ptr);
  result = AttribDataOperatorApplyImpulseDef::init(
    resource,
    impulseType,
    positionInWorldSpace,
    directionInWorldSpace,
    refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorApplyImpulseDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataOperatorApplyImpulseDef), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorApplyImpulseDef* AttribDataOperatorApplyImpulseDef::init(
  NMP::Memory::Resource& resource,
  int                    impulseType,
  bool                   positionInWorldSpace,
  bool                   directionInWorldSpace,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorApplyImpulseDef* result = (AttribDataOperatorApplyImpulseDef*)resource.ptr;
  resource.increment(sizeof(AttribDataOperatorApplyImpulseDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_APPLYIMPULSE_DEF);
  result->setRefCount(refCount);

  result->m_impulseType = impulseType;
  result->m_multiplyByTimestep = false;
  if (impulseType > 1)
  {
    result->m_impulseType -= 2;
    result->m_multiplyByTimestep = true;
  }
  result->m_directionInWorldSpace = directionInWorldSpace;
  result->m_positionInWorldSpace = positionInWorldSpace;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* nodeOperatorApplyImpulseOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  MR::Network* net)
{
  NMP_ASSERT(outputCPPinIndex < NODE_OPERATOR_APPLYIMPULSE_OUT_CP_PINID_COUNT);

  MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  // Create the output attribute if it doesn't already exist.
  MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  MR::OutputCPPin* performanceOutputCPPin =
    nodeBin->getOutputCPPin(NODE_OPERATOR_APPLYIMPULSE_OUT_CP_PINID_PERFORMANCE);
  if (!performanceOutputCPPin->m_attribDataHandle.m_attribData)
    performanceOutputCPPin->m_attribDataHandle = MR::AttribDataInt::create(net->getPersistentMemoryAllocator(), 0);

  // The requested output pin will automatically be flagged as having been updated this frame, but
  // we need to make sure that all pins we have updated are correctly flagged.
  performanceOutputCPPin->m_lastUpdateFrame = net->getCurrentFrameNo();

  // Update input parameter - only do the others if the magnitude > 0.
  MR::AttribDataFloat* inputLocalMagnitudeAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(1), animSet);
  float inputLocalMagnitude = inputLocalMagnitudeAttrib ? inputLocalMagnitudeAttrib->m_value : 0.0f;

  MR::AttribDataFloat* inputFullBodyMagnitudeAttrib =
    net->updateOptionalInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(4), animSet);
  float inputFullBodyMagnitude = inputFullBodyMagnitudeAttrib  ? inputFullBodyMagnitudeAttrib ->m_value : 0.0f;

  // Get the definition data.
  AttribDataOperatorApplyImpulseDef* applyImpulseDef =
    node->getAttribData<AttribDataOperatorApplyImpulseDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);

  if (applyImpulseDef->m_multiplyByTimestep)
  {
    inputFullBodyMagnitude *= net->getLastUpdateTimeStep();
    inputLocalMagnitude *= net->getLastUpdateTimeStep();
  }

  // Perform the actual task if needed
  if (inputLocalMagnitude != 0.0f || inputFullBodyMagnitude != 0.0f)
  {
    MR::PhysicsRig* physicsRig = getPhysicsRig(net);
    if (physicsRig != 0 && physicsRig->isReferenced())
    {
      // Update/get all the other inputs
      //

      // Get optional CP's or their attrib def data defaults if not connected
      //
      MR::AttribDataFloat* inputLocalAngularMultiplierAttrib =
        net->updateInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(2), animSet);
      float inputLocalAngularMultiplier = inputLocalAngularMultiplierAttrib->m_value;

      MR::AttribDataFloat* inputLocalResponseRatioAttrib =
        net->updateInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(3), animSet);
      float inputLocalResponseRatio = inputLocalResponseRatioAttrib->m_value;

      MR::AttribDataFloat* inputFullBodyLinearMultiplierAttrib =
        net->updateInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(5), animSet);
      float inputFullBodyLinearMultiplier = inputFullBodyLinearMultiplierAttrib->m_value;

      MR::AttribDataFloat* inputFullBodyAngularMultiplierAttrib =
        net->updateInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(6), animSet);
      float inputFullBodyAngularMultiplier = inputFullBodyAngularMultiplierAttrib->m_value;

      MR::AttribDataFloat* inputFullBodyResponseRatioAttrib =
        net->updateInputCPConnection<MR::AttribDataFloat>(node->getInputCPConnection(7), animSet);
      float inputFullBodyResponseRatio = inputFullBodyResponseRatioAttrib->m_value;

      // Get remaining CP's
      //
      MR::AttribDataInt* inputPartIndexAttrib =
        net->updateOptionalInputCPConnection<MR::AttribDataInt>(node->getInputCPConnection(0), animSet);
      int inputPartIndex = inputPartIndexAttrib ? (int)inputPartIndexAttrib->m_value : -1;

      MR::AttribDataVector3* inputDirectionAttrib =
        net->updateOptionalInputCPConnection<MR::AttribDataVector3>(node->getInputCPConnection(8), animSet);
      NMP::Vector3 inputDirection = inputDirectionAttrib ? inputDirectionAttrib->m_value : NMP::Vector3::InitTypeZero;

      MR::AttribDataVector3* inputPositionAttrib =
        net->updateOptionalInputCPConnection<MR::AttribDataVector3>(node->getInputCPConnection(9), animSet);
      NMP::Vector3 inputPosition = inputPositionAttrib ? inputPositionAttrib->m_value : NMP::Vector3::InitTypeZero;

      // sanitise the inputs
      inputDirection.normalise(NMP::Vector3::InitTypeZero);

      physicsRig->receiveImpulse(
        inputPartIndex,
        inputPosition,
        inputDirection,
        inputLocalMagnitude,
        inputLocalAngularMultiplier,
        inputLocalResponseRatio,
        inputFullBodyMagnitude,
        inputFullBodyAngularMultiplier,
        inputFullBodyLinearMultiplier,
        inputFullBodyResponseRatio,
        applyImpulseDef->m_positionInWorldSpace,
        applyImpulseDef->m_directionInWorldSpace,
        (applyImpulseDef->m_impulseType == 1));
    } // physicsRig
  }

  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
