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
#ifdef _MSC_VER
  #pragma once
#endif
#ifndef MR_NODE_OPERATOR_HIT_H
#define MR_NODE_OPERATOR_HIT_H
//----------------------------------------------------------------------------------------------------------------------
#include "morpheme/mrNetwork.h"
#include "euphoria/erAttribData.h"
#include "euphoria/erHitUtils.h"
#include "euphoria/erHitReaction.h"
//----------------------------------------------------------------------------------------------------------------------

namespace MR
{
class PhysicsRig;
}

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
/// \brief Hit message data
/// This structure must be kept in sync with the attributes serialised by morpheme connect
//----------------------------------------------------------------------------------------------------------------------
struct HitMessageData
{
  /// \brief Sets all params to their default values.
  void reset();

  /// \brief Endian swaps the whole structure.
  static HitMessageData* endianSwap(void* data);

  NM_INLINE static bool locate(MR::Message* message);
  NM_INLINE static bool dislocate(MR::Message* message);

  // Hit Info
  //
  // the part of the rig that got hit
  int32_t m_rigPartIndex;

  // hit "ray" data
  float m_hitPointLocalX;
  float m_hitPointLocalY;
  float m_hitPointLocalZ;

  float m_hitNormalLocalX;
  float m_hitNormalLocalY;
  float m_hitNormalLocalZ;

  float m_hitDirectionLocalX;
  float m_hitDirectionLocalY;
  float m_hitDirectionLocalZ;

  float m_hitDirectionWorldX;
  float m_hitDirectionWorldY;
  float m_hitDirectionWorldZ;

  float m_sourcePointWorldX;
  float m_sourcePointWorldY;
  float m_sourcePointWorldZ;

  // performance tuning vars
  //
  // the priority of this hit (determines more or less important than previous ones)
  int32_t m_priority;

  // delays and durations
  float m_reachDelay;               // pause time before reach for wound
  float m_reachDuration;            // pause time before reach for wound
  float m_maxReachSpeed;            // effector speed limit for reaching
  float m_reflexAnimStart;          // pause time before running pain reflex anim
  float m_reflexAnimRampDuration;   // duration of ramp up to full pain reflex weight
  float m_reflexAnimFullDuration;   // duration at full pain reflex weight
  float m_reflexAnimDerampDuration; // duration of ramp down from full pain reflex
  float m_reflexAnimMaxWeight;      // reflex anim "saturation" weight
  float m_reflexLookDuration;       // duration of look at wound (if running)
  float m_deathTriggerDelay;        // pause before initiation of death relax
  float m_deathRelaxDuration;       // duration of death relax ramp down

  float m_expandLimitsFullDuration;   // duration at full expansion
  float m_expandLimitsDerampDuration; // duration of ramp down from full expansion
  float m_expandLimitsMaxExpansion;   // maximum expansion to apply

  // impulse tuning params
  float m_partImpactMagnitude; // magnitude of hit impulse applied to the part
  float m_partResponseRatio;   // mass-proportional response control var for the part impulse
  float m_bodyImpactMagnitude; // magnitude of hit impulse applied to the whole character
  float m_bodyResponseRatio;   // mass-proportional response control var for the body impulse
  float m_torqueMultiplier;    // cheat torque multiple of "correct" torque from hit impulse
  float m_desiredMinimumLift;  // desired minimum impulse in direction opposing gravity
  float m_liftResponseRatio;
  float m_impulseYield;        // level of damping reduction on spine (for passive response to hit impulse)
  float m_impulseYieldDuration;// duration of spine damping reduction

  // stagger/balance tuning
  float m_impulseTargetStepSpeed; // balance error due to impulse
  float m_impulseTargetStepDuration;  // duration of balance error

  float m_balanceAssistance;  // desired assistance for balancer

  float m_impulseLegStrengthReduction; // leg weakening due to impulse
  float m_impulseLegStrengthReductionDuration;  // duration of leg weakening

