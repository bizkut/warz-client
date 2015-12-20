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

#ifndef NM_MDF_BDEF_WRITHE_H
#define NM_MDF_BDEF_WRITHE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Writhe.behaviour"

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

struct WrithePoseRequirements
{
  bool enablePoseInput_DefaultPose;

  WrithePoseRequirements()
  {
    enablePoseInput_DefaultPose = true;
  }
};

struct WritheBehaviourData
{
  friend class WritheBehaviour;

private:


  struct ControlParams
  {
    float ImportanceArm[NetworkConstants::networkMaxNumArms];  ///< Sets the strength of the control this behaviour has over the arms, scaling the generated movement.
    float ImportanceHead[NetworkConstants::networkMaxNumHeads];  ///< Sets the strength of the control this behaviour has over the heads, scaling the generated movement.
    float ImportanceLeg[NetworkConstants::networkMaxNumLegs];  ///< Sets the strength of the control this behaviour has over the legs, scaling the generated movement.
    float ImportanceSpine[NetworkConstants::networkMaxNumSpines];  ///< Sets the strength of the control this behaviour has over the spines, scaling the generated movement.
  };
  ControlParams m_cp;

  BodyPoseData  pd_DefaultPose;
  float ArmsDriveCompensationScale[NetworkConstants::networkMaxNumArms];  ///< A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations.
  float ArmsAmplitudeScale[NetworkConstants::networkMaxNumArms];  ///< Scales the length of the randomly generated displacement of the arm position.
  float ArmsFrequency[NetworkConstants::networkMaxNumArms];  ///< How often the randomly generated arm position changes. In Hertz on the standard character.
  float HeadsDriveCompensationScale[NetworkConstants::networkMaxNumHeads];  ///< A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations.
  float HeadsAmplitudeScale[NetworkConstants::networkMaxNumHeads];  ///< Scales the length of the randomly generated displacement of the head position.
  float HeadsFrequency[NetworkConstants::networkMaxNumHeads];  ///< How often the randomly generated head position changes. In Hertz on the standard character.
  float LegsDriveCompensationScale[NetworkConstants::networkMaxNumLegs];  ///< A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations.
  float LegsAmplitudeScale[NetworkConstants::networkMaxNumLegs];  ///< Scales the length of the randomly generated displacement of the leg position.
  float LegsFrequency[NetworkConstants::networkMaxNumLegs];  ///< How often the randomly generated leg position changes. In Hertz on the standard character.
  float SpinesDriveCompensationScale[NetworkConstants::networkMaxNumSpines];  ///< A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations.
  float SpinesAmplitudeScale[NetworkConstants::networkMaxNumSpines];  ///< Scales the length of the randomly generated displacement of the spine position.
  float SpinesFrequency[NetworkConstants::networkMaxNumSpines];  ///< How often the randomly generated spine position changes. In Hertz on the standard character.
  float DefaultPose_ApplyToArm_1;  // Set how much of the pose to use on this Arm.
  float DefaultPose_ApplyToArm_2;  // Set how much of the pose to use on this Arm.
  float DefaultPose_ApplyToHead_1;  // Set how much of the pose to use on this Head.
  float DefaultPose_ApplyToLeg_1;  // Set how much of the pose to use on this Leg.
  float DefaultPose_ApplyToLeg_2;  // Set how much of the pose to use on this Leg.
  float DefaultPose_ApplyToSpine_1;  // Set how much of the pose to use on this Spine.
  bool ArmsBasedOnDefaultPose[NetworkConstants::networkMaxNumArms];  ///< When enabled, the arm is randomly displaced from the position in the pose. If disabled, the random displacement is added to the current position.
  bool HeadsBasedOnDefaultPose[NetworkConstants::networkMaxNumHeads];  ///< When enabled, the head is randomly displaced from the position in the pose. If disabled, the random displacement is added to the current position.
  bool LegsBasedOnDefaultPose[NetworkConstants::networkMaxNumLegs];  ///< When enabled, the leg is randomly displaced from the position in the pose. If disabled, the random displacement is added to the current position.
  bool SpinesBasedOnDefaultPose[NetworkConstants::networkMaxNumSpines];  ///< When enabled, the spine is randomly displaced from the position in the pose. If disabled, the random displacement is added to the current position.


public:

