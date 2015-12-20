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

#ifndef NM_MDF_BDEF_HOLD_H
#define NM_MDF_BDEF_HOLD_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Hold.behaviour"

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

struct HoldPoseRequirements
{

  HoldPoseRequirements()
  {
  }
};

struct HoldBehaviourData
{
  friend class HoldBehaviour;

private:


  struct ControlParams
  {
    float HoldImportance[NetworkConstants::networkMaxNumArms];  ///< Sets the importance of Hold relative to other behaviours for each arm.
    float PullUpAmount;  /// How high the character tries to pull himself up. 1: to chest level, 0: arms fully extended (but note that the arms may still have strength).
    
    float PullUpStrengthScale;  /// How strong the arms will try to pull up. This scales the normal strength is scaled by this factor. 0: no strength, 1: normal strength, 2: twice normal strength (super strong)
    
    bool DoHold;  ///< Dynamically toggle Hold. Set it to false to make character release grabbed edge or to prevent character from grabbing any edges.
    bool IgnoreOvershotEdges;  ///< Prevents the character from grabbing an edge when its chest is likly to land on the horizontal surface above that edge.
  };
  ControlParams m_cp;


  struct OutputControlParams
  {
    NMP::Vector3 EdgeForwardNormal;  ///< The normal to the front surface of the edge (as opposed to the upper surface).
    float NumConstrainedHands;  ///< Number of hands which have successfully grabbed and held onto an edge.
    float HoldAttemptImportance;  ///< Indicates how much the behaviour is attempting to grab the edge (between 0 and 1).
    float HoldDuration;  ///< Indicates how long the character has been constrained to an edge.
    bool HandHolding[NetworkConstants::networkMaxNumArms];  ///< Is the nth hand constrained.
  };
  OutputControlParams m_ocp;

  float MinSupportSlope;  /// If ground is detected below the character or below the edge (if edge raycast probing is enabled) it is not considered supportive (i.e. grabbing is allowed) if it is steeper than this (angle in degrees between "up" and ground normal).
  /// This ensures the grab is only disabled if the surface below the character or below the edge can support the character.
  
  float VerticalSpeedToStart;  /// Grabbing is allowed to start when the character is moving downwards faster than this, in m/s (standard character).
  
  float UnbalancedAmount;  /// Balance amounts greater than this for a period of time are considered supporting (no grab)
  
  float MinUnbalancedPeriod;  /// When balanced more than this time, the character is considered supported so grab is disabled. In seconds (standard character).
  
  float VerticalSpeedToStop;  /// The reach/grab is stopped when the character is moving downwards slower than this, in m/s (standard character). The default value disables grabbing when he is moving upwards. Set to a large -ve number to disable this check.
  
  float MinEdgeQuality;  /// An edge will be considered only if overall "grabbability" is greater than this
  
  float raycastEdgeOffset;  /// Horizontal offset from the edge for the raycast origin. In metres (standard character).
  
  float raycastLength;  /// The length of the raycast. In metres (standard character).
  
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
  
  float timeBeforeLookingDown;  /// The proportion of maximum hold time at which the character will switch from looking at the edge to looking down. This gives an impression of fatigue.
  
  float ChestControlImminence;  /// Used to control the chest (make it upright) by swinging/spinning the legs and arms. In units of 1/seconds (standard character)
  
  float ChestControlStiffnessScale;  /// Strength multiplier for swinging/spinning the legs and arms when controlling the chest
  
  float ChestControlPassOnAmount;  /// When controlling the chest, a small value confines the motions to the arms, a larger value results in leg motions
  
  float MaxReachDistance;  /// An edge will be considered only when closer to the character's chest than this distance. In metres (standard character).
  
  float MinEdgeLength;  /// An edge will be considered only when longer than this. In metres (standard character).
  
  float MinAngleBetweenNormals;  /// An edge will be considered only if the angle between its two faces (normals) is larger than this
  
  float MaxSlope;  /// An edge will be considered only if the angle between the binormal (average of the surface normals either side of the edge) and up is less than this. In degrees.
  
  float MinMass;  /// If an edge belongs to a dynamic or static object, it will only be considered if it is heavier than this. In kilograms (standard character).
  
  float MinVolume;  /// If an edge belongs to a dynamic object, it will only be considered if it is bigger than this. In cubic metres (standard character).
  
  bool enableRaycast;  /// Enables raycast probing to see if there is enough space to hang without unwanted obstructions.
  
  bool DisableAngularDofsUntilHanging;  /// If true then the orientations will only get locked (if requested) after the character has come to hang vertically.
  
  bool HoldOnContact;  /// Hold on to whatever the hand touches. This makes the hands to be magically sticky.
  
  bool DisableCollisions;  /// Disable collisions between hands and object
  
  bool ProjectEdgeNormalOntoGroundPlane;  /// Project edge front surface normal (as opposed to the upper surface normal) into the ground plane.
  


public:

