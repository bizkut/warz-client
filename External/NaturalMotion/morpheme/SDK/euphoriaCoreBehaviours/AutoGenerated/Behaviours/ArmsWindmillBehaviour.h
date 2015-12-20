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

#ifndef NM_MDF_BDEF_ARMSWINDMILL_H
#define NM_MDF_BDEF_ARMSWINDMILL_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ArmsWindmill.behaviour"

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

struct ArmsWindmillPoseRequirements
{

  ArmsWindmillPoseRequirements()
  {
  }
};

struct ArmsWindmillBehaviourData
{
  friend class ArmsWindmillBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 TargetRotationDelta;  ///< Sets a world space rotation vector whose direction determines the axis and magnitude determines the angle of rotation of the chest, in degrees. When operating in local space, the x axis points forwards, z points right and y points up (along the spine).
    float RotationTime;  ///< Sets the amount of time in which to achieve the rotation, in seconds (standard character). Smaller values lead to faster movements. For times under about 0.5s the arms will swing rather than spin.
    float ImportanceForArm[NetworkConstants::networkMaxNumArms];  ///< Sets the strength of control over each arm. Accepts values in the range 0 to 1.
  };
  ControlParams m_cp;

  float MaxAngSpeed;  /// Maximum angular speed of the arm, in revolutions/second (standard character).
  
  float MaxRadius;  /// Maximum radius of spin around the spin centre, in metres (standard character).. 
  
  float SpinAmounts[NetworkConstants::networkMaxNumArms];  ///< Affects how strongly a request to spin the arms is applied. 1 is normal or average stiffness, 0 has no effect, the spin is turned off.
  float SpinCentreLateral;  ///< Centre of spin circle sideways away from the shoulder. In metres (standard character).
  float SpinCentreUp;  ///< Centre of spin circle locally upwards from the shoulder. In metres (standard character).
  float SpinCentreForward;  ///< Centre of spin circle forwards from the shoulder. In metres (standard character).
  float SpinWeightLateral;  ///< Weight of control in the local sideways axis from the chest
  float SpinWeightUp;  ///< Weight of control in the local upwards  axis from the chest
  float SpinWeightForward;  ///< Weight of control in the local forwards axis from the chest
  float SpinOutwardsDistanceWhenBehind;  ///< Distance to push the hands out when they are at the back of the spin cycle. Increase this if the character tends to get his arms caught on the shoulder joint limits. In metres (standard character).
  float SpinArmControlCompensationScale;  ///< Small values make the arm appear more loose, larger values make the arm appear more controlled
  float StrengthReductionTowardsHand;  ///< Small values result in the wrist being as strong as the shoulder. 1.0 will result in the wrist being completely loose. Larger values confine the strength to the joints in the upper arm.
  float SwingAmounts[NetworkConstants::networkMaxNumArms];  ///< Affects how strongly a request to swing the arms is applied. 1 is normal or average stiffness, 0 has no effect, the swing is turned off.
  bool Synchronised;  /// If true then both hands will have the same phase in their respective circles, if false then the hands will circle independently from their initial positions
  
  bool ArmsInPhase;  ///< If true the the arms will move in phase.
  bool SwingOutwardsOnly;  ///< If true, this prevent arms from swinging across the body laterally, they will only swing away from the body.
  bool SpinInLocalSpace;  /// The target rotation delta will be interpretted in local space of the chest, .z value is the rightwards axis used for spinning forwards or backwards, .x value is for spinning around the forwards axis, and .y for spinning around the local vertical or spine axis 
  