  WritheBehaviourData()
  {
    ArmsDriveCompensationScale[0] = float(1);
    ArmsDriveCompensationScale[1] = float(1);
    ArmsAmplitudeScale[0] = float(0.75);
    ArmsAmplitudeScale[1] = float(0.75);
    ArmsFrequency[0] = float(15);
    ArmsFrequency[1] = float(15);
    ArmsBasedOnDefaultPose[0] = true;
    ArmsBasedOnDefaultPose[1] = true;
    HeadsDriveCompensationScale[0] = float(1);
    HeadsAmplitudeScale[0] = float(0.75);
    HeadsFrequency[0] = float(15);
    HeadsBasedOnDefaultPose[0] = true;
    LegsDriveCompensationScale[0] = float(1);
    LegsDriveCompensationScale[1] = float(1);
    LegsAmplitudeScale[0] = float(0.75);
    LegsAmplitudeScale[1] = float(0.75);
    LegsFrequency[0] = float(10);
    LegsFrequency[1] = float(10);
    LegsBasedOnDefaultPose[0] = true;
    LegsBasedOnDefaultPose[1] = true;
    SpinesDriveCompensationScale[0] = float(1);
    SpinesAmplitudeScale[0] = float(0.75);
    SpinesFrequency[0] = float(15);
    SpinesBasedOnDefaultPose[0] = true;
    DefaultPose_ApplyToArm_1 = float(1);
    DefaultPose_ApplyToArm_2 = float(1);
    DefaultPose_ApplyToHead_1 = float(1);
    DefaultPose_ApplyToLeg_1 = float(1);
    DefaultPose_ApplyToLeg_2 = float(1);
    DefaultPose_ApplyToSpine_1 = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 24);
    NMP_ASSERT(data->m_ints->m_numValues == 6);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    ArmsDriveCompensationScale[0] = data->m_floats->m_values[0];
    ArmsDriveCompensationScale[1] = data->m_floats->m_values[1];
    ArmsAmplitudeScale[0] = data->m_floats->m_values[2];
    ArmsAmplitudeScale[1] = data->m_floats->m_values[3];
    ArmsFrequency[0] = data->m_floats->m_values[4];
    ArmsFrequency[1] = data->m_floats->m_values[5];
    ArmsBasedOnDefaultPose[0] = (data->m_ints->m_values[0] != 0);
    ArmsBasedOnDefaultPose[1] = (data->m_ints->m_values[1] != 0);
    HeadsDriveCompensationScale[0] = data->m_floats->m_values[6];
    HeadsAmplitudeScale[0] = data->m_floats->m_values[7];
    HeadsFrequency[0] = data->m_floats->m_values[8];
    HeadsBasedOnDefaultPose[0] = (data->m_ints->m_values[2] != 0);
    LegsDriveCompensationScale[0] = data->m_floats->m_values[9];
    LegsDriveCompensationScale[1] = data->m_floats->m_values[10];
    LegsAmplitudeScale[0] = data->m_floats->m_values[11];
    LegsAmplitudeScale[1] = data->m_floats->m_values[12];
    LegsFrequency[0] = data->m_floats->m_values[13];
    LegsFrequency[1] = data->m_floats->m_values[14];
    LegsBasedOnDefaultPose[0] = (data->m_ints->m_values[3] != 0);
    LegsBasedOnDefaultPose[1] = (data->m_ints->m_values[4] != 0);
    SpinesDriveCompensationScale[0] = data->m_floats->m_values[15];
    SpinesAmplitudeScale[0] = data->m_floats->m_values[16];
    SpinesFrequency[0] = data->m_floats->m_values[17];
    SpinesBasedOnDefaultPose[0] = (data->m_ints->m_values[5] != 0);
    DefaultPose_ApplyToArm_1 = data->m_floats->m_values[18];
    DefaultPose_ApplyToArm_2 = data->m_floats->m_values[19];
    DefaultPose_ApplyToHead_1 = data->m_floats->m_values[20];
    DefaultPose_ApplyToLeg_1 = data->m_floats->m_values[21];
    DefaultPose_ApplyToLeg_2 = data->m_floats->m_values[22];
    DefaultPose_ApplyToSpine_1 = data->m_floats->m_values[23];

