#pragma once

/*
 * Copyright (c) 2013 NaturalMotion Ltd. All rights reserved.
 *
 * Not to be copied, adapted, modified, used, distributed, sold,
 * licensed or commercially exploited in any manner without the
 * written consent of NaturalMotion.
 *
 * All non public elements of this software are the confidential
 * information of NaturalMotion and may not be disclosed to any
 * person nor used for any purpose not expressly approved by
 * NaturalMotion in writing.
 *
 */

//----------------------------------------------------------------------------------------------------------------------
//                                  This file is auto-generated
//----------------------------------------------------------------------------------------------------------------------

#ifndef NM_MDF_BDEF_HOLDACTION_H
#define NM_MDF_BDEF_HOLDACTION_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HoldAction.behaviour"

#include "NMPlatform/NMPlatform.h"
#include "NMPlatform/NMVector3.h"
#include "physics/mrPhysicsSerialisationBuffer.h"
#include "euphoria/erBehaviour.h"
#include "euphoria/erLimb.h"

#include "NetworkDescriptor.h"
#include "NetworkConstants.h"


namespace ER
{
  class Character;
  class Module;
}




#ifdef NM_COMPILER_MSVC
# pragma warning ( push )
# pragma warning ( disable : 4324 ) // structure was padded due to __declspec(align()) 
#endif // NM_COMPILER_MSVC

#ifndef NM_BEHAVIOUR_LIB_NAMESPACE
  #error behaviour library namespace undefined
#endif

namespace NM_BEHAVIOUR_LIB_NAMESPACE
{

struct HoldActionPoseRequirements
{

  HoldActionPoseRequirements()
  {
  }
};

struct HoldActionBehaviourData
{
  friend class HoldActionBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 EdgeStart;  ///< Position in world space of start of edge segment
    NMP::Vector3 EdgeEnd;  ///< Position in world space of end of edge segment
    NMP::Vector3 EdgeNormal;  ///< Normal of (perpendicular to) edge. The back-of-hand normal will try to match this
    float EdgeImportance;  ///< Edge importance that is used to arbitrate with other behaviours when reaching. The user specified endge will always be used if weight is > 1. Otherwise the edge will be evaluated alongside other, detected edges.
    float PullUpAmount;  /// How high the character tries to pull himself up. 1: to chest level, 0: arms fully extended (but note that the arms may still have strength).
    
    float PullUpStrengthScale;  /// How strong the arms will try to pull up. This scales the normal strength is scaled by this factor. 0: no strength, 1: normal strength, 2: twice normal strength (super strong)
    
    float HoldImportance[NetworkConstants::networkMaxNumArms];  ///< The amount each arm should hold onto the edge, in terms of competing with other behaviours for use of the arm. A value of zero will prevent the character from holding with the arm.
    bool DoHold;  ///< Dynamically toggle Hold. Set it to false to make character release the grabbed edge or to prevent the character from grabbing any edges.
    bool IsWall;  ///< True if the object being held is a wall, as opposed to a pole or ledge. This information is used to determine whether the steepness of the surface matters for grabbing, and whether there is a preferred side from which to hang.
    void* PhysicsObjectID;  ///< The object to try to hold. If set, then the edge is specified relative to this object, in the local space of the object.
  };
  ControlParams m_cp;


  struct OutputControlParams
  {
    float NumConstrainedHands;  ///< Number of hands which have successfully grabbed and held onto an edge.
    float HoldAttemptImportance;  ///< Indicates how much the behaviour is attempting to grab the edge (between 0 and 1). When the character has successfully grabbed and held onto an edge the value is 1.
    float HoldDuration;  ///< Indicates how long the character has been constrained to an edge. In absolute units.
    bool HandHolding[NetworkConstants::networkMaxNumArms];  ///< Is the nth hand constrained.
  };
  OutputControlParams m_ocp;

  float MinSupportSlope;  /// If ground is detected below the character it is not considered supportive (i.e. grabbing is allowed) if it is steeper than this (angle in degrees between "up" and ground normal)
  