public:

  ArmsWindmillBehaviourData()
  {
    MaxAngSpeed = float(3);
    MaxRadius = float(0.6);
    Synchronised = true;
    SpinAmounts[0] = float(1);
    SpinAmounts[1] = float(1);
    ArmsInPhase = true;
    SpinCentreLateral = float(0.25);
    SpinCentreUp = float(0);
    SpinCentreForward = float(0.1);
    SpinWeightLateral = float(1);
    SpinWeightUp = float(0.5);
    SpinWeightForward = float(0.5);
    SpinOutwardsDistanceWhenBehind = float(0.2);
    SpinArmControlCompensationScale = float(1);
    StrengthReductionTowardsHand = float(0.8);
    SwingAmounts[0] = float(0);
    SwingAmounts[1] = float(0);
    SwingOutwardsOnly = false;
    SpinInLocalSpace = true;
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 15);
    NMP_ASSERT(data->m_ints->m_numValues == 4);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    MaxAngSpeed = data->m_floats->m_values[0];
    MaxRadius = data->m_floats->m_values[1];
    Synchronised = (data->m_ints->m_values[0] != 0);
    SpinAmounts[0] = data->m_floats->m_values[2];
    SpinAmounts[1] = data->m_floats->m_values[3];
    ArmsInPhase = (data->m_ints->m_values[1] != 0);
    SpinCentreLateral = data->m_floats->m_values[4];
    SpinCentreUp = data->m_floats->m_values[5];
    SpinCentreForward = data->m_floats->m_values[6];
    SpinWeightLateral = data->m_floats->m_values[7];
    SpinWeightUp = data->m_floats->m_values[8];
    SpinWeightForward = data->m_floats->m_values[9];
    SpinOutwardsDistanceWhenBehind = data->m_floats->m_values[10];
    SpinArmControlCompensationScale = data->m_floats->m_values[11];
    StrengthReductionTowardsHand = data->m_floats->m_values[12];
    SwingAmounts[0] = data->m_floats->m_values[13];
    SwingAmounts[1] = data->m_floats->m_values[14];
    SwingOutwardsOnly = (data->m_ints->m_values[2] != 0);
    SpinInLocalSpace = (data->m_ints->m_values[3] != 0);
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 3);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 4);

    m_cp.TargetRotationDelta.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.RotationTime = data->m_floats->m_values[0];
    m_cp.ImportanceForArm[0] = data->m_floats->m_values[1];
    m_cp.ImportanceForArm[1] = data->m_floats->m_values[2];
  }


  /// Settings that control how the arms spin. This the motion to change the torsos orientation and will last for a couple of seconds.

  /// Maximum angular speed of the arm, in revolutions/second (standard character).
  float getMaxAngSpeed() const { return MaxAngSpeed; }
  /// Maximum radius of spin around the spin centre, in metres (standard character).. 
  float getMaxRadius() const { return MaxRadius; }
  /// If true then both hands will have the same phase in their respective circles, if false then the hands will circle independently from their initial positions
  bool getSynchronised() const { return Synchronised; }
  /// Affects how strongly a request to spin the arms is applied. 1 is normal or average stiffness, 0 has no effect, the spin is turned off.
  float getSpinAmounts(unsigned int index) const { NMP_ASSERT(index < 2); return SpinAmounts[index]; }
  enum { maxSpinAmounts = 2 };
  /// If true the the arms will move in phase.
  bool getArmsInPhase() const { return ArmsInPhase; }
  /// Centre of spin circle sideways away from the shoulder. In metres (standard character).
  float getSpinCentreLateral() const { return SpinCentreLateral; }
  /// Centre of spin circle locally upwards from the shoulder. In metres (standard character).
  float getSpinCentreUp() const { return SpinCentreUp; }
  /// Centre of spin circle forwards from the shoulder. In metres (standard character).
  float getSpinCentreForward() const { return SpinCentreForward; }
  /// Weight of control in the local sideways axis from the chest
  float getSpinWeightLateral() const { return SpinWeightLateral; }
  /// Weight of control in the local upwards  axis from the chest
  float getSpinWeightUp() const { return SpinWeightUp; }
  /// Weight of control in the local forwards axis from the chest
  float getSpinWeightForward() const { return SpinWeightForward; }
  /// Distance to push the hands out when they are at the back of the spin cycle. Increase this if the character tends to get his arms caught on the shoulder joint limits. In metres (standard character).
  float getSpinOutwardsDistanceWhenBehind() const { return SpinOutwardsDistanceWhenBehind; }
  /// Small values make the arm appear more loose, larger values make the arm appear more controlled
  float getSpinArmControlCompensationScale() const { return SpinArmControlCompensationScale; }
  /// Small values result in the wrist being as strong as the shoulder. 1.0 will result in the wrist being completely loose. Larger values confine the strength to the joints in the upper arm.
  float getStrengthReductionTowardsHand() const { return StrengthReductionTowardsHand; }

  /// Settings that control how the arms swing. This is a one off motion to apply an impulse to the torso to change it's velocity. 

  /// Affects how strongly a request to swing the arms is applied. 1 is normal or average stiffness, 0 has no effect, the swing is turned off.
  float getSwingAmounts(unsigned int index) const { NMP_ASSERT(index < 2); return SwingAmounts[index]; }
  enum { maxSwingAmounts = 2 };
  /// If true, this prevent arms from swinging across the body laterally, they will only swing away from the body.
  bool getSwingOutwardsOnly() const { return SwingOutwardsOnly; }

  /// The target rotation delta will be interpretted in local space of the chest, .z value is the rightwards axis used for spinning forwards or backwards, .x value is for spinning around the forwards axis, and .y for spinning around the local vertical or spine axis 
  bool getSpinInLocalSpace() const { return SpinInLocalSpace; }

  // control parameters
  /// Sets a world space rotation vector whose direction determines the axis and magnitude determines the angle of rotation of the chest, in degrees. When operating in local space, the x axis points forwards, z points right and y points up (along the spine).
  const NMP::Vector3& getTargetRotationDeltaCP() const { return m_cp.TargetRotationDelta; }
  /// Sets the amount of time in which to achieve the rotation, in seconds (standard character). Smaller values lead to faster movements. For times under about 0.5s the arms will swing rather than spin.
  float getRotationTimeCP() const { return m_cp.RotationTime; }
  /// Sets the strength of control over each arm. Accepts values in the range 0 to 1.
  float getImportanceForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ImportanceForArm[index]; }
  enum { maxImportanceForArmCP = 2 };

  // output control parameters
};


class ArmsWindmillBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "ArmsWindmill"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 5; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class ArmsWindmillBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    TargetRotationDelta_CP_ID = 0,
    RotationTime_CP_ID = 1,
    ImportanceForArm_0_CP_ID = 2,
    ImportanceForArm_1_CP_ID = 3,
  };

  const ArmsWindmillBehaviourData& getParams() const { return m_params; }
  ArmsWindmillBehaviourData& getParams() { return m_params; }
  ArmsWindmillPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ArmsWindmillBehaviourData m_params;
  ArmsWindmillPoseRequirements m_poseRequirements;
};

class ArmsWindmillBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ArmsWindmillBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ArmsWindmillBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ArmsWindmillBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ArmsWindmillBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_ARMSWINDMILL_H