  HoldBehaviourData()
  {
    MinSupportSlope = float(25);
    VerticalSpeedToStart = float(0.8);
    UnbalancedAmount = float(0.65);
    MinUnbalancedPeriod = float(0.5);
    VerticalSpeedToStop = float(0.06);
    enableRaycast = true;
    raycastEdgeOffset = float(0.3);
    raycastLength = float(2.3);
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
    timeBeforeLookingDown = float(0.8);
    ChestControlImminence = float(2);
    ChestControlStiffnessScale = float(1);
    ChestControlPassOnAmount = float(1);
    MaxReachDistance = float(2);
    MinEdgeLength = float(0.25);
    MinAngleBetweenNormals = float(10);
    MaxSlope = float(50);
    MinMass = float(30);
    MinVolume = float(0.5);
    MinEdgeQuality = float(0);
    ProjectEdgeNormalOntoGroundPlane = true;
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 28);
    NMP_ASSERT(data->m_ints->m_numValues == 7);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    MinSupportSlope = data->m_floats->m_values[0];
    VerticalSpeedToStart = data->m_floats->m_values[1];
    UnbalancedAmount = data->m_floats->m_values[2];
    MinUnbalancedPeriod = data->m_floats->m_values[3];
    VerticalSpeedToStop = data->m_floats->m_values[4];
    enableRaycast = (data->m_ints->m_values[0] != 0);
    raycastEdgeOffset = data->m_floats->m_values[5];
    raycastLength = data->m_floats->m_values[6];
    MinHoldPeriod = data->m_floats->m_values[7];
    MaxHoldPeriod = data->m_floats->m_values[8];
    NoHoldPeriod = data->m_floats->m_values[9];
    MaxHandsBehindBackPeriod = data->m_floats->m_values[10];
    MaxReachAttemptPeriod = data->m_floats->m_values[11];
    MinReachRecoveryPeriod = data->m_floats->m_values[12];
    CreateAtDistance = data->m_floats->m_values[13];
    EnableOrientationAtAngle = data->m_floats->m_values[14];
    DestroyAtDistance = data->m_floats->m_values[15];
    DisableOrientationAtAngle = data->m_floats->m_values[16];
    LockedLinearDofs = data->m_ints->m_values[1];
    LockedAngularDofs = data->m_ints->m_values[2];
    HoldOnContact = (data->m_ints->m_values[3] != 0);
    DisableCollisions = (data->m_ints->m_values[4] != 0);
    DisableAngularDofsUntilHanging = (data->m_ints->m_values[5] != 0);
    timeBeforeLookingDown = data->m_floats->m_values[17];
    ChestControlImminence = data->m_floats->m_values[18];
    ChestControlStiffnessScale = data->m_floats->m_values[19];
    ChestControlPassOnAmount = data->m_floats->m_values[20];
    MaxReachDistance = data->m_floats->m_values[21];
    MinEdgeLength = data->m_floats->m_values[22];
    MinAngleBetweenNormals = data->m_floats->m_values[23];
    MaxSlope = data->m_floats->m_values[24];
    MinMass = data->m_floats->m_values[25];
    MinVolume = data->m_floats->m_values[26];
    MinEdgeQuality = data->m_floats->m_values[27];
    ProjectEdgeNormalOntoGroundPlane = (data->m_ints->m_values[6] != 0);
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 4);
    NMP_ASSERT(data->m_ints->m_numValues == 2);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 0);

    m_cp.DoHold = (data->m_ints->m_values[0] != 0);
    m_cp.IgnoreOvershotEdges = (data->m_ints->m_values[1] != 0);
    m_cp.HoldImportance[0] = data->m_floats->m_values[0];
    m_cp.HoldImportance[1] = data->m_floats->m_values[1];
    m_cp.PullUpAmount = data->m_floats->m_values[2];
    m_cp.PullUpStrengthScale = data->m_floats->m_values[3];
  }


  /// Settings that govern when a reach happens

  /// If ground is detected below the character or below the edge (if edge raycast probing is enabled) it is not considered supportive (i.e. grabbing is allowed) if it is steeper than this (angle in degrees between "up" and ground normal).
  /// This ensures the grab is only disabled if the surface below the character or below the edge can support the character.
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

  /// The area underneath the edge to be grabbed can be probed to see if there is enough space to hang without unwanted obstructions. This is done using a raycast against the environment from a horizontal position offset from the edge (Edge offset) directed downwards.

  /// Enables raycast probing to see if there is enough space to hang without unwanted obstructions.
  bool getEnableRaycast() const { return enableRaycast; }
  /// Horizontal offset from the edge for the raycast origin. In metres (standard character).
  float getRaycastEdgeOffset() const { return raycastEdgeOffset; }
  /// The length of the raycast. In metres (standard character).
  float getRaycastLength() const { return raycastLength; }

  /// Settings that govern the time the character spends performing certain actions.

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

  /// The proportion of maximum hold time at which the character will switch from looking at the edge to looking down. This gives an impression of fatigue.
  float getTimeBeforeLookingDown() const { return timeBeforeLookingDown; }
  /// Used to control the chest (make it upright) by swinging/spinning the legs and arms. In units of 1/seconds (standard character)
  float getChestControlImminence() const { return ChestControlImminence; }
  /// Strength multiplier for swinging/spinning the legs and arms when controlling the chest
  float getChestControlStiffnessScale() const { return ChestControlStiffnessScale; }
  /// When controlling the chest, a small value confines the motions to the arms, a larger value results in leg motions
  float getChestControlPassOnAmount() const { return ChestControlPassOnAmount; }

  /// Settings that govern what the character will consider as an edge that can be grabbed.

  /// An edge will be considered only when closer to the character's chest than this distance. In metres (standard character).
  float getMaxReachDistance() const { return MaxReachDistance; }
  /// An edge will be considered only when longer than this. In metres (standard character).
  float getMinEdgeLength() const { return MinEdgeLength; }
  /// An edge will be considered only if the angle between its two faces (normals) is larger than this
  float getMinAngleBetweenNormals() const { return MinAngleBetweenNormals; }
  /// An edge will be considered only if the angle between the binormal (average of the surface normals either side of the edge) and up is less than this. In degrees.
  float getMaxSlope() const { return MaxSlope; }
  /// If an edge belongs to a dynamic or static object, it will only be considered if it is heavier than this. In kilograms (standard character).
  float getMinMass() const { return MinMass; }
  /// If an edge belongs to a dynamic object, it will only be considered if it is bigger than this. In cubic metres (standard character).
  float getMinVolume() const { return MinVolume; }
  /// An edge will be considered only if overall "grabbability" is greater than this
  float getMinEdgeQuality() const { return MinEdgeQuality; }

  /// Settings that govern edge normal.

  /// Project edge front surface normal (as opposed to the upper surface normal) into the ground plane.
  bool getProjectEdgeNormalOntoGroundPlane() const { return ProjectEdgeNormalOntoGroundPlane; }


  // control parameters
  /// Dynamically toggle Hold. Set it to false to make character release grabbed edge or to prevent character from grabbing any edges.
  bool getDoHoldCP() const { return m_cp.DoHold; }
  /// Prevents the character from grabbing an edge when its chest is likly to land on the horizontal surface above that edge.
  bool getIgnoreOvershotEdgesCP() const { return m_cp.IgnoreOvershotEdges; }
  /// Sets the importance of Hold relative to other behaviours for each arm.
  float getHoldImportanceCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.HoldImportance[index]; }
  enum { maxHoldImportanceCP = 2 };
  /// How high the character tries to pull himself up. 1: to chest level, 0: arms fully extended (but note that the arms may still have strength).
  float getPullUpAmountCP() const { return m_cp.PullUpAmount; }
  /// How strong the arms will try to pull up. This scales the normal strength is scaled by this factor. 0: no strength, 1: normal strength, 2: twice normal strength (super strong)
  float getPullUpStrengthScaleCP() const { return m_cp.PullUpStrengthScale; }

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// Number of hands which have successfully grabbed and held onto an edge.
  void setNumConstrainedHandsOCP(float value) { m_ocp.NumConstrainedHands = value; }
  /// Is the nth hand constrained.
  void setHandHoldingOCP(unsigned int index, bool value) { NMP_ASSERT(index < 2); m_ocp.HandHolding[index] = value; }
  enum { maxHandHoldingOCP = 2 };
  /// Indicates how much the behaviour is attempting to grab the edge (between 0 and 1).
  void setHoldAttemptImportanceOCP(float value) { m_ocp.HoldAttemptImportance = value; }
  /// Indicates how long the character has been constrained to an edge.
  void setHoldDurationOCP(float value) { m_ocp.HoldDuration = value; }
  /// The normal to the front surface of the edge (as opposed to the upper surface).
  void setEdgeForwardNormalOCP(const NMP::Vector3&  value) { m_ocp.EdgeForwardNormal = value; }
};


class HoldBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Hold"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 15; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class HoldBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    DoHold_CP_ID = 0,
    IgnoreOvershotEdges_CP_ID = 1,
    HoldImportance_0_CP_ID = 2,
    HoldImportance_1_CP_ID = 3,
    PullUpAmount_CP_ID = 4,
    PullUpStrengthScale_CP_ID = 5,
  };

  enum OutputControlParamIDs
  {
    NumConstrainedHands_OCP_ID = 0,
    HandHolding_0_OCP_ID = 1,
    HandHolding_1_OCP_ID = 2,
    HoldAttemptImportance_OCP_ID = 3,
    HoldDuration_OCP_ID = 4,
    EdgeForwardNormal_OCP_ID = 5,
  };

  const HoldBehaviourData& getParams() const { return m_params; }
  HoldBehaviourData& getParams() { return m_params; }
  HoldPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  HoldBehaviourData m_params;
  HoldPoseRequirements m_poseRequirements;
};

class HoldBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(HoldBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline HoldBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(HoldBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) HoldBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_HOLD_H

