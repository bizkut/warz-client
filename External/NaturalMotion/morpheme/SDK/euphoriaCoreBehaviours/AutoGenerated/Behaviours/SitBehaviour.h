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

#ifndef NM_MDF_BDEF_SIT_H
#define NM_MDF_BDEF_SIT_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Sit.behaviour"

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

struct SitPoseRequirements
{
  bool enablePoseInput_Pose;

  SitPoseRequirements()
  {
    enablePoseInput_Pose = true;
  }
};

struct SitBehaviourData
{
  friend class SitBehaviour;

private:


  struct OutputControlParams
  {
    float SitAmount;  ///< An indicator of how well the character is sitting from 0 to 1.
  };
  OutputControlParams m_ocp;

  BodyPoseData  pd_Pose;
  float MinStandingBalanceAmount;  /// Only attempt to sit when the standing balance amount is below this threshold.
  
  float MinSitAmount;  /// Don't try to maintain sitting balance when sitting amount is below this threshold.
  
  float ArmStepHeight;  /// The maximum height the hands will be raised to when stepping. Increase this to lift the hands further from the ground when stepping. In metres (standard character).
  
  float ArmStepTargetExtrapolationTime;  /// How far into the future should the chest's position be extrapolated to determine the hand placement target position. Increase this to move the hands further from the character when stepping. In seconds (standard character).
  
  float ArmStepTargetSeparationFromBody;  /// Arm target is placed at least this far away from the characters body, as defined by the self avoidance radius. Increase this to move the hands further from the body when the character is stepping. In metres (standard character).
  
  float Pose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float Pose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float Pose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float Pose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float Pose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float Pose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.
  bool SupportWithArms;  ///< The arms will be used to help maintain a sitting position if this attribute is true.
  bool UseSingleFrameForPose;


public:

  SitBehaviourData()
  {
    SupportWithArms = true;
    MinStandingBalanceAmount = float(0.5);
    MinSitAmount = float(0.1);
    ArmStepHeight = float(0.3);
    ArmStepTargetExtrapolationTime = float(0.2);
    ArmStepTargetSeparationFromBody = float(0.2);
    UseSingleFrameForPose = false;
    Pose_ApplyToArm_1 = float(1);
    Pose_ApplyToArm_2 = float(1);
    Pose_ApplyToHead_1 = float(1);
    Pose_ApplyToLeg_1 = float(1);
    Pose_ApplyToLeg_2 = float(1);
    Pose_ApplyToSpine_1 = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 11);
    NMP_ASSERT(data->m_ints->m_numValues == 2);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    SupportWithArms = (data->m_ints->m_values[0] != 0);
    MinStandingBalanceAmount = data->m_floats->m_values[0];
    MinSitAmount = data->m_floats->m_values[1];
    ArmStepHeight = data->m_floats->m_values[2];
    ArmStepTargetExtrapolationTime = data->m_floats->m_values[3];
    ArmStepTargetSeparationFromBody = data->m_floats->m_values[4];
    UseSingleFrameForPose = (data->m_ints->m_values[1] != 0);
    Pose_ApplyToArm_1 = data->m_floats->m_values[5];
    Pose_ApplyToArm_2 = data->m_floats->m_values[6];
    Pose_ApplyToHead_1 = data->m_floats->m_values[7];
    Pose_ApplyToLeg_1 = data->m_floats->m_values[8];
    Pose_ApplyToLeg_2 = data->m_floats->m_values[9];
    Pose_ApplyToSpine_1 = data->m_floats->m_values[10];

    pd_Pose.m_poseWeights[0] = Pose_ApplyToArm_1;
    pd_Pose.m_poseWeights[1] = Pose_ApplyToArm_2;
    pd_Pose.m_poseWeights[2] = Pose_ApplyToHead_1;
    pd_Pose.m_poseWeights[3] = Pose_ApplyToLeg_1;
    pd_Pose.m_poseWeights[4] = Pose_ApplyToLeg_2;
    pd_Pose.m_poseWeights[5] = Pose_ApplyToSpine_1;
  }


  /// The arms will be used to help maintain a sitting position if this attribute is true.
  bool getSupportWithArms() const { return SupportWithArms; }

  /// Only attempt to sit when the standing balance amount is below this threshold.
  float getMinStandingBalanceAmount() const { return MinStandingBalanceAmount; }
  /// Don't try to maintain sitting balance when sitting amount is below this threshold.
  float getMinSitAmount() const { return MinSitAmount; }

  /// Configure how the behaviour repositions the hands when attempting to use them for support whilst sitting.

  /// The maximum height the hands will be raised to when stepping. Increase this to lift the hands further from the ground when stepping. In metres (standard character).
  float getArmStepHeight() const { return ArmStepHeight; }
  /// How far into the future should the chest's position be extrapolated to determine the hand placement target position. Increase this to move the hands further from the character when stepping. In seconds (standard character).
  float getArmStepTargetExtrapolationTime() const { return ArmStepTargetExtrapolationTime; }
  /// Arm target is placed at least this far away from the characters body, as defined by the self avoidance radius. Increase this to move the hands further from the body when the character is stepping. In metres (standard character).
  float getArmStepTargetSeparationFromBody() const { return ArmStepTargetSeparationFromBody; }

  /// Target pose when sitting

  bool getUseSingleFrameForPose() const { return UseSingleFrameForPose; }

  /// Target pose when sitting
  const BodyPoseData& getPose() const { return pd_Pose; }

  // control parameters

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// An indicator of how well the character is sitting from 0 to 1.
  void setSitAmountOCP(float value) { m_ocp.SitAmount = value; }
};


class SitBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Sit"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 27; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class SitBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    Pose_ID = 0,
  };

  enum OutputControlParamIDs
  {
    SitAmount_OCP_ID = 0,
  };

  const SitBehaviourData& getParams() const { return m_params; }
  SitBehaviourData& getParams() { return m_params; }
  SitPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  SitBehaviourData m_params;
  SitPoseRequirements m_poseRequirements;
};

class SitBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(SitBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline SitBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(SitBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) SitBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_SIT_H

