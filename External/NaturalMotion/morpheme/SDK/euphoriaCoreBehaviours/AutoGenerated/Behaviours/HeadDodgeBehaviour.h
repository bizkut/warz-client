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

#ifndef NM_MDF_BDEF_HEADDODGE_H
#define NM_MDF_BDEF_HEADDODGE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HeadDodge.behaviour"

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

struct HeadDodgePoseRequirements
{

  HeadDodgePoseRequirements()
  {
  }
};

struct HeadDodgeBehaviourData
{
  friend class HeadDodgeBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 HazardPosition;  ///< Position in absolute units and in world space
    NMP::Vector3 HazardVelocity;  ///< Velocity in absolute units.
    float HazardRadius;  ///< Radius of the hazard, this affects how far the character lean when dodging. In absolute units.
    float Weight;  /// Sets the importance of this behaviour relative to others. The pin accepts values in the range 0 to 1.
    
  };
  ControlParams m_cp;



public:

  HeadDodgeBehaviourData()
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
    NMP_ASSERT(data->m_floats->m_numValues == 2);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 8);

    m_cp.HazardPosition.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.HazardVelocity.set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.HazardRadius = data->m_floats->m_values[0];
    m_cp.Weight = data->m_floats->m_values[1];
  }


  /// Describes a hazard that the character should dodge in terms of its position, velolcity and radius. In absolute world space units.



  // control parameters
  /// Describes a hazard that the character should dodge in terms of its position, velolcity and radius. In absolute world space units.

  /// Position in absolute units and in world space
  const NMP::Vector3& getHazardPositionCP() const { return m_cp.HazardPosition; }
  /// Velocity in absolute units.
  const NMP::Vector3& getHazardVelocityCP() const { return m_cp.HazardVelocity; }
  /// Radius of the hazard, this affects how far the character lean when dodging. In absolute units.
  float getHazardRadiusCP() const { return m_cp.HazardRadius; }
  /// Sets the importance of this behaviour relative to others. The pin accepts values in the range 0 to 1.
  float getWeightCP() const { return m_cp.Weight; }

  // output control parameters
};


class HeadDodgeBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "HeadDodge"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 14; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class HeadDodgeBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    HazardPosition_CP_ID = 0,
    HazardVelocity_CP_ID = 1,
    HazardRadius_CP_ID = 2,
    Weight_CP_ID = 3,
  };

  const HeadDodgeBehaviourData& getParams() const { return m_params; }
  HeadDodgeBehaviourData& getParams() { return m_params; }
  HeadDodgePoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  HeadDodgeBehaviourData m_params;
  HeadDodgePoseRequirements m_poseRequirements;
};

class HeadDodgeBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(HeadDodgeBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline HeadDodgeBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(HeadDodgeBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) HeadDodgeBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_HEADDODGE_H

