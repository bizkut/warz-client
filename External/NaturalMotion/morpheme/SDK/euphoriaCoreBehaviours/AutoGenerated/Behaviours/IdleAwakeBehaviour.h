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

#ifndef NM_MDF_BDEF_IDLEAWAKE_H
#define NM_MDF_BDEF_IDLEAWAKE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/IdleAwake.behaviour"

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

struct IdleAwakePoseRequirements
{
  bool enablePoseInput_FallenPose;

  IdleAwakePoseRequirements()
  {
    enablePoseInput_FallenPose = true;
  }
};

struct IdleAwakeBehaviourData
{
  friend class IdleAwakeBehaviour;

private:


  struct ControlParams
  {
    float WholeBodyLook;  ///< Sets how much of the body to move when looking. The pin accepts a value in the range 0 to 1.
  };
  ControlParams m_cp;

  BodyPoseData  pd_FallenPose;
  float FwdRange;  ///< Amount the Center of Mass may drift forwards, distance in metres (standard character).
  float BackRange;  ///< Amount the Center of Mass may drift backwards, distance in metres (standard character).
  float LeftRange;  ///< Amount the Center of Mass may drift left, distance in metres (standard character).
  float RightRange;  ///< Amount the Center of Mass may drift right, distance in metres (standard character).
  float StanceChangeTime;  ///< How often to choose a new stance, in seconds (standard character).
  float PoseAdjustTime;  ///< Time period over which to adjust to the new stance, in seconds (standard character).
  float LookTimescale;  ///< Scales how long to look at each target, so smaller values will result in the character changing his look target more frequently.
  float LookRangeUpDown;  ///< Range of angles to look up/down (degrees)
  float LookRangeSideways;  ///< Range of positions to look at sideways (degrees)
  float LookStrengthWhenSupported;  ///< Look importance when balancing
  float LookStrengthWhenUnsupported;  ///< Look importance when fallen
  float LookTransitionTime;  ///< Controls how slowly/smoothly the character switches between look targets. In seconds (standard character).
  float LookFocusDistance;  ///< Distance at which to focus. Affects what is noticed when running autonomous behaviours in parallel. In metres (standard character).
  float LookVerticalOffset;  ///< Positive will look vertically higher, negative will look down. In metres (standard character).
  float FallenPose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float FallenPose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float FallenPose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float FallenPose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float FallenPose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float FallenPose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.


public:

  IdleAwakeBehaviourData()
  {
    FwdRange = float(0);
    BackRange = float(0);
    LeftRange = float(0);
    RightRange = float(0);
    StanceChangeTime = float(0);
    PoseAdjustTime = float(0);
    LookTimescale = float(1);
    LookRangeUpDown = float(30);
    LookRangeSideways = float(90);
    LookStrengthWhenSupported = float(0.5);
    LookStrengthWhenUnsupported = float(1);
    LookTransitionTime = float(0.5);
    LookFocusDistance = float(5);
    LookVerticalOffset = float(-0.8);
    FallenPose_ApplyToArm_1 = float(1);
    FallenPose_ApplyToArm_2 = float(1);
    FallenPose_ApplyToHead_1 = float(1);
    FallenPose_ApplyToLeg_1 = float(1);
    FallenPose_ApplyToLeg_2 = float(1);
    FallenPose_ApplyToSpine_1 = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 20);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    FwdRange = data->m_floats->m_values[0];
    BackRange = data->m_floats->m_values[1];
    LeftRange = data->m_floats->m_values[2];
    RightRange = data->m_floats->m_values[3];
    StanceChangeTime = data->m_floats->m_values[4];
    PoseAdjustTime = data->m_floats->m_values[5];
    LookTimescale = data->m_floats->m_values[6];
    LookRangeUpDown = data->m_floats->m_values[7];
    LookRangeSideways = data->m_floats->m_values[8];
    LookStrengthWhenSupported = data->m_floats->m_values[9];
    LookStrengthWhenUnsupported = data->m_floats->m_values[10];
    LookTransitionTime = data->m_floats->m_values[11];
    LookFocusDistance = data->m_floats->m_values[12];
    LookVerticalOffset = data->m_floats->m_values[13];
    FallenPose_ApplyToArm_1 = data->m_floats->m_values[14];
    FallenPose_ApplyToArm_2 = data->m_floats->m_values[15];
    FallenPose_ApplyToHead_1 = data->m_floats->m_values[16];
    FallenPose_ApplyToLeg_1 = data->m_floats->m_values[17];
    FallenPose_ApplyToLeg_2 = data->m_floats->m_values[18];
    FallenPose_ApplyToSpine_1 = data->m_floats->m_values[19];

