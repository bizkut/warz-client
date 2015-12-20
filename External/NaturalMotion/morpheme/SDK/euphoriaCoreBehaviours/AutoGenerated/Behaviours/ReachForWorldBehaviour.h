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

#ifndef NM_MDF_BDEF_REACHFORWORLD_H
#define NM_MDF_BDEF_REACHFORWORLD_H

// include definition file to create project dependency
#include "./Definition/Behaviours/ReachForWorld.behaviour"

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

struct ReachForWorldPoseRequirements
{

  ReachForWorldPoseRequirements()
  {
  }
};

struct ReachForWorldBehaviourData
{
  friend class ReachForWorldBehaviour;

private:


  struct ControlParams
  {
    NMP::Vector3 PositionForArm[NetworkConstants::networkMaxNumArms];  ///< Point to reach for in world space.
    NMP::Vector3 NormalForArm[NetworkConstants::networkMaxNumArms];  ///< Normal of the point to be reached to, in world space. The back of the hand will be aligned with this direction.
    float ReachImportanceForArm[NetworkConstants::networkMaxNumArms];  ///< Weight for reaching with each arm, 0 will not reach with that arm.
    float ReachImminenceForArm[NetworkConstants::networkMaxNumArms];  ///< Reciprocal of the time period in which the reach request needs to achieve its reach target position. Larger values will cause the hands to move with more urgency. In 1/s (standard character).
    float StiffnessScaleForArm[NetworkConstants::networkMaxNumArms];  ///< A scale on the stiffness of each arm, 1 is normal stiffness.
    float DampingScaleForArm[NetworkConstants::networkMaxNumArms];  ///< A scale on the damping ratio, larger values will give slower movement without oscillation, smaller will give faster movement with more oscillations.
    float DriveCompensationScaleForArm[NetworkConstants::networkMaxNumArms];  ///< A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations
    float LookImminenceForArm[NetworkConstants::networkMaxNumArms];  ///< Reciprocal of the time period in which the look request needs to achieve its look at goal. Larger values will cause the head to move with more urgency. In 1/s (standard character).
    float UnreachableTargetImportanceScale;  ///< How much should the character reach for targets that out of range. 0 = not at all, 1 = try as hard as possible. 
  };
  ControlParams m_cp;

  float PositionWeightForArm[NetworkConstants::networkMaxNumArms];  ///< The weight associated with the target position in the IK solver. A larger value means the hand will get closer to the target position but may be further away from the target orientation.
  float NormalWeightForArm[NetworkConstants::networkMaxNumArms];  ///< The weight associated with the target normal in the IK solver. A larger value means the hand will get closer to the target orientation but may be further away from the target position.
  float IKSubstepSize;  ///< Modify IK targets by interpolating between the hands current transform and the original target transform with this weight. A step size of 1 means no substepping.
  int32_t SwivelMode[NetworkConstants::networkMaxNumArms];  /// Defines how or whether arm swivel should be applied to the reach: "None" means swivel is not required; "Automatic" means swivel will be determined procedurally by the behaviour based on the reach target, "Specified" means the behaviour will use the fixed swivel amount specified below.
  
  float SwivelAmountForArm[NetworkConstants::networkMaxNumArms];  /// Swivel value to use when Swivel Mode is set to "Specified". Value in range [-1,1],  positive values drive elbows out and up, negative drive elbows in and down.
  
  float MaxReachScale[NetworkConstants::networkMaxNumArms];  ///< Maximum reach as a scale of the full arm length
  float TorsoAvoidanceRadiusMultiplier[NetworkConstants::networkMaxNumArms];  ///< Scales the self avoidance radius set on the spine limb.
  float ChestRotationScaleForArm[NetworkConstants::networkMaxNumArms];  ///< Multiplier applied to any rotation that this arm might request from the chest.
  float PelvisRotationScaleForArm[NetworkConstants::networkMaxNumArms];  ///< Multiplier applied to any rotation that this arm might request from the pelvis
  float MaxChestTranslationForArm[NetworkConstants::networkMaxNumArms];  ///< Maximum chest displacement (relative to spineLength) that this arm will request.
  float MaxPelvisTranslationForArm[NetworkConstants::networkMaxNumArms];  ///< Maximum pelvis displacement (relative to legLength) that this arm will request.
  float LookWeightToPositionForArm[NetworkConstants::networkMaxNumArms];  ///< Specifies the weight to look at the reach target for each arm's reach target.
  bool SlideAlongNormalForArm[NetworkConstants::networkMaxNumArms];  ///< To force the hand to slide along the normal when approaching the target.
  bool SelfAvoidanceEnable[NetworkConstants::networkMaxNumArms];  ///< Avoid collisions between the arm and torso when reaching.


public:

  ReachForWorldBehaviourData()
  {
    PositionWeightForArm[0] = float(1);
    PositionWeightForArm[1] = float(1);
    NormalWeightForArm[0] = float(0);
    NormalWeightForArm[1] = float(0);
    SlideAlongNormalForArm[0] = false;
    SlideAlongNormalForArm[1] = false;
    IKSubstepSize = float(1);
    SwivelMode[0] = 0;
    SwivelMode[1] = 0;
    SwivelAmountForArm[0] = float(-1);
    SwivelAmountForArm[1] = float(-1);
    MaxReachScale[0] = float(1);
    MaxReachScale[1] = float(1);
    SelfAvoidanceEnable[0] = true;
    SelfAvoidanceEnable[1] = true;
    TorsoAvoidanceRadiusMultiplier[0] = float(1);
    TorsoAvoidanceRadiusMultiplier[1] = float(1);
    ChestRotationScaleForArm[0] = float(0);
    ChestRotationScaleForArm[1] = float(0);
    PelvisRotationScaleForArm[0] = float(0);
    PelvisRotationScaleForArm[1] = float(0);
    MaxChestTranslationForArm[0] = float(0);
    MaxChestTranslationForArm[1] = float(0);
    MaxPelvisTranslationForArm[0] = float(0);
    MaxPelvisTranslationForArm[1] = float(0);
    LookWeightToPositionForArm[0] = float(0);
    LookWeightToPositionForArm[1] = float(0);
  }

  void updateOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams);

  void updateAndClearEmittedMessages(uint32_t& messages);

  void updateFromMessageParams(const ER::AttribDataBehaviourParameters* data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 21);
    NMP_ASSERT(data->m_ints->m_numValues == 6);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);


    PositionWeightForArm[0] = data->m_floats->m_values[0];
    PositionWeightForArm[1] = data->m_floats->m_values[1];
    NormalWeightForArm[0] = data->m_floats->m_values[2];
    NormalWeightForArm[1] = data->m_floats->m_values[3];
    SlideAlongNormalForArm[0] = (data->m_ints->m_values[0] != 0);
    SlideAlongNormalForArm[1] = (data->m_ints->m_values[1] != 0);
    IKSubstepSize = data->m_floats->m_values[4];
    SwivelMode[0] = data->m_ints->m_values[2];
    SwivelMode[1] = data->m_ints->m_values[3];
    SwivelAmountForArm[0] = data->m_floats->m_values[5];
    SwivelAmountForArm[1] = data->m_floats->m_values[6];
    MaxReachScale[0] = data->m_floats->m_values[7];
    MaxReachScale[1] = data->m_floats->m_values[8];
    SelfAvoidanceEnable[0] = (data->m_ints->m_values[4] != 0);
    SelfAvoidanceEnable[1] = (data->m_ints->m_values[5] != 0);
    TorsoAvoidanceRadiusMultiplier[0] = data->m_floats->m_values[9];
    TorsoAvoidanceRadiusMultiplier[1] = data->m_floats->m_values[10];
    ChestRotationScaleForArm[0] = data->m_floats->m_values[11];
    ChestRotationScaleForArm[1] = data->m_floats->m_values[12];
    PelvisRotationScaleForArm[0] = data->m_floats->m_values[13];
    PelvisRotationScaleForArm[1] = data->m_floats->m_values[14];
    MaxChestTranslationForArm[0] = data->m_floats->m_values[15];
    MaxChestTranslationForArm[1] = data->m_floats->m_values[16];
    MaxPelvisTranslationForArm[0] = data->m_floats->m_values[17];
    MaxPelvisTranslationForArm[1] = data->m_floats->m_values[18];
    LookWeightToPositionForArm[0] = data->m_floats->m_values[19];
    LookWeightToPositionForArm[1] = data->m_floats->m_values[20];
  }

