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

#ifndef NM_MDF_BDEF_SHIELD_H
#define NM_MDF_BDEF_SHIELD_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Shield.behaviour"

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

struct ShieldPoseRequirements
{
  bool enablePoseInput_ShieldPose;

  ShieldPoseRequirements()
  {
    enablePoseInput_ShieldPose = true;
  }
};

struct ShieldBehaviourData
{
  friend class ShieldBehaviour;

private:


  struct ControlParams
  {
    float TurnAwayScale;  ///< Sets the amount the character can turn away from a hazard. Accepts values in the range 0 to 1.
  };
  ControlParams m_cp;


  struct OutputControlParams
  {
    float isShielding;  ///< 1 if we are shielding, 0 otherwise
    float hazardAngle;  ///< Angle in degrees towards the hazard. Zero means straight ahead, and +ve is to the right.
  };
  OutputControlParams m_ocp;

  BodyPoseData  pd_ShieldPose;
  float ImpactResponseForShield;  ///<Shield will be triggered if the character will change speed by more than this on impact, based on the relative masses. In m/s (standard character).
  float AngularSpeedForShield;  ///< The character will shield if he predicts an impact with a hazard and he is rotating faster than this. In revolutions/second (standard character).
  float TangentialSpeedForShield;  ///< The character will shield if his predicted tangential velocity at the time of impact with a hazard is greater than this. In m/s (standard character).
  float SmoothReturnTimePeriod;  ///< Time period the character will need to completely stop shield. In seconds (standard character).
  float DefaultPitchAngle;  ///< Pitch of the spine when the character stands up (in degrees).
  float crouchAmount;  ///< Level of crouching, larger values will crouch more when shielding
  float ShieldPose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float ShieldPose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float ShieldPose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float ShieldPose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float ShieldPose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float ShieldPose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.
  bool UseSingleFrameForShieldPoses;  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  


public:

  ShieldBehaviourData()
  {
    ImpactResponseForShield = float(7);
    AngularSpeedForShield = float(1.5);
    TangentialSpeedForShield = float(6);
    SmoothReturnTimePeriod = float(1);
    DefaultPitchAngle = float(15);
    crouchAmount = float(0.2);
    UseSingleFrameForShieldPoses = false;
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
    NMP_ASSERT(data->m_floats->m_numValues == 12);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    ImpactResponseForShield = data->m_floats->m_values[0];
    AngularSpeedForShield = data->m_floats->m_values[1];
    TangentialSpeedForShield = data->m_floats->m_values[2];
    SmoothReturnTimePeriod = data->m_floats->m_values[3];
    DefaultPitchAngle = data->m_floats->m_values[4];
    crouchAmount = data->m_floats->m_values[5];
    UseSingleFrameForShieldPoses = (data->m_ints->m_values[0] != 0);
    ShieldPose_ApplyToArm_1 = data->m_floats->m_values[6];
    ShieldPose_ApplyToArm_2 = data->m_floats->m_values[7];
    ShieldPose_ApplyToHead_1 = data->m_floats->m_values[8];
    ShieldPose_ApplyToLeg_1 = data->m_floats->m_values[9];
    ShieldPose_ApplyToLeg_2 = data->m_floats->m_values[10];
    ShieldPose_ApplyToSpine_1 = data->m_floats->m_values[11];

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
    NMP_ASSERT(data->m_floats->m_numValues == 1);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 0);

    m_cp.TurnAwayScale = data->m_floats->m_values[0];
  }


  ///Shield will be triggered if the character will change speed by more than this on impact, based on the relative masses. In m/s (standard character).
  float getImpactResponseForShield() const { return ImpactResponseForShield; }
  /// The character will shield if he predicts an impact with a hazard and he is rotating faster than this. In revolutions/second (standard character).
  float getAngularSpeedForShield() const { return AngularSpeedForShield; }
  /// The character will shield if his predicted tangential velocity at the time of impact with a hazard is greater than this. In m/s (standard character).
  float getTangentialSpeedForShield() const { return TangentialSpeedForShield; }
  /// Time period the character will need to completely stop shield. In seconds (standard character).
  float getSmoothReturnTimePeriod() const { return SmoothReturnTimePeriod; }

  /// Pitch of the spine when the character stands up (in degrees).
  float getDefaultPitchAngle() const { return DefaultPitchAngle; }
  /// Level of crouching, larger values will crouch more when shielding
  float getCrouchAmount() const { return crouchAmount; }

  /// If set then pose will be cached on the first update. This means that the pose that is used will not change if the input changes.
  bool getUseSingleFrameForShieldPoses() const { return UseSingleFrameForShieldPoses; }


  /// The basic shield pose itself.


  /// The basic shield pose itself.
  const BodyPoseData& getShieldPose() const { return pd_ShieldPose; }

  // control parameters
  /// Sets the amount the character can turn away from a hazard. Accepts values in the range 0 to 1.
  float getTurnAwayScaleCP() const { return m_cp.TurnAwayScale; }

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// 1 if we are shielding, 0 otherwise
  void setIsShieldingOCP(float value) { m_ocp.isShielding = value; }
  /// Angle in degrees towards the hazard. Zero means straight ahead, and +ve is to the right.
  void setHazardAngleOCP(float value) { m_ocp.hazardAngle = value; }
};


class ShieldBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Shield"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 25; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class ShieldBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    ShieldPose_ID = 0,
  };

  enum ControlParamIDs
  {
    TurnAwayScale_CP_ID = 0,
  };

  enum OutputControlParamIDs
  {
    isShielding_OCP_ID = 0,
    hazardAngle_OCP_ID = 1,
  };

  const ShieldBehaviourData& getParams() const { return m_params; }
  ShieldBehaviourData& getParams() { return m_params; }
  ShieldPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ShieldBehaviourData m_params;
  ShieldPoseRequirements m_poseRequirements;
};

class ShieldBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ShieldBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ShieldBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ShieldBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ShieldBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_SHIELD_H

