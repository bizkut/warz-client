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

#ifndef NM_MDF_BDEF_ANIMATE_H
#define NM_MDF_BDEF_ANIMATE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Animate.behaviour"

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

struct AnimatePoseRequirements
{
  bool enablePoseInput_BodyPose;

  AnimatePoseRequirements()
  {
    enablePoseInput_BodyPose = true;
  }
};

struct AnimateBehaviourData
{
  friend class AnimateBehaviour;

private:


  struct ControlParams
  {
    float ArmStrength[NetworkConstants::networkMaxNumArms];  ///< Scales the strength and weights the arm animation against other arm behaviours.
    float HeadStrength[NetworkConstants::networkMaxNumHeads];  ///< Scales the strength and weights the head animation against other head behaviours.
    float LegStrength[NetworkConstants::networkMaxNumLegs];  ///< Scales the strength and weights the leg animation against other leg behaviours.
    float SpineStrength[NetworkConstants::networkMaxNumSpines];  ///< Scales the strength and weights the spine animation against other spine behaviours.
  };
  ControlParams m_cp;

  BodyPoseData  pd_BodyPose;
  float ArmGravityCompensation[NetworkConstants::networkMaxNumArms];  ///< Gravity compentstation scale for the arm.
  float HeadGravityCompensation[NetworkConstants::networkMaxNumHeads];  ///< Gravity compentstation scale for the head.
  float LegGravityCompensation[NetworkConstants::networkMaxNumLegs];  ///< Gravity compentstation scale for the leg.
  float SpineGravityCompensation[NetworkConstants::networkMaxNumSpines];  ///< Gravity compentstation scale for the spine.
  float BodyPose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float BodyPose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float BodyPose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float BodyPose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float BodyPose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float BodyPose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.
  bool UseSingleFrameForPose;  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  
  bool ArmsPriority;  ///< If set then the animate behaviour will have priority over autonomous behaviours for all the arms
  bool HeadsPriority;  ///< If set then the animate behaviour will have priority over autonomous behaviours for all the heads
  bool LegsPriority;  ///< If set then the animate behaviour will have priority over autonomous behaviours for all the legs
  bool SpinesPriority;  ///< If set then the animate behaviour will have priority over autonomous behaviours for the spine
  bool DisableSleeping;  /// False allows character to sleep when velocity is below a threshold, for performance. Only set to true if the animation won't change after becoming stationary, since
  /// a sleeping character will not auto-awaken when animating
  


public:

  AnimateBehaviourData()
  {
    UseSingleFrameForPose = false;
    ArmsPriority = true;
    HeadsPriority = true;
    LegsPriority = true;
    SpinesPriority = true;
    DisableSleeping = true;
    ArmGravityCompensation[0] = float(0);
    ArmGravityCompensation[1] = float(0);
    HeadGravityCompensation[0] = float(0);
    LegGravityCompensation[0] = float(0);
    LegGravityCompensation[1] = float(0);
    SpineGravityCompensation[0] = float(0);
    BodyPose_ApplyToArm_1 = float(1);
    BodyPose_ApplyToArm_2 = float(1);
    BodyPose_ApplyToHead_1 = float(1);
    BodyPose_ApplyToLeg_1 = float(1);
    BodyPose_ApplyToLeg_2 = float(1);
    BodyPose_ApplyToSpine_1 = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 12);
    NMP_ASSERT(data->m_ints->m_numValues == 6);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    UseSingleFrameForPose = (data->m_ints->m_values[0] != 0);
    ArmsPriority = (data->m_ints->m_values[1] != 0);
    HeadsPriority = (data->m_ints->m_values[2] != 0);
    LegsPriority = (data->m_ints->m_values[3] != 0);
    SpinesPriority = (data->m_ints->m_values[4] != 0);
    DisableSleeping = (data->m_ints->m_values[5] != 0);
    ArmGravityCompensation[0] = data->m_floats->m_values[0];
    ArmGravityCompensation[1] = data->m_floats->m_values[1];
    HeadGravityCompensation[0] = data->m_floats->m_values[2];
    LegGravityCompensation[0] = data->m_floats->m_values[3];
    LegGravityCompensation[1] = data->m_floats->m_values[4];
    SpineGravityCompensation[0] = data->m_floats->m_values[5];
    BodyPose_ApplyToArm_1 = data->m_floats->m_values[6];
    BodyPose_ApplyToArm_2 = data->m_floats->m_values[7];
    BodyPose_ApplyToHead_1 = data->m_floats->m_values[8];
    BodyPose_ApplyToLeg_1 = data->m_floats->m_values[9];
    BodyPose_ApplyToLeg_2 = data->m_floats->m_values[10];
    BodyPose_ApplyToSpine_1 = data->m_floats->m_values[11];

