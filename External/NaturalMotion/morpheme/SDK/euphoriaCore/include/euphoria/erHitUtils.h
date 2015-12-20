// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_HIT_UTILS_H
#define NM_HIT_UTILS_H

#include "euphoria/erSharedEnums.h"
#include "NMPlatform/NMVector3.h"
#include "Physics/PhysX3/mrPhysX3Configure.h"

namespace ER
{

//----------------------------------------------------------------------------------------------------------------------
// minimum data required to do a hit performance impulse
struct ImpulseInfo
{
  NMP::Vector3 m_bodyImpulseDirection; // used to hold the (normalised) sum of lift and (full-)body push inputs
  float m_partImpulseMagnitude;
  float m_partResponseRatio;
  float m_bodyImpulseMagnitude;
  float m_bodyResponseRatio;
  float m_torqueMultiplier;
  float m_liftBoost;
  float m_liftResponseRatio;
};

struct BalanceInfo
{
  NMP::Vector3 m_targetStepVelocity;
  float m_bodyStrength;
  float m_assistanceOrientation;
  float m_assistancePosition;
};

struct ReachInfo
{
  float m_strength;
  float m_speedLimit;
};

struct LookInfo
{
  NMP::Vector3 m_target; // in world space
  float m_weight;
  LookInfo() { zero(); }

  LookInfo(const NMP::Vector3& tgt, float w) :
     m_target(tgt), m_weight(w)
  {}

  void zero()
  {
    m_target.setToZero();
    m_weight = 0.0f;
  }
};

struct BackFrontTestInfo
{
  NMP::Vector3 v0;
  NMP::Vector3 v1;
  NMP::Vector3 v2;
  NMP::Vector3 n;
  NMP::Vector3 x;
};

struct AnimSelectionInfo {
  AnimSelectionInfo() : id(-1), weight(0.0f) {}
  AnimSelectionInfo(int i, float w) : id(i), weight(w) {}
  int id;
  float weight;
};

struct HitAttribs
{
  float m_reachDelay;                 // pause time before reach for wound
  float m_reachDuration;              // duration of reaching
  float m_maxReachSpeed;              // effector speed limit for reaching

  float m_reflexAnimStart;            // pause time before running pain reflex anim
  float m_reflexAnimRampDuration;     // duration of ramp up to full pain reflex weight
  float m_reflexAnimFullDuration;     // duration at full pain reflex weight
  float m_reflexAnimDerampDuration;   // duration of ramp down from full pain reflex
  float m_reflexAnimMaxWeight;        // reflex anim "saturation" weight

  float m_reflexLookDuration;         // duration of look at wound (if running)
  float m_deathTriggerDelay;          // pause before initiation of death relax
  float m_deathRelaxDuration;         // duration of death relax ramp down

  float m_expandLimitsFullDuration;   // duration at full expansion
  float m_expandLimitsDerampDuration; // duration of ramp down from full expansion
  float m_expandLimitsMaxExpansion;   // maximum expansion to apply

  // impulse tuning params
  float m_partImpactMagnitude;  // magnitude of hit impulse applied to the part
  float m_partResponseRatio;
  float m_bodyImpactMagnitude;  // magnitude of hit impulse applied to whole body
  float m_bodyResponseRatio;
  float m_torqueMultiplier;     // cheat torque multiple of "correct" torque from hit impulse
  float m_desiredMinimumLift;   // desired minimum impulse in direction opposing gravity
  float m_liftResponseRatio;
  float m_impulseYield;         // level of damping reduction on spine (for passive response to hit impulse)

  float m_impulseYieldDuration;     // duration of spine damping reduction

  // stagger/balance tuning
  float m_impulseTargetStepSpeed;         // balance error due to impulse
  float m_impulseTargetStepDuration;          // duration of balance error
  float m_balanceAssistance;                    // balance assistance level
  float m_impulseLegStrengthReduction;          // leg weakening due to impulse
  float m_impulseLegStrengthReductionDuration;  // duration of balance error
  float m_deathTargetStepSpeed;       // balance/stepping error while dying
  float m_deathBalanceStrength;             // balance/stepping strength while dying

