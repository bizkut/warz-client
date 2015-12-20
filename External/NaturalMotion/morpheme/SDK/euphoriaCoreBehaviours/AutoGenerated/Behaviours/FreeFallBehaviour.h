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

#ifndef NM_MDF_BDEF_FREEFALL_H
#define NM_MDF_BDEF_FREEFALL_H

// include definition file to create project dependency
#include "./Definition/Behaviours/FreeFall.behaviour"

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

struct FreeFallPoseRequirements
{

  FreeFallPoseRequirements()
  {
  }
};

struct FreeFallBehaviourData
{
  friend class FreeFallBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 orientation;  ///< A rotation vector specifiying an offset to the target orientation for the character's pelvis relative to the orientation defined in "Orientation Configuration".
    float weight;  ///< Multiplier applied to the strength with which the character's orientation is driven.
    bool disableWhenInContact;  ///< If true, then free fall will not attempt to orientate the character when it's in contact.
  };
  ControlParams m_cp;


  struct OutputControlParams
  {
    float orientationError;  ///< Angle between the current and target orientation of the character, in degrees.
  };
  OutputControlParams m_ocp;

  float StartOrientationTime;  /// The time to start driving the character's orientation, in seconds (standard character).
  
  float StopOrientationTime;  /// The time to stop driving the character's orientation, in seconds (standard character).
  
  float AssistanceAmount;  /// Amount of externally applied torque used to achieve the desired orientation. Unitless. No need to alter for larger or heavier characters
  
  float DampingAmount;  /// Amount of externally applied torque used to damp existing rotation (tumbling). Unitless. No need to alter for larger or heavier characters
  
  float RotationTime;  /// Sets the amount of time in which to achieve the rotation, in seconds (standard character). Smaller values lead to faster movements. For times under about 0.5s the arms will swing rather than spin.
  
  float ArmsAmount;  /// The amount to use the arms - when spinning or swinging
  
  float LegsAmount;  /// The amount to use the legs - when spinning or swinging
  
  float ArmsSpinAmount[NetworkConstants::networkMaxNumArms];  ///< Affects how strongly a request to spin the arms is applied. 1 is normal or average movement, 0 has no effect, the spin is turned off.
  float LegsSpinAmount[NetworkConstants::networkMaxNumLegs];  ///< Affects how strongly a request to spin the legs is applied. 1 is normal or average movement, 0 has no effect, the swing is turned off.
  int32_t characterAxis0;  ///< The primary axis local to the character that should be alligned to the primary target axis.
  int32_t characterAxis1;  ///< The secondary axis local to the character that should be alligned to the secondary target axis.
  int32_t targetAxis0;  ///< The primary world space axis with which to align the primary character axis.\n1. Velocity: The direction of movement of the Character's hips in world space.\n2. Landing Up: The best alignment for the character's up direction for a successful landing.\n3. World Up: The "up" direction of the scene, in world space.
  int32_t targetAxis1;  ///< The secondary world space axis with which to align the secondary character axis. \n1. Velocity: The direction of movement of the Character's hips in world space. \n2. Landing Up: The best alignment for the character's up direction for a successful landing. \n3. World Up: The "up" direction of the scene, in world space.
  float secondaryDirectionThreshold;  ///< The secondary axis is ignored if the dot product between the primary and secondary axis is greater than this value. This is useful for preventing undesired rotation around the primary axis in order to face the tiny perpendicular component of an almost parallel secondary axis.
  float AngleLandingAmount;  /// How much to angle the landing to match the lateral velocity of the character. 0 will land vertically, 1 will land oriented in the direction of velocity, like an arrow when it sticks into the ground where it lands. This is only relevant if the "Landing Up" target axis is used in the Base Orientation configuration.
  
  bool StartOrientationAtTimeBeforeImpact;  /// The StartOrientationTime will be interpreted as time before impact if this is true, else it is interpreted as time after entering free fall.
  
  bool StopOrientationAtTimeBeforeImpact;  /// The StopOrientationTime will be interpreted as time before impact if this is true, else it is interpreted as time after entering free fall.
  
  bool ArmsInPhase;  ///< If true the the arms will move in phase.


