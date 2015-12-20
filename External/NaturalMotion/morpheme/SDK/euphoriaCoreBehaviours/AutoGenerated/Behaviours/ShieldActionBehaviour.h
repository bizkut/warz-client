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

#ifndef NM_MDF_BDEF_SHIELDACTION_H
#define NM_MDF_BDEF_SHIELDACTION_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ShieldAction.behaviour"

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

struct ShieldActionPoseRequirements
{
  bool enablePoseInput_ShieldPose;

  ShieldActionPoseRequirements()
  {
    enablePoseInput_ShieldPose = true;
  }
};

struct ShieldActionBehaviourData
{
  friend class ShieldActionBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 HazardPos;  ///< Position of the hazard to shield against in world space.
    float Importance;  ///< This determines whether shield happens or not. A value of 1 will force the character to shield.
    float TurnAwayScale;  ///< Scale used to set the amount of rotation to turn away from the hazard (0 - 1).
  };
  ControlParams m_cp;


  struct OutputControlParams
  {
    float hazardAngle;  ///< Angle in degrees towards the hazard. Zero means straight ahead, and +ve is to the right.
  };
  OutputControlParams m_ocp;

  BodyPoseData  pd_ShieldPose;
  float DefaultPitchAngle;  ///< Pitch of the spine when the character stands up (in degrees).
  float CrouchAmount;  ///< Level of crouching, larger values will crouch more when shielding
  float SmoothReturnTimePeriod;  ///< Minimum time period the character will continue to shield after shielding against a hazard. In seconds (standard character).
  float ShieldPose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float ShieldPose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float ShieldPose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float ShieldPose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float ShieldPose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float ShieldPose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.
  bool UseSingleFrameForShieldPose;  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  


public:

  ShieldActionBehaviourData()
  {
    DefaultPitchAngle = float(15);
    CrouchAmount = float(0.2);
    UseSingleFrameForShieldPose = false;
    SmoothReturnTimePeriod = float(1);
    ShieldPose_ApplyToArm_1 = float(1);
    ShieldPose_ApplyToArm_2 = float(1);
    ShieldPose_ApplyToHead_1 = float(1);
    ShieldPose_ApplyToLeg_1 = float(1);
    ShieldPose_ApplyToLeg_2 = float(1);
    ShieldPose_ApplyToSpine_1 = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 9);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    DefaultPitchAngle = data->m_floats->m_values[0];
    CrouchAmount = data->m_floats->m_values[1];
    UseSingleFrameForShieldPose = (data->m_ints->m_values[0] != 0);
    SmoothReturnTimePeriod = data->m_floats->m_values[2];
    ShieldPose_ApplyToArm_1 = data->m_floats->m_values[3];
    ShieldPose_ApplyToArm_2 = data->m_floats->m_values[4];
    ShieldPose_ApplyToHead_1 = data->m_floats->m_values[5];
    ShieldPose_ApplyToLeg_1 = data->m_floats->m_values[6];
    ShieldPose_ApplyToLeg_2 = data->m_floats->m_values[7];
    ShieldPose_ApplyToSpine_1 = data->m_floats->m_values[8];

    pd_ShieldPose.m_poseWeights[0] = ShieldPose_ApplyToArm_1;
    pd_ShieldPose.m_poseWeights[1] = ShieldPose_ApplyToArm_2;
    pd_ShieldPose.m_poseWeights[2] = ShieldPose_ApplyToHead_1;
    pd_ShieldPose.m_poseWeights[3] = ShieldPose_ApplyToLeg_1;
    pd_ShieldPose.m_poseWeights[4] = ShieldPose_ApplyToLeg_2;
    pd_ShieldPose.m_poseWeights[5] = ShieldPose_ApplyToSpine_1;
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 2);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 4);

    m_cp.Importance = data->m_floats->m_values[0];
    m_cp.HazardPos.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.TurnAwayScale = data->m_floats->m_values[1];
  }


  /// Pitch of the spine when the character stands up (in degrees).
  float getDefaultPitchAngle() const { return DefaultPitchAngle; }
  /// Level of crouching, larger values will crouch more when shielding
  float getCrouchAmount() const { return CrouchAmount; }

  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  bool getUseSingleFrameForShieldPose() const { return UseSingleFrameForShieldPose; }

  /// Minimum time period the character will continue to shield after shielding against a hazard. In seconds (standard character).
  float getSmoothReturnTimePeriod() const { return SmoothReturnTimePeriod; }


  /// The basic shield pose itself. If the character is supported,the shield pose will only be used for his heads and arms. The character will rotate with his spine to turn away from the hazard. If he is not supported, the poses are applied for the all body.


  /// The basic shield pose itself. If the character is supported,the shield pose will only be used for his heads and arms. The character will rotate with his spine to turn away from the hazard. If he is not supported, the poses are applied for the all body.
  const BodyPoseData& getShieldPose() const { return pd_ShieldPose; }

  // control parameters
  /// This determines whether shield happens or not. A value of 1 will force the character to shield.
  float getImportanceCP() const { return m_cp.Importance; }
  /// Position of the hazard to shield against in world space.
  const NMP::Vector3& getHazardPosCP() const { return m_cp.HazardPos; }
  /// Scale used to set the amount of rotation to turn away from the hazard (0 - 1).
  float getTurnAwayScaleCP() const { return m_cp.TurnAwayScale; }

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// Angle in degrees towards the hazard. Zero means straight ahead, and +ve is to the right.
  void setHazardAngleOCP(float value) { m_ocp.hazardAngle = value; }
};


class ShieldActionBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "ShieldAction"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 26; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class ShieldActionBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    ShieldPose_ID = 0,
  };

  enum ControlParamIDs
  {
    Importance_CP_ID = 0,
    HazardPos_CP_ID = 1,
    TurnAwayScale_CP_ID = 2,
  };

  enum OutputControlParamIDs
  {
    hazardAngle_OCP_ID = 0,
  };

  const ShieldActionBehaviourData& getParams() const { return m_params; }
  ShieldActionBehaviourData& getParams() { return m_params; }
  ShieldActionPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ShieldActionBehaviourData m_params;
  ShieldActionPoseRequirements m_poseRequirements;
};

class ShieldActionBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ShieldActionBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ShieldActionBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ShieldActionBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ShieldActionBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_SHIELDACTION_H

