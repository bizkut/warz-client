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
#include "euphoria/erAttribData.h"
#include "euphoria/Nodes/erNodeOperatorHit.h"
#include "euphoria/erHitUtils.h"
#include "euphoria/erCharacter.h"
#include "euphoria/erBody.h"
#include "euphoria/erLimb.h"
#include "physics/mrPhysics.h"
#include "euphoria/erPinInterface.h"
#include "morpheme/Nodes/mrSharedNodeFunctions.h"
//----------------------------------------------------------------------------------------------------------------------

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// HitMessageData function.
//----------------------------------------------------------------------------------------------------------------------
void HitMessageData::reset()
{
  //---------------------
  // hit data
  m_rigPartIndex = -1;

  m_hitPointLocalX = 0.0f;
  m_hitPointLocalY = 0.0f;
  m_hitPointLocalZ = 0.0f;

  m_hitNormalLocalX = 0.0f;
  m_hitNormalLocalY = 0.0f;
  m_hitNormalLocalZ = 0.0f;

  m_hitDirectionLocalX = 0.0f;
  m_hitDirectionLocalY = 0.0f;
  m_hitDirectionLocalZ = 0.0f;

  m_hitDirectionWorldX = 0.0f;
  m_hitDirectionWorldY = 0.0f;
  m_hitDirectionWorldZ = 0.0f;

  m_sourcePointWorldX = 0.0f;
  m_sourcePointWorldY = 0.0f;
  m_sourcePointWorldZ = 0.0f;

  // performance tuning vars
  //
  m_priority = 0;

  // delays and durations
  m_reachDelay = 0.0f;
  m_reachDuration = 0.0f;
  m_maxReachSpeed = 0.0f;
  m_reflexAnimStart = 0.0f;
  m_reflexAnimRampDuration = 0.0f;
  m_reflexAnimFullDuration = 0.0f;
  m_reflexAnimDerampDuration = 0.0f;
  m_reflexAnimMaxWeight = 0.0f;
  m_reflexLookDuration = 0.0f;
  m_deathTriggerDelay = 0.0f;
  m_deathRelaxDuration = 0.0f;

  m_expandLimitsFullDuration = 0.0f;
  m_expandLimitsDerampDuration = 0.0f;
  m_expandLimitsMaxExpansion = 0.0f;

  // impulse tuning params
  m_partImpactMagnitude = 0.0f;
  m_partResponseRatio = 0.0f;
  m_bodyImpactMagnitude = 0.0f;
  m_bodyResponseRatio = 0.0f;
  m_torqueMultiplier = 0.0f;
  m_desiredMinimumLift = 0.0f;
  m_liftResponseRatio = 0.0f;
  m_impulseYield = 0.0f;
  m_impulseYieldDuration = 0.0f;

  // stagger/balance tuning
  m_impulseTargetStepSpeed = 0.0f;
  m_impulseTargetStepDuration = 0.0f;

  m_balanceAssistance = 0.0f;

  m_impulseLegStrengthReduction = 0.0f;
  m_impulseLegStrengthReductionDuration = 0.0f;

  m_deathTargetStepSpeed = 0.0f;
  m_deathBalanceStrength = 0.0f;

  // Int Attributes
  m_reachSku = 0;
  m_reflexAnim = 0;

  // Flags activate or deactivate different aspects of the performance
  m_reachSwitch = false;
  m_reflexAnimSwitch = false;
  m_reflexLookSwitch = false;
  m_forcedFalldownSwitch = false;
  m_targetTimeBeforeFalldown = 0.0f;
  m_targetNumberOfStepsBeforeFalldown = 0;
  m_impulseDirWorldOrLocal = false;
  m_lookAtWoundOrHitSource = false;
  m_deathTrigger = false;
  m_deathTriggerOnHeadhit = false;
  m_expandLimitsSwitch = true;
}