//----------------------------------------------------------------------------------------------------------------------
  void updateFromControlParams(const ER::AttribDataBehaviourState* const data)
  {
    NMP_ASSERT(data->m_floats->m_numValues == 13);
    NMP_ASSERT(data->m_ints->m_numValues == 0);
    NMP_ASSERT(data->m_uint64s->m_numValues == 0);
    // Vector 3 are stored in a float array with 4 entries per item
    NMP_ASSERT(data->m_vector3Data->m_numValues == 16);

    m_cp.PositionForArm[0].set(data->m_vector3Data->m_values[0], data->m_vector3Data->m_values[1], data->m_vector3Data->m_values[2]);
    m_cp.PositionForArm[1].set(data->m_vector3Data->m_values[4], data->m_vector3Data->m_values[5], data->m_vector3Data->m_values[6]);
    m_cp.NormalForArm[0].set(data->m_vector3Data->m_values[8], data->m_vector3Data->m_values[9], data->m_vector3Data->m_values[10]);
    m_cp.NormalForArm[1].set(data->m_vector3Data->m_values[12], data->m_vector3Data->m_values[13], data->m_vector3Data->m_values[14]);
    m_cp.ReachImportanceForArm[0] = data->m_floats->m_values[0];
    m_cp.ReachImportanceForArm[1] = data->m_floats->m_values[1];
    m_cp.ReachImminenceForArm[0] = data->m_floats->m_values[2];
    m_cp.ReachImminenceForArm[1] = data->m_floats->m_values[3];
    m_cp.StiffnessScaleForArm[0] = data->m_floats->m_values[4];
    m_cp.StiffnessScaleForArm[1] = data->m_floats->m_values[5];
    m_cp.DampingScaleForArm[0] = data->m_floats->m_values[6];
    m_cp.DampingScaleForArm[1] = data->m_floats->m_values[7];
    m_cp.DriveCompensationScaleForArm[0] = data->m_floats->m_values[8];
    m_cp.DriveCompensationScaleForArm[1] = data->m_floats->m_values[9];
    m_cp.LookImminenceForArm[0] = data->m_floats->m_values[10];
    m_cp.LookImminenceForArm[1] = data->m_floats->m_values[11];
    m_cp.UnreachableTargetImportanceScale = data->m_floats->m_values[12];
  }


  /// Set the relative importance of the target position and target orientation (normal) for each arm. 

  /// The weight associated with the target position in the IK solver. A larger value means the hand will get closer to the target position but may be further away from the target orientation.
  float getPositionWeightForArm(unsigned int index) const { NMP_ASSERT(index < 2); return PositionWeightForArm[index]; }
  enum { maxPositionWeightForArm = 2 };
  /// The weight associated with the target normal in the IK solver. A larger value means the hand will get closer to the target orientation but may be further away from the target position.
  float getNormalWeightForArm(unsigned int index) const { NMP_ASSERT(index < 2); return NormalWeightForArm[index]; }
  enum { maxNormalWeightForArm = 2 };

  /// Determine how each hand approaches the target.

  /// To force the hand to slide along the normal when approaching the target.
  bool getSlideAlongNormalForArm(unsigned int index) const { NMP_ASSERT(index < 2); return SlideAlongNormalForArm[index]; }
  enum { maxSlideAlongNormalForArm = 2 };
  /// Modify IK targets by interpolating between the hands current transform and the original target transform with this weight. A step size of 1 means no substepping.
  float getIKSubstepSize() const { return IKSubstepSize; }
  /// Defines how or whether arm swivel should be applied to the reach: "None" means swivel is not required; "Automatic" means swivel will be determined procedurally by the behaviour based on the reach target, "Specified" means the behaviour will use the fixed swivel amount specified below.
  int32_t getSwivelMode(unsigned int index) const { NMP_ASSERT(index < 2); return SwivelMode[index]; }
  enum { maxSwivelMode = 2 };
  /// Swivel value to use when Swivel Mode is set to "Specified". Value in range [-1,1],  positive values drive elbows out and up, negative drive elbows in and down.
  float getSwivelAmountForArm(unsigned int index) const { NMP_ASSERT(index < 2); return SwivelAmountForArm[index]; }
  enum { maxSwivelAmountForArm = 2 };
  /// Maximum reach as a scale of the full arm length
  float getMaxReachScale(unsigned int index) const { NMP_ASSERT(index < 2); return MaxReachScale[index]; }
  enum { maxMaxReachScale = 2 };

  /// Configure he behaviour to avoid positioning the hands within a specified distance from the spine.

  /// Avoid collisions between the arm and torso when reaching.
  bool getSelfAvoidanceEnable(unsigned int index) const { NMP_ASSERT(index < 2); return SelfAvoidanceEnable[index]; }
  enum { maxSelfAvoidanceEnable = 2 };
  /// Scales the self avoidance radius set on the spine limb.
  float getTorsoAvoidanceRadiusMultiplier(unsigned int index) const { NMP_ASSERT(index < 2); return TorsoAvoidanceRadiusMultiplier[index]; }
  enum { maxTorsoAvoidanceRadiusMultiplier = 2 };

  /// Multiplier applied to any rotation that this arm might request from the chest.
  float getChestRotationScaleForArm(unsigned int index) const { NMP_ASSERT(index < 2); return ChestRotationScaleForArm[index]; }
  enum { maxChestRotationScaleForArm = 2 };
  /// Multiplier applied to any rotation that this arm might request from the pelvis
  float getPelvisRotationScaleForArm(unsigned int index) const { NMP_ASSERT(index < 2); return PelvisRotationScaleForArm[index]; }
  enum { maxPelvisRotationScaleForArm = 2 };
  /// Maximum chest displacement (relative to spineLength) that this arm will request.
  float getMaxChestTranslationForArm(unsigned int index) const { NMP_ASSERT(index < 2); return MaxChestTranslationForArm[index]; }
  enum { maxMaxChestTranslationForArm = 2 };
  /// Maximum pelvis displacement (relative to legLength) that this arm will request.
  float getMaxPelvisTranslationForArm(unsigned int index) const { NMP_ASSERT(index < 2); return MaxPelvisTranslationForArm[index]; }
  enum { maxMaxPelvisTranslationForArm = 2 };

  /// Specifies the weight to look at the reach target for each arm's reach target.
  float getLookWeightToPositionForArm(unsigned int index) const { NMP_ASSERT(index < 2); return LookWeightToPositionForArm[index]; }
  enum { maxLookWeightToPositionForArm = 2 };



  // control parameters
  /// Point to reach for in world space.
  const NMP::Vector3& getPositionForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.PositionForArm[index]; }
  enum { maxPositionForArmCP = 2 };
  /// Normal of the point to be reached to, in world space. The back of the hand will be aligned with this direction.
  const NMP::Vector3& getNormalForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.NormalForArm[index]; }
  enum { maxNormalForArmCP = 2 };
  /// Weight for reaching with each arm, 0 will not reach with that arm.
  float getReachImportanceForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ReachImportanceForArm[index]; }
  enum { maxReachImportanceForArmCP = 2 };
  /// Reciprocal of the time period in which the reach request needs to achieve its reach target position. Larger values will cause the hands to move with more urgency. In 1/s (standard character).
  float getReachImminenceForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.ReachImminenceForArm[index]; }
  enum { maxReachImminenceForArmCP = 2 };
  /// A scale on the stiffness of each arm, 1 is normal stiffness.
  float getStiffnessScaleForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.StiffnessScaleForArm[index]; }
  enum { maxStiffnessScaleForArmCP = 2 };
  /// A scale on the damping ratio, larger values will give slower movement without oscillation, smaller will give faster movement with more oscillations.
  float getDampingScaleForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.DampingScaleForArm[index]; }
  enum { maxDampingScaleForArmCP = 2 };
  /// A scale on the drive compensation, larger values give more controlled motion, smaller give more springy motion with more oscillations
  float getDriveCompensationScaleForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.DriveCompensationScaleForArm[index]; }
  enum { maxDriveCompensationScaleForArmCP = 2 };
  /// Reciprocal of the time period in which the look request needs to achieve its look at goal. Larger values will cause the head to move with more urgency. In 1/s (standard character).
  float getLookImminenceForArmCP(unsigned int index) const { NMP_ASSERT(index < 2); return m_cp.LookImminenceForArm[index]; }
  enum { maxLookImminenceForArmCP = 2 };
  /// How much should the character reach for targets that out of range. 0 = not at all, 1 = try as hard as possible. 
  float getUnreachableTargetImportanceScaleCP() const { return m_cp.UnreachableTargetImportanceScale; }

  // output control parameters
};


