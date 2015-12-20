// Copyright (c) 2011 NaturalMotion.  All Rights Reserved.
// Not to be copied, adapted, modified, used, distributed, sold,
// licensed or commercially exploited in any manner without the
// written consent of NaturalMotion.
//
// All non public elements of this software are the confidential
// information of NaturalMotion and may not be disclosed to any
// person nor used for any purpose not expressly approved by
// NaturalMotion in writing.

#ifndef NM_HITREACTION_H
#define NM_HITREACTION_H

#include "NMPlatform/NMPlatform.h"
#include "erHitUtils.h"

namespace MR
{
  class PhysicsRig;
  class InstanceDebugInterface;
}

namespace ER
{

class Character;
class DimensionalScaling;

//----------------------------------------------------------------------------------------------------------------------
/// \class ER::HitReaction
/// \brief Hit performance state for the hit node
//----------------------------------------------------------------------------------------------------------------------

class HitReaction
{
public:

  friend class AttribDataOperatorHitState;

  // for clarity these constants stand for numbers of arms/legs handled by this
  // operator and which are currently set in stone
  // (still somewhat unresolved is the thornier issue of whether/how these could made
  // somehow network dependent and reflected in the various places such as the
  // manifest etc. where they need to be known)
  //
  static const int32_t s_numArms = 2;
  static const int32_t s_numLegs = 2;

  // Enum holding current performance status
  // used eg. to determine/activate transitions
  enum StatusCode
  {
    RUNNING = 0,  // started and running
    RECOVERED,    // normal recovery (not fallen not dead)
    FALLEN,       // lost balance and fallen over (but not dead)
    DEAD          // dead
  };

  // Structure filled out during update(). Contains all info required to
  // determine settings of operator cp outputs for the frame
  //
  struct OutputSourceData
  {
    HitSelectionInfo m_reachHitSelectionInfo[s_numArms];
    ReachInfo m_reachInfo[s_numArms];
    BalanceInfo m_balanceInfo;            // error and strength parms for balance and stepping
    LookInfo m_lookInfo;
    AnimSelectionInfo m_animSelectionInfo;
    float m_legStrengths[s_numLegs];
    float m_spineDamping;
    bool m_haveNewReach[s_numArms];     // new info flag for reaching (for each arm)
    bool m_lookFlag;
    bool m_pointFlag;
  };

  void update(
    MR::PhysicsRig* pr, 
    float dt,
    bool newHit,
    ER::HitSelectionInfo& hsi,
    ER::HitAttribs& attribs,
    OutputSourceData& osd,
    MR::InstanceDebugInterface* pDebugDrawInst);
  void reset();
  void init();

private:
  // update aux
  //
  bool updateHitRecords( 
    bool newHitInput,
    float dt,
    HitSelectionInfo& hsi, ER::HitAttribs& attribs,
    MR::InstanceDebugInterface* pDebugDrawInst);

  void updateNewImpulse();
  void updateStatus(float dt, float& bodyStrength);
  void updateSpineAndLegStrengths(float& spineYield, float *legStrengths);
  void updateReaching(bool newReach, const ER::DimensionalScaling& scalingSource);
  void updateBalance(ER::BalanceInfo& balanceInfo);
  void updateLook(ER::LookInfo& lookInfo, const ER::DimensionalScaling& dimensionalScaling);
  void updateAnim(ER::AnimSelectionInfo& animSelectionInfo);
  void updateForcedFall(
    ER::BalanceInfo& balanceInfo,
    float* legStrengths,
    float dt, const ER::DimensionalScaling& dimensionalScaling);
  void updateExpandLimits(MR::PhysicsRig* physicsRig);

  ER::BodyPartTypeEnum::Type determineHSIPartType(
    const ER::HitSelectionInfo& hsi, MR::InstanceDebugInterface* pDebugDrawInst);

  HSILog m_hsiLog;                                          // hit logger
  ER::HitAttribs m_latestHSIAttribs;                        // new attribs for this hit
  ER::HitAttribs m_attribs;                                 // current attribs in effect
  ER::HitSelectionInfo m_latestHSI;                         // new hit info
  ER::HitSelectionInfo m_impulseHitSelectionInfo;           // hsi associated with the impulse to be applied
  ER::HitSelectionInfo m_reachHitSelectionInfo[s_numArms];  // hsi's associated with reach targets
  ImpulseInfo m_impulseInfo;                                // impulse specific details for the current impulse
  ReachInfo m_reachInfo[s_numArms];                         // reach specific details for the current reaches

  NMP::Vector3 m_gravity;                         // the current gravity
  NMP::Vector3 m_forceFallDirection;              // forced fall step direction

  // Pointer to the character on which the hit behaviours will be running
  ER::Character* m_character;

  float m_deathTimer;                             // death delay timer
  float m_recoveredTimer;                         // transition delay timer

  float m_standingStillTime;                      // feedbacks from balancer
  float m_fallenTime;
  float m_steppingTime;
  int32_t m_stepCount;

  StatusCode m_statusCode;                        // overall performance status

  bool m_forceFall;                               // forced fall is required
  bool m_forceFallTriggered;                      // forced fall has now been triggered
  float m_forceFallLegStrengthDecayTimer;         // 1 when the hit comes in, degraded after triggered
  float m_fallStepTimeLimit;                      // forced fall stepping time limit
  int32_t m_fallStepCountLimit;                   // forced fall step count limit

  // from def data
  float m_preFallTargetStepVelocity;
  float m_fallTargetStepVelocity;
  float m_fallBodyImpulseMagnitude;

  float m_standingStillTimeout;  // start transition timer after this long standing still
  float m_fallenAliveTimeout;    // fallen time threshold not dead
  float m_fallenDeadTimeout;     // fallen time threshold dead
  float m_recoveredTimeout;      // start transition after transition timer reaches this time

  int32_t m_reachRecords[s_numArms];  // reach tracking
  int32_t m_leftLegIndex;             // character leg limb indexes
  int32_t m_rightLegIndex;
  int32_t m_firstArmIndex;            // start index for arm limbs
  int32_t m_leftArmIndex;             // character arm limb indexes
  int32_t m_rightArmIndex;

  bool m_haveNewHit;                  // new hit came in this frame
  bool m_haveNewReach[s_numArms];
  bool m_dying;                       // death
  bool m_inDeathRelax;                // death relax state

  bool m_deathTrigger;                // triggers dying
  bool m_deathTriggerOnHeadhit;       // triggers dying
  bool m_lookAtWoundOrHitSource;      // can be used to swap look between wound and a "hit source" eg. the shooter
  bool m_prioritiseReaches;           // whether or not to prioritise reaches
  bool m_haveNewImpulse;              // signals new impulse to be applied

};

} // namespace

#endif // NM_HITREACTION_H
