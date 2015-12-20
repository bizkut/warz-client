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

#ifndef NM_MDF_BDEF_PROTECT_H
#define NM_MDF_BDEF_PROTECT_H

// include definition file to create project dependency
#include "./Definition/Behaviours/Protect.behaviour"

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

struct ProtectPoseRequirements
{

  ProtectPoseRequirements()
  {
  }
};

struct ProtectBehaviourData
{
  friend class ProtectBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 OffsetFromChest;  ///< Offset from the chest that is used as the primary part to protect. X is forward, Y is up and Z is right, relative to the chest. In metres (standard character).
  };
  ControlParams m_cp;

  float HeadLookWeight;  ///< Look towards potential hazards that approach the character
  float HazardLookTime;  ///< How long the character will look at an object that is no longer on collision course. Positive will look where the hazard was going to. Negative will look back to where the hazard came from. Both are extrapolations. In seconds (standard character).
  float HazardLevelThreshold;  ///< Used only in hazardFreeTime to determine the time since the last hazard, as feedback info
  float ObjectTrackingRadius;  ///< Radius for non-hazards, for example to use with headAvoid and armPlacement. In metres (standard character).
  float HeadAvoidWeight;  ///< Keep the head away from nearby hazards to reduce chance of head impact
  float HeadDodgeWeight;  ///< Move head laterally or duck out the way of an incoming projectile
  float BraceWeight;  ///< Move arms to intercept the incoming hazard, and cushion the impact
  float ArmsPlacementWeight;  ///< Place arms near or on nearby hazards that could be a danger in future
  float SensitivityToCloseMovements;  ///< Determines the amount of danger given for a certain level of relative acceleration. Higher will do placement/headAvoid more eagerly. In seconds (standard character).
  float CrouchDownAmount;  ///< How much the character should crouch downwards as the impact approaches
  float CrouchPitchAmount;  ///< How much the character should lean forwards as the impact approaches
  float SupportIgnoreRadius;  ///< Radius of a sphere around the support point (i.e. the feet when balancing) where hazards will be ignored. This prevents bracing against the ground when crouching etc. In metres (standard character).
  float ArmsSwingWeight;  ///< Help rotate or stabilise the chest
  float SwivelAmount;  ///< Controls the arms' swivel, Positive is elbows out and up, 0 matches the guide pose, negative is elbows in and down
  float MaxArmExtensionScale;  ///< Limit the maximum extension of the arm. Expressed as a proportion of the total arm length. For use in bracen and placement
  float LegsSwingWeight;  ///< Help rotate or stabilise the pelvis. 