class ReachForWorldBehaviour : public ER::Behaviour
{
public:
  virtual bool isAnimationRequired(int32_t animationID) const NM_OVERRIDE;
  virtual void interpretControlParams(const ER::AttribDataBehaviourState* data) NM_OVERRIDE;
  virtual void interpretMessageData(const ER::AttribDataBehaviourParameters* data) NM_OVERRIDE;
  virtual void interpretAnimationMessage(const ER::BehaviourAnimationData* animationData, ER::Body* body) NM_OVERRIDE;
  virtual void handleOutputControlParams(ER::AttribDataBehaviourParameters::OutputControlParam* outputControlParams, size_t numOutputControlParams) NM_OVERRIDE;
  virtual void handleEmittedMessages(uint32_t& messages) NM_OVERRIDE;
  virtual const char* getBehaviourName() const NM_OVERRIDE { return "ReachForWorld"; }
  virtual int getBehaviourID() const NM_OVERRIDE { return 24; }
  virtual bool storeState(MR::PhysicsSerialisationBuffer& savedState);
  virtual bool restoreState(MR::PhysicsSerialisationBuffer& savedState);
  class ReachForWorldBehaviourDef* m_definition;

  enum ControlParamIDs
  {
    PositionForArm_0_CP_ID = 0,
    PositionForArm_1_CP_ID = 1,
    NormalForArm_0_CP_ID = 2,
    NormalForArm_1_CP_ID = 3,
    ReachImportanceForArm_0_CP_ID = 4,
    ReachImportanceForArm_1_CP_ID = 5,
    ReachImminenceForArm_0_CP_ID = 6,
    ReachImminenceForArm_1_CP_ID = 7,
    StiffnessScaleForArm_0_CP_ID = 8,
    StiffnessScaleForArm_1_CP_ID = 9,
    DampingScaleForArm_0_CP_ID = 10,
    DampingScaleForArm_1_CP_ID = 11,
    DriveCompensationScaleForArm_0_CP_ID = 12,
    DriveCompensationScaleForArm_1_CP_ID = 13,
    LookImminenceForArm_0_CP_ID = 14,
    LookImminenceForArm_1_CP_ID = 15,
    UnreachableTargetImportanceScale_CP_ID = 16,
  };

