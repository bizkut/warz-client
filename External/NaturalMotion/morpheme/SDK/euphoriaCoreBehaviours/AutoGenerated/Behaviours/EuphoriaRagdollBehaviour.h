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

#ifndef NM_MDF_BDEF_EUPHORIARAGDOLL_H
#define NM_MDF_BDEF_EUPHORIARAGDOLL_H

// include definition file to create project dependency
#include "./Definition/Behaviours/EuphoriaRagdoll.behaviour"

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

struct EuphoriaRagdollPoseRequirements
{

  EuphoriaRagdollPoseRequirements()
  {
  }
};

struct EuphoriaRagdollBehaviourData
{
  friend class EuphoriaRagdollBehaviour;

private:

  bool DisableSleeping;  ///< False allows character to sleep when velocity is below a threshold. Used as an optimisation when there are many stationary characters


public:

  EuphoriaRagdollBehaviourData()
  {
    DisableSleeping = false;
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 0);
    NMP_ASSERT(data->m_ints->m_numValues == 1);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    DisableSleeping = (data->m_ints->m_values[0] != 0);
  }


  /// False allows character to sleep when velocity is below a threshold. Used as an optimisation when there are many stationary characters
  bool getDisableSleeping() const { return DisableSleeping; }

  // control parameters

  // output control parameters
};


class EuphoriaRagdollBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "EuphoriaRagdoll"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 9; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class EuphoriaRagdollBehaviourDef* m_definition;

  const EuphoriaRagdollBehaviourData& getParams() const { return m_params; }
  EuphoriaRagdollBehaviourData& getParams() { return m_params; }
  EuphoriaRagdollPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  EuphoriaRagdollBehaviourData m_params;
  EuphoriaRagdollPoseRequirements m_poseRequirements;
};

class EuphoriaRagdollBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(EuphoriaRagdollBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline EuphoriaRagdollBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(EuphoriaRagdollBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) EuphoriaRagdollBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_EUPHORIARAGDOLL_H

