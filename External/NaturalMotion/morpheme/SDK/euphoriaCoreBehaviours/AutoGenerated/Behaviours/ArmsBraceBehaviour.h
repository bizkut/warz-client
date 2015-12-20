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

#ifndef NM_MDF_BDEF_ARMSBRACE_H
#define NM_MDF_BDEF_ARMSBRACE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ArmsBrace.behaviour"

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

struct ArmsBracePoseRequirements
{

  ArmsBracePoseRequirements()
  {
  }
};

struct ArmsBraceBehaviourData
{
  friend class ArmsBraceBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 HazardVelocity;  ///< Velocity (m/s) of the hazard
    NMP::Vector3 HazardPosition;  ///< Position (m) of the hazard surface
    NMP::Vector3 HazardNormal;  ///< Normal of the hazard surface (doesn't have to be normalised, but should be non-zero)
    float HazardMass;  ///< Mass (kg) of the hazard
    float HazardRadius;  ///< Extent and curvature (1/radius) of the hazard surface
    float Weight;  ///< Sets the importance of this behaviour relative to others. Accepts values in the range 0 to 1.
  };
  ControlParams m_cp;

  float SwivelAmount;  ///< Controls the arms' swivel. A positive value encourages the elbows to be placed out and up, negative for elbows in and down. When zero the swivel will match the guide pose.
  float MaxArmExtensionScale;  ///< Limit the maximum extension of the arm. Expressed as a proportion of the total arm length


public:

  ArmsBraceBehaviourData()
  {
    SwivelAmount = float(0.3);
    MaxArmExtensionScale = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 2);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    SwivelAmount = data->m_floats->m_values[0];
    MaxArmExtensionScale = data->m_floats->m_values[1];
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 3);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 12);

    m_cp.HazardVelocity.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.HazardPosition.set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.HazardNormal.set(data->m_vector3Data->m_values[8], data->m_vector3Data->m_values[9], data->m_vector3Data->m_values[10]);
    m_cp.HazardMass = data->m_floats->m_values[0];
    m_cp.HazardRadius = data->m_floats->m_values[1];
    m_cp.Weight = data->m_floats->m_values[2];
  }


  /// Controls the arms' swivel. A positive value encourages the elbows to be placed out and up, negative for elbows in and down. When zero the swivel will match the guide pose.
  float getSwivelAmount() const { return SwivelAmount; }
  /// Limit the maximum extension of the arm. Expressed as a proportion of the total arm length
  float getMaxArmExtensionScale() const { return MaxArmExtensionScale; }

  /// Describes an approaching object that the arms should brace against.
  /// Hazard position and hazard normal represent the hazard surface whilst
  /// hazard radius defines the extent and curvature (1/radius) of that surface.
  /// In absolute, world space units



  // control parameters
  /// Describes an approaching object that the arms should brace against.
  /// Hazard position and hazard normal represent the hazard surface whilst
  /// hazard radius defines the extent and curvature (1/radius) of that surface.
  /// In absolute, world space units

  /// Velocity (m/s) of the hazard
  const NMP::Vector3& getHazardVelocityCP() const { return m_cp.HazardVelocity; }
  /// Position (m) of the hazard surface
  const NMP::Vector3& getHazardPositionCP() const { return m_cp.HazardPosition; }
  /// Normal of the hazard surface (doesn't have to be normalised, but should be non-zero)
  const NMP::Vector3& getHazardNormalCP() const { return m_cp.HazardNormal; }
  /// Mass (kg) of the hazard
  float getHazardMassCP() const { return m_cp.HazardMass; }
  /// Extent and curvature (1/radius) of the hazard surface
  float getHazardRadiusCP() const { return m_cp.HazardRadius; }
  /// Sets the importance of this behaviour relative to others. Accepts values in the range 0 to 1.
  float getWeightCP() const { return m_cp.Weight; }

  // output control parameters
};


class ArmsBraceBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "ArmsBrace"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 3; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class ArmsBraceBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    HazardVelocity_CP_ID = 0,
    HazardPosition_CP_ID = 1,
    HazardNormal_CP_ID = 2,
    HazardMass_CP_ID = 3,
    HazardRadius_CP_ID = 4,
    Weight_CP_ID = 5,
  };

  const ArmsBraceBehaviourData& getParams() const { return m_params; }
  ArmsBraceBehaviourData& getParams() { return m_params; }
  ArmsBracePoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ArmsBraceBehaviourData m_params;
  ArmsBracePoseRequirements m_poseRequirements;
};

class ArmsBraceBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ArmsBraceBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ArmsBraceBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ArmsBraceBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ArmsBraceBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_ARMSBRACE_H