  const ReachForWorldBehaviourData& getParams() const { return m_params; }
  ReachForWorldBehaviourData& getParams() { return m_params; }
  ReachForWorldPoseRequirements& getPoseRequirements() { return m_poseRequirements; }

protected:
  ReachForWorldBehaviourData m_params;
  ReachForWorldPoseRequirements m_poseRequirements;
};

class ReachForWorldBehaviourDef : public ER::BehaviourDef
{
public:
  static NM_INLINE NMP::Memory::Format getMemoryRequirements()
  {
    NMP::Memory::Format result(sizeof(ReachForWorldBehaviourDef), NMP_NATURAL_TYPE_ALIGNMENT);
    return result;
  }

  inline ReachForWorldBehaviourDef() {}
  virtual ER::Behaviour* newInstance() const NM_OVERRIDE
  {
    void* alignedMemory = NMP::Memory::memAllocAligned(sizeof(ReachForWorldBehaviour), NMP_VECTOR_ALIGNMENT);
    return new(alignedMemory) ReachForWorldBehaviour;
  }

  virtual const ER::BehaviourDef::ModuleToEnable* getModulesToEnable(uint32_t& numToEnable) NM_OVERRIDE;
};


#ifdef NM_COMPILER_MSVC
# pragma warning ( pop )
#endif // NM_COMPILER_MSVC

} // namespace NM_BEHAVIOUR_LIB_NAMESPACE

#endif // NM_MDF_BDEF_REACHFORWORLD_H

