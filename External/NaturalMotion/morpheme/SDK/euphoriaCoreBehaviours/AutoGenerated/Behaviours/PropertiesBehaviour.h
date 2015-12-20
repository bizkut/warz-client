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

#ifndef NM_MDF_BDEF_PROPERTIES_H
#define NM_MDF_BDEF_PROPERTIES_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Properties.behaviour"

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

struct PropertiesPoseRequirements
{

  PropertiesPoseRequirements()
  {
  }
};

struct PropertiesBehaviourData
{
  friend class PropertiesBehaviour;

private:


  struct ControlParams
  {
    float CollidingSupportTime;  ///< When a supporting contact is lost, the character still considers itself partially supported during this time period. This is so that it does not suddenly "panic" if it hops off the ground. In seconds (standard character).
    float MaxSlopeForGround;  ///< If a surface slopes more than this angle, in degrees, then it will not be considered "ground" for the purposes of balance etc.
    float AwarenessPredictionTime;  ///< Only respond objects in the environment (hazards or edges to grab etc) when they are predicted to interact with the character within this time. In seconds (standard character).
    float JointProjectionLinearTolerance;  ///< Tolerance above which joint separation will be corrected.
    float JointProjectionAngularTolerance;  ///< Angular tolerance in degrees above which joint limit errors will be corrected.
    int32_t JointProjectionIterations;  ///< Number of iterations used for resolving joint separation.
    bool EnableJointProjection;  ///< Use joint projection to reduce joint separation
  };
  ControlParams m_cp;



public:

  PropertiesBehaviourData()
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
    NMP_ASSERT(data->m_floats->m_numValues == 5);
    NMP_ASSERT(data->m_ints->m_numValues == 2);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 0);

    m_cp.CollidingSupportTime = data->m_floats->m_values[0];
    m_cp.MaxSlopeForGround = data->m_floats->m_values[1];
    m_cp.AwarenessPredictionTime = data->m_floats->m_values[2];
    m_cp.EnableJointProjection = (data->m_ints->m_values[0] != 0);
    m_cp.JointProjectionLinearTolerance = data->m_floats->m_values[3];
    m_cp.JointProjectionAngularTolerance = data->m_floats->m_values[4];
    m_cp.JointProjectionIterations = data->m_ints->m_values[1];
  }



  /// Enable and configure joint projection, an additional solver step for correcting joint separation in the physics rig.


  // control parameters
  /// When a supporting contact is lost, the character still considers itself partially supported during this time period. This is so that it does not suddenly "panic" if it hops off the ground. In seconds (standard character).
  float getCollidingSupportTimeCP() const { return m_cp.CollidingSupportTime; }
  /// If a surface slopes more than this angle, in degrees, then it will not be considered "ground" for the purposes of balance etc.
  float getMaxSlopeForGroundCP() const { return m_cp.MaxSlopeForGround; }
  /// Only respond objects in the environment (hazards or edges to grab etc) when they are predicted to interact with the character within this time. In seconds (standard character).
  float getAwarenessPredictionTimeCP() const { return m_cp.AwarenessPredictionTime; }
  /// Enable and configure joint projection, an additional solver step for correcting joint separation in the physics rig.

  /// Use joint projection to reduce joint separation
  bool getEnableJointProjectionCP() const { return m_cp.EnableJointProjection; }
  /// Tolerance above which joint separation will be corrected.
  float getJointProjectionLinearToleranceCP() const { return m_cp.JointProjectionLinearTolerance; }
  /// Angular tolerance in degrees above which joint limit errors will be corrected.
  float getJointProjectionAngularToleranceCP() const { return m_cp.JointProjectionAngularTolerance; }
  /// Number of iterations used for resolving joint separation.
  int getJointProjectionIterationsCP() const { return m_cp.JointProjectionIterations; }

  // output control parameters
};


class PropertiesBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Properties"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 21; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class PropertiesBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    CollidingSupportTime_CP_ID = 0,
    MaxSlopeForGround_CP_ID = 1,
    AwarenessPredictionTime_CP_ID = 2,
    EnableJointProjection_CP_ID = 3,
    JointProjectionLinearTolerance_CP_ID = 4,
    JointProjectionAngularTolerance_CP_ID = 5,
    JointProjectionIterations_CP_ID = 6,
  };

  const PropertiesBehaviourData& getParams() const { return m_params; }
  PropertiesBehaviourData& getParams() { return m_params; }
  PropertiesPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  PropertiesBehaviourData m_params;
  PropertiesPoseRequirements m_poseRequirements;
};

class PropertiesBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(PropertiesBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline PropertiesBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(PropertiesBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) PropertiesBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_PROPERTIES_H