//----------------------------------------------------------------------------------------------------------------------
HitMessageData* HitMessageData::endianSwap(void* data)
{
  HitMessageData* messageData = (HitMessageData*)data;

  NMP::endianSwap(messageData->m_rigPartIndex);

  NMP::endianSwap(messageData->m_hitPointLocalX);
  NMP::endianSwap(messageData->m_hitPointLocalY);
  NMP::endianSwap(messageData->m_hitPointLocalZ);

  NMP::endianSwap(messageData->m_hitNormalLocalX);
  NMP::endianSwap(messageData->m_hitNormalLocalY);
  NMP::endianSwap(messageData->m_hitNormalLocalZ);

  NMP::endianSwap(messageData->m_hitDirectionLocalX);
  NMP::endianSwap(messageData->m_hitDirectionLocalY);
  NMP::endianSwap(messageData->m_hitDirectionLocalZ);

  NMP::endianSwap(messageData->m_hitDirectionWorldX);
  NMP::endianSwap(messageData->m_hitDirectionWorldY);
  NMP::endianSwap(messageData->m_hitDirectionWorldZ);

  NMP::endianSwap(messageData->m_sourcePointWorldX);
  NMP::endianSwap(messageData->m_sourcePointWorldY);
  NMP::endianSwap(messageData->m_sourcePointWorldZ);

  NMP::endianSwap(messageData->m_priority);

  NMP::endianSwap(messageData->m_reachDelay);
  NMP::endianSwap(messageData->m_reachDuration);
  NMP::endianSwap(messageData->m_maxReachSpeed);
  NMP::endianSwap(messageData->m_reflexAnimStart);
  NMP::endianSwap(messageData->m_reflexAnimRampDuration);
  NMP::endianSwap(messageData->m_reflexAnimFullDuration);
  NMP::endianSwap(messageData->m_reflexAnimDerampDuration);
  NMP::endianSwap(messageData->m_reflexAnimMaxWeight);
  NMP::endianSwap(messageData->m_reflexLookDuration);
  NMP::endianSwap(messageData->m_deathTriggerDelay);
  NMP::endianSwap(messageData->m_deathRelaxDuration);

  NMP::endianSwap(messageData->m_partImpactMagnitude);
  NMP::endianSwap(messageData->m_bodyImpactMagnitude);
  NMP::endianSwap(messageData->m_torqueMultiplier);
  NMP::endianSwap(messageData->m_desiredMinimumLift);
  NMP::endianSwap(messageData->m_impulseYield);
  NMP::endianSwap(messageData->m_impulseYieldDuration);

  NMP::endianSwap(messageData->m_impulseTargetStepSpeed);
  NMP::endianSwap(messageData->m_impulseTargetStepDuration);

  NMP::endianSwap(messageData->m_balanceAssistance);

  NMP::endianSwap(messageData->m_impulseLegStrengthReduction);
  NMP::endianSwap(messageData->m_impulseLegStrengthReductionDuration);

  NMP::endianSwap(messageData->m_deathTargetStepSpeed);
  NMP::endianSwap(messageData->m_deathBalanceStrength);

  NMP::endianSwap(messageData->m_reachSku);
  NMP::endianSwap(messageData->m_reflexAnim);

  NMP::endianSwap(messageData->m_targetTimeBeforeFalldown);
  NMP::endianSwap(messageData->m_targetNumberOfStepsBeforeFalldown);

  return messageData;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorHitDef functions.
//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorHitDef::locate(MR::AttribData* target)
{
  AttribDataOperatorHitDef* result = (AttribDataOperatorHitDef*)target;

  MR::AttribData::locate(target);
  endianSwap(result);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorHitDef::dislocate(MR::AttribData* target)
{
  AttribDataOperatorHitDef* result = (AttribDataOperatorHitDef*)target;

  MR::AttribData::dislocate(target);
  endianSwap(result);
}

//----------------------------------------------------------------------------------------------------------------------
void AttribDataOperatorHitDef::endianSwap(AttribDataOperatorHitDef* defData)
{
  NMP::endianSwap(defData->m_standingStillTimeout);
  NMP::endianSwap(defData->m_recoveredTimeout);
  NMP::endianSwap(defData->m_fallenAliveTimeout);
  NMP::endianSwap(defData->m_fallenDeadTimeout);
  NMP::endianSwap(defData->m_preFallTargetStepVelocity);
  NMP::endianSwap(defData->m_fallTargetStepVelocity);
  NMP::endianSwap(defData->m_fallBodyPushMagnitude);
  NMP::endianSwap(defData->m_prioritiseReaches);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorHitDef* AttribDataOperatorHitDef::create(
  NMP::MemoryAllocator* allocator,
  float                 standingStillTimeout,
  float                 recoveredTimeout,
  float                 fallenAliveTimeout,
  float                 fallenDeadTimeout,
  bool                  prioritiseReaches,
  float                 preFallTargetStepVelocity,
  float                 fallTargetStepVelocity,
  float                 fallBodyPushMagnitude,
  uint16_t              refCount)
{
  AttribDataOperatorHitDef* result;

  NMP::Memory::Format memReqs = AttribDataOperatorHitDef::getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
  NMP_ASSERT(resource.ptr);
  result = AttribDataOperatorHitDef::init(
    resource,
    standingStillTimeout,
    recoveredTimeout,
    fallenAliveTimeout,
    fallenDeadTimeout,
    prioritiseReaches,
    preFallTargetStepVelocity,
    fallTargetStepVelocity,
    fallBodyPushMagnitude,
    refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorHitDef::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataOperatorHitDef), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorHitDef* AttribDataOperatorHitDef::init(
  NMP::Memory::Resource& resource,
  float                  standingStillTimeout,
  float                  recoveredTimeout,
  float                  fallenAliveTimeout,
  float                  fallenDeadTimeout,
  bool                   prioritiseReaches,
  float                  preFallTargetStepVelocity,
  float                  fallTargetStepVelocity,
  float                  fallBodyPushMagnitude,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorHitDef* result = (AttribDataOperatorHitDef*)resource.ptr;
  resource.increment(sizeof(AttribDataOperatorHitDef));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_HIT_DEF);
  result->setRefCount(refCount);
  result->m_standingStillTimeout      = standingStillTimeout;
  result->m_recoveredTimeout          = recoveredTimeout;
  result->m_fallenAliveTimeout        = fallenAliveTimeout;
  result->m_fallenDeadTimeout         = fallenDeadTimeout;
  result->m_prioritiseReaches         = prioritiseReaches;
  result->m_preFallTargetStepVelocity = preFallTargetStepVelocity;
  result->m_fallTargetStepVelocity    = fallTargetStepVelocity;
  result->m_fallBodyPushMagnitude     = fallBodyPushMagnitude;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataHandle AttribDataOperatorHitDef::create(NMP::MemoryAllocator* allocator)
{
  MR::AttribDataHandle result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);

  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(
    resource,
    0.1f,  // standingStillTimeout,
    0.35f, // recoveredTimeout
    0.5f,  // fallenAliveTimeout,
    1.5f,  // fallenDeadTimeout,
    true,  // prioritiseReaches,
    1.0f,  // preFallTargetStepVelocity,
    3.0f,  // fallTargetStepVelocity,
    1.0f,  // fallBodyPushMagnitude,
    0);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// AttribDataOperatorHitState functions.
//----------------------------------------------------------------------------------------------------------------------
//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorHitState* AttribDataOperatorHitState::create(
  NMP::MemoryAllocator* allocator,
  bool                 newHit,
  uint16_t              refCount)
{
  AttribDataOperatorHitState* result;

  NMP::Memory::Format memReqs = AttribDataOperatorHitState::getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
  NMP_ASSERT(resource.ptr);
  result = AttribDataOperatorHitState::init(resource, newHit, refCount);

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
NMP::Memory::Format AttribDataOperatorHitState::getMemoryRequirements()
{
  return NMP::Memory::Format(
    NMP::Memory::align(sizeof(AttribDataOperatorHitState), MR_ATTRIB_DATA_ALIGNMENT),
    MR_ATTRIB_DATA_ALIGNMENT);
}

//----------------------------------------------------------------------------------------------------------------------
AttribDataOperatorHitState* AttribDataOperatorHitState::init(
  NMP::Memory::Resource& resource,
  bool                  newHit,
  uint16_t               refCount)
{
  NMP_ASSERT(getMemoryRequirements().alignment == MR_ATTRIB_DATA_ALIGNMENT);
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);
  AttribDataOperatorHitState* result = (AttribDataOperatorHitState*)resource.ptr;
  resource.increment(sizeof(AttribDataOperatorHitState));
  resource.align(MR_ATTRIB_DATA_ALIGNMENT);

  result->setType(ATTRIB_TYPE_OPERATOR_HIT_STATE);
  result->setRefCount(refCount);
  result->m_newHit = newHit;
  result->m_lastUpdateFrame = 0;
  result->m_SR.init();

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribDataHandle AttribDataOperatorHitState::create(NMP::MemoryAllocator* allocator)
{
  MR::AttribDataHandle result;

  NMP::Memory::Format memReqs = getMemoryRequirements();
  NMP::Memory::Resource resource = allocator->allocateFromFormat(memReqs NMP_MEMORY_TRACKING_ARGS);
  NMP_ASSERT(resource.ptr);
  result.m_attribData = init(resource, 0.0f, 0);
  result.m_format = memReqs;

  // Store the allocator so we know where to free this attribData from when we destroy it.
  result.m_attribData->m_allocator = allocator;

  return result;
}

//----------------------------------------------------------------------------------------------------------------------
// aux for the update function below
//
void ER::AttribDataOperatorHitState::updateFromConnectedCPInputs(MR::NodeDef* node, MR::Network* net)
{
  // Ensure we have a character
  // later on if a new hit came in with valid limb and part indexes
  // we use the character->body to get hold of an actor pointer for the part that got hit
  // and to determine the "anatomical" type of the body part
  // so we rely on the operator being bound to a character
  //
  m_SR.m_character = ER::networkGetCharacter(net);
  NMP_ASSERT(m_SR.m_character);

  // set left and right arm and leg indexes
  //
  NMP_ASSERT(m_SR.m_character->getBody().getNumLegs() >= HitReaction::s_numLegs);
  // Legs
  // assume right, left ordering
  int32_t firstLegIndex = m_SR.m_character->getBody().m_definition->getFirstLegLimbIndex();
  m_SR.m_rightLegIndex = firstLegIndex;
  m_SR.m_leftLegIndex = firstLegIndex + 1;
  // and reverse if required
  if (m_SR.m_character->getBody().getLimb(firstLegIndex).isLeftLimb())
  {
    NMP::nmSwap(m_SR.m_leftLegIndex, m_SR.m_rightLegIndex);
  }
  // Arms
  // assume right, left ordering
  m_SR.m_firstArmIndex = m_SR.m_character->getBody().m_definition->getFirstArmLimbIndex();
  m_SR.m_rightArmIndex = m_SR.m_firstArmIndex;
  m_SR.m_leftArmIndex = m_SR.m_firstArmIndex + 1;
  // and reverse if required
  if (m_SR.m_character->getBody().getLimb(m_SR.m_firstArmIndex).isLeftLimb())
  {
    NMP::nmSwap(m_SR.m_leftArmIndex, m_SR.m_rightArmIndex);
  }

  // Update cp-overridable items
  //
  // Whether to look at wound or hit source may be specified in the message or entirely controlled by
  // gamecode/script in the latter case the value of the CP input overrides the message setting entirely
  //

  // Whether to look at wound or hit source 
  // (optional input, defaults to true, set in HitReaction::reset())
  //
  MR::AnimSetIndex animSet = net->getOutputAnimSetIndex(node->getNodeID());

  MR::AttribDataBool* inputBoolAttrib = net->updateOptionalInputCPConnection<MR::AttribDataBool>(
    node->getInputCPConnection(CP_IN_LOOKATWOUNDORHITSOURCE), animSet);
  if (inputBoolAttrib)
  {
    m_SR.m_lookAtWoundOrHitSource = inputBoolAttrib->m_value;
  }

  // Items to be updated only after a frame's delay or after re-entry to the hit state.
  //

  // Death can be triggered by message (along with a hit) or by gamecode/script via an optional cp hookup which
  // can only override the message setting if the message flag was not set (a frame's delay guarantees this to avoid 
  // confusion). In the absence of cp input the default value is determined by the message.
  //
  //
  // The m_newHit flag is set by messageHandler() and cleared in writeOutputs() so this is how
  // we determine first frame after message arrived
  //
  if (!m_newHit)
  {
    // Death trigger CP override
    inputBoolAttrib = net->updateOptionalInputCPConnection<MR::AttribDataBool>(
      node->getInputCPConnection(CP_IN_DEATHTRIGGER), animSet);
    if (inputBoolAttrib)
    {
      m_SR.m_deathTrigger = inputBoolAttrib->m_value;
    }
  }

  // The following set of inputs are used to drive internal timers, they must be hooked up for the performance to 
  // provide functionality associated with them but they are optional. When connected to emmitted feedback outputs they 
  // will be valid only after the state has updated at least once i.e. not immediately on re-entry to the state. Those
  // that are not connected need "not-connected" signal values, these are set in reset().
  //

  // Determine re-entry from frame counter value.
  //
  bool reEnteringHitState = (m_lastUpdateFrame < (net->getCurrentFrameNo() - 1));

  // Update inputs as required.
  //
  MR::AttribDataFloat* inputFloatAttrib = net->updateOptionalInputCPConnection<MR::AttribDataFloat>(
      node->getInputCPConnection(CP_IN_STANDINGSTILLTIME), animSet);
  if (inputFloatAttrib)
  {
    m_SR.m_standingStillTime = reEnteringHitState ? 0.0f : inputFloatAttrib->m_value;
  }
  //
  inputFloatAttrib = net->updateOptionalInputCPConnection<MR::AttribDataFloat>(
      node->getInputCPConnection(CP_IN_FALLENTIME), animSet);
  if (inputFloatAttrib)
  {
    m_SR.m_fallenTime = reEnteringHitState ? 0.0f : inputFloatAttrib->m_value;
  }
  //
  inputFloatAttrib = net->updateOptionalInputCPConnection<MR::AttribDataFloat>(
      node->getInputCPConnection(CP_IN_STEPPINGTIME), animSet);
  if (inputFloatAttrib)
  {
    m_SR.m_steppingTime = reEnteringHitState ? 0.0f : inputFloatAttrib->m_value;
  }
  //
  MR::AttribDataInt* inputIntAttrib;
  inputIntAttrib = net->updateOptionalInputCPConnection<MR::AttribDataInt>(
      node->getInputCPConnection(CP_IN_STEPPCOUNT), animSet);
  if (inputIntAttrib)
  {
    m_SR.m_stepCount = reEnteringHitState ? 0 : inputIntAttrib->m_value;
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// void nodeOperatorHitInitInstance(MR::NodeDef* node, MR::Network* net)
// Creates the output pins that we write to during update

static void createOutputs(MR::NodeDef* node, MR::Network* net)
{
  enum OUTPUT_PIN_ATTRIB_TYPE
  {
    BOOL,
    UINT,
    INT,
    FLOAT,
    V3
  };
  OUTPUT_PIN_ATTRIB_TYPE outputPinAttribTypesMap[] =
  {
    BOOL, //CP_OUT_NEWREACH0 = 0,
    BOOL, //CP_OUT_NEWREACH1,
    INT, //CP_OUT_REACHLIMBINDEX0,
    INT, //CP_OUT_REACHLIMBINDEX1,
    INT, //CP_OUT_REACHPARTINDEX0,

    INT, //CP_OUT_REACHPARTINDEX1,
    V3, //CP_OUT_REACHPOSITION0,
    V3, //CP_OUT_REACHPOSITION1,
    V3, //CP_OUT_REACHNORMAL0,
    V3, //CP_OUT_REACHNORMAL1,

    FLOAT, //CP_OUT_REACHSTRENGTHREDUCTION0,
    FLOAT, //CP_OUT_REACHSTRENGTHREDUCTION1,
    FLOAT, //CP_OUT_REACHSPEEDLIMIT0,
    FLOAT, //CP_OUT_REACHSPEEDLIMIT1,

    FLOAT, //CP_OUT_IMPULSESPINEDAMPING,

    V3,     //CP_OUT_LOOKPOSITION,
    FLOAT,  //CP_OUT_LOOKWEIGHT,

    FLOAT,  //CP_OUT_BODYSTRENGTH,

    V3,     //CP_OUT_STEPVELOCITY,
    FLOAT,  //CP_OUT_BALANCEASSISTANCEPOSITION,
    FLOAT,  //CP_OUT_BALANCEASSISTANCEORIENTATION,
    FLOAT,  //CP_OUT_BALANCEASSISTANCEVELOCITY,
    FLOAT,  //CP_OUT_LEGSTRENGTH0,
    FLOAT,  //CP_OUT_LEGSTRENGTH1,

    FLOAT,  //CP_OUT_HITREFLEXANIMIDWEIGHT,
    FLOAT,  //CP_OUT_HITREFLEXANIMWEIGHT,
    FLOAT,  //CP_OUT_PERFORMANCESTATEEXIT,
  };

  MR::NodeBin* nodeBin = net->getNodeBin(node->getNodeID());
  for (int i = 0; i < CP_OUT_COUNT; ++i)
  {
    MR::OutputCPPin* outputCPPin = nodeBin->getOutputCPPin((MR::PinIndex)i);
    NMP_ASSERT(!outputCPPin->m_attribDataHandle.m_attribData); // about to create so should not already exist
    switch (outputPinAttribTypesMap[i])
    {
    case BOOL:
      outputCPPin->m_attribDataHandle = MR::AttribDataBool::create(net->getPersistentMemoryAllocator(), false);
      break;
    case UINT:
      outputCPPin->m_attribDataHandle = MR::AttribDataUInt::create(net->getPersistentMemoryAllocator(), static_cast<uint32_t>(0));
      break;
    case INT:
      outputCPPin->m_attribDataHandle = MR::AttribDataInt::create(net->getPersistentMemoryAllocator(), static_cast<int32_t>(0));
      break;
    case FLOAT:
      outputCPPin->m_attribDataHandle = MR::AttribDataFloat::create(net->getPersistentMemoryAllocator(), 0.0f);
      break;
    case V3:
      outputCPPin->m_attribDataHandle = MR::AttribDataVector3::create(net->getPersistentMemoryAllocator(), NMP::Vector3(0, 0, 0));
      break;
    }
    NMP_ASSERT(outputCPPin->m_attribDataHandle.m_attribData); // failed to create
  }
}

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// void nodeOperatorHitInitInstance(MR::NodeDef* node, MR::Network* net)
// Creates the nodes state data

static AttribDataOperatorHitState* createStateData(MR::NodeDef* node, MR::Network* net)
{
  // AttribData creation
  //
  MR::NodeID nodeID = node->getNodeID();
  MR::AttribDataHandle handle = AttribDataOperatorHitState::create(net->getPersistentMemoryAllocator());
  AttribDataOperatorHitState* stateData = (AttribDataOperatorHitState*)(handle.m_attribData);
  NMP_ASSERT(stateData); // failed to create
  MR::AttribAddress stateAddress(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE, nodeID, MR::INVALID_NODE_ID, MR::VALID_FOREVER);
  net->addAttribData(stateAddress, handle, MR::LIFESPAN_FOREVER);
  return stateData;
}

//----------------------------------------------------------------------------------------------------------------------

void nodeOperatorHitInitInstance(MR::NodeDef* node, MR::Network* net)
{
  createOutputs(node, net);
  createStateData(node, net);
  if(node->getNumReflexiveCPPins() > 0)
  {
    nodeInitPinAttribDataInstance(node, net);
  }
}

//----------------------------------------------------------------------------------------------------------------------

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// nodeOperatorHitOutputCPUpdate
// Retrieves pointer to the operator's state data

static AttribDataOperatorHitState* getStateData(MR::NodeDef* node, MR::Network* net)
{
  MR::NodeID nodeID = node->getNodeID();
  MR::NodeBinEntry* stateEntry = net->getAttribDataNodeBinEntry(
    MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_STATE,
    nodeID,
    MR::INVALID_NODE_ID,
    MR::VALID_FRAME_ANY_FRAME);
  NMP_ASSERT(stateEntry);

  return stateEntry->getAttribData<AttribDataOperatorHitState>();
}


//----------------------------------------------------------------------------------------------------------------------
// Aux for
// nodeOperatorHitOutputCPUpdate
// Pins for arms and legs are paired up to allow for looping over these limb types as and when we may want to handle
// characters with different numbers or arms and legs.

ER::NodeOperatorHitOutCPPinIDs newReachPinId[] = {
  CP_OUT_NEWREACH0,
  CP_OUT_NEWREACH1
};

ER::NodeOperatorHitOutCPPinIDs reachLimbIndexPinId[] = {
  CP_OUT_REACHLIMBINDEX0,
  CP_OUT_REACHLIMBINDEX1
};

ER::NodeOperatorHitOutCPPinIDs reachPartIndexPinId[] = {
  CP_OUT_REACHPARTINDEX0,
  CP_OUT_REACHPARTINDEX1
};

ER::NodeOperatorHitOutCPPinIDs reachPositionPinId[] = {
  CP_OUT_REACHPOSITION0,
  CP_OUT_REACHPOSITION1
};

ER::NodeOperatorHitOutCPPinIDs reachNormalPinId[] = {
  CP_OUT_REACHNORMAL0,
  CP_OUT_REACHNORMAL1
};

ER::NodeOperatorHitOutCPPinIDs reachStrengthPinId[] = {
  CP_OUT_REACHSTRENGTH0,
  CP_OUT_REACHSTRENGTH1
};

ER::NodeOperatorHitOutCPPinIDs reachSpeedLimitPinId[] = {
  CP_OUT_REACHSPEEDLIMIT0,
  CP_OUT_REACHSPEEDLIMIT1
};

ER::NodeOperatorHitOutCPPinIDs legStrengthPinId[] = {
  CP_OUT_LEGSTRENGTH0,
  CP_OUT_LEGSTRENGTH1
};

//----------------------------------------------------------------------------------------------------------------------
// Aux for
// nodeOperatorHitOutputCPUpdate
// Produces outputs from state data

void AttribDataOperatorHitState::tickHitReaction(MR::NodeDef* nodeDef, MR::Network* net)
{
  // If the exit status value is not from the last update frame we have just
  // transitioned to the "hit state" so some reset may be required
  //
  PinInterface pinInterface(net->getNodeBin(nodeDef->getNodeID()));
  MR::FrameCount currFrameNo = net->getCurrentFrameNo();
  if (pinInterface.getLastUpdateFrame(ER::CP_OUT_PERFORMANCESTATEEXIT) != (currFrameNo - 1))
  {
    // Reset:

    // 1 the transition condition output cp
    pinInterface.writeValue(ER::CP_OUT_PERFORMANCESTATEEXIT, 0.0f);

    // 2 the hit reaction state (m_SR) but only if we are entering this state
    // via a tansition *not* predicated on a hit message. In that case the
    // messageHandler() will already have handled the reset and added
    // state relating to the incoming hit (which needs to be preserved).
    //
    if (!m_newHit)
    {
      m_SR.reset();
    }
  }

  // Tick hit reaction state
  //

  // Get the current physics step and the gravity vector
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(net); // may be 0 if there's no physics rig on the network
  MR::PhysicsScene* physicsScene = NULL;
  if (physicsRig)
  {
    physicsScene = physicsRig->getPhysicsScene();
  }
  float dt = 0.0f;
  if (!physicsScene)
  {
    return;
  }
  dt = physicsScene->getNextPhysicsTimeStep();
  m_SR.m_gravity = physicsScene->getGravity();

  MR::Dispatcher* dispatcher = net->getDispatcher();
  MR::InstanceDebugInterface* pDebugDrawInst = dispatcher->getDebugInterface();

  ER::HitReaction::OutputSourceData osd;
  
  m_SR.update(
    physicsRig, dt, 
    m_newHit, m_hsi, m_attribs,
    osd,
    pDebugDrawInst);

  m_newHit = false; // clear the state's new hit

  
  // Apply impulse
  //

  // Set impulse yield (damping) unconditionally
  //
  pinInterface.writeValue(CP_OUT_IMPULSESPINEDAMPING, osd.m_spineDamping);

  // If there is a new impulse to deal with with non-zero elapsed time then fill out the details
  // (impulse is deferred to allow part-determination code to run before the character is potentially
  // bent out of shape by the impulse, particularly for back/front determination with largish impulses)
  if (m_SR.m_haveNewImpulse && m_SR.m_impulseHitSelectionInfo.elapsedTime > 0)
  {
    // Apply impulses for part and body as required
    //
    if (physicsRig)
    {
      // Part impulse
      if (m_SR.m_impulseInfo.m_partImpulseMagnitude > 0.0f)
      {
        physicsRig->receiveImpulse(
          m_SR.m_impulseHitSelectionInfo.rigPartIndex,
          m_SR.m_impulseHitSelectionInfo.pointLocal,
          m_SR.m_impulseHitSelectionInfo.hitDirLocal,
          m_SR.m_impulseInfo.m_partImpulseMagnitude,    // local magnitude
          1.0f,                                         // local angular multiplier
          m_SR.m_impulseInfo.m_partResponseRatio,       // local response ratio
          0.0f,                                         // full body magnitude
          1.0f,                                         // full body angular multiplier
          0.0f,                                         // full body linear multiplier
          1.0f,                                         // full body response ratio
          false,
          false,
          false); // impulse mode
      }
      // Full-body impulse
      if (m_SR.m_impulseInfo.m_bodyImpulseMagnitude > 0.0f)
      {
        physicsRig->receiveImpulse(
          m_SR.m_impulseHitSelectionInfo.rigPartIndex,
          m_SR.m_impulseHitSelectionInfo.pointLocal,
          m_SR.m_impulseInfo.m_bodyImpulseDirection,
          0.0f,
          1.0f,
          1.0f,
          m_SR.m_impulseInfo.m_bodyImpulseMagnitude,
          m_SR.m_impulseInfo.m_torqueMultiplier,
          1.0f,
          m_SR.m_impulseInfo.m_bodyResponseRatio,
          false,
          true,
          false); // impulse mode
      }
      // Lift boost impulse.
      if (m_SR.m_impulseInfo.m_liftBoost > 0.0f)
      {
        physicsRig->receiveImpulse(
          -1,                                     // don't care what part
          physicsRig->calculateCentreOfMass(),    // apply at character com
          (-m_SR.m_gravity).getNormalised(),      // apply upwards (i.e. opposing gravity)
          0.0f,
          1.0f,
          1.0f,
          m_SR.m_impulseInfo.m_liftBoost,
          0.0f,                                   // no torque effects required
          1.0f,
          m_SR.m_impulseInfo.m_liftResponseRatio, // use the lift response ratio specified in the hit message
          true,                                   // position input is world space
          true,                                   // direction input is world space
          false); // impulse mode
      }
    }
    // clear new impulse flag
    m_SR.m_haveNewImpulse = false;
  }

  // Reach reflex
  //
  
  for (int32_t i = 0; i < m_SR.s_numArms; ++i)
  {
    // Output the new reach flags
    pinInterface.writeValue((MR::PinIndex)newReachPinId[i], m_SR.m_haveNewReach[i]);

    // Output the current desired reach strength and speed limits
    pinInterface.writeValue((MR::PinIndex)reachStrengthPinId[i], m_SR.m_reachInfo[i].m_strength);
    pinInterface.writeValue((MR::PinIndex)reachSpeedLimitPinId[i], m_SR.m_reachInfo[i].m_speedLimit);

    // If we want a new reach then fill out the hsi detail
    if (m_SR.m_haveNewReach[i])
    {
      pinInterface.writeValue((MR::PinIndex)reachLimbIndexPinId[i], m_SR.m_reachHitSelectionInfo[i].limbIndex);
      pinInterface.writeValue((MR::PinIndex)reachPartIndexPinId[i], m_SR.m_reachHitSelectionInfo[i].partIndex);
      pinInterface.writeValue((MR::PinIndex)reachPositionPinId[i], m_SR.m_impulseHitSelectionInfo.pointLocal);
      pinInterface.writeValue((MR::PinIndex)reachNormalPinId[i], m_SR.m_impulseHitSelectionInfo.normalLocal);
    }
    // Otherwise invalidate the limb and part indexes
    else
    {
      pinInterface.writeValue((MR::PinIndex)reachLimbIndexPinId[i], -1);
      pinInterface.writeValue((MR::PinIndex)reachPartIndexPinId[i], -1);
    }
  }

  // Balance, strength and assistance
  //

  pinInterface.writeValue(CP_OUT_BODYSTRENGTH, osd.m_balanceInfo.m_bodyStrength);
  pinInterface.writeValue(CP_OUT_STEPVELOCITY, osd.m_balanceInfo.m_targetStepVelocity);
  
  // Write the leg strengths straight to the output pins, but compute average strength for assistance calcs below.
  //
  float avgLegStrength = 0.0f;
  int32_t i = 0;
  for ( ; i < m_SR.s_numLegs; ++i)
  {
    pinInterface.writeValue((MR::PinIndex)legStrengthPinId[i], osd.m_legStrengths[i]);
    avgLegStrength += osd.m_legStrengths[i];
  }
  if (i > 1)
  {
    avgLegStrength /= m_SR.s_numLegs;
  }

  // Balance applies assistance via cheat forces without considering characteristic strength so we need to be quite
  // prescriptive about what assistance we request.
  //

  // Basic position/orientation assistance is linked to body and leg strength to ensure that the character isn't held
  // up by assistance forces when there is, for eg. no leg strength.
  //
  float walkAndStandStrength = osd.m_balanceInfo.m_bodyStrength * avgLegStrength;
  pinInterface.writeValue(CP_OUT_BALANCEASSISTANCEPOSITION, osd.m_balanceInfo.m_assistancePosition * walkAndStandStrength);
  pinInterface.writeValue(CP_OUT_BALANCEASSISTANCEORIENTATION, osd.m_balanceInfo.m_assistanceOrientation * walkAndStandStrength);

  // For low velocity demands requested velocity assistance is scaled with the magnitude of the demand. This reflects
  // our usage of this feature which is to induce stepping in a particular direction if we are not asking for that we
  // want the character's dynamic state to be evolving naturally.
  //
  const ER::DimensionalScaling& dimensionalScaling = Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();
  float scaledVelocityDemand = osd.m_balanceInfo.m_targetStepVelocity.magnitude() / dimensionalScaling.scaleVel(1.0f);
  float maxVelAssistance = NMP::minimum(scaledVelocityDemand, 1.0f);
  pinInterface.writeValue(CP_OUT_BALANCEASSISTANCEVELOCITY, maxVelAssistance * walkAndStandStrength);

  // Anim reflex
  //
  pinInterface.writeValue(CP_OUT_HITREFLEXANIMIDWEIGHT, (float)osd.m_animSelectionInfo.id);
  pinInterface.writeValue(CP_OUT_HITREFLEXANIMWEIGHT, osd.m_animSelectionInfo.weight);

  // Look reflex
  //
  pinInterface.writeValue(CP_OUT_LOOKWEIGHT, osd.m_lookInfo.m_weight);
  // only set remaining params if look weight is non-zero
  if (osd.m_lookInfo.m_weight > 0.0f)
  {
    pinInterface.writeValue(CP_OUT_LOOKPOSITION, osd.m_lookInfo.m_target);
  }

  // Status output code.
  //
  switch (m_SR.m_statusCode)
  {
  case HitReaction::RUNNING:
    pinInterface.writeValue(CP_OUT_PERFORMANCESTATEEXIT, 0.15f);
    break;

  case HitReaction::RECOVERED:
    pinInterface.writeValue(CP_OUT_PERFORMANCESTATEEXIT, 0.25f);
    break;

  case HitReaction::FALLEN:
    pinInterface.writeValue(CP_OUT_PERFORMANCESTATEEXIT, 0.35f);
    break;

  case HitReaction::DEAD:
    pinInterface.writeValue(CP_OUT_PERFORMANCESTATEEXIT, 0.45f);
    break;
  }

  pinInterface.setLastUpdateFrame(net->getCurrentFrameNo(), 0, CP_OUT_COUNT);
}

//----------------------------------------------------------------------------------------------------------------------
MR::AttribData* nodeOperatorHitOutputCPUpdate(
  MR::NodeDef* nodeDef,
  MR::PinIndex outputCPPinIndex, // The output pin we have been asked to update.
  MR::Network* net)
{
  NMP_ASSERT(outputCPPinIndex < CP_OUT_COUNT);

  MR::NodeBin* nodeBin = net->getNodeBin(nodeDef->getNodeID());

  // Update the connected control parameters.(and corresponding state)
  //
  AttribDataOperatorHitState* stateData = getStateData(nodeDef, net);
  NMP_ASSERT(stateData);
  stateData->updateFromConnectedCPInputs(nodeDef, net);

  // Output from stateData updated above
  //
  stateData->tickHitReaction(nodeDef, net);

  // The requested output pin will automatically be flagged as having been updated this frame,
  // but we need to make sure that all pins we have updated are correctly flagged.
  //pininterface flagOutputPins(nodeDef, net);

  // Reset the new hit flag
  stateData->m_newHit = false;
  // update the frame count
  stateData->m_lastUpdateFrame = net->getCurrentFrameNo();

  // Return the value of the requested output cp pin.
  return nodeBin->getOutputCPPin(outputCPPinIndex)->getAttribData();
}

//----------------------------------------------------------------------------------------------------------------------
bool nodeOperatorHitMessageHandler(const MR::Message& message, MR::NodeID nodeId, MR::Network* net)
{
  // Verify we have the right sort of message coming in
  NMP_ASSERT_MSG(message.m_type == MESSAGE_TYPE_HIT, "Unexpected message type.");
#if defined(NMP_ENABLE_ASSERTS)
  uint32_t msgSize;
  msgSize = sizeof(HitMessageData);
#endif // NMP_ENABLE_ASSERTS
  NMP_ASSERT_MSG(msgSize == message.m_dataSize, "Message size mismatch.");

  // Convert it to a message of type HitMessageData for deref
  HitMessageData* smd = (HitMessageData*)message.m_data;

  AttribDataOperatorHitState* stateData = getStateData(net->getNetworkDef()->getNodeDef(nodeId), net);

  return stateData->handleHitMessage(smd, nodeId, net);

}

//----------------------------------------------------------------------------------------------------------------------
bool AttribDataOperatorHitState::handleHitMessage(const HitMessageData* smd, MR::NodeID nodeId, MR::Network* net)
{
  MR::PhysicsRig* physicsRig = MR::getPhysicsRig(net);
  if (!physicsRig)
  {
    return false;
  }

  // Apply any dimensional scaling to message (parameters) and def data (attributes).
  //
  const ER::DimensionalScaling& dimensionalScaling = Body::getFromPhysicsRig(physicsRig)->getDimensionalScaling();

  // Scaled attributes.
  // Overwrite raw state data values with scaled ones.
  MR::NodeDef* node = net->getNetworkDef()->getNodeDef(nodeId);
  AttribDataOperatorHitDef* defData = node->getAttribData<AttribDataOperatorHitDef>(MR::ATTRIB_SEMANTIC_NODE_SPECIFIC_DEF);
  m_SR.m_preFallTargetStepVelocity  = dimensionalScaling.scaleVel(defData->m_preFallTargetStepVelocity);
  m_SR.m_fallTargetStepVelocity     = dimensionalScaling.scaleVel(defData->m_fallTargetStepVelocity);
  m_SR.m_fallBodyImpulseMagnitude      = dimensionalScaling.scaleImpulse(defData->m_fallBodyPushMagnitude);
  m_SR.m_standingStillTimeout = dimensionalScaling.scaleTime(defData->m_standingStillTimeout);
  m_SR.m_fallenAliveTimeout = dimensionalScaling.scaleTime(defData->m_fallenAliveTimeout);
  m_SR.m_fallenDeadTimeout = dimensionalScaling.scaleTime(defData->m_fallenDeadTimeout);
  m_SR.m_recoveredTimeout = dimensionalScaling.scaleTime(defData->m_recoveredTimeout);
  
  // check the current frame number
  // if later than the last time hit was updated reset performance data
  // ready for entry into the hit state
  MR::FrameCount frameNo;
  frameNo = net->getCurrentFrameNo();
  if (frameNo > m_lastUpdateFrame)
  {
    m_SR.reset();
  }

  m_newHit = true;

  m_hsi.pointLocal.set(
    smd->m_hitPointLocalX,
    smd->m_hitPointLocalY,
    smd->m_hitPointLocalZ
  );
  m_hsi.normalLocal.set(
    smd->m_hitNormalLocalX,
    smd->m_hitNormalLocalY,
    smd->m_hitNormalLocalZ
  );
  m_hsi.hitDirWorld.set(
    smd->m_hitDirectionWorldX,
    smd->m_hitDirectionWorldY,
    smd->m_hitDirectionWorldZ
  );
  m_hsi.hitDirLocal.set(
    smd->m_hitDirectionLocalX,
    smd->m_hitDirectionLocalY,
    smd->m_hitDirectionLocalZ
  );
  m_hsi.sourcePointWorld.set(
    smd->m_sourcePointWorldX,
    smd->m_sourcePointWorldY,
    smd->m_sourcePointWorldZ);

  m_attribs.m_reachDelay = smd->m_reachDelay;
  m_attribs.m_reachDuration = smd->m_reachDuration;
  m_attribs.m_maxReachSpeed = dimensionalScaling.scaleVel(smd->m_maxReachSpeed);
  m_attribs.m_reflexAnimStart = smd->m_reflexAnimStart;
  m_attribs.m_reflexAnimRampDuration = smd->m_reflexAnimRampDuration;
  m_attribs.m_reflexAnimFullDuration = smd->m_reflexAnimFullDuration;
  m_attribs.m_reflexAnimDerampDuration = smd->m_reflexAnimDerampDuration;
  m_attribs.m_reflexAnimMaxWeight = smd->m_reflexAnimMaxWeight;
  m_attribs.m_reflexLookDuration = smd->m_reflexLookDuration;
  m_attribs.m_deathTriggerDelay = smd->m_deathTriggerDelay;
  m_attribs.m_deathRelaxDuration = smd->m_deathRelaxDuration;

  m_attribs.m_expandLimitsSwitch = smd->m_expandLimitsSwitch;
  m_attribs.m_expandLimitsFullDuration = smd->m_expandLimitsFullDuration;
  m_attribs.m_expandLimitsDerampDuration = smd->m_expandLimitsDerampDuration;
  m_attribs.m_expandLimitsMaxExpansion = smd->m_expandLimitsMaxExpansion;
  
  m_attribs.m_partImpactMagnitude = dimensionalScaling.scaleImpulse(smd->m_partImpactMagnitude);
  m_attribs.m_partResponseRatio = smd->m_partResponseRatio;
  m_attribs.m_bodyImpactMagnitude = dimensionalScaling.scaleImpulse(smd->m_bodyImpactMagnitude);
  m_attribs.m_bodyResponseRatio = smd->m_bodyResponseRatio;
  m_attribs.m_torqueMultiplier = smd->m_torqueMultiplier;
  m_attribs.m_desiredMinimumLift = dimensionalScaling.scaleVel(smd->m_desiredMinimumLift);
  m_attribs.m_liftResponseRatio = smd->m_liftResponseRatio;
  m_attribs.m_impulseYield = smd->m_impulseYield;
  m_attribs.m_impulseYieldDuration = smd->m_impulseYieldDuration;

  m_attribs.m_impulseTargetStepSpeed = dimensionalScaling.scaleVel(smd->m_impulseTargetStepSpeed);
  m_attribs.m_impulseTargetStepDuration = smd->m_impulseTargetStepDuration;
  m_attribs.m_balanceAssistance = smd->m_balanceAssistance;

  m_attribs.m_impulseLegStrengthReduction = smd->m_impulseLegStrengthReduction;
  m_hsi.legStrengthReduction = smd->m_impulseLegStrengthReduction;
  m_attribs.m_impulseLegStrengthReductionDuration = smd->m_impulseLegStrengthReductionDuration;
  m_hsi.legStrengthReductionDuration = smd->m_impulseLegStrengthReductionDuration;
  m_attribs.m_deathTargetStepSpeed = dimensionalScaling.scaleVel(smd->m_deathTargetStepSpeed);
  m_attribs.m_deathBalanceStrength = smd->m_deathBalanceStrength;

  m_attribs.m_reachSku = smd->m_reachSku;
  m_attribs.m_reflexAnim = smd->m_reflexAnim;

  m_attribs.m_reachSwitch = smd->m_reachSwitch;
  m_attribs.m_reflexAnimSwitch = smd->m_reflexAnimSwitch;
  m_attribs.m_reflexLookSwitch = smd->m_reflexLookSwitch;

  m_attribs.m_forcedFalldownSwitch = smd->m_forcedFalldownSwitch;
  m_attribs.m_targetTimeBeforeFalldown = smd->m_targetTimeBeforeFalldown;
  m_attribs.m_targetNumStepsBeforeFalldown = smd->m_targetNumberOfStepsBeforeFalldown;

  m_attribs.m_reflexLookSwitch = smd->m_reflexLookSwitch;

  m_attribs.m_impulseDirWorldOrLocal = smd->m_impulseDirWorldOrLocal;

  // update the hsi and (rig, part) index and the physics actor pointer
  // (the message holds the rig part index)
  int32_t rigPartIndex = smd->m_rigPartIndex;
  m_hsi.rigPartIndex = rigPartIndex;
  ER::Character* character = ER::networkGetCharacter(net);

  MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation *selectedPart = 
    (MR::PhysicsRigPhysX3Articulation::PartPhysX3Articulation *)character->getBody().getPhysicsRig()->getPart(rigPartIndex);
  NMP_ASSERT(selectedPart);
  m_hsi.selectedActor = selectedPart->getArticulationLink();


  character->getBody().getActorLimbPartIndex(
    (physx::PxActor*)m_hsi.selectedActor, m_hsi.limbIndex, m_hsi.partIndex);

  m_hsi.priority = smd->m_priority;

  // Make the total duration the maximum of the stagger, reach, look and anim durations
  float hsiImpulseStaggerDuration = smd->m_impulseTargetStepDuration;
  float hsiReachDuration = smd->m_reachSwitch ?
    (smd->m_reachDuration + smd->m_reachDelay) : 0.0f;
  float hsiLookDuration = smd->m_reflexLookSwitch ?
    (smd->m_reflexLookDuration) : 0.0f;
  float hsiAnimDuration = smd->m_reflexAnimSwitch ?
    (smd->m_reflexAnimStart +
     smd->m_reflexAnimRampDuration +
     smd->m_reflexAnimFullDuration +
     smd->m_reflexAnimDerampDuration) : 0.0f;
  m_hsi.duration = NMP::maximum( 
    hsiImpulseStaggerDuration, 
    NMP::maximum( NMP::maximum(hsiLookDuration, hsiAnimDuration), hsiReachDuration));

  m_SR.m_lookAtWoundOrHitSource = smd->m_lookAtWoundOrHitSource;
  m_SR.m_deathTrigger = smd->m_deathTrigger;
  // Parameter now deprecated as of 3.5, to be removed for future releases.
  m_SR.m_deathTriggerOnHeadhit = false; //smd->m_deathTriggerOnHeadhit;

  return true;
}

} // namespace ER

//----------------------------------------------------------------------------------------------------------------------