    pd_FallenPose.m_poseWeights[0] = FallenPose_ApplyToArm_1;
    pd_FallenPose.m_poseWeights[1] = FallenPose_ApplyToArm_2;
    pd_FallenPose.m_poseWeights[2] = FallenPose_ApplyToHead_1;
    pd_FallenPose.m_poseWeights[3] = FallenPose_ApplyToLeg_1;
    pd_FallenPose.m_poseWeights[4] = FallenPose_ApplyToLeg_2;
    pd_FallenPose.m_poseWeights[5] = FallenPose_ApplyToSpine_1;
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 1);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 0);

    m_cp.WholeBodyLook = data->m_floats->m_values[0];
  }


  /// Settings that define how the balance pose varies over time.

  /// Amount the Center of Mass may drift forwards, distance in metres (standard character).
  float getFwdRange() const { return FwdRange; }
  /// Amount the Center of Mass may drift backwards, distance in metres (standard character).
  float getBackRange() const { return BackRange; }
  /// Amount the Center of Mass may drift left, distance in metres (standard character).
  float getLeftRange() const { return LeftRange; }
  /// Amount the Center of Mass may drift right, distance in metres (standard character).
  float getRightRange() const { return RightRange; }
  /// How often to choose a new stance, in seconds (standard character).
  float getStanceChangeTime() const { return StanceChangeTime; }
  /// Time period over which to adjust to the new stance, in seconds (standard character).
  float getPoseAdjustTime() const { return PoseAdjustTime; }

  /// Settings that define how the character changes its gaze over time.

  /// Scales how long to look at each target, so smaller values will result in the character changing his look target more frequently.
  float getLookTimescale() const { return LookTimescale; }
  /// Range of angles to look up/down (degrees)
  float getLookRangeUpDown() const { return LookRangeUpDown; }
  /// Range of positions to look at sideways (degrees)
  float getLookRangeSideways() const { return LookRangeSideways; }
  /// Look importance when balancing
  float getLookStrengthWhenSupported() const { return LookStrengthWhenSupported; }
  /// Look importance when fallen
  float getLookStrengthWhenUnsupported() const { return LookStrengthWhenUnsupported; }
  /// Controls how slowly/smoothly the character switches between look targets. In seconds (standard character).
  float getLookTransitionTime() const { return LookTransitionTime; }
  /// Distance at which to focus. Affects what is noticed when running autonomous behaviours in parallel. In metres (standard character).
  float getLookFocusDistance() const { return LookFocusDistance; }
  /// Positive will look vertically higher, negative will look down. In metres (standard character).
  float getLookVerticalOffset() const { return LookVerticalOffset; }


  /// Sets the animation pose that should be adopted by the character when unsupported and lying on its back.


  /// Sets the animation pose that should be adopted by the character when unsupported and lying on its back.
  const BodyPoseData& getFallenPose() const { return pd_FallenPose; }

  // control parameters
  /// Sets how much of the body to move when looking. The pin accepts a value in the range 0 to 1.
  float getWholeBodyLookCP() const { return m_cp.WholeBodyLook; }

  // output control parameters
};


class IdleAwakeBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "IdleAwake"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 17; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class IdleAwakeBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    FallenPose_ID = 0,
  };

  enum ControlParamIDs
  {
    WholeBodyLook_CP_ID = 0,
  };

  const IdleAwakeBehaviourData& getParams() const { return m_params; }
  IdleAwakeBehaviourData& getParams() { return m_params; }
  IdleAwakePoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  IdleAwakeBehaviourData m_params;
  IdleAwakePoseRequirements m_poseRequirements;
};

class IdleAwakeBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(IdleAwakeBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline IdleAwakeBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(IdleAwakeBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) IdleAwakeBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_IDLEAWAKE_H