public:

  ProtectBehaviourData()
  {
    HeadLookWeight = float(1);
    HazardLookTime = float(0.5);
    HazardLevelThreshold = float(0.75);
    ObjectTrackingRadius = float(1);
    HeadAvoidWeight = float(1);
    HeadDodgeWeight = float(1);
    BraceWeight = float(1);
    ArmsPlacementWeight = float(1);
    SensitivityToCloseMovements = float(0.5);
    CrouchDownAmount = float(0.25);
    CrouchPitchAmount = float(0.75);
    SupportIgnoreRadius = float(0.5);
    ArmsSwingWeight = float(1);
    SwivelAmount = float(0.3);
    MaxArmExtensionScale = float(1);
    LegsSwingWeight = float(1);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 16);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    HeadLookWeight = data->m_floats->m_values[0];
    HazardLookTime = data->m_floats->m_values[1];
    HazardLevelThreshold = data->m_floats->m_values[2];
    ObjectTrackingRadius = data->m_floats->m_values[3];
    HeadAvoidWeight = data->m_floats->m_values[4];
    HeadDodgeWeight = data->m_floats->m_values[5];
    BraceWeight = data->m_floats->m_values[6];
    ArmsPlacementWeight = data->m_floats->m_values[7];
    SensitivityToCloseMovements = data->m_floats->m_values[8];
    CrouchDownAmount = data->m_floats->m_values[9];
    CrouchPitchAmount = data->m_floats->m_values[10];
    SupportIgnoreRadius = data->m_floats->m_values[11];
    ArmsSwingWeight = data->m_floats->m_values[12];
    SwivelAmount = data->m_floats->m_values[13];
    MaxArmExtensionScale = data->m_floats->m_values[14];
    LegsSwingWeight = data->m_floats->m_values[15];
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 0);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 4);

    m_cp.OffsetFromChest.set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
  }


  /// Look towards potential hazards that approach the character
  float getHeadLookWeight() const { return HeadLookWeight; }
  /// How long the character will look at an object that is no longer on collision course. Positive will look where the hazard was going to. Negative will look back to where the hazard came from. Both are extrapolations. In seconds (standard character).
  float getHazardLookTime() const { return HazardLookTime; }
  /// Used only in hazardFreeTime to determine the time since the last hazard, as feedback info
  float getHazardLevelThreshold() const { return HazardLevelThreshold; }
  /// Radius for non-hazards, for example to use with headAvoid and armPlacement. In metres (standard character).
  float getObjectTrackingRadius() const { return ObjectTrackingRadius; }

  /// Keep the head away from nearby hazards to reduce chance of head impact
  float getHeadAvoidWeight() const { return HeadAvoidWeight; }
  /// Move head laterally or duck out the way of an incoming projectile
  float getHeadDodgeWeight() const { return HeadDodgeWeight; }
  /// Move arms to intercept the incoming hazard, and cushion the impact
  float getBraceWeight() const { return BraceWeight; }
  /// Place arms near or on nearby hazards that could be a danger in future
  float getArmsPlacementWeight() const { return ArmsPlacementWeight; }
  /// Determines the amount of danger given for a certain level of relative acceleration. Higher will do placement/headAvoid more eagerly. In seconds (standard character).
  float getSensitivityToCloseMovements() const { return SensitivityToCloseMovements; }
  /// How much the character should crouch downwards as the impact approaches
  float getCrouchDownAmount() const { return CrouchDownAmount; }
  /// How much the character should lean forwards as the impact approaches
  float getCrouchPitchAmount() const { return CrouchPitchAmount; }
  /// Radius of a sphere around the support point (i.e. the feet when balancing) where hazards will be ignored. This prevents bracing against the ground when crouching etc. In metres (standard character).
  float getSupportIgnoreRadius() const { return SupportIgnoreRadius; }

  /// Configure the use of arm swing to avoid hazards by altering the chest's velocity.

  /// Help rotate or stabilise the chest
  float getArmsSwingWeight() const { return ArmsSwingWeight; }
  /// Controls the arms' swivel, Positive is elbows out and up, 0 matches the guide pose, negative is elbows in and down
  float getSwivelAmount() const { return SwivelAmount; }
  /// Limit the maximum extension of the arm. Expressed as a proportion of the total arm length. For use in bracen and placement
  float getMaxArmExtensionScale() const { return MaxArmExtensionScale; }

  /// Configure the use of leg swing to avoid hazards by altering the pelvis' velocity.

  /// Help rotate or stabilise the pelvis. 
  float getLegsSwingWeight() const { return LegsSwingWeight; }


  // control parameters
  /// Offset from the chest that is used as the primary part to protect. X is forward, Y is up and Z is right, relative to the chest. In metres (standard character).
  const NMP::Vector3& getOffsetFromChestCP() const { return m_cp.OffsetFromChest; }

  // output control parameters
};


class ProtectBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "Protect"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 22; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class ProtectBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    OffsetFromChest_CP_ID = 0,
  };

  const ProtectBehaviourData& getParams() const { return m_params; }
  ProtectBehaviourData& getParams() { return m_params; }
  ProtectPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ProtectBehaviourData m_params;
  ProtectPoseRequirements m_poseRequirements;
};

class ProtectBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ProtectBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ProtectBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ProtectBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ProtectBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_PROTECT_H