  float VerticalSpeedToStart;  /// Grabbing is allowed to start when the character is moving downwards faster than this, in m/s (standard character).
  
  float UnbalancedAmount;  /// Balance amounts greater than this for a period of time are considered supporting (no grab)
  
  float MinUnbalancedPeriod;  /// When balanced more than this time, the character is considered supported so grab is disabled. In seconds (standard character).
  
  float VerticalSpeedToStop;  /// The reach/grab is stopped when the character is moving downwards slower than this, in m/s (standard character). The default value disables grabbing when he is moving upwards. Set to a large -ve number to disable this check.
  
  float MinHoldPeriod;  /// If a hold is successful it will always be active for at least this long. In seconds (standard character).
  
  float MaxHoldPeriod;  /// A hold will never last longer than this before giving up. In seconds (standard character). A value less than or equal to zero means to never let go due to the hold duration.
  
  float NoHoldPeriod;  /// At least this much time is enforced between successive holds. In seconds (standard character).
  
  float MaxHandsBehindBackPeriod;  /// If the hands are constrained behind the back for longer than this, the hold will be aborted. In seconds (standard character).
  
  float MaxReachAttemptPeriod;  /// If a character has tried but failed to reach an edge for this long, the reach attempt is aborted. In seconds (standard character).
  
  float MinReachRecoveryPeriod;  /// After a failed reach attempt, a new attempt isn't started until this period has elapsed. In seconds (standard character).
  
  float CreateAtDistance;  /// Hands needs to be at least this close to their target for constraint creation. In metres (standard character).
  
  float EnableOrientationAtAngle;  /// Hand orientation is enforced when the angle between it and the target is less than this angle, in degrees
  
  float DestroyAtDistance;  /// When constrained, hands need to be this far from their target for the constraint to be destroyed (the hold to be broken). In metres (standard character).
  
  float DisableOrientationAtAngle;  /// Hand orientation will stop being enforced when the angle between it and the target is greater than this angle, in degrees
  
  int32_t LockedLinearDofs;  /// Which linear constraint axes are locked.
  
  int32_t LockedAngularDofs;  /// Which angular constraint axes are locked.
  
  float TimeBeforeLookingDown;  /// The proportion of maximum hold duration at which the character will switch from looking at the edge to looking down. This gives an impression of fatigue.
  
  float ChestControlImminence;  /// Used to control the chest (make it upright) by swinging/spinning the legs and arms. Smaller values will result in more vigorous movements. In units of 1/seconds (standard character)
  
  float ChestControlStiffnessScale;  /// Strength multiplier for swinging/spinning the legs and arms when controlling the chest
  
  float ChestControlPassOnAmount;  /// When controlling the chest, a small value confines the motions to the arms, a larger value results in leg motions
  
  float MaxReachDistance;  /// An edge will be considered only when closer to the character's chest than this distance. In metres (standard character).
  
  bool HoldOnContact;  /// Hold on to whatever the hand touches. This makes the hands to be magically sticky.
  
  bool DisableCollisions;  /// Disable collisions between hands and object
  
  bool DisableAngularDofsUntilHanging;  /// If true then the orientations will only get locked (if requested) after the character has come to hang vertically.
  


public:

