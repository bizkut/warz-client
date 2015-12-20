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

#ifndef NM_MDF_BDEF_LOOK_H
#define NM_MDF_BDEF_LOOK_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Look.behaviour"

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

struct LookPoseRequirements
{

  LookPoseRequirements()
  {
  }
};

struct LookBehaviourData
{
  friend class LookBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 LookPosition;  ///< Sets the position in world space to look at.
    float WholeBodyLook;  ///< Sets how much of the body to move when looking. Accepts values in the range 0 to 1.
    float LookWeight;  ///< The weight (importance) of the look behaviour relative to others.
  };
  ControlParams m_cp;

  float TargetYawRight;  ///< Ignore look target if yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float TargetYawLeft;  ///< Ignore look target if yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float TargetPitchDown;  ///< Ignore look target if pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float TargetPitchUp;  ///< Ignore look target if pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  bool IgnoreDirectionWhenOutOfRange;


public:

  LookBehaviourData()
  {
    IgnoreDirectionWhenOutOfRange = false;
    TargetYawRight = float(-90);
    TargetYawLeft = float(90);
    TargetPitchDown = float(-90);
    TargetPitchUp = float(90);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 4);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    IgnoreDirectionWhenOutOfRange = (data->m_ints->m_values[0] != 0);
    TargetYawRight = data->m_floats->m_values[0];
    TargetYawLeft = data->m_floats->m_values[1];
    TargetPitchDown = data->m_floats->m_values[2];
    TargetPitchUp = data->m_floats->m_values[3];
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 2);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 4);

    m_cp.LookPosition.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.WholeBodyLook = data->m_floats->m_values[0];
    m_cp.LookWeight = data->m_floats->m_values[1];
  }


  /// Don't Look at a target if the direction from the pelvis to the target is outside of this range. The center of the range points in the pelvis forward direction. Pitch and Yaw are in degrees.

  bool getIgnoreDirectionWhenOutOfRange() const { return IgnoreDirectionWhenOutOfRange; }
  /// Ignore look target if yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float getTargetYawRight() const { return TargetYawRight; }
  /// Ignore look target if yaw is outside of this range. The centre of the range points in the pelvis forward direction and the yaw value increases from the character's right to its left. In degrees.
  float getTargetYawLeft() const { return TargetYawLeft; }
  /// Ignore look target if pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float getTargetPitchDown() const { return TargetPitchDown; }
  /// Ignore look target if pitch is outside of this range. The centre of the range points in the pelvis forward direction and the pitch value increases from character down to up. In degrees.
  float getTargetPitchUp() const { return TargetPitchUp; }


  // control parameters
  /// Sets the position in world space to look at.
  const NMP::Vector3& getLookPositionCP() const { return m_cp.LookPosition; }
  /// Sets how much of the body to move when looking. Accepts values in the range 0 to 1.
  float getWholeBodyLookCP() const { return m_cp.WholeBodyLook; }
  /// The weight (importance) of the look behaviour relative to others.
  float getLookWeightCP() const { return m_cp.LookWeight; }

  // output control parameters
};


class LookBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Look"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 19; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class LookBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    LookPosition_CP_ID = 0,
    WholeBodyLook_CP_ID = 1,
    LookWeight_CP_ID = 2,
  };

  const LookBehaviourData& getParams() const { return m_params; }
  LookBehaviourData& getParams() { return m_params; }
  LookPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  LookBehaviourData m_params;
  LookPoseRequirements m_poseRequirements;
};

class LookBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(LookBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline LookBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(LookBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) LookBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_LOOK_H