public:

  FreeFallBehaviourData()
  {
    StartOrientationTime = float(0.2);
    StopOrientationTime = float(0.4);
    StartOrientationAtTimeBeforeImpact = false;
    StopOrientationAtTimeBeforeImpact = true;
    AssistanceAmount = float(0);
    DampingAmount = float(0);
    RotationTime = float(0.5);
    ArmsAmount = float(1);
    LegsAmount = float(1);
    ArmsSpinAmount[0] = float(1);
    ArmsSpinAmount[1] = float(1);
    ArmsInPhase = true;
    LegsSpinAmount[0] = float(1);
    LegsSpinAmount[1] = float(1);
    characterAxis0 = 1;
    characterAxis1 = 0;
    targetAxis0 = 2;
    targetAxis1 = 0;
    secondaryDirectionThreshold = float(0.9);
    AngleLandingAmount = float(0.5);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 13);
    NMP_ASSERT(data->m_ints->m_numValues == 7);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    StartOrientationTime = data->m_floats->m_values[0];
    StopOrientationTime = data->m_floats->m_values[1];
    StartOrientationAtTimeBeforeImpact = (data->m_ints->m_values[0] != 0);
    StopOrientationAtTimeBeforeImpact = (data->m_ints->m_values[1] != 0);
    AssistanceAmount = data->m_floats->m_values[2];
    DampingAmount = data->m_floats->m_values[3];
    RotationTime = data->m_floats->m_values[4];
    ArmsAmount = data->m_floats->m_values[5];
    LegsAmount = data->m_floats->m_values[6];
    ArmsSpinAmount[0] = data->m_floats->m_values[7];
    ArmsSpinAmount[1] = data->m_floats->m_values[8];
    ArmsInPhase = (data->m_ints->m_values[2] != 0);
    LegsSpinAmount[0] = data->m_floats->m_values[9];
    LegsSpinAmount[1] = data->m_floats->m_values[10];
    characterAxis0 = data->m_ints->m_values[3];
    characterAxis1 = data->m_ints->m_values[4];
    targetAxis0 = data->m_ints->m_values[5];
    targetAxis1 = data->m_ints->m_values[6];
    secondaryDirectionThreshold = data->m_floats->m_values[11];
    AngleLandingAmount = data->m_floats->m_values[12];
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 1);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 4);

    m_cp.orientation.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.weight = data->m_floats->m_values[0];
    m_cp.disableWhenInContact = (data->m_ints->m_values[0] != 0);
  }


  //  Settings that control when, during free fall, the orientation of the character should be controlled.

  /// The time to start driving the character's orientation, in seconds (standard character).
  float getStartOrientationTime() const { return StartOrientationTime; }
  /// The time to stop driving the character's orientation, in seconds (standard character).
  float getStopOrientationTime() const { return StopOrientationTime; }
  /// The StartOrientationTime will be interpreted as time before impact if this is true, else it is interpreted as time after entering free fall.
  bool getStartOrientationAtTimeBeforeImpact() const { return StartOrientationAtTimeBeforeImpact; }
  /// The StopOrientationTime will be interpreted as time before impact if this is true, else it is interpreted as time after entering free fall.
  bool getStopOrientationAtTimeBeforeImpact() const { return StopOrientationAtTimeBeforeImpact; }

  /// Settings that control when position of the body during free fall

  /// Amount of externally applied torque used to achieve the desired orientation. Unitless. No need to alter for larger or heavier characters
  float getAssistanceAmount() const { return AssistanceAmount; }
  /// Amount of externally applied torque used to damp existing rotation (tumbling). Unitless. No need to alter for larger or heavier characters
  float getDampingAmount() const { return DampingAmount; }
  /// Sets the amount of time in which to achieve the rotation, in seconds (standard character). Smaller values lead to faster movements. For times under about 0.5s the arms will swing rather than spin.
  float getRotationTime() const { return RotationTime; }
  /// The amount to use the arms - when spinning or swinging
  float getArmsAmount() const { return ArmsAmount; }
  /// The amount to use the legs - when spinning or swinging
  float getLegsAmount() const { return LegsAmount; }

  /// Settings that control how the arms windmill in free fall

  /// Affects how strongly a request to spin the arms is applied. 1 is normal or average movement, 0 has no effect, the spin is turned off.
  float getArmsSpinAmount(unsigned int index) const { NMP_ASSERT(index < 2); return ArmsSpinAmount[index]; }
  enum { maxArmsSpinAmount = 2 };
  /// If true the the arms will move in phase.
  bool getArmsInPhase() const { return ArmsInPhase; }

  /// Settings that control how the legs spin in free fall

  /// Affects how strongly a request to spin the legs is applied. 1 is normal or average movement, 0 has no effect, the swing is turned off.
  float getLegsSpinAmount(unsigned int index) const { NMP_ASSERT(index < 2); return LegsSpinAmount[index]; }
  enum { maxLegsSpinAmount = 2 };

  /// Define the target orientation for the character's pelvis by choosing to align one or two axis in character space with axis in world space.

  /// The primary axis local to the character that should be alligned to the primary target axis.
  int32_t getCharacterAxis0() const { return characterAxis0; }
  /// The secondary axis local to the character that should be alligned to the secondary target axis.
  int32_t getCharacterAxis1() const { return characterAxis1; }
  /// The primary world space axis with which to align the primary character axis.\n1. Velocity: The direction of movement of the Character's hips in world space.\n2. Landing Up: The best alignment for the character's up direction for a successful landing.\n3. World Up: The "up" direction of the scene, in world space.
  int32_t getTargetAxis0() const { return targetAxis0; }
  /// The secondary world space axis with which to align the secondary character axis. \n1. Velocity: The direction of movement of the Character's hips in world space. \n2. Landing Up: The best alignment for the character's up direction for a successful landing. \n3. World Up: The "up" direction of the scene, in world space.
  int32_t getTargetAxis1() const { return targetAxis1; }
  /// The secondary axis is ignored if the dot product between the primary and secondary axis is greater than this value. This is useful for preventing undesired rotation around the primary axis in order to face the tiny perpendicular component of an almost parallel secondary axis.
  float getSecondaryDirectionThreshold() const { return secondaryDirectionThreshold; }

  /// Define how the "Landing Up Axis" orientation target axis is calculated.

  /// How much to angle the landing to match the lateral velocity of the character. 0 will land vertically, 1 will land oriented in the direction of velocity, like an arrow when it sticks into the ground where it lands. This is only relevant if the "Landing Up" target axis is used in the Base Orientation configuration.
  float getAngleLandingAmount() const { return AngleLandingAmount; }


  // control parameters
  /// A rotation vector specifiying an offset to the target orientation for the character's pelvis relative to the orientation defined in "Orientation Configuration".
  const NMP::Vector3& getOrientationCP() const { return m_cp.orientation; }
  /// Multiplier applied to the strength with which the character's orientation is driven.
  float getWeightCP() const { return m_cp.weight; }
  /// If true, then free fall will not attempt to orientate the character when it's in contact.
  bool getDisableWhenInContactCP() const { return m_cp.disableWhenInContact; }

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// Angle between the current and target orientation of the character, in degrees.
  void setOrientationErrorOCP(float value) { m_ocp.orientationError = value; }
};


class FreeFallBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "FreeFall"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 11; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class FreeFallBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    orientation_CP_ID = 0,
    weight_CP_ID = 1,
    disableWhenInContact_CP_ID = 2,
  };

  enum OutputControlParamIDs
  {
    orientationError_OCP_ID = 0,
  };

  const FreeFallBehaviourData& getParams() const { return m_params; }
  FreeFallBehaviourData& getParams() { return m_params; }
  FreeFallPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  FreeFallBehaviourData m_params;
  FreeFallPoseRequirements m_poseRequirements;
};

class FreeFallBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(FreeFallBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline FreeFallBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(FreeFallBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) FreeFallBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_FREEFALL_H

