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

#ifndef NM_MDF_BDEF_EYES_H
#define NM_MDF_BDEF_EYES_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Eyes.behaviour"

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

struct EyesPoseRequirements
{

  EyesPoseRequirements()
  {
  }
};

struct EyesBehaviourData
{
  friend class EyesBehaviour;

private:


  struct OutputControlParams
  {
    NMP::Vector3 LookDirection;  ///< A normalised vector in the direction that the character is looking.
    NMP::Vector3 FocalCentre;  ///< The position in world space that the character is focussing on. In absolute world space units.
    float FocalRadius;  ///< The radius of the field around the focalCentre in which the character will notice objects. In absolute units.
  };
  OutputControlParams m_ocp;



public:

  EyesBehaviourData()
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


  // control parameters

  // output control parameters

  void clearOutputControlParamsData() { memset(&m_ocp, 0, sizeof(m_ocp)); }

  /// A normalised vector in the direction that the character is looking.
  void setLookDirectionOCP(const NMP::Vector3&  value) { m_ocp.LookDirection = value; }
  /// The position in world space that the character is focussing on. In absolute world space units.
  void setFocalCentreOCP(const NMP::Vector3&  value) { m_ocp.FocalCentre = value; }
  /// The radius of the field around the focalCentre in which the character will notice objects. In absolute units.
  void setFocalRadiusOCP(float value) { m_ocp.FocalRadius = value; }
};


class EyesBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Eyes"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 10; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  virtual void clearOutputControlParamsData() NM_OVERRIDE;
  class EyesBehaviourDef* m_definition;

  enum OutputControlParamIDs
  {
    LookDirection_OCP_ID = 0,
    FocalCentre_OCP_ID = 1,
    FocalRadius_OCP_ID = 2,
  };

  const EyesBehaviourData& getParams() const { return m_params; }
  EyesBehaviourData& getParams() { return m_params; }
  EyesPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  EyesBehaviourData m_params;
  EyesPoseRequirements m_poseRequirements;
};

class EyesBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(EyesBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline EyesBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(EyesBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) EyesBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_EYES_H

