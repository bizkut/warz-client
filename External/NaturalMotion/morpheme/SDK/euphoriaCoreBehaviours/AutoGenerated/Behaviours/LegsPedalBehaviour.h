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

#ifndef NM_MDF_BDEF_LEGSPEDAL_H
#define NM_MDF_BDEF_LEGSPEDAL_H

// include definition file to create project dependency
#include "./Definition/Behaviours/LegsPedal.behaviour"

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

struct LegsPedalPoseRequirements
{

  LegsPedalPoseRequirements()
  {
  }
};

struct LegsPedalBehaviourData
{
  friend class LegsPedalBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 TargetRotationDelta;  ///< Sets a world or local space rotation vector whose direction determines the axis and magnitude determines the angle of rotation of the pelvis, in degrees.
    float RotationTime;  ///< Sets the amount of time in which to achieve the rotation, in seconds (standard character). Smaller values lead to faster movements. For times under about 0.5s the legs will swing rather than spin.
    float ImportanceForLeg[NetworkConstants::networkMaxNumLegs];  ///< Sets the strength of control this behaviour has over each leg. Accepts values in the range 0 to 1.
  };
  ControlParams m_cp;

  float SpinWeightForward;  ///< Weight of rotation control around the forwards / x axis.
  float SpinWeightUp;  ///< Weight of rotation control around the upwards / y axis.
  float SpinWeightLateral;  ///< Weight of rotation control around the sideways / z axis.
  float SpinAmounts[NetworkConstants::networkMaxNumLegs];  ///< Affects how strongly a request to spin the legs is applied. 1 is normal or average stiffness, 0 has no effect, the spin is turned off.
  float MaxAngSpeed;  /// Maximum angular speed of the leg, in revolutions/second (standard character).
  
  float MaxRadius;  /// Maximum radius of spin around the spin centre, this is a length, in metres (standard character).
  
  float SwingAmounts[NetworkConstants::networkMaxNumLegs];  ///< Affects how strongly a request to swing the legs is applied, 0 will not swing, 1 is normal stiffness.
  float FallenFor;  ///< The swing will abort if the character is fallen over for longer than this amount of time (seconds). In seconds (standard character).
  bool Synchronised;  /// If true then both feet will have opposite phases in their respective circles, if false then the feet will circle independently from their initial positions
  
  bool SpinInLocalSpace;  /// If true then the target rotation delta of the character will be interpreted in the local space of the pelvis. Otherwise it is interpreted in world space.
  


public:

  LegsPedalBehaviourData()
  {
    SpinWeightForward = float(1);
    SpinWeightUp = float(1);
    SpinWeightLateral = float(1);
    SpinAmounts[0] = float(1);
    SpinAmounts[1] = float(1);
    MaxAngSpeed = float(3);
    MaxRadius = float(0.4);
    Synchronised = true;
    SwingAmounts[0] = float(0);
    SwingAmounts[1] = float(0);
    FallenFor = float(0.8);
    SpinInLocalSpace = true;
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 10);
    NMP_ASSERT(data->m_ints->m_numValues == 2);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    SpinWeightForward = data->m_floats->m_values[0];
    SpinWeightUp = data->m_floats->m_values[1];
    SpinWeightLateral = data->m_floats->m_values[2];
    SpinAmounts[0] = data->m_floats->m_values[3];
    SpinAmounts[1] = data->m_floats->m_values[4];
    MaxAngSpeed = data->m_floats->m_values[5];
    MaxRadius = data->m_floats->m_values[6];
    Synchronised = (data->m_ints->m_values[0] != 0);
    SwingAmounts[0] = data->m_floats->m_values[7];
    SwingAmounts[1] = data->m_floats->m_values[8];
    FallenFor = data->m_floats->m_values[9];
    SpinInLocalSpace = (data->m_ints->m_values[1] != 0);
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 3);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 4);

    m_cp.TargetRotationDelta.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.RotationTime = data->m_floats->m_values[0];
    m_cp.ImportanceForLeg[0] = data->m_floats->m_values[1];
    m_cp.ImportanceForLeg[1] = data->m_floats->m_values[2];
  }


  /// Affects how strongly the character will spin the legs to drive rotation about each axis. Axis are defined in character or world space depending on the value of the "Operate in local space" attribute.

  /// Weight of rotation control around the forwards / x axis.
  float getSpinWeightForward() const { return SpinWeightForward; }
  /// Weight of rotation control around the upwards / y axis.
  float getSpinWeightUp() const { return SpinWeightUp; }
  /// Weight of rotation control around the sideways / z axis.
  float getSpinWeightLateral() const { return SpinWeightLateral; }

  /// Affects how strongly a request to spin the legs is applied. 1 is normal or average stiffness, 0 has no effect, the spin is turned off.

  /// Affects how strongly a request to spin the legs is applied. 1 is normal or average stiffness, 0 has no effect, the spin is turned off.
  float getSpinAmounts(unsigned int index) const { NMP_ASSERT(index < 2); return SpinAmounts[index]; }
  enum { maxSpinAmounts = 2 };
  /// Maximum angular speed of the leg, in revolutions/second (standard character).
  float getMaxAngSpeed() const { return MaxAngSpeed; }
  /// Maximum radius of spin around the spin centre, this is a length, in metres (standard character).
  float getMaxRadius() const { return MaxRadius; }
  /// If true then both feet will have opposite phases in their respective circles, if false then the feet will circle independently from their initial positions
  bool getSynchronised() const { return Synchronised; }

  /// Affects how strongly a request to swing the legs is applied.

  /// Affects how strongly a request to swing the legs is applied, 0 will not swing, 1 is normal stiffness.
  float getSwingAmounts(unsigned int index) const { NMP_ASSERT(index < 2); return SwingAmounts[index]; }
  enum { maxSwingAmounts = 2 };

  /// The swing will abort if the character is fallen over for longer than this amount of time (seconds). In seconds (standard character).
  float getFallenFor() const { return FallenFor; }

  /// If true then the target rotation delta of the character will be interpreted in the local space of the pelvis. Otherwise it is interpreted in world space.
  bool getSpinInLocalSpace() const { return SpinInLocalSpace; }

  // control parameters
  /// Sets a world or local space rotation vector whose direction determines the axis and magnitude determines the angle of rotation of the pelvis, in degrees.
  const NMP::Vector3& getTargetRotationDeltaCP() const { return m_cp.TargetRotationDelta; }
  /// Sets the amount of time in which to achieve the rotation, in seconds (standard character). Smaller values lead to faster movements. For times under about 0.5s the legs will swing rather than spin.
  float getRotationTimeCP() const { return m_cp.RotationTime; }
  /// Sets the strength of control this behaviour has over each leg. Accepts values in the range 0 to 1.
  float getImportanceForLegCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ImportanceForLeg[index]; }
  enum { maxImportanceForLegCP = 2 };

  // output control parameters
};


class LegsPedalBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "LegsPedal"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 18; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class LegsPedalBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    TargetRotationDelta_CP_ID = 0,
    RotationTime_CP_ID = 1,
    ImportanceForLeg_0_CP_ID = 2,
    ImportanceForLeg_1_CP_ID = 3,
  };

  const LegsPedalBehaviourData& getParams() const { return m_params; }
  LegsPedalBehaviourData& getParams() { return m_params; }
  LegsPedalPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  LegsPedalBehaviourData m_params;
  LegsPedalPoseRequirements m_poseRequirements;
};

class LegsPedalBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(LegsPedalBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline LegsPedalBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(LegsPedalBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) LegsPedalBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_LEGSPEDAL_H