    pd_DefaultPose.m_poseWeights[0] = DefaultPose_ApplyToArm_1;
    pd_DefaultPose.m_poseWeights[1] = DefaultPose_ApplyToArm_2;
    pd_DefaultPose.m_poseWeights[2] = DefaultPose_ApplyToHead_1;
    pd_DefaultPose.m_poseWeights[3] = DefaultPose_ApplyToLeg_1;
    pd_DefaultPose.m_poseWeights[4] = DefaultPose_ApplyToLeg_2;
    pd_DefaultPose.m_poseWeights[5] = DefaultPose_ApplyToSpine_1;
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 6);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 0);

    m_cp.ImportanceArm[0] = data->m_floats->m_values[0];
    m_cp.ImportanceArm[1] = data->m_floats->m_values[1];
    m_cp.ImportanceHead[0] = data->m_floats->m_values[2];
    m_cp.ImportanceLeg[0] = data->m_floats->m_values[3];
    m_cp.ImportanceLeg[1] = data->m_floats->m_values[4];
    m_cp.ImportanceSpine[0] = data->m_floats->m_values[5];
  }


  /// A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations.
  float getArmsDriveCompensationScale(unsigned int index) const { NMP_ASSERT(index < 2); return ArmsDriveCompensationScale[index]; }
  enum { maxArmsDriveCompensationScale = 2 };
  /// Scales the length of the randomly generated displacement of the arm position.
  float getArmsAmplitudeScale(unsigned int index) const { NMP_ASSERT(index < 2); return ArmsAmplitudeScale[index]; }
  enum { maxArmsAmplitudeScale = 2 };
  /// How often the randomly generated arm position changes. In Hertz on the standard character.
  float getArmsFrequency(unsigned int index) const { NMP_ASSERT(index < 2); return ArmsFrequency[index]; }
  enum { maxArmsFrequency = 2 };
  /// When enabled, the arm is randomly displaced from the position in the pose. If disabled, the random displacement is added to the current position.
  bool getArmsBasedOnDefaultPose(unsigned int index) const { NMP_ASSERT(index < 2); return ArmsBasedOnDefaultPose[index]; }
  enum { maxArmsBasedOnDefaultPose = 2 };

  /// A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations.
  float getHeadsDriveCompensationScale(unsigned int index) const { NMP_ASSERT(index < 1); return HeadsDriveCompensationScale[index]; }
  enum { maxHeadsDriveCompensationScale = 1 };
  /// Scales the length of the randomly generated displacement of the head position.
  float getHeadsAmplitudeScale(unsigned int index) const { NMP_ASSERT(index < 1); return HeadsAmplitudeScale[index]; }
  enum { maxHeadsAmplitudeScale = 1 };
  /// How often the randomly generated head position changes. In Hertz on the standard character.
  float getHeadsFrequency(unsigned int index) const { NMP_ASSERT(index < 1); return HeadsFrequency[index]; }
  enum { maxHeadsFrequency = 1 };
  /// When enabled, the head is randomly displaced from the position in the pose. If disabled, the random displacement is added to the current position.
  bool getHeadsBasedOnDefaultPose(unsigned int index) const { NMP_ASSERT(index < 1); return HeadsBasedOnDefaultPose[index]; }
  enum { maxHeadsBasedOnDefaultPose = 1 };

  /// A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations.
  float getLegsDriveCompensationScale(unsigned int index) const { NMP_ASSERT(index < 2); return LegsDriveCompensationScale[index]; }
  enum { maxLegsDriveCompensationScale = 2 };
  /// Scales the length of the randomly generated displacement of the leg position.
  float getLegsAmplitudeScale(unsigned int index) const { NMP_ASSERT(index < 2); return LegsAmplitudeScale[index]; }
  enum { maxLegsAmplitudeScale = 2 };
  /// How often the randomly generated leg position changes. In Hertz on the standard character.
  float getLegsFrequency(unsigned int index) const { NMP_ASSERT(index < 2); return LegsFrequency[index]; }
  enum { maxLegsFrequency = 2 };
  /// When enabled, the leg is randomly displaced from the position in the pose. If disabled, the random displacement is added to the current position.
  bool getLegsBasedOnDefaultPose(unsigned int index) const { NMP_ASSERT(index < 2); return LegsBasedOnDefaultPose[index]; }
  enum { maxLegsBasedOnDefaultPose = 2 };

  /// A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations.
  float getSpinesDriveCompensationScale(unsigned int index) const { NMP_ASSERT(index < 1); return SpinesDriveCompensationScale[index]; }
  enum { maxSpinesDriveCompensationScale = 1 };
  /// Scales the length of the randomly generated displacement of the spine position.
  float getSpinesAmplitudeScale(unsigned int index) const { NMP_ASSERT(index < 1); return SpinesAmplitudeScale[index]; }
  enum { maxSpinesAmplitudeScale = 1 };
  /// How often the randomly generated spine position changes. In Hertz on the standard character.
  float getSpinesFrequency(unsigned int index) const { NMP_ASSERT(index < 1); return SpinesFrequency[index]; }
  enum { maxSpinesFrequency = 1 };
  /// When enabled, the spine is randomly displaced from the position in the pose. If disabled, the random displacement is added to the current position.
  bool getSpinesBasedOnDefaultPose(unsigned int index) const { NMP_ASSERT(index < 1); return SpinesBasedOnDefaultPose[index]; }
  enum { maxSpinesBasedOnDefaultPose = 1 };

  /// The strength this behaviour has to control each limb type


  /// Sets an average pose for the character's body during the behaviour. All random movement of body parts will be interpreted as offsets from this pose.


  /// Sets an average pose for the character's body during the behaviour. All random movement of body parts will be interpreted as offsets from this pose.
  const BodyPoseData& getDefaultPose() const { return pd_DefaultPose; }

  // control parameters
  /// The strength this behaviour has to control each limb type

  /// Sets the strength of the control this behaviour has over the arms, scaling the generated movement.
  float getImportanceArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ImportanceArm[index]; }
  enum { maxImportanceArmCP = 2 };
  /// Sets the strength of the control this behaviour has over the heads, scaling the generated movement.
  float getImportanceHeadCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.ImportanceHead[index]; }
  enum { maxImportanceHeadCP = 1 };
  /// Sets the strength of the control this behaviour has over the legs, scaling the generated movement.
  float getImportanceLegCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ImportanceLeg[index]; }
  enum { maxImportanceLegCP = 2 };
  /// Sets the strength of the control this behaviour has over the spines, scaling the generated movement.
  float getImportanceSpineCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.ImportanceSpine[index]; }
  enum { maxImportanceSpineCP = 1 };

  // output control parameters
};


class WritheBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Writhe"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 29; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class WritheBehaviourDef* m_definition;

  enum AnimInputIDs
  {
    DefaultPose_ID = 0,
  };

  enum ControlParamIDs
  {
    ImportanceArm_0_CP_ID = 0,
    ImportanceArm_1_CP_ID = 1,
    ImportanceHead_0_CP_ID = 2,
    ImportanceLeg_0_CP_ID = 3,
    ImportanceLeg_1_CP_ID = 4,
    ImportanceSpine_0_CP_ID = 5,
  };

  const WritheBehaviourData& getParams() const { return m_params; }
  WritheBehaviourData& getParams() { return m_params; }
  WrithePoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  WritheBehaviourData m_params;
  WrithePoseRequirements m_poseRequirements;
};

class WritheBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(WritheBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline WritheBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(WritheBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) WritheBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_WRITHE_H