  float m_deathTargetStepSpeed; // balance/stepping error while dying
  float m_deathBalanceStrength;       // balance/stepping strength while dying

  // Int Attributes
  int32_t m_reachSku;   // reach with left/right or both arms
  int32_t m_reflexAnim; // which pain reflex anim to use

  // Flags activate or deactivate different aspects of the performance
  //
  bool    m_reachSwitch;               // do reach for wound (or not)
  bool    m_reflexAnimSwitch;          // do pain reflex anim (or not)
  bool    m_reflexLookSwitch;          // do look at wound (or not)
  bool    m_forcedFalldownSwitch;      // whether the character should fall down
  float   m_targetTimeBeforeFalldown;  // target time before forced fall down
  int32_t m_targetNumberOfStepsBeforeFalldown;  // target num steps before forced fall down
  bool    m_impulseDirWorldOrLocal;    // apply impulse using world data (or local)
  bool    m_lookAtWoundOrHitSource;   // target the look towards the wound or the origin of the hit eg. a shooter
  bool    m_deathTrigger;              // specifies a death hit (or not)
  bool    m_deathTriggerOnHeadhit;    // specifies a death hit for head hits
  bool    m_expandLimitsSwitch;         // enables/disables the expand limits feature
};

class AttribDataOperatorHitDef : public MR::AttribData
{
public:
  static AttribDataOperatorHitDef* create(
    NMP::MemoryAllocator* allocator,
    float                 standingStillTimeout,
    float                 recoveredTimeout,
    float                 fallenAliveTimeout,
    float                 fallenDeadTimeout,
    bool                  prioritiseReaches,
    float                 preFallTargetStepVelocity,
    float                 fallTargetStepVelocity,
    float                 fallBodyPushMagnitude,
    uint16_t              refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();
  static AttribDataOperatorHitDef* init(
    NMP::Memory::Resource& resource,
    float                  standingStillTimeout,
    float                  recoveredTimeout,
    float                  fallenAliveTimeout,
    float                  fallenDeadTimeout,
    bool                   prioritiseReaches,
    float                  preFallTargetStepVelocity,
    float                  fallTargetStepVelocity,
    float                  fallBodyPushMagnitude,
    uint16_t               refCount = 0);

  NM_INLINE AttribDataOperatorHitDef() { setType(ATTRIB_TYPE_OPERATOR_HIT_DEF); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_HIT_DEF; }

  /// For Manager registration.
  static void locate(MR::AttribData* target);
  static void dislocate(MR::AttribData* target);
  static MR::AttribDataHandle create(NMP::MemoryAllocator* allocator);
  static void endianSwap(AttribDataOperatorHitDef* defData);

  // the data
  //
  float m_standingStillTimeout;
  float m_recoveredTimeout;
  float m_fallenAliveTimeout;
  float m_fallenDeadTimeout;
  float m_preFallTargetStepVelocity;
  float m_fallTargetStepVelocity;
  float m_fallBodyPushMagnitude;
  bool m_prioritiseReaches;
};

//----------------------------------------------------------------------------------------------------------------------
/// \class ER::AttribDataOperatorHitState
/// \brief Container for hit operator state.
/// \ingroup AttribData
//----------------------------------------------------------------------------------------------------------------------
class AttribDataOperatorHitState : public MR::AttribData
{
public:
  static AttribDataOperatorHitState* create(
    NMP::MemoryAllocator* allocator,
    bool newHit,
    uint16_t refCount = 0);

  static NMP::Memory::Format getMemoryRequirements();

  static AttribDataOperatorHitState* init(
    NMP::Memory::Resource& resource,
    bool newHit, uint16_t refCount = 0);

  NM_INLINE AttribDataOperatorHitState() { setType(ATTRIB_TYPE_OPERATOR_HIT_STATE); setRefCount(0); }
  NM_INLINE static MR::AttribDataType getDefaultType() { return ATTRIB_TYPE_OPERATOR_HIT_STATE; }

