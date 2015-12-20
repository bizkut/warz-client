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

#ifndef NM_MDF_BDEF_HAZARDAWARENESS_H
#define NM_MDF_BDEF_HAZARDAWARENESS_H

// include definition file to create project dependency
#include "./Definition/Behaviours/HazardAwareness.behaviour"

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

struct HazardAwarenessPoseRequirements
{

  HazardAwarenessPoseRequirements()
  {
  }
};

struct HazardAwarenessBehaviourData
{
  friend class HazardAwarenessBehaviour;

private:


  struct OutputControlParams
  {
    float HazardFreeTime;  ///< The amount of time that has passed since a hazard has existed at a hazard level higher than hazardLevelThreshold above. In absolute units.
    float HazardLevel;  ///< A unitless value in the range 0-infinity indicating the threat posed by the current hazard. Calculated as 1/time to impact, scaled by a 0-1 measure of the mass of the hazard.
    float TimeToImpact;  ///< The time left until the hazard collides with the character or 0 if there is no hazard. In absolute units.
    float ImpactSpeed;  ///< The speed with which the hazard will impact the character or 0 if there is no hazard. In absolute units.
  };
  OutputControlParams m_ocp;

  float IgnoreVerticalPredictionAmount;  ///< How much the vertical component of the character's velocity is ignored when predicting impacts. 0 is the default, 1 means that the vertical velocity of the chest is completely ignored so the ground will not present a hazard when the character is falling.
  float HazardLevelThreshold;  ///< Used, as feedback info, to determine the time since the last hazard, as feedback info. Is 1/(1+time to impact), in 1/seconds (standard character).
  float ProbeRadius;  ///< Radius around the chest that the character has "hazard awarenesss" for, in metres (standard character).
  bool UseControlledVelocity;  ///< When set then the chest's velocity is not considered hazardous, hazards are only moving objects


public:

  HazardAwarenessBehaviourData()
  {
    UseControlledVelocity = false;
    IgnoreVerticalPredictionAmount = float(0);
    HazardLevelThreshold = float(0.43);
    ProbeRadius = float(0.5);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 3);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    UseControlledVelocity = (data->m_ints->m_values[0] != 0);
    IgnoreVerticalPredictionAmount = data->m_floats->m_values[0];
    HazardLevelThreshold = data->m_floats->m_values[1];
    ProbeRadius = data->m_floats->m_values[2];
  }


  /// When set then the chest's velocity is not considered hazardous, hazards are only moving objects
  bool getUseControlledVelocity() const { return UseControlledVelocity; }
  /// How much the vertical component of the character's velocity is ignored when predicting impacts. 0 is the default, 1 means that the vertical velocity of the chest is completely ignored so the ground will not present a hazard when the character is falling.
  float getIgnoreVerticalPredictionAmount() const { return IgnoreVerticalPredictionAmount; }
  /// Used, as feedback info, to determine the time since the last hazard, as feedback info. Is 1/(1+time to impact), in 1/seconds (standard character).
  float getHazardLevelThreshold() const { return HazardLevelThreshold; }
  /// Radius around the chest that the character has "hazard awarenesss" for, in metres (standard character).
  float getProbeRadius() const { return ProbeRadius; }

  // control parameters

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// The amount of time that has passed since a hazard has existed at a hazard level higher than hazardLevelThreshold above. In absolute units.
  void setHazardFreeTimeOCP(float value) { m_ocp.HazardFreeTime = value; }
  /// A unitless value in the range 0-infinity indicating the threat posed by the current hazard. Calculated as 1/time to impact, scaled by a 0-1 measure of the mass of the hazard.
  void setHazardLevelOCP(float value) { m_ocp.HazardLevel = value; }
  /// The time left until the hazard collides with the character or 0 if there is no hazard. In absolute units.
  void setTimeToImpactOCP(float value) { m_ocp.TimeToImpact = value; }
  /// The speed with which the hazard will impact the character or 0 if there is no hazard. In absolute units.
  void setImpactSpeedOCP(float value) { m_ocp.ImpactSpeed = value; }
};


class HazardAwarenessBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "HazardAwareness"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 12; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class HazardAwarenessBehaviourDef* m_definition;

  enum OutputControlParamIDs
  {
    HazardFreeTime_OCP_ID = 0,
    HazardLevel_OCP_ID = 1,
    TimeToImpact_OCP_ID = 2,
    ImpactSpeed_OCP_ID = 3,
  };

  const HazardAwarenessBehaviourData& getParams() const { return m_params; }
  HazardAwarenessBehaviourData& getParams() { return m_params; }
  HazardAwarenessPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  HazardAwarenessBehaviourData m_params;
  HazardAwarenessPoseRequirements m_poseRequirements;
};

class HazardAwarenessBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(HazardAwarenessBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline HazardAwarenessBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(HazardAwarenessBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) HazardAwarenessBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_HAZARDAWARENESS_H

