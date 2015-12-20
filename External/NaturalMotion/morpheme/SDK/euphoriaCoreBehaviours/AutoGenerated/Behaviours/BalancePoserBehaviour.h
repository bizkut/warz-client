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

#ifndef NM_MDF_BDEF_BALANCEPOSER_H
#define NM_MDF_BDEF_BALANCEPOSER_H

// include definition file to create project dependency
#include "./Definition/Behaviours/BalancePoser.behaviour"

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

struct BalancePoserPoseRequirements
{

  BalancePoserPoseRequirements()
  {
  }
};

struct BalancePoserBehaviourData
{
  friend class BalancePoserBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 ChestTranslation;  ///< Sets the requested translation of the chest from the current position. This is interpreted in absolute units if the 'translation in' attribute is set to 'World Space'. Otherwise, if the 'translation in' attribute is set to 'Character Space' then +ve x is forwards, +ve y is upwards, +ve z is rightwards.
    float Yaw;  ///< Sets the rotation around the spine axis. Positive values cause a rotation to the left. In degrees.
    float Pitch;  ///< Sets the angle with which the character leans forwards or backwards. Positive values pitch the character forwards and down. In degrees.
    float Roll;  ///< Sets the angle with which the character leans to the left or right. Positive values tip the character to its left. In degrees.
    float CrouchAmount;  ///< Interpolate the character's target height (as a fraction of its full height) between 1 (when crouch amount is 0) and the value of the behaviour's "Crouch height fraction" attribute (when crouch amount is 1).
  };
  ControlParams m_cp;

  float CrouchHeightFraction;  ///< Fraction of the balance pose pelvis height when fully crouching
  float PelvisWeight;  ///< Fraction of the pose angle change applied to the pelvis
  float SpineWeight;  ///< Fraction of the pose angle change applied to the chest
  float HeadWeight;  ///< Fraction of the pose angle change applied to the head
  float PitchPerCrouchAmount;  ///< Pitch forward (degrees) when CrouchAmount = 1
  bool ChestTranslationInCharacterSpace;  ///< If true then the chest translation is done in character space so that x = forward, y = up and z = right


public:

  BalancePoserBehaviourData()
  {
    CrouchHeightFraction = float(0.6);
    PelvisWeight = float(0.2);
    SpineWeight = float(0.8);
    HeadWeight = float(0.5);
    PitchPerCrouchAmount = float(30);
    ChestTranslationInCharacterSpace = false;
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 5);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    CrouchHeightFraction = data->m_floats->m_values[0];
    PelvisWeight = data->m_floats->m_values[1];
    SpineWeight = data->m_floats->m_values[2];
    HeadWeight = data->m_floats->m_values[3];
    PitchPerCrouchAmount = data->m_floats->m_values[4];
    ChestTranslationInCharacterSpace = (data->m_ints->m_values[0] != 0);
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 4);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 4);

    m_cp.Yaw = data->m_floats->m_values[0];
    m_cp.Pitch = data->m_floats->m_values[1];
    m_cp.Roll = data->m_floats->m_values[2];
    m_cp.CrouchAmount = data->m_floats->m_values[3];
    m_cp.ChestTranslation.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
  }


  /// Fraction of the balance pose pelvis height when fully crouching
  float getCrouchHeightFraction() const { return CrouchHeightFraction; }
  /// Fraction of the pose angle change applied to the pelvis
  float getPelvisWeight() const { return PelvisWeight; }
  /// Fraction of the pose angle change applied to the chest
  float getSpineWeight() const { return SpineWeight; }
  /// Fraction of the pose angle change applied to the head
  float getHeadWeight() const { return HeadWeight; }
  /// Pitch forward (degrees) when CrouchAmount = 1
  float getPitchPerCrouchAmount() const { return PitchPerCrouchAmount; }
  /// If true then the chest translation is done in character space so that x = forward, y = up and z = right
  bool getChestTranslationInCharacterSpace() const { return ChestTranslationInCharacterSpace; }


  // control parameters
  /// Sets the rotation around the spine axis. Positive values cause a rotation to the left. In degrees.
  float getYawCP() const { return m_cp.Yaw; }
  /// Sets the angle with which the character leans forwards or backwards. Positive values pitch the character forwards and down. In degrees.
  float getPitchCP() const { return m_cp.Pitch; }
  /// Sets the angle with which the character leans to the left or right. Positive values tip the character to its left. In degrees.
  float getRollCP() const { return m_cp.Roll; }
  /// Interpolate the character's target height (as a fraction of its full height) between 1 (when crouch amount is 0) and the value of the behaviour's "Crouch height fraction" attribute (when crouch amount is 1).
  float getCrouchAmountCP() const { return m_cp.CrouchAmount; }
  /// Sets the requested translation of the chest from the current position. This is interpreted in absolute units if the 'translation in' attribute is set to 'World Space'. Otherwise, if the 'translation in' attribute is set to 'Character Space' then +ve x is forwards, +ve y is upwards, +ve z is rightwards.
  const NMP::Vector3& getChestTranslationCP() const { return m_cp.ChestTranslation; }

  // output control parameters
};


class BalancePoserBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "BalancePoser"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 7; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class BalancePoserBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    Yaw_CP_ID = 0,
    Pitch_CP_ID = 1,
    Roll_CP_ID = 2,
    CrouchAmount_CP_ID = 3,
    ChestTranslation_CP_ID = 4,
  };

  const BalancePoserBehaviourData& getParams() const { return m_params; }
  BalancePoserBehaviourData& getParams() { return m_params; }
  BalancePoserPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  BalancePoserBehaviourData m_params;
  BalancePoserPoseRequirements m_poseRequirements;
};

class BalancePoserBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(BalancePoserBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline BalancePoserBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(BalancePoserBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) BalancePoserBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_BALANCEPOSER_H