  /// For Manager registration.
  static MR::AttribDataHandle create(NMP::MemoryAllocator* allocator);

  void updateFromConnectedCPInputs(MR::NodeDef* node, MR::Network* net);
  void tickHitReaction(MR::NodeDef* nodeDef, MR::Network* net);
  bool handleHitMessage(const HitMessageData* smd, MR::NodeID nodeId, MR::Network* net);
  
  MR::FrameCount m_lastUpdateFrame;
  bool m_newHit;
  ER::HitSelectionInfo m_hsi;
  ER::HitAttribs m_attribs;
  HitReaction m_SR;
};

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.
MR::AttribData* nodeOperatorHitOutputCPUpdate(
  MR::NodeDef* node,
  MR::PinIndex outputCPPinIndex, /// The output pin we have been asked to update. We have 1:
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// OutputCP and state data init.
void nodeOperatorHitInitInstance(
  MR::NodeDef* node,
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// OutputCP op declarations.
bool nodeOperatorHitMessageHandler(
  const MR::Message& message,
  MR::NodeID nodeID,
  MR::Network* net);

//----------------------------------------------------------------------------------------------------------------------
// Enumerating the output control parameter pins.
enum NodeOperatorHitOutCPPinIDs
{
  CP_OUT_NEWREACH0 = 0,
  CP_OUT_NEWREACH1,
  CP_OUT_REACHLIMBINDEX0,
  CP_OUT_REACHLIMBINDEX1,
  CP_OUT_REACHPARTINDEX0,

  CP_OUT_REACHPARTINDEX1,
  CP_OUT_REACHPOSITION0,
  CP_OUT_REACHPOSITION1,
  CP_OUT_REACHNORMAL0,
  CP_OUT_REACHNORMAL1,

  CP_OUT_REACHSTRENGTH0,
  CP_OUT_REACHSTRENGTH1,
  CP_OUT_REACHSPEEDLIMIT0,
  CP_OUT_REACHSPEEDLIMIT1,

  CP_OUT_IMPULSESPINEDAMPING,

  CP_OUT_LOOKPOSITION,
  CP_OUT_LOOKWEIGHT,
  CP_OUT_BODYSTRENGTH,

  CP_OUT_STEPVELOCITY,
  CP_OUT_BALANCEASSISTANCEPOSITION,
  CP_OUT_BALANCEASSISTANCEORIENTATION,
  CP_OUT_BALANCEASSISTANCEVELOCITY,
  CP_OUT_LEGSTRENGTH0,
  CP_OUT_LEGSTRENGTH1,
  CP_OUT_HITREFLEXANIMIDWEIGHT,
  CP_OUT_HITREFLEXANIMWEIGHT,
  CP_OUT_PERFORMANCESTATEEXIT,

  CP_OUT_COUNT
};

enum NodeOperatorHitInputCPPinIDs
{
  CP_IN_LOOKATWOUNDORHITSOURCE = 0,
  CP_IN_DEATHTRIGGER,
  CP_IN_STANDINGSTILLTIME,
  CP_IN_FALLENTIME,
  CP_IN_STEPPINGTIME,
  CP_IN_STEPPCOUNT,

  CP_IN_COUNT
};


//----------------------------------------------------------------------------------------------------------------------
// HitMessageData inline functions.
//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool HitMessageData::locate(MR::Message* message)
{
  NMP_ASSERT(message->m_dataSize == sizeof(HitMessageData));
  endianSwap(message->m_data);
  return true;
}

//----------------------------------------------------------------------------------------------------------------------
NM_INLINE bool HitMessageData::dislocate(MR::Message* message)
{
  NMP_ASSERT(message->m_dataSize == sizeof(HitMessageData));
  endianSwap(message->m_data);
  return true;
}

} // namespace MR

//----------------------------------------------------------------------------------------------------------------------
#endif // MR_NODE_OPERATOR_HIT_H
//----------------------------------------------------------------------------------------------------------------------
