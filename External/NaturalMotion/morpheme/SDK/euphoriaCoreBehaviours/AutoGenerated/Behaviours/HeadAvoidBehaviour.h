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

#ifndef NM_MDF_BDEF_HEADAVOID_H
#define NM_MDF_BDEF_HEADAVOID_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HeadAvoid.behaviour"

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

struct HeadAvoidPoseRequirements
{

  HeadAvoidPoseRequirements()
  {
  }
};

struct HeadAvoidBehaviourData
{
  friend class HeadAvoidBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 AvoidPlanePosition;  /// Position of the hazard plane to avoid. In absolute world space units.
    
    NMP::Vector3 AvoidPlaneNormal;  /// Surface normal of the hazard plane to avoid.
    
    float AvoidSafeDistance;  /// Sets the maximum distance a hazard plane can be from the character before it is ignored by the behaviour. In metres (standard character).
    
    float Weight;  /// Sets the importance of this behaviour relative to others. The pin accepts values in the range 0 to 1.
    
  };
  ControlParams m_cp;

  bool twoSidedPlane;  /// Determines if character always avoids the plane from both sides, or a just one side.
  


public:

  HeadAvoidBehaviourData()
  {
    twoSidedPlane = false;
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 0);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    twoSidedPlane = (data->m_ints->m_values[0] != 0);
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 2);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 8);

    m_cp.AvoidPlanePosition.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.AvoidPlaneNormal.set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.AvoidSafeDistance = data->m_floats->m_values[0];
    m_cp.Weight = data->m_floats->m_values[1];
  }


  /// The plane to avoid

  /// Determines if character always avoids the plane from both sides, or a just one side.
  bool getTwoSidedPlane() const { return twoSidedPlane; }


  // control parameters
  /// The plane to avoid

  /// Position of the hazard plane to avoid. In absolute world space units.
  const NMP::Vector3& getAvoidPlanePositionCP() const { return m_cp.AvoidPlanePosition; }
  /// Surface normal of the hazard plane to avoid.
  const NMP::Vector3& getAvoidPlaneNormalCP() const { return m_cp.AvoidPlaneNormal; }
  /// Sets the maximum distance a hazard plane can be from the character before it is ignored by the behaviour. In metres (standard character).
  float getAvoidSafeDistanceCP() const { return m_cp.AvoidSafeDistance; }
  /// Sets the importance of this behaviour relative to others. The pin accepts values in the range 0 to 1.
  float getWeightCP() const { return m_cp.Weight; }

  // output control parameters
};


class HeadAvoidBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "HeadAvoid"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 13; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class HeadAvoidBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    AvoidPlanePosition_CP_ID = 0,
    AvoidPlaneNormal_CP_ID = 1,
    AvoidSafeDistance_CP_ID = 2,
    Weight_CP_ID = 3,
  };

  const HeadAvoidBehaviourData& getParams() const { return m_params; }
  HeadAvoidBehaviourData& getParams() { return m_params; }
  HeadAvoidPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  HeadAvoidBehaviourData m_params;
  HeadAvoidPoseRequirements m_poseRequirements;
};

class HeadAvoidBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(HeadAvoidBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline HeadAvoidBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(HeadAvoidBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) HeadAvoidBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_HEADAVOID_H