  HoldActionBehaviourData()
  {
    MinSupportSlope = float(25);
    VerticalSpeedToStart = float(0.8);
    UnbalancedAmount = float(0.65);
    MinUnbalancedPeriod = float(0.5);
    VerticalSpeedToStop = float(0.06);
    MinHoldPeriod = float(0.1);
    MaxHoldPeriod = float(10);
    NoHoldPeriod = float(0.5);
    MaxHandsBehindBackPeriod = float(1);
    MaxReachAttemptPeriod = float(5);
    MinReachRecoveryPeriod = float(3);
    CreateAtDistance = float(0.1);
    EnableOrientationAtAngle = float(120);
    DestroyAtDistance = float(0.25);
    DisableOrientationAtAngle = float(160);
    LockedLinearDofs = 7;
    LockedAngularDofs = 7;
    HoldOnContact = false;
    DisableCollisions = true;
    DisableAngularDofsUntilHanging = true;
    TimeBeforeLookingDown = float(0.8);
    ChestControlImminence = float(2);
    ChestControlStiffnessScale = float(1);
    ChestControlPassOnAmount = float(1);
    MaxReachDistance = float(2);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 20);
    NMP_ASSERT(data->m_ints->m_numValues == 5);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    MinSupportSlope = data->m_floats->m_values[0];
    VerticalSpeedToStart = data->m_floats->m_values[1];
    UnbalancedAmount = data->m_floats->m_values[2];
    MinUnbalancedPeriod = data->m_floats->m_values[3];
    VerticalSpeedToStop = data->m_floats->m_values[4];
    MinHoldPeriod = data->m_floats->m_values[5];
    MaxHoldPeriod = data->m_floats->m_values[6];
    NoHoldPeriod = data->m_floats->m_values[7];
    MaxHandsBehindBackPeriod = data->m_floats->m_values[8];
    MaxReachAttemptPeriod = data->m_floats->m_values[9];
    MinReachRecoveryPeriod = data->m_floats->m_values[10];
    CreateAtDistance = data->m_floats->m_values[11];
    EnableOrientationAtAngle = data->m_floats->m_values[12];
    DestroyAtDistance = data->m_floats->m_values[13];
    DisableOrientationAtAngle = data->m_floats->m_values[14];
    LockedLinearDofs = data->m_ints->m_values[0];
    LockedAngularDofs = data->m_ints->m_values[1];
    HoldOnContact = (data->m_ints->m_values[2] != 0);
    DisableCollisions = (data->m_ints->m_values[3] != 0);
    DisableAngularDofsUntilHanging = (data->m_ints->m_values[4] != 0);
    TimeBeforeLookingDown = data->m_floats->m_values[15];
    ChestControlImminence = data->m_floats->m_values[16];
    ChestControlStiffnessScale = data->m_floats->m_values[17];
    ChestControlPassOnAmount = data->m_floats->m_values[18];
    MaxReachDistance = data->m_floats->m_values[19];
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 5);
    NMP_ASSERT(data->m_ints->m_numValues == 2);
    NMP_ASSERT(data->m_uint64s->m_numValues == 1);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 12);

    m_cp.DoHold = (data->m_ints->m_values[0] != 0);
    m_cp.IsWall = (data->m_ints->m_values[1] != 0);
    m_cp.EdgeImportance = data->m_floats->m_values[0];
    m_cp.EdgeStart.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.EdgeEnd.set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.EdgeNormal.set(data->m_vector3Data->m_values[8], data->m_vector3Data->m_values[9], data->m_vector3Data->m_values[10]);
    m_cp.PhysicsObjectID = (void*) (data->m_uint64s->m_values[0]);
    m_cp.PullUpAmount = data->m_floats->m_values[1];
    m_cp.PullUpStrengthScale = data->m_floats->m_values[2];
    m_cp.HoldImportance[0] = data->m_floats->m_values[3];
    m_cp.HoldImportance[1] = data->m_floats->m_values[4];
  }


  /// Settings that govern when a reach happens

  /// If ground is detected below the character it is not considered supportive (i.e. grabbing is allowed) if it is steeper than this (angle in degrees between "up" and ground normal)
  float getMinSupportSlope() const { return MinSupportSlope; }
  /// Grabbing is allowed to start when the character is moving downwards faster than this, in m/s (standard character).
  float getVerticalSpeedToStart() const { return VerticalSpeedToStart; }
  /// Balance amounts greater than this for a period of time are considered supporting (no grab)
  float getUnbalancedAmount() const { return UnbalancedAmount; }
  /// When balanced more than this time, the character is considered supported so grab is disabled. In seconds (standard character).
  float getMinUnbalancedPeriod() const { return MinUnbalancedPeriod; }

  /// Settings that govern when a reach ends

  /// The reach/grab is stopped when the character is moving downwards slower than this, in m/s (standard character). The default value disables grabbing when he is moving upwards. Set to a large -ve number to disable this check.
  float getVerticalSpeedToStop() const { return VerticalSpeedToStop; }

  /// If a hold is successful it will always be active for at least this long. In seconds (standard character).
  float getMinHoldPeriod() const { return MinHoldPeriod; }
  /// A hold will never last longer than this before giving up. In seconds (standard character). A value less than or equal to zero means to never let go due to the hold duration.
  float getMaxHoldPeriod() const { return MaxHoldPeriod; }
  /// At least this much time is enforced between successive holds. In seconds (standard character).
  float getNoHoldPeriod() const { return NoHoldPeriod; }
  /// If the hands are constrained behind the back for longer than this, the hold will be aborted. In seconds (standard character).
  float getMaxHandsBehindBackPeriod() const { return MaxHandsBehindBackPeriod; }
  /// If a character has tried but failed to reach an edge for this long, the reach attempt is aborted. In seconds (standard character).
  float getMaxReachAttemptPeriod() const { return MaxReachAttemptPeriod; }
  /// After a failed reach attempt, a new attempt isn't started until this period has elapsed. In seconds (standard character).
  float getMinReachRecoveryPeriod() const { return MinReachRecoveryPeriod; }

  /// A hold is achieved by establishing a constraint, the hold is broken (the edge released) when the constraint is destroyed.

  /// Hands needs to be at least this close to their target for constraint creation. In metres (standard character).
  float getCreateAtDistance() const { return CreateAtDistance; }
  /// Hand orientation is enforced when the angle between it and the target is less than this angle, in degrees
  float getEnableOrientationAtAngle() const { return EnableOrientationAtAngle; }
  /// When constrained, hands need to be this far from their target for the constraint to be destroyed (the hold to be broken). In metres (standard character).
  float getDestroyAtDistance() const { return DestroyAtDistance; }
  /// Hand orientation will stop being enforced when the angle between it and the target is greater than this angle, in degrees
  float getDisableOrientationAtAngle() const { return DisableOrientationAtAngle; }
  /// Which linear constraint axes are locked.
  int32_t getLockedLinearDofs() const { return LockedLinearDofs; }
  /// Which angular constraint axes are locked.
  int32_t getLockedAngularDofs() const { return LockedAngularDofs; }
  /// Hold on to whatever the hand touches. This makes the hands to be magically sticky.
  bool getHoldOnContact() const { return HoldOnContact; }
  /// Disable collisions between hands and object
  bool getDisableCollisions() const { return DisableCollisions; }
  /// If true then the orientations will only get locked (if requested) after the character has come to hang vertically.
  bool getDisableAngularDofsUntilHanging() const { return DisableAngularDofsUntilHanging; }

  /// When holding a character can attempt to pull himself up rather than simply hanging. This can add to the realism.

  /// The proportion of maximum hold duration at which the character will switch from looking at the edge to looking down. This gives an impression of fatigue.
  float getTimeBeforeLookingDown() const { return TimeBeforeLookingDown; }
  /// Used to control the chest (make it upright) by swinging/spinning the legs and arms. Smaller values will result in more vigorous movements. In units of 1/seconds (standard character)
  float getChestControlImminence() const { return ChestControlImminence; }
  /// Strength multiplier for swinging/spinning the legs and arms when controlling the chest
  float getChestControlStiffnessScale() const { return ChestControlStiffnessScale; }
  /// When controlling the chest, a small value confines the motions to the arms, a larger value results in leg motions
  float getChestControlPassOnAmount() const { return ChestControlPassOnAmount; }

  /// Settings that govern what the character will consider as an edge that can be grabbed.

  /// An edge will be considered only when closer to the character's chest than this distance. In metres (standard character).
  float getMaxReachDistance() const { return MaxReachDistance; }


  // control parameters
  /// Dynamically toggle Hold. Set it to false to make character release the grabbed edge or to prevent the character from grabbing any edges.
  bool getDoHoldCP() const { return m_cp.DoHold; }
  /// True if the object being held is a wall, as opposed to a pole or ledge. This information is used to determine whether the steepness of the surface matters for grabbing, and whether there is a preferred side from which to hang.
  bool getIsWallCP() const { return m_cp.IsWall; }
  /// Edge importance that is used to arbitrate with other behaviours when reaching. The user specified endge will always be used if weight is > 1. Otherwise the edge will be evaluated alongside other, detected edges.
  float getEdgeImportanceCP() const { return m_cp.EdgeImportance; }
  /// Position in world space of start of edge segment
  const NMP::Vector3& getEdgeStartCP() const { return m_cp.EdgeStart; }
  /// Position in world space of end of edge segment
  const NMP::Vector3& getEdgeEndCP() const { return m_cp.EdgeEnd; }
  /// Normal of (perpendicular to) edge. The back-of-hand normal will try to match this
  const NMP::Vector3& getEdgeNormalCP() const { return m_cp.EdgeNormal; }
  /// The object to try to hold. If set, then the edge is specified relative to this object, in the local space of the object.
  void* getPhysicsObjectIDCP() const { return m_cp.PhysicsObjectID; }
  /// How high the character tries to pull himself up. 1: to chest level, 0: arms fully extended (but note that the arms may still have strength).
  float getPullUpAmountCP() const { return m_cp.PullUpAmount; }
  /// How strong the arms will try to pull up. This scales the normal strength is scaled by this factor. 0: no strength, 1: normal strength, 2: twice normal strength (super strong)
  float getPullUpStrengthScaleCP() const { return m_cp.PullUpStrengthScale; }
  /// The amount each arm should hold onto the edge, in terms of competing with other behaviours for use of the arm. A value of zero will prevent the character from holding with the arm.
  float getHoldImportanceCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.HoldImportance[index]; }
  enum { maxHoldImportanceCP = 2 };

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// Number of hands which have successfully grabbed and held onto an edge.
  void setNumConstrainedHandsOCP(float value) { m_ocp.NumConstrainedHands = value; }
  /// Is the nth hand constrained.
  void setHandHoldingOCP(unsigned int index, bool value) { NMP_ASSERT(index < 2); m_ocp.HandHolding[index] = value; }
  enum { maxHandHoldingOCP = 2 };
  /// Indicates how much the behaviour is attempting to grab the edge (between 0 and 1). When the character has successfully grabbed and held onto an edge the value is 1.
  void setHoldAttemptImportanceOCP(float value) { m_ocp.HoldAttemptImportance = value; }
  /// Indicates how long the character has been constrained to an edge. In absolute units.
  void setHoldDurationOCP(float value) { m_ocp.HoldDuration = value; }
};


class HoldActionBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "HoldAction"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 16; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class HoldActionBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    DoHold_CP_ID = 0,
    IsWall_CP_ID = 1,
    EdgeImportance_CP_ID = 2,
    EdgeStart_CP_ID = 3,
    EdgeEnd_CP_ID = 4,
    EdgeNormal_CP_ID = 5,
    PhysicsObjectID_CP_ID = 6,
    PullUpAmount_CP_ID = 7,
    PullUpStrengthScale_CP_ID = 8,
    HoldImportance_0_CP_ID = 9,
    HoldImportance_1_CP_ID = 10,
  };

  enum OutputControlParamIDs
  {
    NumConstrainedHands_OCP_ID = 0,
    HandHolding_0_OCP_ID = 1,
    HandHolding_1_OCP_ID = 2,
    HoldAttemptImportance_OCP_ID = 3,
    HoldDuration_OCP_ID = 4,
  };

  const HoldActionBehaviourData& getParams() const { return m_params; }
  HoldActionBehaviourData& getParams() { return m_params; }
  HoldActionPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  HoldActionBehaviourData m_params;
  HoldActionPoseRequirements m_poseRequirements;
};

class HoldActionBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(HoldActionBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline HoldActionBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(HoldActionBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) HoldActionBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_HOLDACTION_H

