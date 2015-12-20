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

#ifndef NM_MDF_BDEF_CHARACTERISTICS_H
#define NM_MDF_BDEF_CHARACTERISTICS_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Characteristics.behaviour"

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

struct CharacteristicsPoseRequirements
{

  CharacteristicsPoseRequirements()
  {
  }
};

struct CharacteristicsBehaviourData
{
  friend class CharacteristicsBehaviour;

private:


  struct ControlParams
  {
    float WholeBodyStrengthScale;
    float ArmStrengthScale[NetworkConstants::networkMaxNumArms];  ///< Strength multiplier for this arm
    float HeadStrengthScale[NetworkConstants::networkMaxNumHeads];  ///< Strength multiplier for this head
    float LegStrengthScale[NetworkConstants::networkMaxNumLegs];  ///< Strength multiplier for this leg
    float SpineStrengthScale[NetworkConstants::networkMaxNumSpines];  ///< Strength multiplier for the spine
    float WholeBodyControlCompensationScale;
    float ArmControlCompensationScale[NetworkConstants::networkMaxNumArms];  ///< Control compensation multiplier for this arm
    float HeadControlCompensationScale[NetworkConstants::networkMaxNumHeads];  ///< Control compensation multiplier for this head
    float LegControlCompensationScale[NetworkConstants::networkMaxNumLegs];  ///< Control compensation multiplier for this leg
    float SpineControlCompensationScale[NetworkConstants::networkMaxNumSpines];  ///< Control compensation multiplier for the spine
    float WholeBodyExternalComplianceScale;
    float ArmExternalComplianceScale[NetworkConstants::networkMaxNumArms];  ///< External Compliance multiplier for this arm
    float HeadExternalComplianceScale[NetworkConstants::networkMaxNumHeads];  ///< External Compliance multiplier for this head
    float LegExternalComplianceScale[NetworkConstants::networkMaxNumLegs];  ///< External Compliance multiplier for this leg
    float SpineExternalComplianceScale[NetworkConstants::networkMaxNumSpines];  ///< External Compliance multiplier for the spine
    float WholeBodyDampingRatioScale;
    float ArmDampingRatioScale[NetworkConstants::networkMaxNumArms];  ///< Damping ratio multiplier for this arm
    float HeadDampingRatioScale[NetworkConstants::networkMaxNumHeads];  ///< Damping ratio multiplier for this head
    float LegDampingRatioScale[NetworkConstants::networkMaxNumLegs];  ///< Damping ratio multiplier for this leg
    float SpineDampingRatioScale[NetworkConstants::networkMaxNumSpines];  ///< Damping ratio multiplier for the spine
    float WholeBodySoftLimitStiffnessScale;
    float ArmSoftLimitStiffnessScale[NetworkConstants::networkMaxNumArms];  ///< Control soft limit stiffness multiplier for this arm
    float HeadSoftLimitStiffnessScale[NetworkConstants::networkMaxNumHeads];  ///< Control soft limit stiffness multiplier for this head
    float LegSoftLimitStiffnessScale[NetworkConstants::networkMaxNumLegs];  ///< Control soft limit stiffness multiplier for this leg
    float SpineSoftLimitStiffnessScale[NetworkConstants::networkMaxNumSpines];  ///< Control soft limit stiffness multiplier for the spine
  };
  ControlParams m_cp;



public:

  CharacteristicsBehaviourData()
  {
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* NMP_USED_FOR_ASSERTS(data))
  {
    NMP_ASSERT(data->m_floats->m_numValues == 0);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 35);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 0);

    m_cp.WholeBodyStrengthScale = data->m_floats->m_values[0];
    m_cp.ArmStrengthScale[0] = data->m_floats->m_values[1];
    m_cp.ArmStrengthScale[1] = data->m_floats->m_values[2];
    m_cp.HeadStrengthScale[0] = data->m_floats->m_values[3];
    m_cp.LegStrengthScale[0] = data->m_floats->m_values[4];
    m_cp.LegStrengthScale[1] = data->m_floats->m_values[5];
    m_cp.SpineStrengthScale[0] = data->m_floats->m_values[6];
    m_cp.WholeBodyControlCompensationScale = data->m_floats->m_values[7];
    m_cp.ArmControlCompensationScale[0] = data->m_floats->m_values[8];
    m_cp.ArmControlCompensationScale[1] = data->m_floats->m_values[9];
    m_cp.HeadControlCompensationScale[0] = data->m_floats->m_values[10];
    m_cp.LegControlCompensationScale[0] = data->m_floats->m_values[11];
    m_cp.LegControlCompensationScale[1] = data->m_floats->m_values[12];
    m_cp.SpineControlCompensationScale[0] = data->m_floats->m_values[13];
    m_cp.WholeBodyExternalComplianceScale = data->m_floats->m_values[14];
    m_cp.ArmExternalComplianceScale[0] = data->m_floats->m_values[15];
    m_cp.ArmExternalComplianceScale[1] = data->m_floats->m_values[16];
    m_cp.HeadExternalComplianceScale[0] = data->m_floats->m_values[17];
    m_cp.LegExternalComplianceScale[0] = data->m_floats->m_values[18];
    m_cp.LegExternalComplianceScale[1] = data->m_floats->m_values[19];
    m_cp.SpineExternalComplianceScale[0] = data->m_floats->m_values[20];
    m_cp.WholeBodyDampingRatioScale = data->m_floats->m_values[21];
    m_cp.ArmDampingRatioScale[0] = data->m_floats->m_values[22];
    m_cp.ArmDampingRatioScale[1] = data->m_floats->m_values[23];
    m_cp.HeadDampingRatioScale[0] = data->m_floats->m_values[24];
    m_cp.LegDampingRatioScale[0] = data->m_floats->m_values[25];
    m_cp.LegDampingRatioScale[1] = data->m_floats->m_values[26];
    m_cp.SpineDampingRatioScale[0] = data->m_floats->m_values[27];
    m_cp.WholeBodySoftLimitStiffnessScale = data->m_floats->m_values[28];
    m_cp.ArmSoftLimitStiffnessScale[0] = data->m_floats->m_values[29];
    m_cp.ArmSoftLimitStiffnessScale[1] = data->m_floats->m_values[30];
    m_cp.HeadSoftLimitStiffnessScale[0] = data->m_floats->m_values[31];
    m_cp.LegSoftLimitStiffnessScale[0] = data->m_floats->m_values[32];
    m_cp.LegSoftLimitStiffnessScale[1] = data->m_floats->m_values[33];
    m_cp.SpineSoftLimitStiffnessScale[0] = data->m_floats->m_values[34];
  }


  /// Sets the strength scale for a character's limbs. Stronger limbs will be move faster and be better able to reach and maintain target poses.


  /// The control compensation is used to make joints compensate for controlled motion in other parts of the rig. A high value will cause a character to appear coordinated whilst a low value will lead to loose, uncontrolled motion.


  /// The external compliance is used to make joints compensate for external forces on other parts of the rig (e.g. contact forces). A high value will allow motion caused by external forces to propagate through the rig (making it seem quite loose) whilst a low value will limit movement to the affected part, making the rest of the rig appear stiff.


  /// Sets the damping scale for a character's limbs. High values (above 1) will result in the charater moving his joints more slowly in a controlled way, and they will deform less as a result of external influences. Small values (below 1) will result in the joints being looser andless well controlled. 


  /// Sets the stiffness scale for the soft limits. Lowering the value to less than 1 will reduce the effect of the soft limits when the limb is being weakly controlled.


  // control parameters
  /// Sets the strength scale for a character's limbs. Stronger limbs will be move faster and be better able to reach and maintain target poses.

  float getWholeBodyStrengthScaleCP() const { return m_cp.WholeBodyStrengthScale; }
  /// Strength multiplier for this arm
  float getArmStrengthScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmStrengthScale[index]; }
  enum { maxArmStrengthScaleCP = 2 };
  /// Strength multiplier for this head
  float getHeadStrengthScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadStrengthScale[index]; }
  enum { maxHeadStrengthScaleCP = 1 };
  /// Strength multiplier for this leg
  float getLegStrengthScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LegStrengthScale[index]; }
  enum { maxLegStrengthScaleCP = 2 };
  /// Strength multiplier for the spine
  float getSpineStrengthScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineStrengthScale[index]; }
  enum { maxSpineStrengthScaleCP = 1 };
  /// The control compensation is used to make joints compensate for controlled motion in other parts of the rig. A high value will cause a character to appear coordinated whilst a low value will lead to loose, uncontrolled motion.

  float getWholeBodyControlCompensationScaleCP() const { return m_cp.WholeBodyControlCompensationScale; }
  /// Control compensation multiplier for this arm
  float getArmControlCompensationScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmControlCompensationScale[index]; }
  enum { maxArmControlCompensationScaleCP = 2 };
  /// Control compensation multiplier for this head
  float getHeadControlCompensationScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadControlCompensationScale[index]; }
  enum { maxHeadControlCompensationScaleCP = 1 };
  /// Control compensation multiplier for this leg
  float getLegControlCompensationScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LegControlCompensationScale[index]; }
  enum { maxLegControlCompensationScaleCP = 2 };
  /// Control compensation multiplier for the spine
  float getSpineControlCompensationScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineControlCompensationScale[index]; }
  enum { maxSpineControlCompensationScaleCP = 1 };
  /// The external compliance is used to make joints compensate for external forces on other parts of the rig (e.g. contact forces). A high value will allow motion caused by external forces to propagate through the rig (making it seem quite loose) whilst a low value will limit movement to the affected part, making the rest of the rig appear stiff.

  float getWholeBodyExternalComplianceScaleCP() const { return m_cp.WholeBodyExternalComplianceScale; }
  /// External Compliance multiplier for this arm
  float getArmExternalComplianceScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmExternalComplianceScale[index]; }
  enum { maxArmExternalComplianceScaleCP = 2 };
  /// External Compliance multiplier for this head
  float getHeadExternalComplianceScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadExternalComplianceScale[index]; }
  enum { maxHeadExternalComplianceScaleCP = 1 };
  /// External Compliance multiplier for this leg
  float getLegExternalComplianceScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LegExternalComplianceScale[index]; }
  enum { maxLegExternalComplianceScaleCP = 2 };
  /// External Compliance multiplier for the spine
  float getSpineExternalComplianceScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineExternalComplianceScale[index]; }
  enum { maxSpineExternalComplianceScaleCP = 1 };
  /// Sets the damping scale for a character's limbs. High values (above 1) will result in the charater moving his joints more slowly in a controlled way, and they will deform less as a result of external influences. Small values (below 1) will result in the joints being looser andless well controlled. 

  float getWholeBodyDampingRatioScaleCP() const { return m_cp.WholeBodyDampingRatioScale; }
  /// Damping ratio multiplier for this arm
  float getArmDampingRatioScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmDampingRatioScale[index]; }
  enum { maxArmDampingRatioScaleCP = 2 };
  /// Damping ratio multiplier for this head
  float getHeadDampingRatioScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadDampingRatioScale[index]; }
  enum { maxHeadDampingRatioScaleCP = 1 };
  /// Damping ratio multiplier for this leg
  float getLegDampingRatioScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LegDampingRatioScale[index]; }
  enum { maxLegDampingRatioScaleCP = 2 };
  /// Damping ratio multiplier for the spine
  float getSpineDampingRatioScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineDampingRatioScale[index]; }
  enum { maxSpineDampingRatioScaleCP = 1 };
  /// Sets the stiffness scale for the soft limits. Lowering the value to less than 1 will reduce the effect of the soft limits when the limb is being weakly controlled.

  float getWholeBodySoftLimitStiffnessScaleCP() const { return m_cp.WholeBodySoftLimitStiffnessScale; }
  /// Control soft limit stiffness multiplier for this arm
  float getArmSoftLimitStiffnessScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ArmSoftLimitStiffnessScale[index]; }
  enum { maxArmSoftLimitStiffnessScaleCP = 2 };
  /// Control soft limit stiffness multiplier for this head
  float getHeadSoftLimitStiffnessScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.HeadSoftLimitStiffnessScale[index]; }
  enum { maxHeadSoftLimitStiffnessScaleCP = 1 };
  /// Control soft limit stiffness multiplier for this leg
  float getLegSoftLimitStiffnessScaleCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LegSoftLimitStiffnessScale[index]; }
  enum { maxLegSoftLimitStiffnessScaleCP = 2 };
  /// Control soft limit stiffness multiplier for the spine
  float getSpineSoftLimitStiffnessScaleCP(unsigned int index) const { NMP_ASSERT(index < 1); return m_cp.SpineSoftLimitStiffnessScale[index]; }
  enum { maxSpineSoftLimitStiffnessScaleCP = 1 };

  // output control parameters
};


class CharacteristicsBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Characteristics"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 8; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class CharacteristicsBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    WholeBodyStrengthScale_CP_ID = 0,
    ArmStrengthScale_0_CP_ID = 1,
    ArmStrengthScale_1_CP_ID = 2,
    HeadStrengthScale_0_CP_ID = 3,
    LegStrengthScale_0_CP_ID = 4,
    LegStrengthScale_1_CP_ID = 5,
    SpineStrengthScale_0_CP_ID = 6,
    WholeBodyControlCompensationScale_CP_ID = 7,
    ArmControlCompensationScale_0_CP_ID = 8,
    ArmControlCompensationScale_1_CP_ID = 9,
    HeadControlCompensationScale_0_CP_ID = 10,
    LegControlCompensationScale_0_CP_ID = 11,
    LegControlCompensationScale_1_CP_ID = 12,
    SpineControlCompensationScale_0_CP_ID = 13,
    WholeBodyExternalComplianceScale_CP_ID = 14,
    ArmExternalComplianceScale_0_CP_ID = 15,
    ArmExternalComplianceScale_1_CP_ID = 16,
    HeadExternalComplianceScale_0_CP_ID = 17,
    LegExternalComplianceScale_0_CP_ID = 18,
    LegExternalComplianceScale_1_CP_ID = 19,
    SpineExternalComplianceScale_0_CP_ID = 20,
    WholeBodyDampingRatioScale_CP_ID = 21,
    ArmDampingRatioScale_0_CP_ID = 22,
    ArmDampingRatioScale_1_CP_ID = 23,
    HeadDampingRatioScale_0_CP_ID = 24,
    LegDampingRatioScale_0_CP_ID = 25,
    LegDampingRatioScale_1_CP_ID = 26,
    SpineDampingRatioScale_0_CP_ID = 27,
    WholeBodySoftLimitStiffnessScale_CP_ID = 28,
    ArmSoftLimitStiffnessScale_0_CP_ID = 29,
    ArmSoftLimitStiffnessScale_1_CP_ID = 30,
    HeadSoftLimitStiffnessScale_0_CP_ID = 31,
    LegSoftLimitStiffnessScale_0_CP_ID = 32,
    LegSoftLimitStiffnessScale_1_CP_ID = 33,
    SpineSoftLimitStiffnessScale_0_CP_ID = 34,
  };

  const CharacteristicsBehaviourData& getParams() const { return m_params; }
  CharacteristicsBehaviourData& getParams() { return m_params; }
  CharacteristicsPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  CharacteristicsBehaviourData m_params;
  CharacteristicsPoseRequirements m_poseRequirements;
};

class CharacteristicsBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(CharacteristicsBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline CharacteristicsBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(CharacteristicsBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) CharacteristicsBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_CHARACTERISTICS_H

