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

#ifndef NM_MDF_BDEF_USERHAZARD_H
#define NM_MDF_BDEF_USERHAZARD_H

// include definition file to create project dependency
#include "./Definition/Behaviours/UserHazard.behaviour"

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

struct UserHazardPoseRequirements
{

  UserHazardPoseRequirements()
  {
  }
};

struct UserHazardBehaviourData
{
  friend class UserHazardBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 HazardPosition;  ///< Sets the position of the user defined hazard. In absolute world space units.
    NMP::Vector3 HazardVelocity;  ///< Sets the velocity of the user defined hazard. In absolute world space units.
    float HazardMass;  ///< Sets the mass of the user defined hazard, which affects the strength of bracing. In absolute units.
    float HazardRadius;  ///< Sets the radius of the user defined hazard (the object is approximated as a sphere). In absolute units.
    bool UserHazard;  ///< If true, a user defined hazard will be used alongside hazards identified by environment awareness.
    bool IgnoreOtherHazards;  ///< Force the behaviour to use only this hazard, rather than using the environment awareness.
  };
  ControlParams m_cp;



public:

  UserHazardBehaviourData()
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
    NMP_ASSERT(data->m_ints->m_numValues == 2);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 8);

    m_cp.UserHazard = (data->m_ints->m_values[0] != 0);
    m_cp.IgnoreOtherHazards = (data->m_ints->m_values[1] != 0);
    m_cp.HazardPosition.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.HazardVelocity.set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.HazardMass = data->m_floats->m_values[0];
    m_cp.HazardRadius = data->m_floats->m_values[1];
  }




  // control parameters
  /// If true, a user defined hazard will be used alongside hazards identified by environment awareness.
  bool getUserHazardCP() const { return m_cp.UserHazard; }
  /// Force the behaviour to use only this hazard, rather than using the environment awareness.
  bool getIgnoreOtherHazardsCP() const { return m_cp.IgnoreOtherHazards; }
  /// Sets the position of the user defined hazard. In absolute world space units.
  const NMP::Vector3& getHazardPositionCP() const { return m_cp.HazardPosition; }
  /// Sets the velocity of the user defined hazard. In absolute world space units.
  const NMP::Vector3& getHazardVelocityCP() const { return m_cp.HazardVelocity; }
  /// Sets the mass of the user defined hazard, which affects the strength of bracing. In absolute units.
  float getHazardMassCP() const { return m_cp.HazardMass; }
  /// Sets the radius of the user defined hazard (the object is approximated as a sphere). In absolute units.
  float getHazardRadiusCP() const { return m_cp.HazardRadius; }

  // output control parameters
};


class UserHazardBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "UserHazard"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 28; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class UserHazardBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    UserHazard_CP_ID = 0,
    IgnoreOtherHazards_CP_ID = 1,
    HazardPosition_CP_ID = 2,
    HazardVelocity_CP_ID = 3,
    HazardMass_CP_ID = 4,
    HazardRadius_CP_ID = 5,
  };

  const UserHazardBehaviourData& getParams() const { return m_params; }
  UserHazardBehaviourData& getParams() { return m_params; }
  UserHazardPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  UserHazardBehaviourData m_params;
  UserHazardPoseRequirements m_poseRequirements;
};

class UserHazardBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(UserHazardBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline UserHazardBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(UserHazardBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) UserHazardBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_USERHAZARD_H

