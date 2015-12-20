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

#ifndef NM_MDF_BDEF_ARMSPLACEMENT_H
#define NM_MDF_BDEF_ARMSPLACEMENT_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ArmsPlacement.behaviour"

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

struct ArmsPlacementPoseRequirements
{

  ArmsPlacementPoseRequirements()
  {
  }
};

struct ArmsPlacementBehaviourData
{
  friend class ArmsPlacementBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 TargetPosition;  ///< Position of the target in world space and absolute units.
    NMP::Vector3 TargetNormal;  ///< Normal of target in world space - does not need to be normalised.
    NMP::Vector3 TargetVelocity;  ///< Velocity of the target in world space and absolute units.
    float Weight;  ///< Sets the importance of this behaviour relative to others.
  };
  ControlParams m_cp;

  float SwivelAmount;  ///< Controls the arms' swivel. A positive value for elbows out and up, negative for elbows in and down, at zero the swivel will match the guide pose.
  float MaxArmExtensionScale;  ///< Limit the maximum extension of the arm. Expressed as a proportion of the total arm length.
  float handOffsetMultiplier;  ///< Controls how close the hands are together when placing. 0 places hands in the same position, 1 is shoulder width apart so arms are effectively parallel, 2 is two shoulder widths apart.


public:

  ArmsPlacementBehaviourData()
  {
    SwivelAmount = float(0.3);
    MaxArmExtensionScale = float(1);
    handOffsetMultiplier = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 3);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    SwivelAmount = data->m_floats->m_values[0];
    MaxArmExtensionScale = data->m_floats->m_values[1];
    handOffsetMultiplier = data->m_floats->m_values[2];
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 1);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 12);

    m_cp.TargetPosition.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.TargetNormal.set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.TargetVelocity.set(data->m_vector3Data->m_values[8], data->m_vector3Data->m_values[9], data->m_vector3Data->m_values[10]);
    m_cp.Weight = data->m_floats->m_values[0];
  }


  /// Controls the arms' swivel. A positive value for elbows out and up, negative for elbows in and down, at zero the swivel will match the guide pose.
  float getSwivelAmount() const { return SwivelAmount; }
  /// Limit the maximum extension of the arm. Expressed as a proportion of the total arm length.
  float getMaxArmExtensionScale() const { return MaxArmExtensionScale; }
  /// Controls how close the hands are together when placing. 0 places hands in the same position, 1 is shoulder width apart so arms are effectively parallel, 2 is two shoulder widths apart.
  float getHandOffsetMultiplier() const { return handOffsetMultiplier; }

  /// Describes a surface, in terms of its position, normal and velocity, on which the palms of the hands should be placed. All in absolute, world space coordinates.



  // control parameters
  /// Describes a surface, in terms of its position, normal and velocity, on which the palms of the hands should be placed. All in absolute, world space coordinates.

  /// Position of the target in world space and absolute units.
  const NMP::Vector3& getTargetPositionCP() const { return m_cp.TargetPosition; }
  /// Normal of target in world space - does not need to be normalised.
  const NMP::Vector3& getTargetNormalCP() const { return m_cp.TargetNormal; }
  /// Velocity of the target in world space and absolute units.
  const NMP::Vector3& getTargetVelocityCP() const { return m_cp.TargetVelocity; }
  /// Sets the importance of this behaviour relative to others.
  float getWeightCP() const { return m_cp.Weight; }

  // output control parameters
};


class ArmsPlacementBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "ArmsPlacement"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 4; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class ArmsPlacementBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    TargetPosition_CP_ID = 0,
    TargetNormal_CP_ID = 1,
    TargetVelocity_CP_ID = 2,
    Weight_CP_ID = 3,
  };

  const ArmsPlacementBehaviourData& getParams() const { return m_params; }
  ArmsPlacementBehaviourData& getParams() { return m_params; }
  ArmsPlacementPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ArmsPlacementBehaviourData m_params;
  ArmsPlacementPoseRequirements m_poseRequirements;
};

class ArmsPlacementBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ArmsPlacementBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ArmsPlacementBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ArmsPlacementBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ArmsPlacementBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_ARMSPLACEMENT_H