  float m_targetTimeBeforeFalldown; // target time before forced fall down

  // int
  int32_t m_reachSku;                     // reach with left/right or both arms
  int32_t m_reflexAnim;                   // which pain reflex anim to use
  int32_t m_targetNumStepsBeforeFalldown; // target number of steps before forced fall down

  // bool
  bool m_reachSwitch;              // do reach for wound (or not)
  bool m_reflexAnimSwitch;         // do pain reflex anim (or not)
  bool m_reflexLookSwitch;         // do look at wound (or not)
  bool m_forcedFalldownSwitch;     // force fall down (or not)
  bool m_impulseDirWorldOrLocal;   // apply impulse using world data (or local)
  bool m_prioritiseReaches;        // apply priority logic or treat all hits as equally important
  bool m_expandLimitsSwitch;
};

// hit selection info captures details that allow us to "shoot" the character via mouse click
//
struct HitSelectionInfo
{
  // codes returned by hit selection update
  enum UpdateCode
  {
    noHit,
    newHit,
    oldHit
  };

  void invalidate();
  bool isInvalid();
  bool isNotBodyHit();
  bool isBodyHit();
  void endianSwap();

  PhysXActor* selectedActor;           // pointer to the actor that got hit
  int32_t limbIndex;                       // for body hits: identifies limb
  int32_t partIndex;                       //                identifies part on limb
  int32_t rigPartIndex;                    //                identifies part in rig

  ER::LimbTypeEnum::Type limbType;         // one of a number of limb types we might have selected
  ER::BodyPartTypeEnum::Type partType;     // one of a number body part types
  int32_t hitType;                         // code for interpretation of what sort of hit this was eg. what type of gun was used etc
  float elapsedTime;

  float duration;                          // intended overall duration of the hit selection event
  int32_t priority;                        // priority eg. as a function of partType
  float damage;                            // deprecated param
  float legStrengthReduction;

  float legStrengthReductionDuration;
  int32_t id;                              // id field
#ifdef NM_HOST_64_BIT
  int32_t pad[1];
#else
  int32_t pad[2];
#endif
  
  // eg. from raycast
  NMP::Vector3 pointLocal;                 // hit pointLocal on body in actor local coords
  NMP::Vector3 normalLocal;                // hit normalLocal on in actor local coords
  NMP::Vector3 hitDirLocal;                // direction of the hit (eg. line along which a hit impulse would be applied)
  NMP::Vector3 hitDirWorld;
  NMP::Vector3 sourcePointWorld;
};

// container class to hold HSI's
//
struct HSILog
{
  HSILog();

  // insert this hsi at an appropriate (priority ordered) place in the array
  // returns the log index of the item if stored or -1 if dropped
  // (an item may be ignored if the log is full with higher priority entries)
  //
  int32_t logHSI(HitSelectionInfo& hsiToLog);
  // return the index of the item with matching id
  int32_t find(int32_t idToMatch);
  // set all priorities to zero and set size to zero
  void clear();
  // add an item (aux for logHSI())
  void push_back(HitSelectionInfo& hsiToPushBack);
  void insert(int32_t at, HitSelectionInfo& hsiToInsert);

  // removes an element from the log
  void remove(int32_t toRemove);
  // special aux functions
  //
  // sum of damage stored in the log
  float getTotalDamage();
  // sum of damage to a given limb identified by the limb index
  float getLimbDamage(int32_t limbIndex);
  // increments elapsedTime by dt, for each item in the log
  void updateTimers(float dt);
  // subtracts rateOfHealing * dt, from each item in the log
  void healDamage(float rateOfHealing, float dt);
  // removes all items with damage <= 0
  void removeZeroDamageItems();
  // removes all items with elapsedTime > duration
  void removeExpiredItems();

  static const int32_t capacity = 10; // max entries
  static int32_t lastID;
  int32_t size;                       // current num entries
  HitSelectionInfo HSIs[capacity];    // entries
};

float calcRampedWeight(float t, float tStart, float maxWeight, float rampDuration, float fullDuration, float derampDuration);

} // namespace

#endif // NM_HIT_UTILS_H
