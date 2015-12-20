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

#ifndef NM_MDF_BDEF_OBSERVE_H
#define NM_MDF_BDEF_OBSERVE_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Observe.behaviour"

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

struct ObservePoseRequirements
{

  ObservePoseRequirements()
  {
  }
};

struct ObserveBehaviourData
{
  friend class ObserveBehaviour;

private:


  struct ControlParams
  {
    float ObserveWeight;  ///< The weight (importance) of look relative to other looking. 0 can be used to get output control params without actually looking
  };
  ControlParams m_cp;


  struct OutputControlParams
  {
    float observedWeight;  ///< Indicates how strongly the object selected by this behaviour is being observed. The value is 0 when no objects are observed, 1 when the character is only looking at the selected object.
  };
  OutputControlParams m_ocp;

  float MinSpeed;  ///< The minimum relative speed at which the character will observe a moving object, in m/s (standard character).
  float MinAcceleration;  ///< The minimum relative acceleration at which the character will observe a moving object, in m/s^2 (standard character).
  float MaxDistance;  ///< The maximum distance for the character at which the object will be observed, in metres (standard character).
  float MaxSize;  ///< The maximum size of an object, at which it will be observed. Larger may look odd as the character looks at the object centre. In metres (standard character).
  float MinMass;  ///< The smallest mass at which to observe the object. In kg (standard character).
  float winnersAdvantage;  ///< An extra boost in value for the highest rated object, to reduce flipping between many objects
  float interestReductionRate;  ///< Rate of exponential decrease of the output observe weight, so an object will still be observed for a while even if it stops moving. Units 1/seconds (standard character).


public:

  ObserveBehaviourData()
  {
    MinSpeed = float(1.5);
    MinAcceleration = float(8);
    MaxDistance = float(4);
    MaxSize = float(4);
    MinMass = float(0.1);
    winnersAdvantage = float(0.5);
    interestReductionRate = float(0.5);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 7);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    MinSpeed = data->m_floats->m_values[0];
    MinAcceleration = data->m_floats->m_values[1];
    MaxDistance = data->m_floats->m_values[2];
    MaxSize = data->m_floats->m_values[3];
    MinMass = data->m_floats->m_values[4];
    winnersAdvantage = data->m_floats->m_values[5];
    interestReductionRate = data->m_floats->m_values[6];
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 1);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 0);

    m_cp.ObserveWeight = data->m_floats->m_values[0];
  }


  /// The minimum relative speed at which the character will observe a moving object, in m/s (standard character).
  float getMinSpeed() const { return MinSpeed; }
  /// The minimum relative acceleration at which the character will observe a moving object, in m/s^2 (standard character).
  float getMinAcceleration() const { return MinAcceleration; }
  /// The maximum distance for the character at which the object will be observed, in metres (standard character).
  float getMaxDistance() const { return MaxDistance; }
  /// The maximum size of an object, at which it will be observed. Larger may look odd as the character looks at the object centre. In metres (standard character).
  float getMaxSize() const { return MaxSize; }
  /// The smallest mass at which to observe the object. In kg (standard character).
  float getMinMass() const { return MinMass; }
  /// An extra boost in value for the highest rated object, to reduce flipping between many objects
  float getWinnersAdvantage() const { return winnersAdvantage; }
  /// Rate of exponential decrease of the output observe weight, so an object will still be observed for a while even if it stops moving. Units 1/seconds (standard character).
  float getInterestReductionRate() const { return interestReductionRate; }


  // control parameters
  /// The weight (importance) of look relative to other looking. 0 can be used to get output control params without actually looking
  float getObserveWeightCP() const { return m_cp.ObserveWeight; }

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// Indicates how strongly the object selected by this behaviour is being observed. The value is 0 when no objects are observed, 1 when the character is only looking at the selected object.
  void setObservedWeightOCP(float value) { m_ocp.observedWeight = value; }
};


class ObserveBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Observe"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 20; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class ObserveBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    ObserveWeight_CP_ID = 0,
  };

  enum OutputControlParamIDs
  {
    observedWeight_OCP_ID = 0,
  };

  const ObserveBehaviourData& getParams() const { return m_params; }
  ObserveBehaviourData& getParams() { return m_params; }
  ObservePoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ObserveBehaviourData m_params;
  ObservePoseRequirements m_poseRequirements;
};

class ObserveBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ObserveBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ObserveBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ObserveBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ObserveBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_OBSERVE_H