    pd_BodyPose.m_poseWeights[0] = BodyPose_ApplyToArm_1;
    pd_BodyPose.m_poseWeights[1] = BodyPose_ApplyToArm_2;
    pd_BodyPose.m_poseWeights[2] = BodyPose_ApplyToHead_1;
    pd_BodyPose.m_poseWeights[3] = BodyPose_ApplyToLeg_1;
    pd_BodyPose.m_poseWeights[4] = BodyPose_ApplyToLeg_2;
    pd_BodyPose.m_poseWeights[5] = BodyPose_ApplyToSpine_1;
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 6);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 0);

    m_cp.ArmStrength[0] = data->m_floats->m_values[0];
    m_cp.ArmStrength[1] = data->m_floats->m_values[1];
    m_cp.HeadStrength[0] = data->m_floats->m_values[2];
    m_cp.LegStrength[0] = data->m_floats->m_values[3];
    m_cp.LegStrength[1] = data->m_floats->m_values[4];
    m_cp.SpineStrength[0] = data->m_floats->m_values[5];
  }


  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  bool getUseSingleFrameForPose() const { return UseSingleFrameForPose; }

  /// These determine whether the animate behaviour will have priority over autonomous behaviours, or if it will operate in the background and be easily over-ridden by other behaviours when they need to be active.

  /// If set then the animate behaviour will have priority over autonomous behaviours for all the arms
  bool getArmsPriority() const { return ArmsPriority; }
  /// If set then the animate behaviour will have priority over autonomous behaviours for all the heads
  bool getHeadsPriority() const { return HeadsPriority; }
  /// If set then the animate behaviour will have priority over autonomous behaviours for all the legs
  bool getLegsPriority() const { return LegsPriority; }
  /// If set then the animate behaviour will have priority over autonomous behaviours for the spine
  bool getSpinesPriority() const { return SpinesPriority; }

  /// False allows character to sleep when velocity is below a threshold, for performance. Only set to true if the animation won't change after becoming stationary, since
  /// a sleeping character will not auto-awaken when animating
  bool getDisableSleeping() const { return DisableSleeping; }

  /// Gravity compensation is used to compensate for the sag of limbs due to gravity. In order for this to operate the limb needs to be supported at its root or end. This can happen if, for example, balance or hard keyframing is used on the lower body, and the animate behaviour is used on the upper body. Gravity compensation values above one should only normally be used if you want full gravity compensation with a limb that has got strength less than one.

  /// Gravity compentstation scale for the arm.
  float getArmGravityCompensation(unsigned int index) const { NMP_ASSERT(index < 2); return ArmGravityCompensation[index]; }
  enum { maxArmGravityCompensation = 2 };
  /// Gravity compentstation scale for the head.
  float getHeadGravityCompensation(unsigned int index) const { NMP_ASSERT(index < 1); return HeadGravityCompensation[index]; }
  enum { maxHeadGravityCompensation = 1 };
  /// Gravity compentstation scale for the leg.
  float getLegGravityCompensation(unsigned int index) const { NMP_ASSERT(index < 2); return LegGravityCompensation[index]; }
  enum { maxLegGravityCompensation = 2 };
  /// Gravity compentstation scale for the spine.
  float getSpineGravityCompensation(unsigned int index) const { NMP_ASSERT(index < 1); return SpineGravityCompensation[index]; }
  enum { maxSpineGravityCompensation = 1 };

  // These values act to scale both the physical strength, and the importance/weight of the animate behaviour against other behaviours that may be trying to use the limb. For example, a small (non zero) value on the arms will allow the arms to follow the input animation loosely, with influences form other behaviours.


  // Note that the body pose weight attributes are ignored and should not be displayed


  // Note that the body pose weight attributes are ignored and should not be displayed
  const BodyPoseData& getBodyPose() const { return pd_BodyPose; }

  // control parameters
  // These values act to scale both the physical strength, and the importance/weight of the animate behaviour against other behaviours that may be trying to use the limb. For example, a small (non zero) value on the arms will allow the arms to follow the input animation loosely, with influences form other behaviours.

  /// Scales the strength and weights the arm animation against other arm behaviours.
  float getArmStrengthCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmStrength[index]; }
  enum { maxArmStrengthCP = 2 };
  /// Scales the strength and weights the head animation against other head behaviours.
  float getHeadStrengthCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadStrength[index]; }
  enum { maxHeadStrengthCP = 1 };
  /// Scales the strength and weights the leg animation against other leg behaviours.
  float getLegStrengthCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LegStrength[index]; }
  enum { maxLegStrengthCP = 2 };
  /// Scales the strength and weights the spine animation against other spine behaviours.
  float getSpineStrengthCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineStrength[index]; }
  enum { maxSpineStrengthCP = 1 };

  // output control parameters
};


class AnimateBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Animate"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 2; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class AnimateBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    BodyPose_ID = 0,
  };

  enum ControlParamIDs
  {
    ArmStrength_0_CP_ID = 0,
    ArmStrength_1_CP_ID = 1,
    HeadStrength_0_CP_ID = 2,
    LegStrength_0_CP_ID = 3,
    LegStrength_1_CP_ID = 4,
    SpineStrength_0_CP_ID = 5,
  };

  const AnimateBehaviourData& getParams() const { return m_params; }
  AnimateBehaviourData& getParams() { return m_params; }
  AnimatePoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  AnimateBehaviourData m_params;
  AnimatePoseRequirements m_poseRequirements;
};

class AnimateBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(AnimateBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline AnimateBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(AnimateBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) AnimateBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_ANIMATE_H

